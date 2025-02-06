find_program(LV_IMG_CONV_EXCUTABLE lv_img_conv REQUIRED HINTS ${ZEPHYR_BASE}/../node_modules/.bin)
function(generate_lv_image_for_target
    target
    input_file     # image file
    image_name     # name of image structure
    color_format   # color format of output image, choice of
                   # CF_ALPHA_1_BIT, CF_ALPHA_2_BIT, CF_ALPHA_4_BIT, CF_ALPHA_8_BIT,
                   # CF_INDEXED_1_BIT, CF_INDEXED_2_BIT, CF_INDEXED_4_BIT,
                   # CF_INDEXED_8_BIT, CF_RAW, CF_RAW_CHROMA, CF_RAW_ALPHA,
                   # CF_TRUE_COLOR, CF_TRUE_COLOR_ALPHA, CF_TRUE_COLOR_CHROMA, CF_RGB565A8
  )

  if(IS_ABSOLUTE ${input_file})
    get_filename_component(input_file ${input_file} ABSOLUTE)
  else()
    get_filename_component(input_file ${CMAKE_CURRENT_SOURCE_DIR}/${input_file} ABSOLUTE)
  endif()

  cmake_path(GET input_file STEM LAST_ONLY output_file)
  set(output_file ${CMAKE_CURRENT_BINARY_DIR}/${output_file}.c)

  # Ensure 'output_file' is generated before 'target' by creating a
  # 'custom_target' for it and setting up a dependency between the two
  # targets

  # But first create a unique name for the custom target
  generate_unique_target_name_from_filename(${output_file} gen_target)
  add_custom_target(${gen_target} DEPENDS ${output_file} SOURCES ${input_file})

  add_custom_command(
    OUTPUT ${output_file}
    DEPENDS ${input_file}
    COMMENT "Converting image (${input_file})"
    COMMAND
    ${LV_IMG_CONV_EXCUTABLE}
    --output-file ${output_file}
    --image-name ${image_name}
    --color-format ${color_format}
    --force
    ${ARGN} # Extra arguments are passed to lv_img_conv
    ${input_file}
  )
  add_dependencies(${target} ${gen_target})
  target_sources(${target} PRIVATE ${output_file})
endfunction()
