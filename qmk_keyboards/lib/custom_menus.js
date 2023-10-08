const _ = require('underscore'),
  ID_CUSTOM_MAGIC_CHANNEL = 5,
  ID_CUSTOM_RC_CHANNEL = 6,
  ID_CUSTOM_NON_MAC_FN_CHANNEL = 7,
  ID_CUSTOM_TD_CHANNEL_START = 8

const QMK_MAGIC_MENU = {
  label: 'QMK Magic',
  content: [
    {
      label: 'Settings',
      content: [
        {
          label: 'Swap Caps Lock and Left Control',
          type: 'toggle',
          content: [
            'id_custom_magic_swap_control_capslock',
            ID_CUSTOM_MAGIC_CHANNEL,
            1
          ]
        },
        {
          label: 'Swap Caps Lock and Escape',
          type: 'toggle',
          content: [
            'id_custom_magic_swap_escape_capslock',
            ID_CUSTOM_MAGIC_CHANNEL,
            2
          ]
        },
        {
          label: 'Treat Caps Lock as Control',
          type: 'toggle',
          content: [
            'id_custom_magic_capslock_to_control',
            ID_CUSTOM_MAGIC_CHANNEL,
            3
          ]
        },
        {
          label: 'Swap Left Control and GUI',
          type: 'toggle',
          content: [
            'id_custom_magic_swap_lctl_lgui',
            ID_CUSTOM_MAGIC_CHANNEL,
            4
          ]
        },
        {
          label: 'Swap Right Control and GUI',
          type: 'toggle',
          content: [
            'id_custom_magic_swap_rctl_rgui',
            ID_CUSTOM_MAGIC_CHANNEL,
            5
          ]
        },
        {
          label: 'Swap Left Alt and GUI',
          type: 'toggle',
          content: [
            'id_custom_magic_swap_lalt_lgui',
            ID_CUSTOM_MAGIC_CHANNEL,
            6
          ]
        },
        {
          label: 'Swap Right Alt and GUI',
          type: 'toggle',
          content: [
            'id_custom_magic_swap_ralt_rgui',
            ID_CUSTOM_MAGIC_CHANNEL,
            7
          ]
        },
        {
          label: 'Disable the GUI keys',
          type: 'toggle',
          content: ['id_custom_magic_no_gui', ID_CUSTOM_MAGIC_CHANNEL, 8]
        },
        {
          label: 'Swap ` and Escape',
          type: 'toggle',
          content: [
            'id_custom_magic_swap_grave_esc',
            ID_CUSTOM_MAGIC_CHANNEL,
            9
          ]
        },
        {
          label: 'Enable N-key rollover',
          type: 'toggle',
          content: ['id_custom_magic_host_nkro', ID_CUSTOM_MAGIC_CHANNEL, 10]
        },
        {
          label: 'Swap Backspace and Backslash',
          type: 'toggle',
          content: ['id_custom_magic_swap_bs_bsls', ID_CUSTOM_MAGIC_CHANNEL, 11]
        }
      ]
    }
  ]
}

const RADIAL_CONTROLLER_MENU = {
  label: 'Radial Controller',
  content: [
    {
      label: 'Settings',
      content: [
        {
          label: 'Encoder Clicks per Rotation',
          type: 'dropdown',
          options: [
            ['16', 16],
            ['18', 18],
            ['20', 20],
            ['30', 30],
            ['36', 36]
          ],
          content: ['id_custom_rc_encoder_clicks', ID_CUSTOM_RC_CHANNEL, 1]
        },
        {
          label: 'Angular Speed While Key Pressing: (15 - 270 deg/sec)',
          type: 'range',
          options: [0, 255],
          content: ['id_custom_rc_key_angular_speed', ID_CUSTOM_RC_CHANNEL, 2]
        },
        {
          label: 'Fine-tune Ratio',
          type: 'dropdown',
          options: [
            ['None', 0],
            ['1/2', 1],
            ['1/4', 2],
            ['1/8', 3]
          ],
          content: ['id_custom_rc_fine_tune_ratio', ID_CUSTOM_RC_CHANNEL, 3]
        },
        {
          showIf: '{id_custom_rc_fine_tune_ratio} != 0',
          label: 'Fine-tune Modifier: CTRL',
          type: 'toggle',
          content: ['id_custom_rc_fine_tune_mod_ctrl', ID_CUSTOM_RC_CHANNEL, 4]
        },
        {
          showIf: '{id_custom_rc_fine_tune_ratio} != 0',
          label: 'Fine-tune Modifier: SHIFT',
          type: 'toggle',
          content: ['id_custom_rc_fine_tune_mod_shift', ID_CUSTOM_RC_CHANNEL, 5]
        },
        {
          showIf: '{id_custom_rc_fine_tune_ratio} != 0',
          label: 'Fine-tune Modifier: ALT',
          type: 'toggle',
          content: ['id_custom_rc_fine_tune_mod_alt', ID_CUSTOM_RC_CHANNEL, 6]
        },
        {
          showIf: '{id_custom_rc_fine_tune_ratio} != 0',
          label: 'Fine-tune Modifier: GUI',
          type: 'toggle',
          content: ['id_custom_rc_fine_tune_mod_gui', ID_CUSTOM_RC_CHANNEL, 7]
        },
        {
          showIf: '{id_custom_rc_fine_tune_ratio} != 0',
          label: 'Fine-tune Modifier: fn🌐',
          type: 'toggle',
          content: [
            'id_custom_rc_fine_tune_mod_apple_fn',
            ID_CUSTOM_RC_CHANNEL,
            8
          ]
        }
      ]
    }
  ]
}

