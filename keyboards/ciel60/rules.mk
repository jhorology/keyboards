# MCU name
MCU = atmega32u2

# Bootloader selection
BOOTLOADER = atmel-dfu

# reducing firmware size
ifeq ($(strip $(VIAL_ENABLE)), yes)
	KEY_OVERRIDE_ENABLE = no
endif
