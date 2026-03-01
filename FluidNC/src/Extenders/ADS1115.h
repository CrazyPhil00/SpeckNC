#pragma once

#include "Configuration/Configurable.h"
#include <cstdint>

namespace Machine {
class I2CBus;
}

namespace Extenders {
class ADS1115 : public Configuration::Configurable {
public:
  ADS1115() {}
  ~ADS1115() {}

  void group(Configuration::HandlerBase& handler) override;
  void afterParse() override {}
  void init();

  uint16_t readChannel(uint8_t channel);
  int16_t  readChannelMV(uint8_t channel);

private:
  static const uint8_t REG_CONVERSION = 0x00;
  static const uint8_t REG_CONFIG     = 0x01;

  Machine::I2CBus* _i2cBus        = nullptr;
  uint8_t          _i2cAddress    = 0x48;
  uint8_t          _pga           = 2;
  uint8_t          _dataRate      = 4;
  bool             _continuousMode = false;

  uint16_t readADC(uint8_t channel);
  uint16_t readRegister(uint8_t reg);
  void     writeRegister(uint8_t reg, uint16_t value);
};
}