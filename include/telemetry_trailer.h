/*
This file is part of TSTelemetryServer.

Copyright (C) 2024 OrkenWhite.

TSTelemetryServer is free software: you can redistribute it and/or modify it under the terms of the GNU Lesser General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

TSTelemetryServer is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License along with TSTelemetryServer. If not, see <https://www.gnu.org/licenses/>. 
*/

#ifndef TELEMETRY_TRAILER_H
#define TELEMETRY_TRAILER_H
#include "scs_sdk/scssdk.h"
#include "telemetry_common.h"

struct TelemetryTrailerConfig{
        std::string id;
        std::string cargoAccessoryId;
        TelemetryVec3D hookPosition = {};
        std::string brandId;
        std::string brand;
        std::string name;
        std::string licensePlate;
        std::string licensePlateCountry;
        std::string licensePlateCountryId;
        std::string chainType;
        std::string bodyType;
        scs_u32_t wheelCount = scs_u32_t(0);
};

struct TelemetryTrailerWear{
        scs_double_t body = scs_double_t(0.0);
        scs_double_t chassis = scs_double_t(0.0); 
        scs_double_t wheels = scs_double_t(0.0); 
};

struct TelemetryTrailer{
        TelemetryTrailerConfig config = {};
        TelemetryPlacement worldPlacement = {};
        TelemetryVec3D localLinearVelocity = {};
        TelemetryVec3D localAngularVelocity = {};
        TelemetryVec3D localLinearAcceleration = {};
        TelemetryVec3D localAngularAcceleration = {};
        TelemetryTrailerWear wear = {};
        bool connected = false;
        scs_double_t cargoDamage = scs_double_t(0.0);
        TelemetryWheel wheels[MAX_WHEEL_COUNT];
        
};
#endif
