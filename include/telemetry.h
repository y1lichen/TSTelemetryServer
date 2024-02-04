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

#ifndef TELEMETRY_H
#define TELEMETRY_H
#include "scs_sdk/scssdk.h"
#include "scs_sdk/common/scssdk_telemetry_common_configs.h"
#include "telemetry_trailer.h"
#include "telemetry_truck.h"
#include "telemetry_job.h"
#include <unordered_map>
#include <string>
#include <variant>

#define MAX_TRAILERS SCS_TELEMETRY_trailers_count

#define GAMEPLAY_ATTR_VARIANT std::variant<std::string,scs_float_t,scs_double_t,scs_s32_t,scs_s64_t,scs_u32_t,scs_u64_t,bool>

struct TelemetryFrame{
        scs_u32_t gameTime = scs_u32_t(0);
        scs_double_t localScale = scs_double_t(0.0);
        scs_s32_t multiplayerTimeOffset = scs_s32_t(0);
        scs_s32_t restStop = scs_s32_t(0);
        bool paused = true;
        TelemetryTruck truck = {};
        TelemetryTrailer trailer[MAX_TRAILERS] = {};
        TelemetryJob job = {};
};

/*
 * Gameplay event type.
 * All attributes are converted to string for safe storage.
 * */
struct TelemetryGameplayEvent{
        std::string eventType;
        std::unordered_map<std::string, GAMEPLAY_ATTR_VARIANT> attributes;
};

template <typename T>
struct TelemetryPayload{
        std::string payloadType;
        T* payload;
};
#endif
