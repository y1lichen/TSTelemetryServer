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


#include "scs_sdk/scssdk.h"
#include "scs_sdk/scssdk_telemetry.h"
#include "scs_sdk/eurotrucks2/scssdk_eut2.h"
#include "scs_sdk/eurotrucks2/scssdk_telemetry_eut2.h"
#include "scs_sdk/amtrucks/scssdk_ats.h"
#include "scs_sdk/amtrucks/scssdk_telemetry_ats.h"

#include "network_handler.h"
#include "telemetry.h"
#include "event_queue.h"
#include "json_telemetry_serializer.h"
#include "config_handler.h"
#include "scs_variable_saver.h"

#include <string.h>
#include <thread>

#ifdef _WIN32
#include <windows.h>
#endif

#define UNUSED(x)

EventQueue eventQueue;
TelemetryFrame telemetryData = {};
bool frameChanged = true;

AbstractTelemetrySerializer *serializer = nullptr;

std::jthread *networkThread;

scs_log_t gameLog;

SCSAPI_VOID telemetry_frame_start(const scs_event_t UNUSED(event), const void *const UNUSED(event_info), scs_context_t UNUSED(context))
{
        /* Nothing useful here yet */
}

SCSAPI_VOID telemetry_frame_end(const scs_event_t UNUSED(event), const void *const UNUSED(event_info), scs_context_t UNUSED(context))
{
        if (frameChanged)
        {
                eventQueue.PushEvent(serializer->SerializeFrame(&telemetryData), EVENT_FRAME);
        }
        frameChanged = false;
}

SCSAPI_VOID telemetry_pause(const scs_event_t UNUSED(event), const void *const UNUSED(event_info), scs_context_t UNUSED(context))
{
        telemetryData.paused = !telemetryData.paused;
        frameChanged = true;
}

SCSAPI_VOID telemetry_configuration(const scs_event_t UNUSED(event), const void *const event_info, scs_context_t UNUSED(context))
{
        auto info = static_cast<const scs_telemetry_configuration_t*>(event_info);
        if (strcmp(SCS_TELEMETRY_CONFIG_truck, info->id) == 0)
        {
                ConfigHandler::HandleTruckConfig(info->attributes, &telemetryData.truck);
        }
        else if (strcmp(SCS_TELEMETRY_CONFIG_job, info->id) == 0)
        {
                ConfigHandler::HandleJobConfig(info->attributes, &telemetryData.job);
        }
        else if (strcmp(SCS_TELEMETRY_CONFIG_controls, info->id) == 0)
        {
                ConfigHandler::HandleControlConfig(info->attributes, &telemetryData.truck);
        }
        else if (strncmp(SCS_TELEMETRY_CONFIG_trailer, info->id, 7) == 0)
        {
                size_t id_len = strlen(info->id);
                if (id_len == 7)
                {
                        ConfigHandler::HandleTrailerConfig(info->attributes, &telemetryData.trailer[0]);
                }
                else
                {
                        std::string id(info->id);
                        int trailerId = atoi(id.substr(id.find(".") + 1).c_str());
                        if (trailerId > 9)
                        {
                                return;
                        }
                        ConfigHandler::HandleTrailerConfig(info->attributes, &telemetryData.trailer[trailerId]);
                }
        }
        else
        {
                return;
        }
        frameChanged = true;
}

SCSAPI_VOID telemetry_gameplay(const scs_event_t UNUSED(event), const void *const event_info, scs_context_t UNUSED(context))
{
        auto info = static_cast<const scs_telemetry_gameplay_event_t*>(event_info);
        if (std::string(info->id).find("job") != std::string::npos)
        {
                telemetryData.job = {};
        }
        TelemetryGameplayEvent eventObj = {};
        eventObj.eventType = info->id;
        for (auto attr = info->attributes;attr->name; ++attr)
        {
                switch (attr->value.type)
                {
                case SCS_VALUE_TYPE_string:
                        eventObj.attributes[std::string(attr->name)] = attr->value.value_string.value;
                        break;
                case SCS_VALUE_TYPE_float:
                        eventObj.attributes[std::string(attr->name)] = attr->value.value_float.value;
                        break;
                case SCS_VALUE_TYPE_double:
                        eventObj.attributes[std::string(attr->name)] = attr->value.value_double.value;
                        break;
                case SCS_VALUE_TYPE_s32:
                        eventObj.attributes[std::string(attr->name)] = attr->value.value_s32.value;
                        break;
                case SCS_VALUE_TYPE_s64:
                        eventObj.attributes[std::string(attr->name)] = attr->value.value_s64.value;
                        break;
                case SCS_VALUE_TYPE_u32:
                        eventObj.attributes[std::string(attr->name)] = attr->value.value_u32.value;
                        break;
                case SCS_VALUE_TYPE_u64:
                        eventObj.attributes[std::string(attr->name)] = attr->value.value_u64.value;
                        break;
                case SCS_VALUE_TYPE_bool:
                        eventObj.attributes[std::string(attr->name)] = attr->value.value_bool.value != 0;
                        break;
                default:
                        break;
                }
        }
        eventQueue.PushEvent(serializer->SerializeEvent(&eventObj), EVENT_GAMEPLAY);
}