function createTapDanceMenu(size) {
  // id_field should be unique at all of content scope
  return {
    label: 'Tap Dance',
    content: _.range(size).map((i) => ({
      label:
        'TD' + i + ' - 0x57' + i.toString(16).toUpperCase().padStart(2, '0'),
      content: [
        {
          label: 'Single Tap',
          type: 'keycode',
          content: [
            `id_custom_td_${i}_single_tap`,
            ID_CUSTOM_TD_CHANNEL_START + i,
            1
          ]
        },
        {
          label: 'Hold',
          type: 'keycode',
          content: [
            `id_custom_td_${i}_single_hold`,
            ID_CUSTOM_TD_CHANNEL_START + i,
            2
          ]
        },
        {
          label: 'Double Tap',
          type: 'keycode',
          content: [
            `id_custom_td_${i}_multi_tap`,
            ID_CUSTOM_TD_CHANNEL_START + i,
            3
          ]
        },
        {
          label: 'Tap Hold',
          type: 'keycode',
          content: [
            `id_custom_td_${i}_tap_hold`,
            ID_CUSTOM_TD_CHANNEL_START + i,
            4
          ]
        },
        {
          label: 'Tapping Term: (50 - 1000 ms)',
          type: 'range',
          options: [50, 1000],
          content: [
            `id_custom_td_${i}_tapping_term`,
            ID_CUSTOM_TD_CHANNEL_START + i,
            5
          ]
        }
      ]
    }))
  }
}

