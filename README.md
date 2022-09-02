# keyboards
This is a personal project for my own keyboards.

## Custom Keycodes
| Code | Descrption| Notes|
|------|-----------|--------
| USER00 | Apple fn/🌐 key ||
| USER01 | Apple fn/🌐 key with remapping F1-12 to 1-0,minus,equal keys ||
| USER02 | Cycle through RGB on/off modes | FK680ProV2 only |

## Apple Fn/Globe Shortcuts
|Shortcut|Descrption|
|--------|----------|
| Fn/Globe + F1 | Decrease display brightness |
| Fn/Globe + F2 | Increase display brightness |
| Fn/Globe + F3 | Mission control |
| Fn/Globe + F4 | Open Launchpad |
| Fn/Globe + F5 |  F5 |
| Fn/Globe + F6 |  F6 |
| Fn/Globe + F7 | Media track prev |
| Fn/Globe + F8 | Media play/pause |
| Fn/Globe + F9 | Media track next |
| Fn/Globe + F10 | Mute audio |
| Fn/Globe + F11 | Increase audio volume|
| Fn/Globe + F12 | Decrease audio volume|
| Fn/Globe + Space | Hey Siri |
| Fn/Globe + Q | Quick Notes |
| Fn/Globe + E | Open Emoji (Character Viewer) |
| Fn/Globe + A | Focus Dock |
| Fn/Globe + D | Enable Dictation |
| Fn/Globe + F | Toggle full screen mode |
| Fn/Globe + H | Show Desktop |
| Fn/Globe + C | Show Control Ceneter |
| Fn/Globe + N | Show Notifications |
| Fn/Globe + M | Focus Menubar |

## Amethyst Shortcuts
|Shortcut|Descrption|
|--------|----------|
| F18 | Cycle layout forward |
| F17 | Cycle layout backwords |
| shift + F17 | Shrink main pane |
| shift + F18 | Expand main pane |
| ctrl + F18 | Increase main pane count |
| ctrl + F17 | Decrease main pane count |
| F14 (Scroll Lock) | Move focus counter clockwise |
| F15 (Pause) | Move focus clockwise |
| F13 (Print Screen) | Move focus to counter clockwise screen |
| shift + F13 (Print Screen) | Swap focused window to counter clockwise screen |
| shift + F14 (Scroll Lock) | Swap focused window counter clockwise |
| shift + F15 (Pause) | Swap focused window clockwise |
| F16 | Force windows to be reevaluated |

## Product Ids
| Id | Keyboad | Descrption|
|------|--------|-----------|
| 0x021d | Bakeneko 60 | Aluminum Mini Keyboard (ANSI) |
| "      | Ciel 60 | " |
| "      | D60 | " |
| 0x0220 | Prime_E | Aluminum Keyboard (ANSI) |
| 0x024F | QK65 | Aluminium Keyboard (ANSI)<br/>Keychron K seriese uses this ID |
| "      | FK680Pro V2 | " |

## References
- qmk_firmware
  - [git](https://github.com/qmk/qmk_firmware)
  - [docs](https://docs.qmk.fm/#/)
- [QMK Apple Fn Key](https://gist.github.com/fauxpark/010dcf5d6377c3a71ac98ce37414c6c4)
- Radial Controller
  - [Radial controller protocol implementation](https://docs.microsoft.com/en-us/windows-hardware/design/component-guidelines/radial-controller-protocol-implementation)
  - [zqlib](https://github.com/zhaqian12/qmk_firmware/blob/zhaqian/keyboards/zhaqian/readme.md)
- VIAL
  - [vial-qmk](https://github.com/vial-kb/vial-qmk)
  - [docs](https://get.vial.today/)
- [VIA](https://www.caniusevia.com/)
- [STM32F103xB UF2 bootloader](https://github.com/mmoskal/uf2-stm32f103)
  - [qmk-zq](https://github.com/zhaqian12/qmk_firmware)
  - [ZQ-Keyboards](https://github.com/zhaqian12/ZQ-Keyboard)
  - [zfrontier article](https://www.zfrontier.com/app/flow/eMzZjJZRgP6z)
- Amethyst
  - [git](https://github.com/ianyh/Amethyst)
  - [docs](https://ianyh.com/amethyst)
