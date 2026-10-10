#!/usr/bin/env python3
"""Local preview with automatic reload; standard library only."""
import hashlib
import json
from http.server import SimpleHTTPRequestHandler, ThreadingHTTPServer
from pathlib import Path
from urllib.parse import urlsplit

ROOT = Path(__file__).resolve().parents[1] / 'site' / 'dist'

def version():
    values = []
    for name in ('index.html', 'style.css', 'app.js'):
        path = ROOT / name
        values.append(path.read_bytes() if path.exists() else b'')
    return hashlib.sha256(b'\0'.join(values)).hexdigest()

RELOAD = b'''<script>
(async function () {
  let version = null;
  async function check() {
    try {
      const response = await fetch('/__preview_version', {cache: 'no-store'});
      if (!response.ok) return;
      const data = await response.json();
      if (version !== null && data.version !== version) { location.reload(); return; }
      version = data.version;
    } catch (_) { /* Retry while the preview server restarts. */ }
    setTimeout(check, 1500);
  }
  check();
})();
</script>'''

class PreviewHandler(SimpleHTTPRequestHandler):
    def __init__(self, *args, **kwargs):
        super().__init__(*args, directory=str(ROOT), **kwargs)

    def end_headers(self):
        self.send_header('Cache-Control', 'no-store')
        super().end_headers()

    def do_GET(self):
        route = urlsplit(self.path).path
        if route == '/__preview_version':
            body = json.dumps({'version': version()}).encode()
            content_type = 'application/json'
        elif route in ('/', '/index.html'):
            body = (ROOT / 'index.html').read_bytes().replace(b'</body>', RELOAD + b'</body>')
            content_type = 'text/html; charset=utf-8'
        else:
            return super().do_GET()
        self.send_response(200)
        self.send_header('Content-Type', content_type)
        self.send_header('Content-Length', str(len(body)))
        self.end_headers()
        self.wfile.write(body)

if __name__ == '__main__':
    try:
        server = ThreadingHTTPServer(('127.0.0.1', 8000), PreviewHandler)
        print('Abre en Brave: http://localhost:8000', flush=True)
        print('Recarga automática al guardar. Ctrl+C para detener.', flush=True)
        server.serve_forever()
    except KeyboardInterrupt:
        pass
