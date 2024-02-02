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


#include "scs_variable_saver.h"
#include "telemetry.h"

namespace ScsVariableSaver
{
    void StoreScsOrientation(const scs_string_t name, const scs_u32_t index, const scs_value_t *const value, const scs_context_t context)
    {
        TelemetryOrientation *target = (TelemetryOrientation *)context;

        target->heading = value->value_euler.heading * 360.0f;
        target->pitch = value->value_euler.pitch * 360.0f;
        target->roll = value->value_euler.roll * 360.0f;
    }
    void StoreScsVector(const scs_string_t name, const scs_u32_t index, const scs_value_t *const value, const scs_context_t context)
    {
        TelemetryVec3D *target = (TelemetryVec3D *)context;

        target->x = value->value_dvector.x;
        target->y = value->value_dvector.y;
        target->z = value->value_dvector.z;
    }
    void StoreScsPlacement(const scs_string_t name, const scs_u32_t index, const scs_value_t *const value, const scs_context_t context)
    {
        TelemetryPlacement *target = (TelemetryPlacement *)context;
        scs_value_dplacement_t *source = (scs_value_dplacement_t *)&value->value_dplacement;

        target->orientation.heading = source->orientation.heading * 360.0f;
        target->orientation.pitch = source->orientation.pitch * 360.0f;
        target->orientation.roll = source->orientation.roll * 360.0f;

        target->position.x = source->position.x;
        target->position.y = source->position.y;
        target->position.z = source->position.z;
    }
    void StoreScsString(const scs_string_t name, const scs_u32_t index, const scs_value_t *const value, const scs_context_t context)
    {
        std::string *target = (std::string *)context;
        *target = std::string(value->value_string.value);
    }
    void StoreScsDouble(const scs_string_t name, const scs_u32_t index, const scs_value_t *const value, const scs_context_t context)
    {
        scs_double_t *target = (scs_double_t *)context;
        *target = value->value_double.value;
    }
    void StoreScsFloat(const scs_string_t name, const scs_u32_t index, const scs_value_t *const value, const scs_context_t context)
    {
        scs_float_t *target = (scs_float_t *)context;
        *target = value->value_float.value;
    }
    void StoreScsS32(const scs_string_t name, const scs_u32_t index, const scs_value_t *const value, const scs_context_t context)
    {
        scs_s32_t *target = (scs_s32_t *)context;
        *target = value->value_s32.value;
    }
    void StoreScsU32(const scs_string_t name, const scs_u32_t index, const scs_value_t *const value, const scs_context_t context)
    {
        scs_u32_t *target = (scs_u32_t *)context;
        *target = value->value_u32.value;
    }
    void StoreScsS64(const scs_string_t name, const scs_u32_t index, const scs_value_t *const value, const scs_context_t context)
    {
        scs_s64_t *target = (scs_s64_t *)context;
        *target = value->value_s64.value;
    }
    void StoreScsU64(const scs_string_t name, const scs_u32_t index, const scs_value_t *const value, const scs_context_t context)
    {
        scs_u64_t *target = (scs_u64_t *)context;
        *target = value->value_u64.value;
    }
    void StoreScsBool(const scs_string_t name, const scs_u32_t index, const scs_value_t *const value, const scs_context_t context)
    {
        bool *target = (bool *)context;
        *target = value->value_bool.value != 0;
    }
}