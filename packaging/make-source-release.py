#!/usr/bin/env python3
"""Produce a deterministic source-only release; never include local build state."""
from pathlib import Path
import gzip
import hashlib
import io
import tarfile

root = Path(__file__).resolve().parents[1]
out = root / 'release'
out.mkdir(exist_ok=True)
name = 'prince-of-persia-hd-1.0'
archive = out / (name + '.tar.gz')
epoch = 1792540800  # 2026-10-21 UTC
files = [root / 'Makefile', root / 'README.md', root / 'README-ru.md']
for base in ('dos/src', 'dos/tools', 'dos/tests', 'dos/data', 'dos/doc', 'dos/third_party', 'packaging'):
    files.extend(p for p in (root / base).rglob('*') if p.is_file())
files.extend(p for p in (root / 'dos').iterdir() if p.is_file() and (p.suffix in ('.md', '.ini') or p.name in ('Makefile', 'COPYING')))
files.append(out / 'prince-of-persia-hd.spec')
with archive.open('wb') as raw:
    with gzip.GzipFile(filename='', fileobj=raw, mode='wb', mtime=epoch) as gz:
        with tarfile.open(fileobj=gz, mode='w') as tar:
            for p in sorted(set(files)):
                relative = p.relative_to(root)
                if '__pycache__' in relative.parts or p.suffix in ('.o', '.pyc'):
                    continue
                dest = Path(name) / relative
                if relative == Path('release/prince-of-persia-hd.spec'):
                    dest = Path(name) / 'prince-of-persia-hd.spec'
                data = p.read_bytes()
                info = tarfile.TarInfo(str(dest))
                info.size, info.mtime = len(data), epoch
                info.mode = 0o755 if p.suffix in ('.sh', '.py') else 0o644
                tar.addfile(info, io.BytesIO(data))
checksum = hashlib.sha256(archive.read_bytes()).hexdigest()
(out / (archive.name + '.sha256')).write_text(f'{checksum}  {archive.name}\n')
print(f'{archive}: {archive.stat().st_size} bytes; SHA256 {checksum}')
