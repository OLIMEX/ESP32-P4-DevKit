# API Reference

## Header files

- [iperf_cmd.h](#file-iperf_cmdh)

## File iperf_cmd.h






## Functions

| Type | Name |
| ---: | :--- |
|  esp\_err\_t | [**iperf\_cmd\_register\_iperf**](#function-iperf_cmd_register_iperf) (void) <br>_Registers console commands: iperf._ |

## Macros

| Type | Name |
| ---: | :--- |
| define  | [**app\_register\_iperf\_commands**](#define-app_register_iperf_commands)  iperf\_cmd\_register\_iperf<br> |


## Functions Documentation

### function `iperf_cmd_register_iperf`

_Registers console commands: iperf._
```c
esp_err_t iperf_cmd_register_iperf (
    void
) 
```


**Returns:**

ESP\_OK on success

## Macros Documentation

### define `app_register_iperf_commands`

```c
#define app_register_iperf_commands iperf_cmd_register_iperf
```