SCSAPI_VOID channel_wrapper(scs_string_t name, scs_u32_t index, const scs_value_t *value, scs_context_t context)
{
        switch (value->type)
        {
        case SCS_VALUE_TYPE_bool:
                ScsVariableSaver::StoreScsBool(name, index, value, context);
                break;
        case SCS_VALUE_TYPE_double:
                ScsVariableSaver::StoreScsDouble(name, index, value, context);
                break;
        case SCS_VALUE_TYPE_float:
                ScsVariableSaver::StoreScsFloat(name, index, value, context);
                break;
        case SCS_VALUE_TYPE_s32:
                ScsVariableSaver::StoreScsS32(name, index, value, context);
                break;
        case SCS_VALUE_TYPE_s64:
                ScsVariableSaver::StoreScsS64(name, index, value, context);
                break;
        case SCS_VALUE_TYPE_u32:
                ScsVariableSaver::StoreScsU32(name, index, value, context);
                break;
        case SCS_VALUE_TYPE_u64:
                ScsVariableSaver::StoreScsU64(name, index, value, context);
                break;
        case SCS_VALUE_TYPE_string:
                ScsVariableSaver::StoreScsString(name, index, value, context);
                break;
        case SCS_VALUE_TYPE_dvector:
                ScsVariableSaver::StoreScsVector(name, index, value, context);
                break;
        case SCS_VALUE_TYPE_euler:
                ScsVariableSaver::StoreScsOrientation(name, index, value, context);
                break;
        case SCS_VALUE_TYPE_dplacement:
                ScsVariableSaver::StoreScsPlacement(name, index, value, context);
                break;
        default:
                gameLog(SCS_LOG_TYPE_warning, "TSTelemetryServer: Invalid channel value type!");
                return;
        }
        frameChanged = true;
}

/* The handling of the trailer indexes in channel names is ugly */
const char *trailer_indexed_channel_name(std::string channel_name, scs_u32_t trailer_index)
{
        return channel_name.replace(0, 7, std::string("trailer.") + std::to_string(trailer_index)).c_str();
}

