const emoji = require('./preset_emoji'),
  PRESET_BANK_SIZE = 8,
  PRESET_VALUE_IDs = {
    1: 'actuation_mode',
    2: 'actuation_threshold',
    3: 'actuation_travel',
    4: 'release_mode',
    5: 'release_threshold',
    6: 'release_travel',
    7: 'deadzone',
    8: 'sub_action_enable',
    9: 'sub_action_keycode',
    10: 'sub_action_actuation_threshold',
    11: 'sub_action_release_mode',
    12: 'sub_action_release_threshold'
  }

const mapForEachIndex = (count, mapper) =>
  Array(count)
    .fill(0)
    .map((_, index) => mapper(index))

module.exports = function (options, defines) {
  const NUM_PRESET_BANKS = (((defines.EC_NUM_PRESETS - 1) / PRESET_BANK_SIZE) | 0) + 1,
    EC_TOOLS_CHANNEL_ID = defines.EC_VIA_CUSTOM_CHANNEL_ID_START,
    EC_PRESET_CHANNEL_ID_START = defines.EC_VIA_CUSTOM_CHANNEL_ID_START + 1,
    ifdef = (key, array) => (options[key] === 'yes' ? array : [])

  return [
    {
      label: 'EC Tools',
      content: [
        {
          label: 'EC Tools',
          content: [
            {
              label: 'Bottoming Calibration',
              type: 'toggle',
              content: ['id_ec_tools_bottoming_calibration', EC_TOOLS_CHANNEL_ID, 1]
            },
            {
              label:
                'Show Calibration Data *(In 3 sconds after click, It will send data as keystrokes.)',
              type: 'toggle',
              content: ['id_ec_tools_show_calibration_data', EC_TOOLS_CHANNEL_ID, 2]
            },
            ...ifdef('EC_DEBUG_ENABLE', [
              {
                label: 'Keyscan Test',
                type: 'toggle',
                content: ['id_ec_tools_matrix_scan_test', EC_TOOLS_CHANNEL_ID, 3]
              },
              {
                label:
                  'Show Debug Data *(In 3 sconds after click, It will send data as keystrokes.)',
                type: 'toggle',
                content: ['id_ec_tools_debug_send_config', EC_TOOLS_CHANNEL_ID, 4]
              },
              {
                label: 'Bootloader Jump',
                type: 'toggle',
                content: ['id_ec_tools_bootloader_jump', EC_TOOLS_CHANNEL_ID, 5]
              }
            ])
          ]
        }
      ]
    },
    ...mapForEachIndex(NUM_PRESET_BANKS, (bank) => ({
      label: 'EC Preset Bank ' + bank,
      content: mapForEachIndex(PRESET_BANK_SIZE, (i) =>
        createPresetMenu(
          defines,
          bank,
          i,
          bank * PRESET_BANK_SIZE + i,
          EC_PRESET_CHANNEL_ID_START + bank * PRESET_BANK_SIZE + i
        )
      )
    }))
  ]
}

function createPresetMenu(defines, bank, index, presetIndex, channelId) {
  const ref = (id) => `id_ec_preset_${presetIndex}_${PRESET_VALUE_IDs[id]}`,
    content = (id) => [ref(id), channelId, id],
    perc2rabngeValue = (perc) => ((defines.EC_SCALE_RANGE * perc) / 100) | 0,
    range = (name) => ({
      label: `${name} (${defines.EC_SAFETY_MARGIN_PERC}% | ${
        100 - defines.EC_SAFETY_MARGIN_PERC
      } %)`,
      type: 'range',
      options: [
        perc2rabngeValue(defines.EC_SAFETY_MARGIN_PERC),
        perc2rabngeValue(100 - defines.EC_SAFETY_MARGIN_PERC)
      ]
    }),
    halfRange = (name) => ({
      label: `${name} (${defines.EC_SAFETY_MARGIN_PERC}% | 50 %)`,
      type: 'range',
      options: [perc2rabngeValue(defines.EC_SAFETY_MARGIN_PERC), perc2rabngeValue(50)]
    })
  return {
    label: `Preset ${bank}${index} ${emoji[bank % emoji.length][index % emoji[bank].length]}`,
    content: [
      {
        label: 'Actuation Mode',
        type: 'dropdown',
        options: [
          ['Edge to Bottom', 0],
          ['Level', 1],
          ['Dynamic Travel', 2]
        ],
        content: content(1)
      },
      {
        showIf: `{${ref(1)}} == 0 || {${ref(1)}} == 1`,
        ...range('Actuation Threshold'),
        content: content(2)
      },
      {
        showIf: `{${ref(1)}} == 2`,
        ...halfRange('Actuation Travel'),
        content: content(3)
      },
      {
        label: 'Release Mode',
        type: 'dropdown',
        options: [
          ['Edge to Top', 0],
          ['Level', 1],
          ['Dynamic Travel', 2]
        ],
        content: content(4)
      },
      {
        showIf: `{${ref(4)}} == 0 || {${ref(4)}} == 1`,
        ...range('Release Threshold'),
        content: content(5)
      },
      {
        showIf: `{${ref(4)}} == 2`,
        ...halfRange('Release Travel'),
        content: content(6)
      },
      {
        ...halfRange('Deadzone'),
        content: content(7)
      },
      {
        label: 'Sub Action (*exprimental)',
        type: 'toggle',
        content: content(8)
      },
      {
        showIf: `{${ref(8)}} == 1`,
        label: 'Sub Action Keycode',
        type: 'keycode',
        content: content(9)
      },
      {
        showIf: `{${ref(8)}} == 1`,
        ...range('Sub Action Actuation Threshold'),
        content: content(10)
      },
      {
        showIf: `{${ref(8)}} == 1`,
        label: `Sub Action Release Mode`,
        type: 'dropdown',
        options: [
          ['Sync Primary', 0],
          ['Use Threshold', 1]
        ],
        content: content(11)
      },
      {
        showIf: `{${ref(8)}} == 1 && {${ref(11)}} == 1`,
        ...range('Sub Action Release Threshold'),
        content: content(12)
      }
    ]
  }
}
