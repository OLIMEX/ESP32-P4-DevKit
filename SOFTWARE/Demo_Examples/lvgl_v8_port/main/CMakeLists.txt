idf_component_register(
    SRCS "main.cpp" "lvgl_v8_port.cpp"
    INCLUDE_DIRS .
)

target_compile_options(${COMPONENT_LIB} PRIVATE -Wno-missing-field-initializers)

# Get the exact component name
idf_build_get_property(build_components BUILD_COMPONENTS)
foreach(COMPONENT ${build_components})
    # The following code is to set custom board configuration file  
    if(COMPONENT MATCHES "esp32_display_panel" OR COMPONENT MATCHES "espressif__esp32_display_panel")
        idf_component_get_property(DISPLAY_LIB ${COMPONENT} COMPONENT_LIB)
        if(CONFIG_BOARD_OLIMEX_ESP32_P4_DEVKIT)
            target_compile_definitions(${DISPLAY_LIB} PRIVATE ESP_PANEL_BOARD_USE_CUSTOM_FILE)
            target_compile_definitions(${DISPLAY_LIB} PRIVATE ESP_PANEL_BOARD_CUSTOM_FILE_PATH=../../../../../main/BOARD_OLIMEX_ESP32_P4_DEVKIT.h)
        endif()
    endif()

    # The following code is to avoid the error:
    # lvgl_v8_port/managed_components/lvgl__lvgl/demos/stress/lv_demo_stress.c:92:29: error: format '%d' expects argument of
    # type 'int', but argument 6 has type 'uint32_t' {aka 'long unsigned int'} [-Werror=format=]
    if(COMPONENT MATCHES "lvgl" OR COMPONENT MATCHES "lvgl__lvgl")
        idf_component_get_property(LVGL_LIB ${COMPONENT} COMPONENT_LIB)
        target_compile_options(${LVGL_LIB} PRIVATE "-Wno-format")
    endif()
endforeach()
