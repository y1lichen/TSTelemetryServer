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

#ifndef CONFIG_HANDLER_H
#define CONFIG_HANDLER_H

#include "scs_sdk/scssdk_value.h"
#include "telemetry.h"

#include <map>

namespace ConfigHandler{
    void HandleTruckConfig(const scs_named_value_t* attributes,TelemetryTruck* context);
    void HandleTrailerConfig(const scs_named_value_t* attributes,TelemetryTrailer* context);
    void HandleJobConfig(const scs_named_value_t* attributes,TelemetryJob* context);
    void HandleControlConfig(const scs_named_value_t* attributes,TelemetryTruck* context);

}
#endif
