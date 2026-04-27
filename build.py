#!/usr/bin/env python3
"""Configure and build with CMake + Ninja."""

from __future__ import annotations

import shutil
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parent
BUILD_DIR = ROOT / "_build"


def need_exe(name: str) -> None:
    if shutil.which(name) is None:
        print(f"missing executable on PATH: {name}", file=sys.stderr)
        sys.exit(1)


def main() -> None:
    required = [
        (ROOT / "public", "dir"),
        (ROOT / "runtime" / "wasm_loader.js", "file"),
        (ROOT / "src" / "main.c", "file"),
        (ROOT / "CMakeLists.txt", "file"),
    ]
    for path, kind in required:
        ok = path.is_dir() if kind == "dir" else path.is_file()
        if not ok:
            print(f"missing: {path.relative_to(ROOT)}", file=sys.stderr)
            sys.exit(1)

    need_exe("cmake")
    need_exe("ninja")
    need_exe("clang")

    targets = subprocess.run(
        ["clang", "-print-targets"],
        check=False,
        capture_output=True,
        text=True,
    )
    if "wasm32" not in targets.stdout:
        print(
            "clang on PATH must support wasm32 (install LLVM and use its clang).",
            file=sys.stderr,
        )
        sys.exit(1)

    subprocess.run(
        ["cmake", "-S", str(ROOT), "-B", str(BUILD_DIR), "-G", "Ninja"],
        check=True,
    )
    subprocess.run(["cmake", "--build", str(BUILD_DIR)], check=True)

    print()
    print(f"Built: {ROOT / 'dist' / 'app.wasm'}")
    print(f"Package: {ROOT / 'dist'}")
    print("Run: python dev.py")


if __name__ == "__main__":
    main()
