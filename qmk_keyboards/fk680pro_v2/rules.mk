# Configure for 128K flash
MCU_LDSCRIPT = STM32F103xB

RGB_MATRIX_ENABLE = yes

# debounce
DEBOUNCE_TYPE = sym_eager_pk

# os detection debug
OS_FINGERPRINT_DEBUG_ENABLE = yes
OPT_DEFS += -DOS_FINGERPRINT_DEBUG_ENABLE
SEND_STRING_ENABLE = yes

# radial controller
RADIAL_CONTROLLER_ENABLE = yes
SRC += lib/radial_controller.c

# gcc optimization
OPT = 2
