/*
This file is part of TSTelemetryServer.

Copyright (C) 2024 OrkenWhite.

TSTelemetryServer is free software: you can redistribute it and/or modify it under the terms of the GNU Lesser General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

TSTelemetryServer is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License along with TSTelemetryServer. If not, see <https://www.gnu.org/licenses/>. 
*/

#include "json_telemetry_serializer.h"
#include "telemetry.h"
#include "rfl.hpp"
#include "rfl/json.hpp"

template <typename T>
struct TelemetryPayload{
        std::string payloadType;
        T* payload;
};

std::string JsonTelemetrySerializer::SerializeFrame(TelemetryFrame* frame){
        TelemetryPayload<TelemetryFrame> payload;
        payload.payloadType = "frame";
        payload.payload = frame;
        return rfl::json::write(&payload);
}
std::string JsonTelemetrySerializer::SerializeEvent(TelemetryGameplayEvent* frame){
        TelemetryPayload<TelemetryGameplayEvent> payload;
        payload.payloadType = "event";
        payload.payload = frame;
        return rfl::json::write(&payload);
}
