#!/usr/bin/env node
const fs = require('fs')
const path = require('path')
const qmk_home = process.env['QMK_HOME']
const make_targets = process.argv.slice(2)
const common_custom_keycodes_file = path.join(qmk_home, 'keyboards', 'my_keyboards', 'lib', 'custom_keycodes_via.json')

var custom_keycodes = []
if (fs.existsSync(common_custom_keycodes_file)) {
  custom_keycodes = JSON.parse(fs.readFileSync(common_custom_keycodes_file))
}

make_targets.forEach(target => {
  const target_dir = path.join(qmk_home, 'keyboards', target)
  const via = JSON.parse(fs.readFileSync(path.join(target_dir, 'via_template.json')))
  if (fs.existsSync(path.join(target_dir, 'custom_keycodes_via.json'))) {
    custom_keycodes = custom_keycodes.concat(
      JSON.parse(fs.readFileSync(path.join(target_dir, 'custom_keycodes_via.json'), 'utf8'))
    )
  }
  if (custom_keycodes) {
    via.customKeycodes = custom_keycodes
  }
  let fileName = fs.writeFileSync(
    path.join(__dirname, '..', 'dist', target.replace('my_keyboards/', '').replaceAll('/', '_') + '_via.json'),
    JSON.stringify(via, undefined, 2)
  )
})
