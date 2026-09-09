set(public_include_dirs "./interface/include"
                        "./sw/include"
                        "./port/include/")
set(private_include_dirs  "./port/inc/"
                          "./sw/libs/tinyh264_inc"
                          "./sw/libs/openh264_inc"
                          "./sw/src/")
set(src_dirs "./port/src/"
             "./interface/src" 
             "./sw/src")
       
IF ("${IDF_VERSION_MAJOR}.${IDF_VERSION_MINOR}" VERSION_LESS "5.3" )

set(exclude_srcs "./port/src/esp_h264_alloc.c"
                 "./port/src/esp_h264_cache.c")

ELSE()

set(exclude_srcs "./port/src/esp_h264_alloc_less_than_5_3.c")
list(APPEND public_requires esp_mm)

ENDIF ()

IF (${IDF_TARGET} STREQUAL "esp32p4")

list(APPEND public_include_dirs "./hw/include")
list(APPEND private_include_dirs  "./hw/hal/esp32p4"
                                  "./hw/soc/esp32p4") 
list(APPEND src_dirs "./hw/src"
                     "./hw/hal/esp32p4" )

ENDIF ()

IF (${IDF_TARGET} STREQUAL "esp32s3") 

list(APPEND src_dirs "./sw/src/asm/${IDF_TARGET}")

ENDIF ()

idf_component_register(SRC_DIRS "${src_dirs}"
                       EXCLUDE_SRCS "${exclude_srcs}"
                       INCLUDE_DIRS "${public_include_dirs}"
                       PRIV_INCLUDE_DIRS "${private_include_dirs}"
                       REQUIRES "${public_requires}")

set(TARGET_LIB_NAME "${CMAKE_CURRENT_SOURCE_DIR}/sw/libs/${IDF_TARGET}/libtinyh264.a"  "${CMAKE_CURRENT_SOURCE_DIR}/sw/libs/${IDF_TARGET}/libopenh264.a")
target_link_libraries(${COMPONENT_TARGET} "-Wl,--start-group" ${TARGET_LIB_NAME} "-Wl,--end-group")

IF (${IDF_TARGET} STREQUAL "esp32s3") 

add_definitions(-DHAVE_ESP32S3 )

ENDIF ()
