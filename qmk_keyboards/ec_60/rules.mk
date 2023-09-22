CUSTOM_MATRIX = lite
QUANTUM_LIB_SRC += analog.c
SRC += ec_config.c ec_switch_matrix.c

SEND_STRING_ENABLE = yes

# os detection debug
OS_FINGERPRINT_DEBUG_ENABLE = yes
OPT_DEFS += -DOS_FINGERPRINT_DEBUG_ENABLE
