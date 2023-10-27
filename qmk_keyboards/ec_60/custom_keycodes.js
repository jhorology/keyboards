const emoji = require('./preset_emoji'),
  PRESET_BANK_SIZE = 8

const mapForEachIndex = (count, mapper) =>
  Array(count)
    .fill(0)
    .map((_, index) => mapper(index))

module.exports = function (options, defines) {
  const presetDesc = `It can be mapped in only EC preset map layers(${
      defines.DYNAMIC_KEYMAP_LAYER_COUNT - defines.EC_NUM_PRESET_MAPS
    } - ${defines.DYNAMIC_KEYMAP_LAYER_COUNT - 1}).`,
    ifdef = (key, array) => (options[key] === 'yes' ? array : [])

  return [
    {
      name: 'EC\nCalD',
      title: 'Show calibration data as keystrokes',
      shortName: 'EC.C'
    },
    {
      name: 'EC\nPSet',
      title: 'Show presets as keystrokes',
      shortName: 'EC.P'
    },
    // preset map selector keys
    ...mapForEachIndex(defines.EC_NUM_PRESET_MAPS, (i) => ({
      name: `EC\n Map ${i}`,
      title: `Select EC preset map ${i}.`,
      shortName: `ECM(${i})`
    })),
    // not keycode, preset
    ...mapForEachIndex(defines.EC_NUM_PRESETS, (presetIndex) => {
      const bank = (presetIndex / PRESET_BANK_SIZE) | 0,
        index = presetIndex % PRESET_BANK_SIZE,
        icon = emoji[bank % emoji.length][index % emoji[bank].length]
      return {
        name: `EC${bank}${index}\n${icon}`,
        title: `EC Preset Bank${bank} - ${index}. ${presetDesc}`,
        shortName: `${bank}${index} ${icon}`
      }
    }),
    ...ifdef('EC_DEBUG_ENABLE', [
      {
        name: 'EC\nDBG0',
        title: 'Show debug data as keystrokes',
        shortName: 'EC.D0'
      },
      {
        name: 'EC\nDBG1',
        title: 'Show calibration releated data as keystrokes',
        shortName: 'EC.D1'
      },
      {
        name: 'EC\nDBG0',
        title: 'Show current configuraion of matrix as keystrokes',
        shortName: 'EC.D2'
      }
    ])
  ]
}
