CUSTOM_MATRIX = yes
ANALOG_DRIVER_REQUIRED = yes
SRC += ec_config.c ec_switch_matrix.c ec_auto_calibration.c

SEND_STRING_ENABLE = yes

# for EC debug
# EC_DEBUG_ENABLE = yes
# OPT_DEFS += -DEC_DEBUG_ENABLE

# os detection debug
# OS_FINGERPRINT_DEBUG_ENABLE = yes
# OPT_DEFS += -DOS_FINGERPRINT_DEBUG_ENABLE

# gcc optimization
OPT = 2
