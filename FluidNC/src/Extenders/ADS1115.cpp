#include "ADS1115.h"
#include "Machine/MachineConfig.h"
#include "Machine/I2CBus.h"
#include "Logging.h"
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

namespace Extenders {
    void ADS1115::group(Configuration::HandlerBase& handler) {
        handler.item("address", _i2cAddress);
        handler.item("pga", _pga);
        handler.item("data_rate", _dataRate);
        handler.item("continuous_mode", _continuousMode);
    }

    void ADS1115::init() {
        if (!config || !config->_i2c[0]) {
            log_error("ADS1115: No I2C bus available");
            return;
        }
        _i2cBus = config->_i2c[0];

        uint16_t testReg = readRegister(REG_CONFIG);
        if (testReg == 0xFFFF) {
            log_error("ADS1115: Device not found at address 0x" << String(_i2cAddress, HEX).c_str());
            return;
        }

        log_info("ADS1115: Initialized at address 0x" << String(_i2cAddress, HEX).c_str());
    }

    uint16_t ADS1115::readChannel(uint8_t channel) {
        if (channel > 3) {
            log_error("ADS1115: Invalid channel " << (int)channel);
            return 0;
        }
        return readADC(channel);
    }

    int16_t ADS1115::readChannelMV(uint8_t channel) {
        uint16_t raw = readChannel(channel);
        const int16_t fsRanges[] = { 6144, 4096, 2048, 1024, 512, 256, 256, 256 };
        int16_t fullScaleRange = fsRanges[_pga & 0x07];
        return (int16_t)(((int32_t)(int16_t)raw * fullScaleRange) / 32767);
    }

    uint16_t ADS1115::readADC(uint8_t channel) {
        if (!_i2cBus) {
            return 0;
        }

        uint16_t cfg = readRegister(REG_CONFIG);
        if (cfg == 0xFFFF) return 0;

        // Set channel (bits 14:12), OS bit, PGA, mode, data rate
        cfg &= ~(0x07 << 12);
        cfg |= ((channel & 0x03) + 4) << 12;  // single-ended: 4=AIN0, 5=AIN1, 6=AIN2, 7=AIN3
        cfg |= 0x8000;  // start conversion

        writeRegister(REG_CONFIG, cfg);
        vTaskDelay(pdMS_TO_TICKS(10));

        return readRegister(REG_CONVERSION);
    }

    uint16_t ADS1115::readRegister(uint8_t reg) {
        if (!_i2cBus) return 0xFFFF;

        uint8_t buffer[2] = { 0, 0 };
        if (_i2cBus->write(_i2cAddress, &reg, 1) != 1) return 0xFFFF;
        if (_i2cBus->read(_i2cAddress, buffer, 2) != 2) return 0xFFFF;

        return (uint16_t)((buffer[0] << 8) | buffer[1]);
    }

    void ADS1115::writeRegister(uint8_t reg, uint16_t value) {
        if (!_i2cBus) return;

        uint8_t buffer[3] = {
            reg,
            (uint8_t)((value >> 8) & 0xFF),
            (uint8_t)(value & 0xFF)
        };
        _i2cBus->write(_i2cAddress, buffer, 3);
    }
}
