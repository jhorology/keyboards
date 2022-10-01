const  fs = require('fs/promises'),
  path = require('path')

module.exports = async function build(
  appKind,
  libDir,
  targetDir,
  outputFilePath
) {
  const info = await getInfo(appKind, targetDir),
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

  // TODO lighting
  via.lighting = 'none'

  // matrix
  via.matrix = await getMatrix(appKind, targetDir)

  // layouts
  via.layouts = await getLayouts(appKind, targetDir)

  // customKeycodes
  via.customKeycodes = await getCustomKeycodes(appKind, libDir)
  Array.prototype.push.apply(
    via.customKeycodes,
    await getCustomKeycodes(appKind, targetDir)
  )
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
    rows: parseInt(src.match(/#\s*define\s+MATRIX_ROWS\s+(\d+)/)[1]),
    cols: parseInt(src.match(/#\s*define\s+MATRIX_COLS\s+(\d+)/)[1])
  }
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
  } catch  {
    return false
  }
}