function createNonMacFnMenu(hasDipSwitch) {
  return {
    label: 'Non-macos fn🌐',
    content: [
      {
        label: 'Settings',
        content: [
          {
            label:
              'Auto-detect mac/non-mac mode' +
              (hasDipSwitch ? ' *(dip-switch will not work)' : ''),
            type: 'toggle',
            content: [
              'id_custom_non_mac_fn_auto_detect',
              ID_CUSTOM_NON_MAC_FN_CHANNEL,
              1
            ]
          },
          {
            label: 'When mac mode is disable, fn🌐 key apply to F1-12 keys',
            type: 'toggle',
            content: [
              'id_custom_non_mac_fn_fkey',
              ID_CUSTOM_NON_MAC_FN_CHANNEL,
              2
            ]
          },
          {
            label: 'When mac mode is disable, fn🌐 key apply to alpha keys',
            type: 'toggle',
            content: [
              'id_custom_non_mac_fn_alpha',
              ID_CUSTOM_NON_MAC_FN_CHANNEL,
              3
            ]
          },
          {
            label: 'When mac mode is disabled, fn🌐 key apply to cursor keys',
            type: 'toggle',
            content: [
              'id_custom_non_mac_fn_cursor',
              ID_CUSTOM_NON_MAC_FN_CHANNEL,
              4
            ]
          },
          ...[
            // see https://manuals.info.apple.com/MANUALS/2000/MA2010/en_US/magic-keyboard-touchID-03404572-ug.pdf
            {
              key: 'F1',
              desc: 'Decrese brightness of display',
              depends: 'id_custom_non_mac_fn_fkey'
            },
            {
              key: 'F2',
              desc: 'Increase brightness of display',
              depends: 'id_custom_non_mac_fn_fkey'
            },
            {
              key: 'F3',
              desc: 'Open Mission Control',
              depends: 'id_custom_non_mac_fn_fkey'
            },
            {
              key: 'F4',
              desc: 'Open Spotlight',
              depends: 'id_custom_non_mac_fn_fkey'
            },
            {
              key: 'F5',
              desc: 'Activate dictation',
              depends: 'id_custom_non_mac_fn_fkey'
            },
            {
              key: 'F6',
              desc: 'Turn Do Not Disturb on or off',
              depends: 'id_custom_non_mac_fn_fkey'
            },
            {
              key: 'F7',
              desc: 'Media Track prev',
              depends: 'id_custom_non_mac_fn_fkey'
            },
            {
              key: 'F8',
              desc: 'Media Play/Pause',
              depends: 'id_custom_non_mac_fn_fkey'
            },
            {
              key: 'F9',
              desc: 'Media Track next',
              depends: 'id_custom_non_mac_fn_fkey'
            },
            {
              key: 'F10',
              desc: 'Mute sound',
              depends: 'id_custom_non_mac_fn_fkey'
            },
            {
              key: 'F11',
              desc: 'Decrease volume of sound',
              depends: 'id_custom_non_mac_fn_fkey'
            },
            {
              key: 'F12',
              desc: 'Increase volume of sound',
              depends: 'id_custom_non_mac_fn_fkey'
            },
            {
              key: 'Space',
              desc: 'Hey Siri',
              depends: 'id_custom_non_mac_fn_alpha'
            },
            {
              key: 'Q',
              desc: 'Quick Notes',
              depends: 'id_custom_non_mac_fn_alpha'
            },
            {
              key: 'E',
              desc: 'Emoji & Symbols',
              depends: 'id_custom_non_mac_fn_alpha'
            },
            {
              key: 'A',
              desc: 'Focus Dock',
              depends: 'id_custom_non_mac_fn_alpha'
            },
            {
              key: 'D',
              desc: 'Dictation',
              depends: 'id_custom_non_mac_fn_alpha'
            },
            {
              key: 'F',
              desc: 'Toggle Full Screen Mode',
              depends: 'id_custom_non_mac_fn_alpha'
            },
            {
              key: 'H',
              desc: 'Show Desktop',
              depends: 'id_custom_non_mac_fn_alpha'
            },
            {
              key: 'C',
              desc: 'Open Control Center',
              depends: 'id_custom_non_mac_fn_alpha'
            },
            {
              key: 'N',
              desc: 'Open Notification',
              depends: 'id_custom_non_mac_fn_alpha'
            },
            {
              key: 'M',
              desc: 'Focus Menubar',
              depends: 'id_custom_non_mac_fn_alpha'
            },
            {
              key: 'Backspace',
              desc: 'Forward delete',
              depends: 'id_custom_non_mac_fn_cursor'
            },
            {
              key: 'Up Arrow',
              desc: 'Scroll up one page',
              depends: 'id_custom_non_mac_fn_cursor'
            },
            {
              key: 'Down Arrow',
              desc: 'Scroll down one page',
              depends: 'id_custom_non_mac_fn_cursor'
            },
            {
              key: 'Left Arrow',
              desc: 'Scroll to the beginning of a document',
              depends: 'id_custom_non_mac_fn_cursor'
            },
            {
              key: 'Right Arrow',
              desc: 'Scroll to the end of a document',
              depends: 'id_custom_non_mac_fn_cursor'
            }
          ].map((e, i) => ({
            showIf: `{${e.depends}} == 1`,
            label: `fn + ${e.key}: ${e.desc}`,
            type: 'keycode',
            content: [
              `id_custom_non_mac_fn_${e.key
                .toLowerCase()
                .replaceAll(' ', '_')}`,
              ID_CUSTOM_NON_MAC_FN_CHANNEL,
              i + 5
            ]
          }))
        ]
      }
    ]
  }
}

module.exports = function (options, defines) {
  const customMenus = [QMK_MAGIC_MENU]
  if (defines.TAP_DANCE_ENTRIES) {
    customMenus.push(createTapDanceMenu(defines.TAP_DANCE_ENTRIES))
  }
  customMenus.push(createNonMacFnMenu(options.DIP_SWITCH_ENABLE === 'yes'))
  if (options.RADIAL_CONTROLLER_ENABLE === 'yes') {
    customMenus.push(RADIAL_CONTROLLER_MENU)
  }
  return customMenus
}
