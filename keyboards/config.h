
#define DEBOUNCE 5

#define LOCKING_SUPPORT_ENABLE
#define LOCKING_RESYNC_ENABLE

#if !defined(TAPPING_TERM_PER_KEY) && defined(TAP_DANCE_ENABLE)
#  define TAPPING_TERM_PER_KEY
#endif

/*
   polling rate
   see https://github.com/qmk/qmk_firmware/blob/master/docs/config_options.md
 */
#define USB_POLLING_INTERVAL_MS 1
