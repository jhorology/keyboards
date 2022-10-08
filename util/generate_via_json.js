#!/usr/bin/env node

const path = require('path'),
  fs = require('fs/promises'),
  {
    keyboardDefinitionV2ToVIADefinitionV2,
    isVIADefinitionV2,
    isKeyboardDefinitionV2,
    keyboardDefinitionV3ToVIADefinitionV3,
    isVIADefinitionV3,
    isKeyboardDefinitionV3
  } = require('via-reader'),
  VIA_VERSION = parseInt(process.env['VIA_VERSION']),
  MAKE_TARGETS = process.argv.slice(2),
  PROJECT_DIR = path.join(__dirname, '..'),
  OUTPUT_DIR = process.env['OUTPUT_DIR'] || path.join(PROJECT_DIR, 'dist'),
  KEYBOARDS_DIR = path.join(PROJECT_DIR, 'qmk_keyboards'),
  LIB_DIR = path.join(KEYBOARDS_DIR, 'lib')

async function build(target) {
  const targetDir = path.join(KEYBOARDS_DIR, target),
    options = await getMakeOptions(targetDir),
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
    : { rows: defines.MATRIX_ROWS, cols: defines.MATRIX_COLS }

  // layouts
  via.layouts = await getLayouts(targetDir)

  // customKeycodes
  via.customKeycodes = (await getCustomKeycodes(targetDir)).filter((e) => {
    const opt = e.option
    e.option = undefined
    return !opt || options[opt] === 'yes'
  })

  // validate
  validate(target, via)

  // output JSON

  await fs.writeFile(
    path.join(OUTPUT_DIR, `${target}_via_v${VIA_VERSION}.json`),
    JSON.stringify(via, null, 2)
  )
  // alternate device ID
  if (defines.ALTERNATE_VENDOR_ID && defines.ALTERNATE_PRODUCT_ID) {
    via.vendorId =
      '0x' + defines.ALTERNATE_VENDOR_ID.toString(16).padStart(4, '0')
    via.productId =
      '0x' + defines.ALTERNATE_PRODUCT_ID.toString(16).padStart(4, '0')
    await fs.writeFile(
      path.join(OUTPUT_DIR, `${target}_via_v${VIA_VERSION}_alt.json`),
      JSON.stringify(via, null, 2)
    )
  }
}

async function getInfo(targetDir) {
  return await readJson(path.join(targetDir, 'info.json'), undefined)
}

async function getLayouts(targetDir) {
  return await readJson(path.join(targetDir, 'layouts.json'), undefined)
}

async function getDefineValues(targetDir) {
  const regexp = /^\s*#\s*define\s+(\w+)\s+(0x[0-9a-fA-F]+|[0-9]+)\b/gm,
    hasSecureConfig = readable(path.join(targetDir, 'secure_config.h'))
  return [
    ...(
      await fs.readFile(path.join(KEYBOARDS_DIR, 'config.h'), 'utf-8')
    ).matchAll(regexp),
    ...(await fs.readFile(path.join(targetDir, 'config.h'), 'utf-8')).matchAll(
      regexp
    ),
    ...(hasSecureConfig
      ? (
          await fs.readFile(path.join(targetDir, 'secure_config.h'), 'utf-8')
        ).matchAll(regexp)
      : [])
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

function validate(target, via) {
  const validator = [
    undefined,
    undefined,
    isKeyboardDefinitionV2,
    isKeyboardDefinitionV3
  ][VIA_VERSION]
  if (!validator) {
    throw new Error(
      `via_json_generator: keyboard: ${target} - unsuppoted VIA version: ${VIA_VERSION}`
    )
  }
  if (!validator(via)) {
    throw new Error(
      `via_json_generator: keyboard: ${target} - VIA v${VIA_VERSION} validation fails with errors:\n ${JSON.stringify(
        validator.errors,
        null,
        2
      )}`
    )
  }
}

Promise.all(
  MAKE_TARGETS.map((target) =>
    build(target.replace(/^my_keyboards\//, '').replaceAll('/', '_'))
  )
).catch((err) => {
  console.error(err)
  process.exit(1)
})
