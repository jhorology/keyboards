#!/usr/bin/env node

const path = require('path'),
  fs = require('fs/promises'),
  QMK_HOME = process.env['QMK_HOME'],
  VIA_APP_HOME = process.env['VIA_APP_HOME'],
  VIA_VERSION = parseInt(process.env['VIA_VERSION']),
  MAKE_TARGETS = process.argv.slice(2),
  KEYBOARDS_DIR = path.join(QMK_HOME, 'keyboards', 'my_keyboards'),
  LIB_DIR = path.join(KEYBOARDS_DIR, 'lib')

async function build(targetDir, outputFilePath) {
  const options = await getMakeOptions(targetDir),
    defines = await getDefineValues(targetDir),
    info = await getInfo(targetDir),
    via = {
      name: info.keyboard_name,
      vendorId: info.usb.vid,
      productId: info.usb.pid
    }

  if (VIA_VERSION === 2) {
    // lighting
    via.lighting =
      options.BACKLIGHT_ENABLE === 'yes' && options.RGBLIGHT_ENABLE === 'yes'
        ? 'qmk_backlight_rgblight'
        : options.BACKLIGHT_ENABLE === 'yes'
        ? 'qmk_backlight'
        : options.RGBLIGHT_ENABLE === 'yes'
        ? 'qmk_rgblight'
        : options.RGB_MATRIX_ENABLE === 'yes'
        ? { extends: 'none', keycodes: 'qmk' }
        : 'none'
  } else if (VIA_VERSION === 3) {
    // https://www.caniusevia.com/docs/v3_changes
    via.firmwareVersion = parseInt(defines.VIA_FIRMWARE_VERSION)
    const lighting =
      options.BACKLIGHT_ENABLE === 'yes' && options.RGBLIGHT_ENABLE === 'yes'
        ? 'qmk_backlight_rgblight'
        : options.BACKLIGHT_ENABLE === 'yes'
        ? 'qmk_backlight'
        : options.RGBLIGHT_ENABLE === 'yes'
        ? 'qmk_rgblight'
        : options.RGB_MATRIX_ENABLE === 'yes'
        ? 'qmk_rgb_matrix'
        : undefined
    if (lighting) {
      // TODO custom configuration
      via.menus = [lighting]
      via.keycodes = ['qmk_lighting']
    }
  }
  // matrix
  via.matrix = info.matrix_pins
    ? {
        rows: info.matrix_pins.rows.length,
        cols: info.matrix_pins.cols.length
      }
    : { rows: defines.MATRIX_COLS, cols: defines.MATRIX_ROWS }

  // layouts
  via.layouts = await getLayouts(targetDir)

  // customKeycodes
  via.customKeycodes = (await getCustomKeycodes(targetDir)).filter((e) => {
    const opt = e.option
    e.option = undefined
    return !opt || options[opt] === 'yes'
  })

  await fs.writeFile(outputFilePath, JSON.stringify(via, undefined, 2))
}

async function getInfo(targetDir) {
  return await readJson(path.join(targetDir, 'info.json'), undefined)
}

async function getLayouts(targetDir) {
  return await readJson(path.join(targetDir, 'layouts.json'), undefined)
}

async function getDefineValues(targetDir) {
  const regexp = /^\s*#\s*define\s+(\w+)\s+(\d+)/gm
  return [
    ...(
      await fs.readFile(path.join(KEYBOARDS_DIR, 'config.h'), 'utf-8')
    ).matchAll(regexp),
    ...(await fs.readFile(path.join(targetDir, 'config.h'), 'utf-8')).matchAll(
      regexp
    ),
    ...(
      await fs.readFile(path.join(targetDir, 'secure_config.h'), 'utf-8')
    ).matchAll(regexp)
  ].reduce((obj, match) => {
    obj[match[1]] = parseInt(match[2])
    return obj
  }, {})
}

async function getMakeOptions(targetDir) {
  const regexp = /^\s*(\w+)\s*=\s*(yes|no)/gm
  return [
    ...(
      await fs.readFile(path.join(KEYBOARDS_DIR, 'rules.mk'), 'utf-8')
    ).matchAll(regexp),
    ...(await fs.readFile(path.join(targetDir, 'rules.mk'), 'utf-8')).matchAll(
      regexp
    )
  ].reduce((opts, match) => {
    opts[match[1]] = match[2]
    return opts
  }, {})
}

async function getCustomKeycodes(targetDir) {
  return [
    ...(await readJson(path.join(LIB_DIR, 'custom_keycodes.json'), [])),
    ...(await readJson(path.join(targetDir, 'custom_keycodes.json'), []))
  ]
}

async function readJson(filePath, emptyValue) {
  if (await readable(filePath)) {
    return JSON.parse(await fs.readFile(filePath), 'utf-8')
  }
  return emptyValue
}

async function readable(filePath) {
  try {
    await fs.access(filePath, fs.constants.R_OK)
    return true
  } catch {
    return false
  }
}

Promise.all(
  MAKE_TARGETS.map((target) =>
    build(
      path.join(QMK_HOME, 'keyboards', target),
      path.join(
        __dirname,
        '..',
        'dist',
        target.replace('my_keyboards/', '').replaceAll('/', '_') +
          `_via_v${VIA_VERSION}.json`
      )
    )
  )
).catch((err) => {
  console.error(err)
  process.exit(1)
})
