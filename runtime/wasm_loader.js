function decodeCString(mem, ptr) {
  if (ptr === 0) {
    return "";
  }
  const bytes = new Uint8Array(mem.buffer);
  let end = ptr;
  while (bytes[end] !== 0) {
    end += 1;
  }
  return new TextDecoder().decode(bytes.subarray(ptr, end));
}

function colorHex(c) {
  return "#" + (c & 0xffffff).toString(16).padStart(6, "0");
}

const NAV_KEYS = new Set([
  " ",
  "ArrowUp",
  "ArrowDown",
  "ArrowLeft",
  "ArrowRight",
]);

function keyCodeFromEvent(e) {
  if (typeof e.keyCode === "number" && e.keyCode > 0) {
    return e.keyCode;
  }
  if (typeof e.which === "number" && e.which > 0) {
    return e.which;
  }
  if (typeof e.key === "string" && e.key.length === 1) {
    return e.key.toUpperCase().charCodeAt(0);
  }
  return 0;
}

async function loadWasm() {
  const url = new URL("app.wasm", import.meta.url);
  const res = await fetch(url);
  if (!res.ok) {
    throw new Error(`Failed to fetch WASM: ${res.status} ${url}`);
  }
  const buf = await res.arrayBuffer();

  /** @type {WebAssembly.Memory | null} */
  let memory = null;
  /** @type {WebAssembly.Instance | null} */
  let instance = null;

  function callFn(fnIdx, ...args) {
    if (!instance) {
      return;
    }
    const table = instance.exports.__indirect_function_table;
    const fn = table.get(fnIdx);
    if (typeof fn === "function") {
      fn(...args);
    }
  }

  function getCanvas2D(sel) {
    const el = document.querySelector(sel);
    if (!el || !(el instanceof HTMLCanvasElement)) {
      return null;
    }
    return { el, ctx: el.getContext("2d") };
  }

  const imports = {
    env: {
      set_text(selPtr, textPtr) {
        const sel = decodeCString(memory, selPtr);
        const text = decodeCString(memory, textPtr);
        const el = document.querySelector(sel);
        if (el) {
          el.innerText = text;
        }
      },
      on_click(selPtr, fnIdx) {
        const sel = decodeCString(memory, selPtr);
        const el = document.querySelector(sel);
        if (!el) {
          return;
        }
        el.addEventListener("click", () => callFn(fnIdx));
      },
      on_input(selPtr, fnIdx) {
        const sel = decodeCString(memory, selPtr);
        const el = document.querySelector(sel);
        if (!el) {
          return;
        }
        const run = () => callFn(fnIdx);
        el.addEventListener("input", run);
        el.addEventListener("change", run);
      },
      get_input_int(selPtr) {
        const sel = decodeCString(memory, selPtr);
        const el = document.querySelector(sel);
        if (!el) {
          return 0;
        }
        const target = /** @type {HTMLInputElement} */ (el);
        if (target.value !== undefined && target.value !== "") {
          const n = parseInt(String(target.value), 10);
          return Number.isFinite(n) ? n : 0;
        }
        return 0;
      },
      on_key(fnIdx) {
        window.addEventListener("keydown", (e) => {
          const t = /** @type {HTMLElement | null} */ (e.target);
          if (
            t &&
            (t.tagName === "INPUT" ||
              t.tagName === "TEXTAREA" ||
              t.isContentEditable)
          ) {
            return;
          }
          const code = keyCodeFromEvent(e);
          if (code === 0) {
            return;
          }
          if (NAV_KEYS.has(e.key)) {
            e.preventDefault();
          }
          callFn(fnIdx, code);
        });
      },
      request_frame(fnIdx) {
        requestAnimationFrame(() => callFn(fnIdx));
      },
      rand_below(n) {
        if (n <= 0) {
          return 0;
        }
        return Math.floor(Math.random() * n);
      },
      canvas_width(selPtr) {
        const sel = decodeCString(memory, selPtr);
        const c = getCanvas2D(sel);
        return c ? c.el.width : 0;
      },
      canvas_height(selPtr) {
        const sel = decodeCString(memory, selPtr);
        const c = getCanvas2D(sel);
        return c ? c.el.height : 0;
      },
      canvas_clear(selPtr, color) {
        const sel = decodeCString(memory, selPtr);
        const c = getCanvas2D(sel);
        if (!c || !c.ctx) {
          return;
        }
        c.ctx.fillStyle = colorHex(color);
        c.ctx.fillRect(0, 0, c.el.width, c.el.height);
      },
      canvas_fill_rect(selPtr, x, y, w, h, color) {
        const sel = decodeCString(memory, selPtr);
        const c = getCanvas2D(sel);
        if (!c || !c.ctx) {
          return;
        }
        c.ctx.fillStyle = colorHex(color);
        c.ctx.fillRect(x, y, w, h);
      },
      canvas_fill_circle(selPtr, x, y, r, color) {
        const sel = decodeCString(memory, selPtr);
        const c = getCanvas2D(sel);
        if (!c || !c.ctx) {
          return;
        }
        c.ctx.fillStyle = colorHex(color);
        c.ctx.beginPath();
        c.ctx.arc(x, y, r, 0, Math.PI * 2);
        c.ctx.fill();
      },
    },
  };

  const result = await WebAssembly.instantiate(buf, imports);
  instance = result.instance;
  memory = instance.exports.memory;
  instance.exports.init();
}

loadWasm().catch((err) => {
  console.error(err);
  const status = document.getElementById("status");
  if (status) {
    status.innerText = "WASM load failed — see console.";
  }
});
