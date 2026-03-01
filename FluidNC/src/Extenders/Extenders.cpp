// Copyright (c) 2021 -  Stefan de Bruijn
// Use of this source code is governed by a GPLv3 license that can be found in the LICENSE file.

#include "Extenders.h"
#include "ADS1115.h"

namespace Extenders {
PinExtender::PinExtender() : _driver(nullptr) {}

void PinExtender::group(Configuration::HandlerBase& handler) {
    PinExtenderFactory::factory(handler, _driver);
}

void PinExtender::init() {
    if (_driver) {
        _driver->init();
    }
}

PinExtender::~PinExtender() {
    delete _driver;
}

Extenders::Extenders() {
    for (int i = 0; i < 16; ++i) {
        _pinDrivers[i] = nullptr;
    }
    for (int i = 0; i < 4; ++i) {
        _adcDevices[i] = nullptr;
    }
}

void Extenders::group(Configuration::HandlerBase& handler) {
    for (int i = 0; i < 10; ++i) {
        char tmp[14];
        snprintf(tmp, sizeof(tmp), "pinextender%d", i);
        handler.section(tmp, _pinDrivers[i]);
    }

    handler.section("ads1115_0", _adcDevices[0]);
    handler.section("ads1115_1", _adcDevices[1]);
    handler.section("ads1115_2", _adcDevices[2]);
    handler.section("ads1115_3", _adcDevices[3]);
}

void Extenders::init() {
    for (int i = 0; i < 16; ++i) {
        if (_pinDrivers[i] != nullptr) {
            _pinDrivers[i]->init();
        }
    }
    for (int i = 0; i < 4; ++i) {
        if (_adcDevices[i] != nullptr) {
            _adcDevices[i]->init();
        }
    }
}

Extenders::~Extenders() {
    for (int i = 0; i < 16; ++i) {
        delete _pinDrivers[i];
    }
    for (int i = 0; i < 4; ++i) {
        delete _adcDevices[i];
    }
}
}
