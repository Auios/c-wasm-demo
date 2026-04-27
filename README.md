# C WASM Demo

C compiles to `.wasm`; a small JS loader talks to the DOM.

**Requirements:**

- **clang** with **wasm32** target
- **wasm-ld**
- **CMake** 3.20+
- **Ninja**
- **Python 3**

1. **Windows**
   1. Download and install [LLVM](https://releases.llvm.org/) or run `winget install LLVM.LLVM`.
   2. Install Ninja by running `winget install Ninja-build.Ninja`.
   3. Add the LLVM `bin` directory to your `PATH` environment variable.  
      For example, if LLVM is installed to `C:\Program Files\LLVM`, run in Command Prompt:
      ```
      set "PATH=C:\Program Files\LLVM\bin;%PATH%"
      ```

2. **macOS**
   1. Run `brew install llvm ninja cmake` to install dependencies.
   2. Add LLVM to your path with: `export PATH="$(brew --prefix llvm)/bin:$PATH"`.
      - Note: The default Apple Clang usually does **not** support wasm32.

3. **Linux**
   1. Install the required tools (replace with your distribution’s package manager if needed):  
      `sudo apt install cmake ninja-build clang lld`
   2. Verify that `clang` supports **wasm32** by running:  
      `clang -print-targets`  
      and checking that **wasm32** appears in the list.

**Build and run** (repo root):

```sh
python build.py
python dev.py
```

`dev.py` serves **`dist/`** on port 8080 and opens the browser.

Without the scripts: `cmake -B _build -G Ninja && cmake --build _build`, then serve **`dist/`** over HTTP (browsers block `file://`).

After building, you'll find **`compile_commands.json`** in the project root. This file enables powerful code navigation and autocompletion with tools like **clangd** for intellisense.  
For enhanced C/C++ intellisense, code navigation, and debugging, install both the [clangd extension](https://marketplace.cursorapi.com/items/?itemName=llvm-vs-code-extensions.vscode-clangd) and the [LLDB extension](https://marketplace.cursorapi.com/items/?itemName=vadimcn.vscode-lldb) in VSCode or Cursor. After building, run `clangd: Restart language server` to ensure the editor picks up project changes.
