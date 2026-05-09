# PKES Calibration Snapshot - 2026-05-07

This snapshot records the calibration data currently compiled into `PE_Demo/PE2/02_LF_RF` before the next RSSI measurement pass.

## Temporary Measurement Setting

- `User/main.c`
- `HANDLE_LF_POLL_ROUNDS = 30`
- `ANTENNA_COUNT = 4`
- One door-handle trigger now polls `4 * 30 = 120` LF wakeup slots.
- LF timer period is `300 ms`, so one full polling window is about `36 s`.

## Antenna Mapping

- Ant1: left / driver side
- Ant2: right / passenger side
- Ant3: front
- Ant4: rear

LF driver mapping in `User/LF.c`:

| Antenna | LF driver |
| --- | --- |
| 1 | `DRIVER_A30` |
| 2 | `DRIVER_A02` |
| 3 | `DRIVER_A31` |
| 4 | `DRIVER_A10` |

RF RSSI frame mapping:

| RF byte0 | Antenna |
| --- | --- |
| `0xA2` | 1 |
| `0xA3` | 2 |
| `0xA4` | 3 |
| `0xA5` | 4 |

## Ranging Constants

Source: `User/pkes_ranging.h`

| Name | Value |
| --- | --- |
| `PKES_RANGING_MIN_CM` | `9` |
| `PKES_RANGING_MAX_CM` | `179` |
| `PKES_RANGING_TOO_FAR_CM` | `180` |
| `PKES_RANGING_MISSING_RSSI_RAW` | `616` |
| `PKES_RANGING_AXIS_CENTER_CM` | `40` |
| `PKES_RANGING_AXIS_SIDE_CM` | `55` |

Source: `User/pkes_ranging.c`

| Name | Value |
| --- | --- |
| `PKES_RANGING_TEMPLATE_K` | `9` |
| `PKES_RANGING_TEMPLATE_ACCEPT_PERCENT` | `85` |
| `PKES_RANGING_INSIDE_K` | `9` |
| `PKES_RANGING_INSIDE_ACCEPT_PERCENT` | `80` |
| `PKES_RANGING_INSIDE_MAX_CM` | `135` |

## RSSI To Distance Table

Source: `User/pkes_ranging.c`

RSSI values are stored as `x2`. Distance values are stored as `cm x10`.

## New Measurement Notes

These values are newly measured and have not yet been applied to `User/pkes_ranging.c`.

| Distance | Measured RSSI | Current code RSSI | Note |
| --- | ---: | ---: | --- |
| 9 cm | 2881 | 2726.5 | User-corrected measurement |
| 14 cm | 2512 | 2562 | User-corrected measurement |
| 19 cm | 2249 | 2253.5 | User-corrected measurement |
| 24 cm | 2124 | 2209 | User-corrected measurement |
| 29 cm | 2086 | 2171.5 | User-corrected measurement |
| 34 cm | 1801 | 1850 | User-corrected measurement |
| 39 cm | 1710 | 1736 | User-corrected measurement |
| 44 cm | 1630 | 1640 | User-corrected measurement |
| 49 cm | 1551 | 1563 | User-corrected measurement |
| 54 cm | 1483 | 1506.5 | User-corrected measurement |
| 59 cm | 1427 | 1454.5 | User-corrected measurement |
| 64 cm | 1140 | 1174.5 | User-corrected measurement |
| 69 cm | 1080 | 1109.5 | User-corrected measurement |
| 74 cm | 1025 | 1059 | User-corrected measurement |
| 79 cm | 980 | 1021.5 | User-corrected measurement |
| 84 cm | 940 | 977.5 | User-corrected measurement |
| 89 cm | 900 | 942 | User-corrected measurement |
| 99 cm | 836 | 869 | User-corrected measurement |
| 109 cm | 777 | 815 | User-corrected measurement |
| 119 cm | 730 | 763.5 | User-corrected measurement |
| 129 cm | 694 | 729 | User-corrected measurement |
| 139 cm | 660 | 685 | User-corrected measurement |
| 149 cm | 630 | 655 | User-corrected measurement |
| 159 cm | 600 | 624.5 | User-corrected measurement |
| 169 cm | 580 | 620.25 | User-corrected measurement; current code value is interpolated |
| 179 cm | 560 | 616 | User-corrected measurement |

| distance_cm_x10 | rssi_x2 |
| --- | --- |
| 90 | 5453 |
| 140 | 5124 |
| 190 | 4507 |
| 240 | 4418 |
| 290 | 4343 |
| 340 | 3700 |
| 390 | 3472 |
| 440 | 3280 |
| 490 | 3126 |
| 540 | 3013 |
| 590 | 2909 |
| 640 | 2349 |
| 690 | 2219 |
| 740 | 2118 |
| 790 | 2043 |
| 840 | 1955 |
| 890 | 1884 |
| 940 | 1818 |
| 990 | 1738 |
| 1040 | 1702 |
| 1090 | 1630 |
| 1140 | 1572 |
| 1190 | 1527 |
| 1240 | 1492 |
| 1290 | 1458 |
| 1390 | 1370 |
| 1490 | 1310 |
| 1590 | 1249 |
| 1790 | 1232 |

## Template Tables

| File | Count | Notes |
| --- | ---: | --- |
| `User/pkes_rssi_template_table_5cm.h` | 4704 | Outside / mask region templates |
| `User/pkes_inside_template_table_3cm.h` | 1681 | Inside templates |

Region distribution in `pkes_rssi_template_table_5cm.h`:

| Region | Count |
| --- | ---: |
| `PKES_REGION_UNKNOWN` | 2304 |
| `PKES_REGION_LEFT_DOOR` | 600 |
| `PKES_REGION_RIGHT_DOOR` | 600 |
| `PKES_REGION_TRUNK` | 600 |
| `PKES_REGION_FRONT` | 600 |
| `PKES_REGION_INSIDE` | 0 |
| `PKES_REGION_BOUNDARY` | 0 |

## File Hashes

SHA256 hashes for exact source comparison:

| File | SHA256 |
| --- | --- |
| `User/pkes_ranging.c` | `7DA24204D8F7EAA287D2FACBED60270B226839C5F6056CDC828FEC256AED1E42` |
| `User/pkes_ranging.h` | `22D7239BD5F0B4E7A6BE24FDFBA127395C2F74DE444A487F9977E337ED81146B` |
| `User/pkes_rssi_template_table_5cm.h` | `0AF4622B41D96E29E228D744B9844AA0A4D97D050B3DB54F391E3C6623F558B6` |
| `User/pkes_inside_template_table_3cm.h` | `A9ED074C97DD62080BEBED17EB9C7F16A2FA605B303B27294C0887CC6AC98CEB` |
