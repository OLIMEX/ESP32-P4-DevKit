Visual Studio Code 

ESP-IDF Extension for VS Code Installation
https://github.com/espressif/vscode-esp-idf-extension/blob/master/README.md

You can have more than one version of ESP-IDF.
Make sure that you have version 5.4.0 or later installed

File | Open Folder 
Pick a folder with the example

Confirm that you trust the authors of the files in this folder.

If you have more than one version of ESP-IDF select 5.4.0 by
ESP-IDF Explorer | Commands | Select current ESP-IDF version

Select target device by
ESP-IDF Explorer | Commands | Select Espressif Device Traget
	esp32p4
	ESP32-P4 chip via builtin USB-JTAG

Select port by
ESP-IDF Explorer | Commands | Select Port to Use
	<port where the board is connected> 
	something like /dev/tty/ACM0

Select flash method by
ESP-IDF Explorer | Commands | ESP-IDF: Select Flash Method
	UART

Build, Flash and Monitor
ESP-IDF Explorer | Commands | ESP-IDF: Build, Flash and Monitor


