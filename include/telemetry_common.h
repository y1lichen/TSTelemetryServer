/*
This file is part of TSTelemetryServer.

Copyright (C) 2024 OrkenWhite.

TSTelemetryServer is free software: you can redistribute it and/or modify it under the terms of the GNU Lesser General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

TSTelemetryServer is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License along with TSTelemetryServer. If not, see <https://www.gnu.org/licenses/>. 
*/

#ifndef TELEMETRY_COMMON_H
#define TELEMETRY_COMMON_H
#include "scs_sdk/scssdk.h"

#include <string>

#define MAX_WHEEL_COUNT 14
#define MAX_SUBSTANCE_COUNT 25

struct TelemetryVec3D{
        scs_double_t x,y,z = scs_double_t(0.0);
};

struct TelemetryOrientation{
        scs_double_t heading,pitch,roll = scs_double_t(0.0);
};

struct TelemetryVelocity{
        TelemetryVec3D linear = {};
        TelemetryVec3D angular = {};
};

struct TelemetryPlacement{
        TelemetryVec3D position = {};
        TelemetryOrientation orientation = {};
};

struct TelemetryWheelConfig{
        bool isLiftable = false;
        TelemetryVec3D position = {};
        bool isPowered = false;
        scs_double_t radius = scs_double_t(0.0);
        bool isSimulated = false;
        bool isSteerable = false;
};

struct TelemetryWheel{
        scs_double_t lift = scs_double_t(0.0);
        scs_double_t liftOffset = scs_double_t(0.0);
        bool isOnGround = false;
        scs_double_t rotation = scs_double_t(0.0);
        scs_double_t steering = scs_double_t(0.0);
        scs_u32_t substance = scs_u32_t(0);
        scs_double_t suspensionDeflection = scs_double_t(0.0);
        scs_double_t velocity = scs_double_t(0.0);
        TelemetryWheelConfig config = {};

};
#endif
