const fs = require('fs/promises'),
  path = require('path')

module.exports = async function build(
  appKind,
  libDir,
  targetDir,
  outputFilePath
) {
  const options = Object.assign(
      await getMakeOptions(appKind, path.join(libDir, '..')),
      await getMakeOptions(appKind, targetDir)
    ),
    info = await getInfo(appKind, targetDir),
    via =
      appKind === 'vial'
        ? {
            name: info.keyboard_name
          }
        : {
            name: info.keyboard_name,
            vendorId: info.usb.vid,
            productId: info.usb.pid
          }

  // lighting
  via.lighting =
    options.RGBLIGHT_ENABLE === 'yes' || options.RGB_MATRIX_ENABLE === 'yes'
      ? { extends: 'none', keycodes: 'qmk' }
      : 'none'

  // matrix
  via.matrix = await getMatrix(appKind, targetDir)

  // layouts
  via.layouts = await getLayouts(appKind, targetDir)

  // customKeycodes
  via.customKeycodes = [
    ...(await getCustomKeycodes(appKind, libDir)),
    ...(await getCustomKeycodes(appKind, targetDir))
  ].filter((e) => {
    const opt = e.option
    e.option = undefined
    return !opt || options[opt] === 'yes'
  })

  await fs.writeFile(outputFilePath, JSON.stringify(via, undefined, 2))
}

async function getInfo(appKind, dir) {
  return await readJson(appKind, dir, 'info')
}

async function getLayouts(appKind, dir) {
  return await readJson(appKind, dir, 'layouts')
}

async function getMatrix(appKind, dir) {
  const src = await fs.readFile(path.join(dir, 'config.h'), 'utf-8')
  return {
    rows: parseInt(src.match(/^#\s*define\s+MATRIX_ROWS\s+(\d+)/m)[1]),
    cols: parseInt(src.match(/^#\s*define\s+MATRIX_COLS\s+(\d+)/m)[1])
  }
}

async function getMakeOptions(appKind, dir) {
  const src = await fs.readFile(path.join(dir, 'rules.mk'), 'utf-8')
  return [...src.matchAll(/^\s*(\w+)\s*=\s*(yes|no)/gm)].reduce(
    (opts, match) => {
      opts[match[1]] = match[2]
      return opts
    },
    {}
  )
}

async function getCustomKeycodes(appKind, dir) {
  let customKeycodes = await readJson(appKind, dir, 'custom_keycodes')
  return customKeycodes || []
}

async function readJson(appKind, dir, filename) {
  let filePath = path.join(dir, `${filename}_${appKind}.json`)
  if (await readable(filePath)) {
    return JSON.parse(await fs.readFile(filePath), 'utf-8')
  }
  filePath = path.join(dir, filename + '.json')
  if (await readable(filePath)) {
    return JSON.parse(await fs.readFile(filePath), 'utf-8')
  }
  return null
}

async function readable(filePath) {
  try {
    await fs.access(filePath, fs.constants.R_OK)
    return true
  } catch {
    return false
  }
}
