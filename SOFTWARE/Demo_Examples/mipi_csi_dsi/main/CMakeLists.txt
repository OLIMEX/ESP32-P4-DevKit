idf_component_register(
    SRCS "main.cpp"
    INCLUDE_DIRS "."
)

# Get the all components
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
endforeach()
