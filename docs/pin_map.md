# Pin map: Software Development Board (PIC18F26Q43, SSOP28)

Source: dev board schematic (Micro.SchDoc, OptoSense.SchDoc, rev 2.0, 2026-09-15),
checked against Snoopy v0.22 `main.c`. Your own board (PIC18F47Q43, LQFP44) should be
checked separately against `hardware/kicad/`.

| Pin | Port | Net | Notes |
|---|---|---|---|
| 1  | RE3 | MCLRN | 10k pull-up, goes to J5 |
| 2  | RA0 | POT1 | analog |
| 3  | RA1 | POT2 | analog |
| 4  | RA2 | POT3 | analog |
| 5  | RA3 | SW2 | button, active low, 10k pull-up |
| 6  | RA4 | SERIN | 74HC595 data in |
| 7  | RA5 | SCLK | 74HC595 shift clock |
| 9  | RA7 | SEROUT | 74HC595 serial out |
| 10 | RA6 | RCLK | 74HC595 latch clock |
| 11 | RC0 | SCLRN | 74HC595 clear, active low, keep high |
| 12 | RC1 | SENSOR1 | pin HIGH = black, LOW = white |
| 13 | RC2 | SENSOR2 | |
| 14 | RC3 | SENSOR3 | |
| 15 | RC4 | SENSOR4 | centerline sensor |
| 16 | RC5 | SENSOR5 | |
| 17 | RC6 | SENSOR6 | |
| 18 | RC7 | SENSOR7 | |
| 21 | RB0 | RIGHT_MOT_IN1 | PWM1 |
| 22 | RB1 | RIGHT_MOT_IN2 | PWM1 |
| 23 | RB2 | LEFT_MOT_IN1  | PWM2 |
| 24 | RB3 | LEFT_MOT_IN2  | PWM2 |
| 25 | RB4 | PICTX | UART1 TX -> P3 |
| 26 | RB5 | PICRX | UART1 RX, weak pull-up |
| 27 | RB6 | PGCLK / LEDN | ICSP clock, shared with debug LED (active low) |
| 28 | RB7 | PGDAT / DAC1 | ICSP data, shared with DAC1 -> VREFB |

## Notes
- Motor current limit and pot reference both come from VREFB (DAC1 -> MCP6001 buffer),
  about 2.04 V with the demo's FVR 2.048 V setting (`DAC1_SetOutput(0xFF)`).
- Sensor LEDs are switched by the 74HC595 (QB..QH = OPTO1..OPTO7).
- Serial: 38400 baud, 8-N-1 (UART1).

## TODO
- Confirm the PIC18F47Q43 pin map for our own board against the KiCad netlist.
- BOM still lists PIC18F26Q43 for U12 but the footprint is LQFP-44: fix the BOM.
