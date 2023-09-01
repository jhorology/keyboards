module.exports = function (options, defines) {
  const keycodes = []
  for (let i = 0; i < defines.EC_NUM_PRESETS; i++) {
    keycodes.push({
      name: `EC${i}`,
      title: `EC Preset ${i}. It can be mapped in only final layer.`,
      shortName: `EC${i}`
    })
  }
  keycodes.push({
    name: 'EC\nSend',
    title: 'Show calibration data as keystrokes',
    shortName: 'EC.S'
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
