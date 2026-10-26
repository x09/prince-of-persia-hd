# Custom opening music

2026-10-20: user's exports/soundtrack.mp3 converted to intro_theme.ogg.
Vorbis quality 7, stereo 44100 Hz, 24.814 seconds. Audio stream only (MP3 embedded artwork excluded).
Existing names.txt mapping 54=intro_theme selects this file through the standard game loader. Applies to both DOS and HD display modes. No code or package changes required for playback.

Validation: tests/check_intro_ogg.c loads sound 54 with the actual game loader, requires sound_ogg, decodes entire track using the built-in stb_vorbis decoder. Result: 1094276 stereo frames, 2186247 nonzero samples. ffmpeg full decode also passed.
Logs: build/intro-replacement-loader.log, build/intro-replacement-smoke.log, build/intro-replacement-encode.log.

Regenerate from dos directory:
```sh
ffmpeg -i exports/soundtrack.mp3 -map 0:a:0 -vn -ar 44100 -ac 2 -c:a libvorbis -q:a 7 data/music/intro_theme.ogg
```

To restore the original AdLib arrangement, move intro_theme.ogg out of data/music; the engine falls back to its original MIDI resource. The uploaded MP3 and extracted original exports/intro_theme.* remain available.
