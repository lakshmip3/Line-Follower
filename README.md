# Line-Following Robot: Surface Mounted Bulls

Bright Manufacturing Challenge 2026.

## Boards
- **Software development board** (organizer-built, PIC18F26Q43, SSOP28): used to write and test firmware.
- **Our own board** (PIC18F47Q43, LQFP44): the one we assemble and race in Round 3. Design files in `hardware/`.

## Layout
- `firmware/SNOOPY.X/`  MPLAB X project: organizer demo firmware (Snoopy v0.22), our baseline
- `hardware/`  KiCad project, BOM, fab outputs
- `docs/`  pin map, notes, tuning log
- `tools/`  helper scripts

## Toolchain (as used by the baseline project)
- MPLAB X IDE v6.30
- XC8 compiler v3.10
- DFP: PIC18F-Q_DFP 1.30.487
- MCC (MPLAB Code Configurator) for peripheral setup
- Programmer: PICkit (ICSP header J5 on the dev board)

## Build and flash
1. Open `firmware/SNOOPY.X` in MPLAB X.
2. Build, then program the dev board through the PICkit.
3. Open a terminal on P3 at 38400 baud, 8-N-1. You should see "Snoopy v0.22".

## Status
- [ ] Baseline Snoopy builds and runs on the dev board
- [ ] Sensor read and calibration
- [ ] Motor control with current limit set
- [ ] Line following (PID)
- [ ] Turn / loop / crossing handling
- [ ] Port to our own board

## License / sharing
Private for now. The baseline firmware is organizer-supplied (with Microchip headers):
confirm sharing terms before making this repo public.
