# These functions can be used to generate a LVGL image .c file from
# an image file.
function(generate_lcustom_logo
    input_file   # The image file
    output_file  # The generated lvgl image .c file
    image_name   # name of image structure
    color_format # color format of output image, choice of
                 # CF_ALPHA_1_BIT, CF_ALPHA_2_BIT, CF_ALPHA_4_BIT, CF_ALPHA_8_BIT,
                 # CF_INDEXED_1_BIT, CF_INDEXED_2_BIT, CF_INDEXED_4_BIT,
                 # CF_INDEXED_8_BIT, CF_RAW, CF_RAW_CHROMA, CF_RAW_ALPHA,
                 # CF_TRUE_COLOR, CF_TRUE_COLOR_ALPHA, CF_TRUE_COLOR_CHROMA, CF_RGB565A8
    )
  add_custom_command(
    OUTPUT ${output_file}
    COMMAND
    lv_img_conv
    --color-format
    --output-file ${output_file}
    --bindir ${CMAKE_BINARY_DIR}
    --width ${width}
    --height ${height}
    ${ARGN} # Extra arguments are passed to gen_cfb_font_header.py
    ${input_file}
    DEPENDS ${source_file}
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    )
endfunction()

function(generate_cfb_font_for_gen_target
    target          # The cmake target that depends on the generated file
    input_file      # The TrueType/OpenType font file or the image file
    output_file     # The generated header file
    width           # Width of the CFB font elements in pixels
    height          # Height of the CFB font elements in pixels
    gen_target      # The generated file target we depend on
                    # Any additional arguments are passed on to
                    # gen_cfb_font_header.py
    )
  generate_cfb_font(${input_file} ${output_file} ${width} ${height} ${ARGN})

  # Ensure 'output_file' is generated before 'target' by creating a
  # dependency between the two targets

  add_dependencies(${target} ${gen_target})
endfunction()

function(generate_custom_logo_for_target
    target          # The cmake target that depends on the generated file
    input_file      # The TrueType/OpenType font file or image file
    output_file     # The generated header file
    width           # Width of the CFB font elements in pixels
    height          # Height of the CFB font elements in pixels
                    # Any additional arguments are passed on to
                    # gen_cfb_font_header.py
    )
  # Ensure 'output_file' is generated before 'target' by creating a
  # 'custom_target' for it and setting up a dependency between the two
  # targets

  # But first create a unique name for the custom target
  generate_unique_target_name_from_filename(${output_file} generated_target_name)

  add_custom_target(${generated_target_name} DEPENDS ${output_file})
  generate_cfb_font_for_gen_target(${target} ${input_file} ${output_file}
    ${width} ${height} ${generated_target_name} ${ARGN})
endfunction()
