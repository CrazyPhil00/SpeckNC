#include "AnalogPinDetail.h"
#include "PinCapabilities.h"
#include "Machine/MachineConfig.h"
#include "Extenders/ADS1115.h"
#include "Extenders/Extenders.h"
#include "Logging.h"

namespace Pins {
AnalogPinDetail::AnalogPinDetail(uint8_t device, pinnum_t channel, const PinOptionsParser& options)
    : PinDetail(channel), _channel(channel) {

  if (config && config->_extenders && device < 4) {
    _adc = config->_extenders->_adcDevices[device];
  }
  if (_adc == nullptr) {
    log_error("AnalogPinDetail: No ADS1115 device " << (int)device);
  }
}

PinCapabilities AnalogPinDetail::capabilities() const {
  return PinCapabilities::Input | PinCapabilities::ADC;
}

void AnalogPinDetail::write(bool high) {}
void AnalogPinDetail::synchronousWrite(bool high) {}

bool AnalogPinDetail::read() {
  return readAnalog() > 32767;
}

void AnalogPinDetail::setAttr(PinAttributes value, uint32_t frequency) {
  _attributes = value;
}

PinAttributes AnalogPinDetail::getAttr() const {
  return _attributes;
}

uint16_t AnalogPinDetail::readAnalog() {
  if (_adc == nullptr) return 0;
  return _adc->readChannel(_channel);
}

int16_t AnalogPinDetail::readAnalogMV() {
  if (_adc == nullptr) return 0;
  return _adc->readChannelMV(_channel);
}

}
