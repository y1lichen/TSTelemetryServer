/*
This file is part of TSTelemetryServer.

Copyright (C) 2024 OrkenWhite.

TSTelemetryServer is free software: you can redistribute it and/or modify it 
under the terms of the GNU Lesser General Public License as published by the 
Free Software Foundation, either version 3 of the License, 
or (at your option) any later version.

TSTelemetryServer is distributed in the hope that it will be useful, 
but WITHOUT ANY WARRANTY; without even the implied warranty of 
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 
See the GNU Lesser General Public License for more details.

You should have received a copy of the 
GNU Lesser General Public License along with TSTelemetryServer. 
If not, see <https://www.gnu.org/licenses/>. 
*/


#ifndef TELEMETRY_JOB_H
#define TELEMETRY_JOB_H
#include "scs_sdk/scssdk.h"
#include "telemetry_common.h"

struct TelemetryJob{
        std::string cargoId = std::string();
        std::string cargo = std::string();
        scs_u32_t cargoUnitCount = scs_u32_t(0);
        scs_double_t cargoMass = scs_double_t(0.0);
        scs_double_t cargoUnitMass = scs_double_t(0.0);
        scs_u32_t deliveryTime = scs_u32_t(0);
        scs_u32_t plannedDistance = scs_u32_t(0);
        scs_u64_t income = scs_u64_t(0);
        std::string destinationCity = std::string();
        std::string destinationCityId = std::string();
        std::string sourceCity = std::string();
        std::string sourceCityId = std::string();
        std::string destinationCompany = std::string();
        std::string destinationCompanyId = std::string();
        std::string sourceCompany = std::string();
        std::string sourceCompanyId = std::string();
        std::string jobMarket = std::string();
        bool isCargoLoaded = false;
        bool isSpecialJob = false;
        scs_double_t cargoDamage = scs_double_t(0.0);
};

#endif
