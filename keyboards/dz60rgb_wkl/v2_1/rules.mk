# MCU name
MCU = atmega32u4

# Bootloader selection
BOOTLOADER = lufa-ms
BOOTLOADER_SIZE = 6144

# Build Options
#   change yes to no to disable
#
BOOTMAGIC_ENABLE = yes         # Enable Bootmagic Lite
MOUSEKEY_ENABLE = yes          # Mouse keys
EXTRAKEY_ENABLE = yes          # Audio control and System control
CONSOLE_ENABLE = no            # Console for debug
COMMAND_ENABLE = no            # Commands for debug and configuration
BACKLIGHT_ENABLE = no          # Enable keyboard backlight functionality
RGBLIGHT_ENABLE = no           # Enable keyboard RGB underglow
AUDIO_ENABLE = no              # Audio output
RGB_MATRIX_ENABLE = yes        # Use RGB matrix
RGB_MATRIX_DRIVER = IS31FL3733
LTO_ENABLE = yes               # Link time optimization
# Apple Fn/Globe key patch
# see https://gist.github.com/fauxpark/010dcf5d6377c3a71ac98ce37414c6c4
NKRO_ENABLE = no
APPLE_FN_ENABLE = yes

SRC += lib/apple_fn.c