#define REGISTER_CHANNEL(name, i, type, context) registerChannel(name, i, type, scs_u32_t(0), channel_wrapper, context)
SCSAPI_VOID register_channels(scs_telemetry_register_for_channel_t registerChannel)
{
        /* General channels */
        REGISTER_CHANNEL(SCS_TELEMETRY_CHANNEL_game_time, SCS_U32_NIL, SCS_VALUE_TYPE_u32, &telemetryData.gameTime);
        REGISTER_CHANNEL(SCS_TELEMETRY_CHANNEL_local_scale, SCS_U32_NIL, SCS_VALUE_TYPE_double, &telemetryData.localScale);
        REGISTER_CHANNEL(SCS_TELEMETRY_CHANNEL_multiplayer_time_offset, SCS_U32_NIL, SCS_VALUE_TYPE_s32, &telemetryData.multiplayerTimeOffset);
        REGISTER_CHANNEL(SCS_TELEMETRY_CHANNEL_next_rest_stop, SCS_U32_NIL, SCS_VALUE_TYPE_s32, &telemetryData.restStop);

        /* Non-indexed truck channels */
        REGISTER_CHANNEL(SCS_TELEMETRY_TRUCK_CHANNEL_world_placement, SCS_U32_NIL, SCS_VALUE_TYPE_dplacement, &telemetryData.truck.worldPlacement);
        REGISTER_CHANNEL(SCS_TELEMETRY_TRUCK_CHANNEL_local_linear_velocity, SCS_U32_NIL, SCS_VALUE_TYPE_dvector,
                         &telemetryData.truck.localLinearVelocity);
        REGISTER_CHANNEL(SCS_TELEMETRY_TRUCK_CHANNEL_local_linear_acceleration, SCS_U32_NIL, SCS_VALUE_TYPE_dvector,
                         &telemetryData.truck.localLinearAcceleration);
        REGISTER_CHANNEL(SCS_TELEMETRY_TRUCK_CHANNEL_local_angular_acceleration, SCS_U32_NIL, SCS_VALUE_TYPE_dvector,
                         &telemetryData.truck.localAngularAcceleration);
        REGISTER_CHANNEL(SCS_TELEMETRY_TRUCK_CHANNEL_local_angular_velocity, SCS_U32_NIL, SCS_VALUE_TYPE_dvector,
                         &telemetryData.truck.localAngularVelocity);
        REGISTER_CHANNEL(SCS_TELEMETRY_TRUCK_CHANNEL_cabin_offset, SCS_U32_NIL, SCS_VALUE_TYPE_dplacement,
                         &telemetryData.truck.cabin.offset);
        REGISTER_CHANNEL(SCS_TELEMETRY_TRUCK_CHANNEL_cabin_angular_acceleration, SCS_U32_NIL, SCS_VALUE_TYPE_dvector,
                         &telemetryData.truck.cabin.angularAcceleration);
        REGISTER_CHANNEL(SCS_TELEMETRY_TRUCK_CHANNEL_cabin_angular_velocity, SCS_U32_NIL, SCS_VALUE_TYPE_dvector,
                         &telemetryData.truck.cabin.angularVelocity);
        REGISTER_CHANNEL(SCS_TELEMETRY_TRUCK_CHANNEL_head_offset, SCS_U32_NIL, SCS_VALUE_TYPE_dplacement,
                         &telemetryData.truck.headOffset);
        REGISTER_CHANNEL(SCS_TELEMETRY_TRUCK_CHANNEL_speed, SCS_U32_NIL, SCS_VALUE_TYPE_double,
                         &telemetryData.truck.speed);
        REGISTER_CHANNEL(SCS_TELEMETRY_TRUCK_CHANNEL_engine_rpm, SCS_U32_NIL, SCS_VALUE_TYPE_double,
                         &telemetryData.truck.engine.rpm);
        REGISTER_CHANNEL(SCS_TELEMETRY_TRUCK_CHANNEL_engine_gear, SCS_U32_NIL, SCS_VALUE_TYPE_s32,
                         &telemetryData.truck.engine.gear);
        REGISTER_CHANNEL(SCS_TELEMETRY_TRUCK_CHANNEL_engine_enabled, SCS_U32_NIL, SCS_VALUE_TYPE_bool,
                         &telemetryData.truck.engine.enabled);
        REGISTER_CHANNEL(SCS_TELEMETRY_TRUCK_CHANNEL_displayed_gear, SCS_U32_NIL, SCS_VALUE_TYPE_s32,
                         &telemetryData.truck.displayedGear);
        REGISTER_CHANNEL(SCS_TELEMETRY_TRUCK_CHANNEL_input_steering, SCS_U32_NIL, SCS_VALUE_TYPE_double,
                         &telemetryData.truck.input.steering);
        REGISTER_CHANNEL(SCS_TELEMETRY_TRUCK_CHANNEL_input_brake, SCS_U32_NIL, SCS_VALUE_TYPE_double,
                         &telemetryData.truck.input.brake);
        REGISTER_CHANNEL(SCS_TELEMETRY_TRUCK_CHANNEL_input_clutch, SCS_U32_NIL, SCS_VALUE_TYPE_double,
                         &telemetryData.truck.input.clutch);
        REGISTER_CHANNEL(SCS_TELEMETRY_TRUCK_CHANNEL_effective_steering, SCS_U32_NIL, SCS_VALUE_TYPE_double,
                         &telemetryData.truck.effective.steering);
        REGISTER_CHANNEL(SCS_TELEMETRY_TRUCK_CHANNEL_effective_brake, SCS_U32_NIL, SCS_VALUE_TYPE_double,
                         &telemetryData.truck.effective.brake);
        REGISTER_CHANNEL(SCS_TELEMETRY_TRUCK_CHANNEL_effective_clutch, SCS_U32_NIL, SCS_VALUE_TYPE_double,
                         &telemetryData.truck.effective.clutch);
        REGISTER_CHANNEL(SCS_TELEMETRY_TRUCK_CHANNEL_cruise_control, SCS_U32_NIL, SCS_VALUE_TYPE_double,
                         &telemetryData.truck.cruiseControl);
        REGISTER_CHANNEL(SCS_TELEMETRY_TRUCK_CHANNEL_parking_brake, SCS_U32_NIL, SCS_VALUE_TYPE_bool,
                         &telemetryData.truck.brake.parking);
        REGISTER_CHANNEL(SCS_TELEMETRY_TRUCK_CHANNEL_motor_brake, SCS_U32_NIL, SCS_VALUE_TYPE_bool,
                         &telemetryData.truck.brake.motor);
        REGISTER_CHANNEL(SCS_TELEMETRY_TRUCK_CHANNEL_retarder_level, SCS_U32_NIL, SCS_VALUE_TYPE_u32,
                         &telemetryData.truck.brake.retarder);
        REGISTER_CHANNEL(SCS_TELEMETRY_TRUCK_CHANNEL_brake_air_pressure, SCS_U32_NIL, SCS_VALUE_TYPE_double,
                         &telemetryData.truck.brake.airPressure);
        REGISTER_CHANNEL(SCS_TELEMETRY_TRUCK_CHANNEL_brake_air_pressure_warning, SCS_U32_NIL, SCS_VALUE_TYPE_bool,
                         &telemetryData.truck.brake.airPressureWarning);
        REGISTER_CHANNEL(SCS_TELEMETRY_TRUCK_CHANNEL_brake_air_pressure_emergency, SCS_U32_NIL, SCS_VALUE_TYPE_bool,
                         &telemetryData.truck.brake.airPressureEmergency);
        REGISTER_CHANNEL(SCS_TELEMETRY_TRUCK_CHANNEL_brake_temperature, SCS_U32_NIL, SCS_VALUE_TYPE_double,
                         &telemetryData.truck.brake.temperature);
        REGISTER_CHANNEL(SCS_TELEMETRY_TRUCK_CHANNEL_fuel, SCS_U32_NIL, SCS_VALUE_TYPE_double,
                         &telemetryData.truck.fuel.amount);
        REGISTER_CHANNEL(SCS_TELEMETRY_TRUCK_CHANNEL_fuel_average_consumption, SCS_U32_NIL, SCS_VALUE_TYPE_double,
                         &telemetryData.truck.fuel.averageConsumption);
        REGISTER_CHANNEL(SCS_TELEMETRY_TRUCK_CHANNEL_fuel_range, SCS_U32_NIL, SCS_VALUE_TYPE_double,
                         &telemetryData.truck.fuel.range);
        REGISTER_CHANNEL(SCS_TELEMETRY_TRUCK_CHANNEL_fuel_warning, SCS_U32_NIL, SCS_VALUE_TYPE_bool,
                         &telemetryData.truck.fuel.warning);
        REGISTER_CHANNEL(SCS_TELEMETRY_TRUCK_CHANNEL_adblue, SCS_U32_NIL, SCS_VALUE_TYPE_double,
                         &telemetryData.truck.adblue.amount);
        /* Not supported by the latest game version (yet) */
        REGISTER_CHANNEL(SCS_TELEMETRY_TRUCK_CHANNEL_adblue_average_consumption, SCS_U32_NIL, SCS_VALUE_TYPE_double,
                         &telemetryData.truck.adblue.averageConsumption);
        REGISTER_CHANNEL(SCS_TELEMETRY_TRUCK_CHANNEL_adblue_warning, SCS_U32_NIL, SCS_VALUE_TYPE_bool,
                         &telemetryData.truck.adblue.warning);
        REGISTER_CHANNEL(SCS_TELEMETRY_TRUCK_CHANNEL_oil_pressure, SCS_U32_NIL, SCS_VALUE_TYPE_double,
                         &telemetryData.truck.oil.pressure);
        REGISTER_CHANNEL(SCS_TELEMETRY_TRUCK_CHANNEL_oil_temperature, SCS_U32_NIL, SCS_VALUE_TYPE_double,
                         &telemetryData.truck.oil.temperature);
        REGISTER_CHANNEL(SCS_TELEMETRY_TRUCK_CHANNEL_oil_pressure_warning, SCS_U32_NIL, SCS_VALUE_TYPE_bool,
                         &telemetryData.truck.oil.pressureWarning);
        REGISTER_CHANNEL(SCS_TELEMETRY_TRUCK_CHANNEL_water_temperature, SCS_U32_NIL, SCS_VALUE_TYPE_double,
                         &telemetryData.truck.waterTemperature);
        REGISTER_CHANNEL(SCS_TELEMETRY_TRUCK_CHANNEL_water_temperature_warning, SCS_U32_NIL, SCS_VALUE_TYPE_bool,
                         &telemetryData.truck.waterTemperatureWarning);
        REGISTER_CHANNEL(SCS_TELEMETRY_TRUCK_CHANNEL_battery_voltage, SCS_U32_NIL, SCS_VALUE_TYPE_double,
                         &telemetryData.truck.batteryVoltage);
        REGISTER_CHANNEL(SCS_TELEMETRY_TRUCK_CHANNEL_battery_voltage_warning, SCS_U32_NIL, SCS_VALUE_TYPE_bool,
                         &telemetryData.truck.batteryVoltageWarning);
        REGISTER_CHANNEL(SCS_TELEMETRY_TRUCK_CHANNEL_electric_enabled, SCS_U32_NIL, SCS_VALUE_TYPE_bool,
                         &telemetryData.truck.electricEnabled);
        REGISTER_CHANNEL(SCS_TELEMETRY_TRUCK_CHANNEL_lblinker, SCS_U32_NIL, SCS_VALUE_TYPE_bool,
                         &telemetryData.truck.leftBlinker);
        REGISTER_CHANNEL(SCS_TELEMETRY_TRUCK_CHANNEL_rblinker, SCS_U32_NIL, SCS_VALUE_TYPE_bool,
                         &telemetryData.truck.rightBlinker);
        REGISTER_CHANNEL(SCS_TELEMETRY_TRUCK_CHANNEL_hazard_warning, SCS_U32_NIL, SCS_VALUE_TYPE_bool,
                         &telemetryData.truck.hazardWarning);
        REGISTER_CHANNEL(SCS_TELEMETRY_TRUCK_CHANNEL_light_lblinker, SCS_U32_NIL, SCS_VALUE_TYPE_bool,
                         &telemetryData.truck.light.leftBlinker);
        REGISTER_CHANNEL(SCS_TELEMETRY_TRUCK_CHANNEL_light_rblinker, SCS_U32_NIL, SCS_VALUE_TYPE_bool,
                         &telemetryData.truck.light.rightBlinker);
        REGISTER_CHANNEL(SCS_TELEMETRY_TRUCK_CHANNEL_light_parking, SCS_U32_NIL, SCS_VALUE_TYPE_bool,
                         &telemetryData.truck.light.parking);
        REGISTER_CHANNEL(SCS_TELEMETRY_TRUCK_CHANNEL_light_low_beam, SCS_U32_NIL, SCS_VALUE_TYPE_bool,
                         &telemetryData.truck.light.lowBeam);
        REGISTER_CHANNEL(SCS_TELEMETRY_TRUCK_CHANNEL_light_high_beam, SCS_U32_NIL, SCS_VALUE_TYPE_bool,
                         &telemetryData.truck.light.highBeam);
        REGISTER_CHANNEL(SCS_TELEMETRY_TRUCK_CHANNEL_light_aux_front, SCS_U32_NIL, SCS_VALUE_TYPE_u32,
                         &telemetryData.truck.light.auxFront);
        REGISTER_CHANNEL(SCS_TELEMETRY_TRUCK_CHANNEL_light_aux_roof, SCS_U32_NIL, SCS_VALUE_TYPE_u32,
                         &telemetryData.truck.light.auxRoof);
        REGISTER_CHANNEL(SCS_TELEMETRY_TRUCK_CHANNEL_light_beacon, SCS_U32_NIL, SCS_VALUE_TYPE_bool,
                         &telemetryData.truck.light.beacon);
        REGISTER_CHANNEL(SCS_TELEMETRY_TRUCK_CHANNEL_light_brake, SCS_U32_NIL, SCS_VALUE_TYPE_bool,
                         &telemetryData.truck.light.brake);
        REGISTER_CHANNEL(SCS_TELEMETRY_TRUCK_CHANNEL_light_reverse, SCS_U32_NIL, SCS_VALUE_TYPE_bool,
                         &telemetryData.truck.light.reverse);
        REGISTER_CHANNEL(SCS_TELEMETRY_TRUCK_CHANNEL_wipers, SCS_U32_NIL, SCS_VALUE_TYPE_bool,
                         &telemetryData.truck.wipers);
        REGISTER_CHANNEL(SCS_TELEMETRY_TRUCK_CHANNEL_dashboard_backlight, SCS_U32_NIL, SCS_VALUE_TYPE_double,
                         &telemetryData.truck.dashboardBacklight);
        REGISTER_CHANNEL(SCS_TELEMETRY_TRUCK_CHANNEL_differential_lock, SCS_U32_NIL, SCS_VALUE_TYPE_bool,
                         &telemetryData.truck.differentialLock);
        REGISTER_CHANNEL(SCS_TELEMETRY_TRUCK_CHANNEL_lift_axle, SCS_U32_NIL, SCS_VALUE_TYPE_bool,
                         &telemetryData.truck.liftAxle);
        REGISTER_CHANNEL(SCS_TELEMETRY_TRUCK_CHANNEL_lift_axle_indicator, SCS_U32_NIL, SCS_VALUE_TYPE_bool,
                         &telemetryData.truck.liftAxleIndicator);
        REGISTER_CHANNEL(SCS_TELEMETRY_TRUCK_CHANNEL_trailer_lift_axle, SCS_U32_NIL, SCS_VALUE_TYPE_bool,
                         &telemetryData.truck.trailerLiftAxle);
        REGISTER_CHANNEL(SCS_TELEMETRY_TRUCK_CHANNEL_trailer_lift_axle_indicator, SCS_U32_NIL, SCS_VALUE_TYPE_bool,
                         &telemetryData.truck.trailerLiftAxleIndicator);
        REGISTER_CHANNEL(SCS_TELEMETRY_TRUCK_CHANNEL_wear_cabin, SCS_U32_NIL, SCS_VALUE_TYPE_double,
                         &telemetryData.truck.wear.cabin);
        REGISTER_CHANNEL(SCS_TELEMETRY_TRUCK_CHANNEL_wear_chassis, SCS_U32_NIL, SCS_VALUE_TYPE_double,
                         &telemetryData.truck.wear.chassis);
        REGISTER_CHANNEL(SCS_TELEMETRY_TRUCK_CHANNEL_wear_engine, SCS_U32_NIL, SCS_VALUE_TYPE_double,
                         &telemetryData.truck.wear.engine);
        REGISTER_CHANNEL(SCS_TELEMETRY_TRUCK_CHANNEL_wear_transmission, SCS_U32_NIL, SCS_VALUE_TYPE_double,
                         &telemetryData.truck.wear.transmission);
        REGISTER_CHANNEL(SCS_TELEMETRY_TRUCK_CHANNEL_wear_wheels, SCS_U32_NIL, SCS_VALUE_TYPE_double,
                         &telemetryData.truck.wear.wheels);
        REGISTER_CHANNEL(SCS_TELEMETRY_TRUCK_CHANNEL_odometer, SCS_U32_NIL, SCS_VALUE_TYPE_double,
                         &telemetryData.truck.odometer);
        REGISTER_CHANNEL(SCS_TELEMETRY_TRUCK_CHANNEL_navigation_distance, SCS_U32_NIL, SCS_VALUE_TYPE_double,
                         &telemetryData.truck.navigation.distance);
        REGISTER_CHANNEL(SCS_TELEMETRY_TRUCK_CHANNEL_navigation_speed_limit, SCS_U32_NIL, SCS_VALUE_TYPE_double,
                         &telemetryData.truck.navigation.speed_limit);
        REGISTER_CHANNEL(SCS_TELEMETRY_TRUCK_CHANNEL_navigation_time, SCS_U32_NIL, SCS_VALUE_TYPE_double,
                         &telemetryData.truck.navigation.time);

        /* Indexed truck channels */
        /* Wheels */
        for (scs_u32_t j = 0; j < MAX_WHEEL_COUNT; ++j)
        {
                REGISTER_CHANNEL(SCS_TELEMETRY_TRUCK_CHANNEL_wheel_susp_deflection, j, SCS_VALUE_TYPE_double,
                                 &telemetryData.truck.wheels[j].suspensionDeflection);
                REGISTER_CHANNEL(SCS_TELEMETRY_TRUCK_CHANNEL_wheel_rotation, j, SCS_VALUE_TYPE_double,
                                 &telemetryData.truck.wheels[j].rotation);
                REGISTER_CHANNEL(SCS_TELEMETRY_TRUCK_CHANNEL_wheel_velocity, j, SCS_VALUE_TYPE_double,
                                 &telemetryData.truck.wheels[j].velocity);
                REGISTER_CHANNEL(SCS_TELEMETRY_TRUCK_CHANNEL_wheel_steering, j, SCS_VALUE_TYPE_double,
                                 &telemetryData.truck.wheels[j].steering);
                REGISTER_CHANNEL(SCS_TELEMETRY_TRUCK_CHANNEL_wheel_lift, j, SCS_VALUE_TYPE_double,
                                 &telemetryData.truck.wheels[j].lift);
                REGISTER_CHANNEL(SCS_TELEMETRY_TRUCK_CHANNEL_wheel_lift_offset, j, SCS_VALUE_TYPE_double,
                                 &telemetryData.truck.wheels[j].liftOffset);
                REGISTER_CHANNEL(SCS_TELEMETRY_TRUCK_CHANNEL_wheel_on_ground, j, SCS_VALUE_TYPE_bool,
                                 &telemetryData.truck.wheels[j].isOnGround);
        }
        /* Trailer channels */
        for (scs_u32_t i = 0; i < MAX_TRAILERS; ++i)
        {
                /* Non-indexed */
                REGISTER_CHANNEL(trailer_indexed_channel_name(SCS_TELEMETRY_TRAILER_CHANNEL_connected, i),
                                 SCS_U32_NIL, SCS_VALUE_TYPE_bool, &telemetryData.trailer[i].connected);
                REGISTER_CHANNEL(trailer_indexed_channel_name(SCS_TELEMETRY_TRAILER_CHANNEL_cargo_damage, i),
                                 SCS_U32_NIL, SCS_VALUE_TYPE_double, &telemetryData.trailer[i].cargoDamage);
                REGISTER_CHANNEL(trailer_indexed_channel_name(SCS_TELEMETRY_TRAILER_CHANNEL_world_placement, i),
                                 SCS_U32_NIL, SCS_VALUE_TYPE_dplacement, &telemetryData.trailer[i].worldPlacement);
                REGISTER_CHANNEL(trailer_indexed_channel_name(SCS_TELEMETRY_TRAILER_CHANNEL_local_linear_velocity, i),
                                 SCS_U32_NIL, SCS_VALUE_TYPE_dvector, &telemetryData.trailer[i].localLinearVelocity);
                REGISTER_CHANNEL(trailer_indexed_channel_name(SCS_TELEMETRY_TRAILER_CHANNEL_local_linear_acceleration, i),
                                 SCS_U32_NIL, SCS_VALUE_TYPE_dvector, &telemetryData.trailer[i].localLinearAcceleration);
                REGISTER_CHANNEL(trailer_indexed_channel_name(SCS_TELEMETRY_TRAILER_CHANNEL_local_angular_acceleration, i),
                                 SCS_U32_NIL, SCS_VALUE_TYPE_dvector, &telemetryData.trailer[i].localAngularAcceleration);
                REGISTER_CHANNEL(trailer_indexed_channel_name(SCS_TELEMETRY_TRAILER_CHANNEL_wear_body, i),
                                 SCS_U32_NIL, SCS_VALUE_TYPE_double, &telemetryData.trailer[i].wear.body);
                REGISTER_CHANNEL(trailer_indexed_channel_name(SCS_TELEMETRY_TRAILER_CHANNEL_wear_chassis, i),
                                 SCS_U32_NIL, SCS_VALUE_TYPE_double, &telemetryData.trailer[i].wear.chassis);
                REGISTER_CHANNEL(trailer_indexed_channel_name(SCS_TELEMETRY_TRAILER_CHANNEL_wear_wheels, i),
                                 SCS_U32_NIL, SCS_VALUE_TYPE_double, &telemetryData.trailer[i].wear.wheels);

                /* Indexed(what?!) */
                /* Wheels */
                for (scs_u32_t j = 0; j < MAX_WHEEL_COUNT; ++j)
                {
                        REGISTER_CHANNEL(trailer_indexed_channel_name(SCS_TELEMETRY_TRAILER_CHANNEL_wheel_susp_deflection, i),
                                         j, SCS_VALUE_TYPE_double, &telemetryData.trailer[i].wheels[j].suspensionDeflection);
                        REGISTER_CHANNEL(trailer_indexed_channel_name(SCS_TELEMETRY_TRAILER_CHANNEL_wheel_rotation, i),
                                         j, SCS_VALUE_TYPE_double, &telemetryData.trailer[i].wheels[j].rotation);
                        REGISTER_CHANNEL(trailer_indexed_channel_name(SCS_TELEMETRY_TRAILER_CHANNEL_wheel_velocity, i),
                                         j, SCS_VALUE_TYPE_double, &telemetryData.trailer[i].wheels[j].velocity);
                        REGISTER_CHANNEL(trailer_indexed_channel_name(SCS_TELEMETRY_TRAILER_CHANNEL_wheel_steering, i),
                                         j, SCS_VALUE_TYPE_double, &telemetryData.trailer[i].wheels[j].steering);
                        REGISTER_CHANNEL(trailer_indexed_channel_name(SCS_TELEMETRY_TRAILER_CHANNEL_wheel_lift, i),
                                         j, SCS_VALUE_TYPE_double, &telemetryData.trailer[i].wheels[j].lift);
                        REGISTER_CHANNEL(trailer_indexed_channel_name(SCS_TELEMETRY_TRAILER_CHANNEL_wheel_lift_offset, i),
                                         j, SCS_VALUE_TYPE_double, &telemetryData.trailer[i].wheels[j].liftOffset);
                        REGISTER_CHANNEL(trailer_indexed_channel_name(SCS_TELEMETRY_TRAILER_CHANNEL_wheel_on_ground, i),
                                         j, SCS_VALUE_TYPE_bool, &telemetryData.trailer[i].wheels[j].isOnGround);
                }
        }

        /* Job channels */
        REGISTER_CHANNEL(SCS_TELEMETRY_JOB_CHANNEL_cargo_damage, SCS_U32_NIL, SCS_VALUE_TYPE_float,
                         &telemetryData.job.cargoDamage);
}

