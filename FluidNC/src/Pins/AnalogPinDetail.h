#pragma once

#include "PinDetail.h"
#include "PinOptionsParser.h"

namespace Extenders {
class ADS1115;
}

namespace Pins {
class AnalogPinDetail : public PinDetail {
  Extenders::ADS1115* _adc     = nullptr;
  uint8_t             _channel = 0;
  PinAttributes       _attributes;

public:
  AnalogPinDetail(uint8_t device, pinnum_t channel, const PinOptionsParser& options);

  PinCapabilities capabilities() const override;
  void            write(bool high) override;
  void            synchronousWrite(bool high) override;
  bool            read() override;
  void            setAttr(PinAttributes value, uint32_t frequency = 0) override;
  PinAttributes   getAttr() const override;
  uint16_t        readAnalog() override;
  int16_t         readAnalogMV() override;
  ~AnalogPinDetail() override {}
};
}
