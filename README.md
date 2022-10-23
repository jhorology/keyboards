# keyboards
This is a personal project for my own keyboards.

## Custom Keycodes
| Code | Descrption| Notes|
|------|-----------|--------
| RHID_TOGG | Toggle allow or deny the access of RAW HID. | - Non-volatile setting<br/> - When using VIA, it should be on.|
| RHID_ON | Allow the access of RAW HID. |"|
| RHID_OFF | Deny the access of RAW HID. |"|
| MAC_TOGG | Toggle enabling or disabling true apple mode with switching base layer 0(apple mode) or 1. | - Non-volatile setting<br/> - When using VIA, it should be off.|
| MAC_ON | Enablle true apple mode with switching base layer 0(apple mode) |"|
| MAC_OFF | Disable true apple mode with switching base layer 1 |"|
| APPLE_FN | Apple fn/🌐 key. |When NKRO is disabled, the key combinations of fn + F1-12 dosen't work as expected.|
| APPLE_FF | Apple fn/🌐 key for the keyboard that dosen't have F1-12 keys. F1-12 keys can be mapped on top row. When mac mode is off, It simulates mac fn functions. |"|
| EISU_KANA | Toggle send KC_LANG2(英数) or KC_LNG1(かな) ||
| USJ_TOGG | Toggle enabling or disabling 社畜PC mode that is available to use ANSI layout under JIS environment. |Non-volatile setting|
| USJ_ON | Enable 社畜PC mode |"|
| USJ_OFF | Disable 社畜PC mode |"|
| RC_BTN | Button located on radial controller ||
| RC_CCW| Counter clockwise rotation of the radial controller ||
| RC_CW| Clockwise rotation of the radial controller ||
| RC_FINE| Rotation speed of the radial controller becomes slow while key pressing ||
| RGB_TOGX | Toggle keylight on/off. While pressing shift key, toggle underglow on/off  | FK680ProV2 only |

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
|Shortcut|keymap|Descrption|
|--------|----------|----------|
| F18 | MO(2) + E | Cycle layout forward |
| F17 | MO(2) + W |Cycle layout backwords |
| shift + F17 | MO(2) + shift + W | Shrink main pane |
| shift + F18 | MO(2) + shift + E | Expand main pane |
| ctrl + F18 | MO(2) + ctrl + E | Increase main pane count |
| ctrl + F17 | MO(2) + ctrl + W | Decrease main pane count |
| F14 (Scroll Lock) | MO(2) + O | Move focus counter clockwise |
| F15 (Pause) | MO(2) + P | Move focus clockwise |
| F13 (Print Screen) | MO(2) + I | Move focus to counter clockwise screen |
| shift + F13 (Print Screen) | MO(2) + shift + I | Swap focused window to counter clockwise screen |
| shift + F14 (Scroll Lock) | MO(2) + shift + O | Swap focused window counter clockwise |
| shift + F15 (Pause) | MO(2) + shift + P | Swap focused window clockwise |
| F16 | MO(2) + Q | Force windows to be reevaluated |

## References
- qmk_firmware
  - [git](https://github.com/qmk/qmk_firmware)
  - [docs](https://docs.qmk.fm/#/)
- [QMK Apple Fn Key](https://gist.github.com/fauxpark/010dcf5d6377c3a71ac98ce37414c6c4)
- Radial Controller
  - [Radial controller protocol implementation](https://docs.microsoft.com/en-us/windows-hardware/design/component-guidelines/radial-controller-protocol-implementation)
  - [zqlib](https://github.com/zhaqian12/qmk_firmware/blob/zhaqian/keyboards/zhaqian/readme.md)
- [VIA](https://www.caniusevia.com/)
- [STM32F103xB UF2 bootloader](https://github.com/mmoskal/uf2-stm32f103)
  - [qmk-zq](https://github.com/zhaqian12/qmk_firmware)
  - [ZQ-Keyboards](https://github.com/zhaqian12/ZQ-Keyboard)
  - [zfrontier article](https://www.zfrontier.com/app/flow/eMzZjJZRgP6z)
- Amethyst
  - [git](https://github.com/ianyh/Amethyst)
  - [docs](https://ianyh.com/amethyst)
