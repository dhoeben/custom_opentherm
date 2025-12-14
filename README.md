# Custom OpenTherm Component 
[![GitHub release](https://img.shields.io/github/v/release/dhoeben/custom_opentherm.svg)](https://GitHub.com/dhoeben/custom_opentherm/releases/) [![ESPHome version](https://img.shields.io/badge/Based_on_ESPHome-v2025.12.1-blue)](https://GitHub.com/esphome/esphome/releases/)

This is a `custom_component` for ESPHome. It is mainly used in my OpenTherm Gateway. Feel free to use it as you require.

### Other projects
[OpenTherm Gatway](https://github.com/dhoeben/esp32-opentherm)
[Themostat](https://github.com/dhoeben/esp32-thermostat)

Feel free to also use my thermostat I created, which uses a E-ink display and is also fully customizable. Also has his own PCB you can just order! https://github.com/dhoeben/esp32-thermostat. 

### How to use
Add this to your config file in ESPHome:
```yaml
external_components:
  - source: "github://dhoeben/custom_opentherm@stable"
    refresh: 24h

custom_opentherm:
```

### Configuration
| Option | Required | Value (Example) | Description |
| :--- | :---: | :--- | :--- |
| **`id`** | no | `otgw` | Unique ID for the OpenTherm Gateway component. |
| **`in_pin`** | yes | `!secret pin_ot_rx` | GPIO pin connected to the OpenTherm RX (receive). |
| **`out_pin`** | yes | `!secret pin_ot_tx` | GPIO pin connected to the OpenTherm TX (transmit). |
| **`poll_interval`** | yes | `!secret poll_interval` | How often the gateway should poll for data. |
| **`rx_timeout`** | yes | `!secret rx_timeout` | Maximum wait time for a response before timing out. |
| **`debug`** | no | `true` | Enables detailed debug logging. |
| **`boiler_temp`** | yes | `boiler_temp` | Sensor ID for the boiler water temperature. |
| **`return_temp`** | yes | `return_temp` | Sensor ID for the return water temperature. |
| **`modulation`** | yes | `modulation` | Sensor ID for the current modulation level. |
| **`setpoint`** | yes | `setpoint` | Sensor ID for the active setpoint (target temperature). |


Currently only the stable tag is in use.