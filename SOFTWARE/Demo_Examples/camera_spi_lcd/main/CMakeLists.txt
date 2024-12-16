idf_component_register(SRCS "camera_spi_lcd_main.c"
                       INCLUDE_DIRS "."
                       REQUIRES esp_mm esp_driver_isp esp_driver_cam esp_driver_i2c esp_driver_ppa dsi_init sensor_init
                       )
