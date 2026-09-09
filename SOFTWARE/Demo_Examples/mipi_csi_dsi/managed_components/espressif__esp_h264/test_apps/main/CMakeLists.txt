set(srcs
 "test_app_main.c"
 "esp_h264_sw_enc_test.c"
 "esp_h264_sw_dec_test.c"
 "h264_io.c"
 "test_case.c")

IF (${IDF_TARGET} STREQUAL "esp32p4") 
set(srcs
 "test_app_main.c"
 "esp_h264_hw_enc_test.c"
 "esp_h264_sw_enc_test.c"
 "esp_h264_sw_dec_test.c"
 "h264_io.c"
 "test_case.c")
ENDIF ()

set(priv_requires
        esp_h264
        # general
        unity
        esp_psram
)

idf_component_register(SRCS ${srcs}
                       INCLUDE_DIRS "."
                       PRIV_REQUIRES ${priv_requires}
                       REQUIRES
                       WHOLE_ARCHIVE TRUE)

