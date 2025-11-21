### Custom ESPHOME component: OpenTherm Gateway 
This is a `custom_component` for ESPHome. It is mainly used in my creates OpenTherm Gateway. Feel free to use it as you require.
Check out my OpenTherm Gatway: https://github.com/dhoeben/esp32-opentherm.

Feel free to also use my thermostat I created, which uses a E-ink display and is also fully customizable. Also has his own PCB you can just order! https://github.com/dhoeben/esp32-thermostat. 

### How to use
Add this to your config file in ESPHome:
```yaml
external_components:
  - source: "github://dhoeben/custom_opentherm@stable"
    refresh: 24h
```

Currently only the stable tag is in use.