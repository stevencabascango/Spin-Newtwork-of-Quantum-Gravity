#!/usr/bin/env python3
"""Refresh the source download and the optional Linux Workshop 1 bundle."""
from pathlib import Path
import shutil
import zipfile

ROOT = Path(__file__).resolve().parents[1]
DEST = ROOT / 'deliverables'
DEST.mkdir(exist_ok=True)
exclude = {'.deps', 'bin', 'obj', 'build', 'deliverables', '__pycache__', '.git'}
source = DEST / 'SpinNetworkLaboratory-Workshop1-source.zip'
with zipfile.ZipFile(source, 'w', zipfile.ZIP_DEFLATED) as archive:
    for path in sorted(ROOT.rglob('*')):
        relative = path.relative_to(ROOT)
        if not path.is_file() or any(part in exclude for part in relative.parts):
            continue
        if path.suffix in {'.zip', '.pyc', '.layout', '.depend'}:
            continue
        archive.write(path, 'SpinNetworkLaboratory/' + str(relative))
shutil.copyfile(source, ROOT / 'site/dist/SpinNetworkLaboratory.zip')

binary = ROOT / 'bin/spin_network'
local = ROOT / '.deps/sfml/usr'
if binary.exists() and local.exists():
    bundle = DEST / 'SpinNetworkLaboratory-Workshop1-linux-x86_64.zip'
    with zipfile.ZipFile(bundle, 'w', zipfile.ZIP_DEFLATED) as archive:
        archive.write(binary, 'SpinNetworkLaboratory/bin/spin_network')
        for path in (ROOT / 'assets').rglob('*'):
            if path.is_file():
                archive.write(path, 'SpinNetworkLaboratory/' + str(path.relative_to(ROOT)))
        for name in ('libsfml-graphics.so.2.6*', 'libsfml-window.so.2.6*', 'libsfml-system.so.2.6*', 'libstb.so.0*'):
            for path in (local / 'lib/x86_64-linux-gnu').glob(name):
                archive.write(path, 'SpinNetworkLaboratory/bin/lib/' + path.name)
        for path in (local / 'share/doc').rglob('copyright'):
            archive.write(path, 'SpinNetworkLaboratory/licenses/' + path.parent.name + '.txt')
        archive.write(ROOT / 'docs/report/workshop1.pdf', 'SpinNetworkLaboratory/docs/report/workshop1.pdf')
        archive.writestr('SpinNetworkLaboratory/LEEME.txt',
            'Workshop 1: Linux x86_64, SFML 2.6.2.\n'
            'Desde la carpeta extraida: chmod +x bin/spin_network && ./bin/spin_network\n'
            'Requiere escritorio X11, OpenGL, FreeType y bibliotecas X11 del sistema.\n'
            'Este binario no es un ejecutable Windows.\n'
            'La ventana SFML no pudo probarse en el sandbox: verificar en un escritorio normal.\n'
            'La captura y prueba automatica se ejecutan con ./bin/spin_network --smoke-test\n')
for package in sorted(DEST.glob('*.zip')):
    with zipfile.ZipFile(package) as archive:
        assert archive.testzip() is None
        print(package.name, len(archive.namelist()), 'files,', package.stat().st_size, 'bytes, integrity OK')
