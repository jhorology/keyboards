const emoji = require('./preset_emoji')

module.exports = function (options, defines) {
  const keycodes = []
  // not keycode, key preset
  for (let i = 0; i < defines.EC_NUM_PRESETS; i++) {
    keycodes.push({
      name: `EC${i}\n${emoji[i % emoji.length]}`,
      title: `EC Preset ${i}. It can be mapped in EC preset map layers.`,
      shortName: `${i} ${emoji[i % emoji.length]}`
    })
  }
  // select EC map
  for (let i = 0; i < defines.EC_NUM_PRESET_MAPS; i++) {
    keycodes.push({
      name: `EC\n Map ${i}`,
      title: `Select EC preset map ${i}.`,
      shortName: `ECM(${i})`
    })
  }
  keycodes.push({
    name: 'EC\nCalD',
    title: 'Show calibration data as keystrokes',
    shortName: 'EC.C'
  })
  keycodes.push({
    name: 'EC\nPSet',
    title: 'Show presets as keystrokes',
    shortName: 'EC.P'
  })
  keycodes.push({
    name: 'EC\nPMap',
    title: 'Show preset map as keystrokes',
    shortName: 'EC.M'
  })
  if (defines.EC_DEBUG) {
    keycodes.push({
      name: 'EC\nDebug',
      title: 'Show debug data as keystrokes',
      shortName: 'EC.D'
    })
  }
  return keycodes
}
