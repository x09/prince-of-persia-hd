# PoPHD packaging notes

Project history, dependencies, build instructions and graphics options:
[English](../README.md) · [Русский](../README-ru.md).

Create the source release from the repository root:

```sh
python3 packaging/make-source-release.py
```

The resulting archive, SHA256 file and ALT Linux spec are in `release/`.
The source archive has one `prince-of-persia-hd-1.0/` root and includes both
README translations, the spec, engine sources, tests, tools and game resources.
Local binaries, build directories and verification logs are excluded.

The `install` target installs README.md and README-ru.md alongside COPYING,
README.SDLPoP.md and ASSET_NOTICES.md. The spec includes that documentation
folder in the RPM. No RPM is built by the source-release script.
