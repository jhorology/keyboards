# MCU name
MCU = atmega32u4

# Bootloader selection
BOOTLOADER = atmel-dfu

ENCODER_ENABLE = yes        # Enable Encoder
ENCODER_MAP_ENABLE = yes

# radial controller
RADIAL_CONTROLLER_ENABLE = yes
# radial controller requires DEFERRED_EXEC_ENABLE
DEFERRED_EXEC_ENABLE = yes
SRC += lib/radial_controller.c
