# MCU name
MCU = atmega32u4

# Bootloader selection
BOOTLOADER = atmel-dfu

# Build Options
#   change yes to no to disable
#
BOOTMAGIC_ENABLE = yes     # Enable Bootmagic Lite
MOUSEKEY_ENABLE = no       # Mouse keys
EXTRAKEY_ENABLE = yes      # Audio control and System control
CONSOLE_ENABLE = no        # Console for debug
COMMAND_ENABLE = no        # Commands for debug and configuration
AUDIO_ENABLE = no          # Audio output
LTO_ENABLE = yes           # Link time optimization
# Apple Fn/Globe key patch
# see https://gist.github.com/fauxpark/010dcf5d6377c3a71ac98ce37414c6c4
NKRO_ENABLE = no
APPLE_FN_ENABLE = yes

DEFAULT_FOLDER = primekb/prime_e/std
