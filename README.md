# SpeckNC

Custom FluidNC fork for SpeckNC hardware and WebUI workflow.

## What Was Added

- Custom WebUI dashboard and configuration workflow in `web/index.html`
- GUI-to-YAML conversion and upload to `/config.yaml`
- Global hardware pin mapping object (`HARDWARE_PIN_MAP`) for header/axis pin assignment
- Numeric axis header IDs (`axis1..axis4`) with legacy migration (`axis-x/y/z/a` -> numeric)
- Live analog voltage endpoint from `user_outputs.analog0_pin` and `analog1_pin`
- Configurable status widgets UI (add/edit/reorder/delete, live or manual values)
- Websocket robustness improvements (Blob message decode, reconnect handling)
- Editor fallback when ACE CDN is unavailable
- SD-first board image loading (`/sd/render.png`) with flash fallback (`render.png`)
- ESP32-S3 default build + 16MB partition setup

## Firmware Additions

File: `FluidNC/src/WebUI/WebCommands.cpp`

- `showUserOutputVoltages(...)` (ESP430)
  - Reads:
    - `config->_userOutputs->_analogOutput[0].readAnalogMV()`
    - `config->_userOutputs->_analogOutput[1].readAnalogMV()`
  - Supports JSON output:
    - `analog0_v`
    - `analog1_v`
- Web command registration:
  - `ESP430` -> `"UserOutputs/Voltages"`

## ADS1115 I2C ADC Integration

ADS1115 support is present across parser, extenders, and pin implementation:

- Pin parser support in `FluidNC/src/Pin.cpp`
  - Pin syntax: `ads1115_[0-3].[0-3]`
  - Example: `ads1115_0.1`
- ADC device implementation in:
  - `FluidNC/src/Extenders/ADS1115.h`
  - `FluidNC/src/Extenders/ADS1115.cpp`
- Analog pin wrapper in:
  - `FluidNC/src/Pins/AnalogPinDetail.h`
  - `FluidNC/src/Pins/AnalogPinDetail.cpp`
- Extender registration in:
  - `FluidNC/src/Extenders/Extenders.cpp`
  - Available config sections: `ads1115_0` .. `ads1115_3`

Supported ADS1115 config keys (per device):

- `address`
- `pga`
- `data_rate`
- `continuous_mode`

Example YAML snippet:

```yaml
i2c:
  sda_pin: gpio.8
  scl_pin: gpio.9
  frequency: 100000

extenders:
  ads1115_0:
    address: 0x48
    pga: 2
    data_rate: 4
    continuous_mode: false

user_outputs:
  analog0_pin: ads1115_0.0
  analog1_pin: ads1115_0.1
```

## WebUI Additions (Key Functions)

File: `web/index.html`

- YAML/config pipeline:
  - `buildConfigYamlFromGui()`
  - `updateConfigYaml()`
  - `mapInputToAxisLimit()`
  - `applyAxisSettings()`
  - `objectToYaml()`
  - `yamlScalar()`
- Status system:
  - `refreshUserOutputVoltages()`
  - `refreshRealtimeStatus()`
  - `updateTelemetry()`
  - `renderStatusWidgets()`
  - `renderStatusWidgetList()`
  - `openStatusEditor()`
  - `initializeStatusWidgetEditor()`
  - `addStatusWidget()`
  - `moveStatusWidget()`
  - `deleteStatusWidget()`
  - `resetStatusWidgetsToDefault()`
- Communication/reliability:
  - `parseStatusMessage()`
  - `sendFluidCommand()`
  - `startSocket()`
- File editor reliability:
  - `initAceEditor()` with fallback editor when `ace` is unavailable

## Build & Flash Configuration

File: `platformio.ini`

- Default environment:
  - `default_envs = wifi_s3`
- S3 flash config:
  - `board_upload.flash_size = 16MB`
  - `board_build.partitions = FluidNC/ld/esp32s3/app3M_spiffs9M_16MB.csv`

This expands local filesystem space significantly compared to `min_littlefs.csv`.

## PCB Image Strategy

In `web/index.html`, the board image is loaded from SD first:

- Primary: `/sd/render.png`
- Fallback: `render.png` (local flash filesystem)

HTML:

```html
<img src="/sd/render.png" onerror="this.onerror=null;this.src='render.png';" ...>
```

## Quick Usage Notes

- To update config from GUI, use **Update Config YAML** in the Configuration tab.
- To read analog voltages from firmware:
  - `/command_silent?commandText=[ESP430]json=yes`
- To use SD image:
  - place `render.png` on SD card root.
