#!/usr/bin/env python3
"""Serve dist/ over HTTP and open the browser."""

from __future__ import annotations

import functools
import http.server
import os
import socketserver
import subprocess
import sys
import threading
import time
import urllib.error
import urllib.request
import webbrowser
from pathlib import Path

ROOT = Path(__file__).resolve().parent
DIST = ROOT / "dist"
PORT = int(os.environ.get("PORT", "8080"))
HOST = "127.0.0.1"
BASE = f"http://{HOST}:{PORT}/"


class ReuseAddrTCPServer(socketserver.TCPServer):
    allow_reuse_address = True


def free_port(port: int) -> None:
    if sys.platform == "win32":
        ps = (
            f"$c = Get-NetTCPConnection -LocalPort {port} -State Listen "
            f"-ErrorAction SilentlyContinue; "
            f"foreach ($x in $c) {{ "
            f"if ($x.OwningProcess -gt 0) {{ "
            f"Stop-Process -Id $x.OwningProcess -Force -ErrorAction SilentlyContinue "
            f"}} }}"
        )
        subprocess.run(
            ["powershell", "-NoProfile", "-Command", ps],
            capture_output=True,
            check=False,
        )
        time.sleep(0.3)
        return

    try:
        out = subprocess.check_output(
            ["lsof", "-ti", f"TCP:{port}", "-sTCP:LISTEN"],
            stderr=subprocess.DEVNULL,
            text=True,
        )
    except (subprocess.CalledProcessError, FileNotFoundError):
        return
    for line in out.splitlines():
        pid = line.strip()
        if pid.isdigit():
            try:
                os.kill(int(pid), 15)
            except ProcessLookupError:
                pass
    time.sleep(0.3)


def wait_for_http(path: str) -> bool:
    url = BASE.rstrip("/") + path
    for _ in range(50):
        try:
            req = urllib.request.Request(url, method="HEAD")
            urllib.request.urlopen(req, timeout=0.5)
            return True
        except (urllib.error.URLError, OSError):
            time.sleep(0.1)
    return False


def main() -> None:
    for name in ("index.html", "wasm_loader.js", "app.wasm"):
        if not (DIST / name).is_file():
            print(
                f"missing {DIST / name} — run python build.py first",
                file=sys.stderr,
            )
            sys.exit(1)

    free_port(PORT)

    handler = functools.partial(
        http.server.SimpleHTTPRequestHandler,
        directory=str(DIST),
    )

    try:
        httpd = ReuseAddrTCPServer((HOST, PORT), handler)
    except OSError as e:
        print(f"could not bind {HOST}:{PORT}: {e}", file=sys.stderr)
        sys.exit(1)

    thread = threading.Thread(target=httpd.serve_forever, daemon=True)
    thread.start()

    if not wait_for_http("/wasm_loader.js"):
        httpd.shutdown()
        print("server did not become ready", file=sys.stderr)
        sys.exit(1)

    print(f"Serving {DIST} at {BASE}")
    webbrowser.open(BASE)
    try:
        input("Press Enter to stop the server…\n")
    except EOFError:
        pass
    finally:
        httpd.shutdown()
        thread.join(timeout=2.0)


if __name__ == "__main__":
    main()
