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
    11: 'sub_action_release_threshold'
  }

module.exports = function (options, defines) {
  const NUM_PRESET_BANKS =
      (((defines.EC_NUM_PRESETS - 1) / PRESET_BANK_SIZE) | 0) + 1,
    EC_TOOLS_CHANNEL_ID = defines.EC_VIA_CUSTOM_CHANNEL_ID_START,
    EC_PRESET_CHANNEL_ID_START = defines.EC_VIA_CUSTOM_CHANNEL_ID_START + 1

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
              content: ['id_ec_bottoming_calibration', EC_TOOLS_CHANNEL_ID, 1]
            },
            {
              label:
                'Show Calibration Data *(In 3 sconds after click, It will send data as keystrokes.)',
              type: 'toggle',
              content: ['id_ec_show_calibration_data', EC_TOOLS_CHANNEL_ID, 2]
            },
            ...(!defines.EC_DEBUG
              ? []
              : [
                  {
                    label:
                      'Show Debug Data *(In 3 sconds after click, It will send data as keystrokes.)',
                    type: 'toggle',
                    content: ['id_ec_debug_send_config', EC_TOOLS_CHANNEL_ID, 3]
                  },
                  {
                    label: 'Bootloader Jump',
                    type: 'toggle',
                    content: ['id_ec_bootloader_jump', EC_TOOLS_CHANNEL_ID, 4]
                  }
                ])
          ]
        }
      ]
    },
    ...Array(NUM_PRESET_BANKS)
      .fill(0)
      .map((_, bank) => {
        return {
          label: 'EC Preset Bank ' + bank,
          content: Array(PRESET_BANK_SIZE)
            .fill(0)
            .map((_, i) =>
              createPresetMenu(
                defines,
                bank,
                i,
                bank * PRESET_BANK_SIZE + i,
                EC_PRESET_CHANNEL_ID_START + bank * PRESET_BANK_SIZE + i
              )
            )
        }
      })
  ]
}

function createPresetMenu(defines, bank, index, presetIndex, channelId) {
  const ref = (id) => `id_ec_preset_${presetIndex}_${PRESET_VALUE_IDs[id]}`,
    content = (id) => [ref(id), channelId, id],
    rangeMax = defines.EC_SCALE_RANGE,
    rangeMinPerc = defines.EC_SAFETY_MARGIN_PERC,
    rangeMin = ((rangeMax * rangeMinPerc) / 100) | 0
  return {
    label: `Preset ${bank}${index} ${
      emoji[bank % emoji.length][index % emoji[bank].length]
    }`,
    content: [
      {
        label: 'Actuation Mode',
        type: 'dropdown',
        options: [
          ['STATIC', 0],
          ['DYNAMIC', 1]
        ],
        content: content(1)
      },
      {
        showIf: `{${ref(1)}} == 0`,
        label: `Actuation Threshold (${rangeMinPerc}% | 100%)`,
        type: 'range',
        options: [rangeMin, rangeMax],
        content: content(2)
      },
      {
        showIf: `{${ref(1)}} == 1`,
        label: `Actuation Travel (${rangeMinPerc}% | 50%)`,
        type: 'range',
        options: [rangeMin, rangeMax >> 1],
        content: content(3)
      },
      {
        label: 'Release Mode',
        type: 'dropdown',
        options: [
          ['STATIC', 0],
          ['DYNAMIC', 1]
        ],
        content: content(4)
      },
      {
        showIf: `{${ref(4)}} == 0`,
        label: `Release Threshold (${rangeMinPerc}% | 100%)`,
        type: 'range',
        options: [rangeMin, rangeMax],
        content: content(5)
      },
      {
        showIf: `{${ref(4)}} == 1`,
        label: `Release Travel (${rangeMinPerc}% | 50%)`,
        type: 'range',
        options: [rangeMin, rangeMax >> 1],
        content: content(6)
      },
      {
        showIf: `{${ref(1)}} == 1 || {${ref(4)}} == 1`,
        label: `Deadzone Travel (${rangeMinPerc}% | 50%)`,
        type: 'range',
        options: [rangeMin, rangeMax >> 1],
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
        label: `Sub Action Actuation Threshold (${rangeMinPerc}% | 100%)`,
        type: 'range',
        options: [rangeMin, rangeMax],
        content: content(10)
      },
      {
        showIf: `{${ref(8)}} == 1`,
        label: `Sub Action Release Threshold (${rangeMinPerc}% | 100%)`,
        type: 'range',
        options: [rangeMin, rangeMax],
        content: content(11)
      }
    ]
  }
}
