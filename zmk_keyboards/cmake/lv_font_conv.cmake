find_program(LV_FONT_CONV_EXCUTABLE lv_font_conv REQUIRED HINTS ${ZEPHYR_BASE}/../node_modules/.bin)

function(generate_lv_font_for_target
    target
    font_file      # Source font path
    font_name      # name of image structure
    bpp            # bits per pixel
    size           # output font size (pixels)
  )

  if(IS_ABSOLUTE ${font_file})
    get_filename_component(font_file ${font_file} ABSOLUTE)
  else()
    get_filename_component(font_file ${CMAKE_CURRENT_SOURCE_DIR}/${font_file} ABSOLUTE)
  endif()

  set(output_file ${CMAKE_CURRENT_BINARY_DIR}/${font_name}.c)

  # Ensure 'output_file' is generated before 'target' by creating a
  # 'custom_target' for it and setting up a dependency between the two
  # targets

  # But first create a unique name for the custom target
  generate_unique_target_name_from_filename(${output_file} gen_target)
  add_custom_target(${gen_target} DEPENDS ${output_file} SOURCES ${font_file})

  add_custom_command(
    OUTPUT ${output_file}
    DEPENDS ${font_file}
    COMMENT "Converting font (${font_file})"
    COMMAND
    ${LV_FONT_CONV_EXCUTABLE}
    --font ${font_file}
    --output ${output_file}
    --bpp ${bpp}
    --size ${size}
    --format lvgl
    --lv-include lvgl.h
    ${ARGN} # Extra arguments are passed to lv_font_conv
  )
  add_dependencies(${target} ${gen_target})
  target_sources(${target} PRIVATE ${output_file})
endfunction()
