# Build Options
#   change yes to no to disable.
#
DIP_SWITCH_ENABLE = yes
RGB_MATRIX_ENABLE = yes
RGB_MATRIX_DRIVER = ckled2001
EEPROM_DRIVER = wear_leveling
WEAR_LEVELING_DRIVER = embedded_flash

# os detection debug
OS_FINGERPRINT_DEBUG_ENABLE = yes
OPT_DEFS += -DOS_FINGERPRINT_DEBUG_ENABLE
SEND_STRING_ENABLE = yes

# gcc optimization
OPT = 2
