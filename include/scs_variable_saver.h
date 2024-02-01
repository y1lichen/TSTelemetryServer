/*
This file is part of TSTelemetryServer.

Copyright (C) 2024 OrkenWhite.

TSTelemetryServer is free software: you can redistribute it and/or modify it under the terms of the GNU Lesser General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

TSTelemetryServer is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License along with TSTelemetryServer. If not, see <https://www.gnu.org/licenses/>. 
*/

#ifndef SCS_VARIABLE_SAVER_H
#define SCS_VARIABLE_SAVER_H
#include "scs_sdk/scssdk.h"
#include "scs_sdk/scssdk_value.h"


/*
 * Namespace containing all functions for storing channel variables
 */
namespace ScsVariableSaver{
        void StoreScsOrientation(const scs_string_t name, const scs_u32_t index,const scs_value_t *const value, const scs_context_t context);
        void StoreScsVector(const scs_string_t name, const scs_u32_t index,const scs_value_t *const value, const scs_context_t context);
        void StoreScsPlacement(const scs_string_t name, const scs_u32_t index,const scs_value_t *const value, const scs_context_t context);
        void StoreScsDouble(const scs_string_t name, const scs_u32_t index,const scs_value_t *const value, const scs_context_t context);
        void StoreScsFloat(const scs_string_t name, const scs_u32_t index,const scs_value_t *const value, const scs_context_t context);
        void StoreScsString(const scs_string_t name, const scs_u32_t index,const scs_value_t *const value, const scs_context_t context);
        void StoreScsU32(const scs_string_t name, const scs_u32_t index,const scs_value_t *const value, const scs_context_t context);
        void StoreScsU64(const scs_string_t name, const scs_u32_t index,const scs_value_t *const value, const scs_context_t context);
        void StoreScsS32(const scs_string_t name, const scs_u32_t index,const scs_value_t *const value, const scs_context_t context);
        void StoreScsS64(const scs_string_t name, const scs_u32_t index,const scs_value_t *const value, const scs_context_t context);
        void StoreScsBool(const scs_string_t name, const scs_u32_t index,const scs_value_t *const value, const scs_context_t context);
};
#endif