SCSAPI_RESULT scs_telemetry_init(const scs_u32_t version, const scs_telemetry_init_params_t *const params)
{
        if (version != SCS_TELEMETRY_VERSION_1_01)
                return SCS_RESULT_unsupported;
        const scs_telemetry_init_params_v101_t *const version_params = static_cast<const scs_telemetry_init_params_v101_t *>(params);
        gameLog = version_params->common.log;
        auto registerChannel = version_params->register_for_channel;
        auto registerEvent = version_params->register_for_event;
        scs_u32_t minSupportedVersion = scs_u32_t(0);

        if (strcmp(version_params->common.game_id, SCS_GAME_ID_EUT2) == 0)
                minSupportedVersion = SCS_TELEMETRY_EUT2_GAME_VERSION_1_18;
        else if (strcmp(version_params->common.game_id, SCS_GAME_ID_ATS) == 0)
                minSupportedVersion = SCS_TELEMETRY_ATS_GAME_VERSION_1_05;
        else
        {
                gameLog(SCS_LOG_TYPE_error, "TSTelemetryServer: Unsupported game!");
                return SCS_RESULT_unsupported;
        }
        if (minSupportedVersion > version_params->common.game_version)
        {
                gameLog(SCS_LOG_TYPE_error, "TSTelemetryServer: Game is too old!");
                return SCS_RESULT_unsupported;
        }
        gameLog(SCS_LOG_TYPE_message, "TSTelemetryServer: Game check complete, registering channels and starting JSON server...");

        serializer = new JsonTelemetrySerializer;

        const auto eventRegistration = (registerEvent(SCS_TELEMETRY_EVENT_configuration, telemetry_configuration, NULL) == SCS_RESULT_ok) &&
                                       (registerEvent(SCS_TELEMETRY_EVENT_paused, telemetry_pause, NULL) == SCS_RESULT_ok) &&
                                       (registerEvent(SCS_TELEMETRY_EVENT_started, telemetry_pause, NULL) == SCS_RESULT_ok) &&
                                       (registerEvent(SCS_TELEMETRY_EVENT_frame_start, telemetry_frame_start, NULL) == SCS_RESULT_ok) &&
                                       (registerEvent(SCS_TELEMETRY_EVENT_frame_end, telemetry_frame_end, NULL) == SCS_RESULT_ok) && 
                                       (registerEvent(SCS_TELEMETRY_EVENT_gameplay,telemetry_gameplay,NULL) == SCS_RESULT_ok);
        if (!eventRegistration)
        {
                gameLog(SCS_LOG_TYPE_error, "TSTelemetryServer: Unable to register events!");
                return SCS_RESULT_generic_error;
        }
        gameLog(SCS_LOG_TYPE_message, "TSTelemetryServer: Registered events!");

        register_channels(registerChannel);
        gameLog(SCS_LOG_TYPE_message, "TSTelemetryServer: Registered channels!");

        try
        {
                networkThread = NetworkHandler::GetEventThread(&eventQueue);
        }
        catch (std::exception &e)
        {
                std::string error = "TSTelemetryServer: ";
                error.append(e.what());
                gameLog(SCS_LOG_TYPE_error, error.c_str());
                return SCS_RESULT_generic_error;
        }
        gameLog(SCS_LOG_TYPE_message, "TSTelemetryServer: Plugin init complete!");
        return SCS_RESULT_ok;
}

inline void shutdown()
{
        if (networkThread != nullptr)
        {
                /* The destructor of jthread does the stop request and joins the thread automatically, don't do that twice */
                delete networkThread;
                networkThread = nullptr;
                NetworkHandler::Cleanup();
                if (serializer != nullptr)
                {
                        delete serializer;
                        serializer = nullptr;
                }
                gameLog(SCS_LOG_TYPE_message, "TSTelemetryServer: Cleanup successful!");
        }
}

SCSAPI_VOID scs_telemetry_shutdown()
{
        shutdown();
}

#ifdef _WIN32
BOOL APIENTRY DllMain(HMODULE UNUSED(module), DWORD reason_for_call, LPVOID UNUSED(reserved))
{
        if (reason_for_call == DLL_PROCESS_DETACH)
        {
                shutdown();
        }
        return true;
}
#endif
#ifdef __linux
void __attribute__((destructor)) unload()
{
        shutdown();
}
#endif
