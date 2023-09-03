const colors = ['🟩', '🟨', '🟧', '🟥']

module.exports = function (options, defines) {
  const keycodes = []
  for (let i = 0; i < defines.EC_NUM_PRESETS; i++) {
    keycodes.push({
      name: `EC${i}\n${colors[i % colors.length]}`,
      title: `EC Preset ${i}. It can be mapped in only final layer.`,
      shortName: `${colors[i % colors.length]}${i}`
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
