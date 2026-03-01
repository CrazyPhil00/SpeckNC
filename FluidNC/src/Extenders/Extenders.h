// Copyright (c) 2021 -  Stefan de Bruijn
// Use of this source code is governed by a GPLv3 license that can be found in the LICENSE file.

#pragma once

#include "Configuration/Configurable.h"
#include "Configuration/GenericFactory.h"
#include "PinExtender.h"
#include "ADS1115.h"

namespace Extenders {
class Extenders : public Configuration::Configurable {
public:
    PinExtender* _pinDrivers[16];
    ADS1115*     _adcDevices[4];

    Extenders();
    ~Extenders();

    void group(Configuration::HandlerBase& handler) override;
    void init();
    void afterParse() override {}
};

using PinExtenderFactory = Configuration::GenericFactory<PinExtenderDriver>;
}
