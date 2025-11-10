Embedded Clock System – Release 2

Build
- EDS shell or terminal: `make` (produces `clock_app`)
- Clean: `make clean`

Run
- Copy `clock_app` to HPS.
- Execute: `./clock_app`
- Options:
  - `--start HH:MM:SS`
  - `--demo` (forces 12:34:56 once)

Code Map
- main.c – loop, timekeeping, CLI
- hal-api.c/.h – /dev/mem mmap LW bridge
- hex-display.* – HEX init/write/clear
- led.* – LED utilities
- switch.* – read slide switches
- driver_stub.h – placeholder “driver” APIs

Design Docs
- Design Report (Release 2) – see `/docs/DesignReport.docx`
- Test Plan – see `/docs/TestPlan.docx`

Links
- Git: <repo URL>
- Screencast: <video URL>
