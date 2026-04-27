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
        if (!el || !instance) {
          return;
        }
        const table = instance.exports.__indirect_function_table;
        el.addEventListener("click", () => {
          table.get(fnIdx)();
        });
      },
      on_input(selPtr, fnIdx) {
        const sel = decodeCString(memory, selPtr);
        const el = document.querySelector(sel);
        if (!el || !instance) {
          return;
        }
        const table = instance.exports.__indirect_function_table;
        const run = () => {
          table.get(fnIdx)();
        };
        el.addEventListener("input", run);
        el.addEventListener("change", run);
      },
      get_input_int(selPtr) {
        const sel = decodeCString(memory, selPtr);
        const el = document.querySelector(sel);
        if (!el) {
          return 0;
        }
        if ("value" in el && el.value !== undefined && el.value !== "") {
          const n = parseInt(String(el.value), 10);
          return Number.isFinite(n) ? n : 0;
        }
        return 0;
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
  const msg = document.getElementById("message");
  if (msg) {
    msg.innerText = "WASM load failed — see console.";
  }
});
