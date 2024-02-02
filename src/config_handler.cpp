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


#include "config_handler.h"

#include "scs_sdk/common/scssdk_telemetry_common_configs.h"

#include <string>


std::map<std::string,void(*)(const scs_value_t*,scs_u32_t,TelemetryTruck*)> truckConfigHandlerTable = {
    {SCS_TELEMETRY_CONFIG_ATTRIBUTE_brand_id,[](const scs_value_t* v,scs_u32_t i, TelemetryTruck* tr){
        tr->config.brandId = std::string(v->value_string.value);}},
    {SCS_TELEMETRY_CONFIG_ATTRIBUTE_brand,[](const scs_value_t* v,scs_u32_t i, TelemetryTruck* tr){
        tr->config.brand = std::string(v->value_string.value);}},
    {SCS_TELEMETRY_CONFIG_ATTRIBUTE_id,[](const scs_value_t* v,scs_u32_t i, TelemetryTruck* tr){
        tr->config.id = std::string(v->value_string.value);}},
    {SCS_TELEMETRY_CONFIG_ATTRIBUTE_name,[](const scs_value_t* v,scs_u32_t i, TelemetryTruck* tr){
        tr->config.name = std::string(v->value_string.value);}},
    {SCS_TELEMETRY_CONFIG_ATTRIBUTE_fuel_capacity,[](const scs_value_t* v,scs_u32_t i, TelemetryTruck* tr){
        tr->config.fuelCapacity = v->value_float.value;}},
    {SCS_TELEMETRY_CONFIG_ATTRIBUTE_fuel_warning_factor,[](const scs_value_t* v,scs_u32_t i, TelemetryTruck* tr){
        tr->config.fuelWarningFactor = v->value_float.value;}},
    {SCS_TELEMETRY_CONFIG_ATTRIBUTE_adblue_capacity,[](const scs_value_t* v,scs_u32_t i, TelemetryTruck* tr){
        tr->config.adblueCapacity = v->value_float.value;}},
    {SCS_TELEMETRY_CONFIG_ATTRIBUTE_adblue_warning_factor,[](const scs_value_t* v,scs_u32_t i, TelemetryTruck* tr){
        tr->config.adblueWarningFactor = v->value_float.value;}},
    {SCS_TELEMETRY_CONFIG_ATTRIBUTE_air_pressure_emergency,[](const scs_value_t* v,scs_u32_t i, TelemetryTruck* tr){
        tr->config.airPressureEmergency = v->value_float.value;}},
    {SCS_TELEMETRY_CONFIG_ATTRIBUTE_air_pressure_warning,[](const scs_value_t* v,scs_u32_t i, TelemetryTruck* tr){
        tr->config.airPressureWarning = v->value_float.value;}},
    {SCS_TELEMETRY_CONFIG_ATTRIBUTE_oil_pressure_warning,[](const scs_value_t* v,scs_u32_t i, TelemetryTruck* tr){
        tr->config.oilPressureWarning = v->value_float.value;}},
    {SCS_TELEMETRY_CONFIG_ATTRIBUTE_water_temperature_warning,[](const scs_value_t* v,scs_u32_t i, TelemetryTruck* tr){
        tr->config.waterTemperatureWarning = v->value_float.value;}},
    {SCS_TELEMETRY_CONFIG_ATTRIBUTE_battery_voltage_warning,[](const scs_value_t* v,scs_u32_t i, TelemetryTruck* tr){
        tr->config.batteryVoltageWarning = v->value_float.value;}},
    {SCS_TELEMETRY_CONFIG_ATTRIBUTE_rpm_limit,[](const scs_value_t* v,scs_u32_t i, TelemetryTruck* tr){
        tr->config.rpmLimit = v->value_float.value;}},
    {SCS_TELEMETRY_CONFIG_ATTRIBUTE_forward_gear_count,[](const scs_value_t* v,scs_u32_t i, TelemetryTruck* tr){
        tr->config.forwardGearCount = v->value_u32.value;}},
    {SCS_TELEMETRY_CONFIG_ATTRIBUTE_reverse_gear_count,[](const scs_value_t* v,scs_u32_t i, TelemetryTruck* tr){
        tr->config.reverseGearCount = v->value_u32.value;}},
    {SCS_TELEMETRY_CONFIG_ATTRIBUTE_retarder_step_count,[](const scs_value_t* v,scs_u32_t i, TelemetryTruck* tr){
        tr->config.retarderStepCount = v->value_u32.value;}},
    {SCS_TELEMETRY_CONFIG_ATTRIBUTE_cabin_position,[](const scs_value_t* v,scs_u32_t i, TelemetryTruck* tr){
        tr->config.cabinPosition.x = v->value_fvector.x;
        tr->config.cabinPosition.y = v->value_fvector.y;
        tr->config.cabinPosition.z = v->value_fvector.z;}},
    {SCS_TELEMETRY_CONFIG_ATTRIBUTE_head_position,[](const scs_value_t* v,scs_u32_t i, TelemetryTruck* tr){
        tr->config.headPosition.x = v->value_fvector.x;
        tr->config.headPosition.y = v->value_fvector.y;
        tr->config.headPosition.z = v->value_fvector.z;}},
    {SCS_TELEMETRY_CONFIG_ATTRIBUTE_hook_position,[](const scs_value_t* v,scs_u32_t i, TelemetryTruck* tr){
        tr->config.hookPosition.x = v->value_fvector.x;
        tr->config.hookPosition.y = v->value_fvector.y;
        tr->config.hookPosition.z = v->value_fvector.z;}},
    {SCS_TELEMETRY_CONFIG_ATTRIBUTE_license_plate,[](const scs_value_t* v,scs_u32_t i, TelemetryTruck* tr){
        tr->config.licensePlate = std::string(v->value_string.value);}},
    {SCS_TELEMETRY_CONFIG_ATTRIBUTE_license_plate_country,[](const scs_value_t* v,scs_u32_t i, TelemetryTruck* tr){
        tr->config.licensePlateCountry = std::string(v->value_string.value);}},
    {SCS_TELEMETRY_CONFIG_ATTRIBUTE_license_plate_country_id,[](const scs_value_t* v,scs_u32_t i, TelemetryTruck* tr){
        tr->config.licensePlateCountryId = std::string(v->value_string.value);}},
    {SCS_TELEMETRY_CONFIG_ATTRIBUTE_wheel_count,[](const scs_value_t* v,scs_u32_t i, TelemetryTruck* tr){
        tr->config.wheelCount = v->value_u32.value;
        /*Zero out all non-existent wheels*/
        for(scs_u32_t j = v->value_u32.value;j<MAX_WHEEL_COUNT;++j){
            tr->wheels[j] = {0};
        }}},
    {SCS_TELEMETRY_CONFIG_ATTRIBUTE_wheel_position,[](const scs_value_t* v,scs_u32_t i, TelemetryTruck* tr){
        tr->wheels[i].config.position.x = v->value_fvector.x;
        tr->wheels[i].config.position.y = v->value_fvector.y;
        tr->wheels[i].config.position.z = v->value_fvector.z;}},
    {SCS_TELEMETRY_CONFIG_ATTRIBUTE_wheel_liftable,[](const scs_value_t* v,scs_u32_t i, TelemetryTruck* tr){
        tr->wheels[i].config.isLiftable = v->value_bool.value != 0;}},
    {SCS_TELEMETRY_CONFIG_ATTRIBUTE_wheel_powered,[](const scs_value_t* v,scs_u32_t i, TelemetryTruck* tr){
        tr->wheels[i].config.isPowered = v->value_bool.value != 0;}},
    {SCS_TELEMETRY_CONFIG_ATTRIBUTE_wheel_simulated,[](const scs_value_t* v,scs_u32_t i, TelemetryTruck* tr){
        tr->wheels[i].config.isSimulated = v->value_bool.value != 0;}},
    {SCS_TELEMETRY_CONFIG_ATTRIBUTE_wheel_radius,[](const scs_value_t* v,scs_u32_t i, TelemetryTruck* tr){
        tr->wheels[i].config.radius = v->value_float.value;}}
};

std::map<std::string,void(*)(const scs_value_t*,scs_u32_t,TelemetryTrailer*)> trailerConfigHandlerTable ={
    {SCS_TELEMETRY_CONFIG_ATTRIBUTE_id,[](const scs_value_t* v,scs_u32_t i,TelemetryTrailer* tr){
        tr->config.id = std::string(v->value_string.value);}},
    {SCS_TELEMETRY_CONFIG_ATTRIBUTE_cargo_accessory_id,[](const scs_value_t* v,scs_u32_t i,TelemetryTrailer* tr){
        tr->config.cargoAccessoryId = std::string(v->value_string.value);}},
    {SCS_TELEMETRY_CONFIG_ATTRIBUTE_hook_position,[](const scs_value_t* v,scs_u32_t i, TelemetryTrailer* tr){
        tr->config.hookPosition.x = v->value_fvector.x;
        tr->config.hookPosition.y = v->value_fvector.y;
        tr->config.hookPosition.z = v->value_fvector.z;}},
    {SCS_TELEMETRY_CONFIG_ATTRIBUTE_brand_id,[](const scs_value_t* v,scs_u32_t i,TelemetryTrailer* tr){
        tr->config.brandId = std::string(v->value_string.value);}},
    {SCS_TELEMETRY_CONFIG_ATTRIBUTE_brand,[](const scs_value_t* v,scs_u32_t i,TelemetryTrailer* tr){
        tr->config.brand = std::string(v->value_string.value);}},
    {SCS_TELEMETRY_CONFIG_ATTRIBUTE_name,[](const scs_value_t* v,scs_u32_t i,TelemetryTrailer* tr){
        tr->config.name = std::string(v->value_string.value);}},
    {SCS_TELEMETRY_CONFIG_ATTRIBUTE_chain_type,[](const scs_value_t* v,scs_u32_t i,TelemetryTrailer* tr){
        tr->config.chainType = std::string(v->value_string.value);}},
    {SCS_TELEMETRY_CONFIG_ATTRIBUTE_body_type,[](const scs_value_t* v,scs_u32_t i,TelemetryTrailer* tr){
        tr->config.bodyType = std::string(v->value_string.value);}},
    {SCS_TELEMETRY_CONFIG_ATTRIBUTE_license_plate,[](const scs_value_t* v,scs_u32_t i,TelemetryTrailer* tr){
        tr->config.licensePlate = std::string(v->value_string.value);}},
    {SCS_TELEMETRY_CONFIG_ATTRIBUTE_license_plate_country,[](const scs_value_t* v,scs_u32_t i,TelemetryTrailer* tr){
        tr->config.licensePlateCountry = std::string(v->value_string.value);}},
    {SCS_TELEMETRY_CONFIG_ATTRIBUTE_license_plate_country_id,[](const scs_value_t* v,scs_u32_t i,TelemetryTrailer* tr){
        tr->config.licensePlateCountryId = std::string(v->value_string.value);}},
    {SCS_TELEMETRY_CONFIG_ATTRIBUTE_wheel_count,[](const scs_value_t* v,scs_u32_t i, TelemetryTrailer* tr){
        tr->config.wheelCount = v->value_u32.value;
        /*Zero out all non-existent wheels*/
        for(scs_u32_t j = v->value_u32.value;j<MAX_WHEEL_COUNT;++j){
            tr->wheels[j] = {0};
        }}},
    {SCS_TELEMETRY_CONFIG_ATTRIBUTE_wheel_position,[](const scs_value_t* v,scs_u32_t i, TelemetryTrailer* tr){
        tr->wheels[i].config.position.x = v->value_fvector.x;
        tr->wheels[i].config.position.y = v->value_fvector.y;
        tr->wheels[i].config.position.z = v->value_fvector.z;}},
    {SCS_TELEMETRY_CONFIG_ATTRIBUTE_wheel_liftable,[](const scs_value_t* v,scs_u32_t i, TelemetryTrailer* tr){
        tr->wheels[i].config.isLiftable = v->value_bool.value != 0;}},
    {SCS_TELEMETRY_CONFIG_ATTRIBUTE_wheel_powered,[](const scs_value_t* v,scs_u32_t i, TelemetryTrailer* tr){
        tr->wheels[i].config.isPowered = v->value_bool.value != 0;}},
    {SCS_TELEMETRY_CONFIG_ATTRIBUTE_wheel_simulated,[](const scs_value_t* v,scs_u32_t i, TelemetryTrailer* tr){
        tr->wheels[i].config.isSimulated = v->value_bool.value != 0;}},
    {SCS_TELEMETRY_CONFIG_ATTRIBUTE_wheel_radius,[](const scs_value_t* v,scs_u32_t i, TelemetryTrailer* tr){
        tr->wheels[i].config.radius = v->value_float.value;}}
};

std::map<std::string,void(*)(const scs_value_t*,scs_u32_t,TelemetryJob*)> jobConfigHandlerTable ={
    {SCS_TELEMETRY_CONFIG_ATTRIBUTE_cargo_id,[](const scs_value_t* v,scs_u32_t i,TelemetryJob* job){
        job->cargoId = std::string(v->value_string.value);}},
    {SCS_TELEMETRY_CONFIG_ATTRIBUTE_cargo,[](const scs_value_t* v,scs_u32_t i,TelemetryJob* job){
        job->cargo = std::string(v->value_string.value);}},
    {SCS_TELEMETRY_CONFIG_ATTRIBUTE_cargo_mass,[](const scs_value_t* v,scs_u32_t i,TelemetryJob* job){
        job->cargoMass = v->value_float.value;}},
    {SCS_TELEMETRY_CONFIG_ATTRIBUTE_cargo_unit_mass,[](const scs_value_t* v,scs_u32_t i,TelemetryJob* job){
        job->cargoUnitMass = v->value_float.value;}},
    {SCS_TELEMETRY_CONFIG_ATTRIBUTE_destination_city,[](const scs_value_t* v,scs_u32_t i,TelemetryJob* job){
        job->destinationCity = std::string(v->value_string.value);}},
    {SCS_TELEMETRY_CONFIG_ATTRIBUTE_destination_city_id,[](const scs_value_t* v,scs_u32_t i,TelemetryJob* job){
        job->destinationCityId = std::string(v->value_string.value);}},
    {SCS_TELEMETRY_CONFIG_ATTRIBUTE_source_city,[](const scs_value_t* v,scs_u32_t i,TelemetryJob* job){
        job->sourceCity = std::string(v->value_string.value);}},
    {SCS_TELEMETRY_CONFIG_ATTRIBUTE_source_city_id,[](const scs_value_t* v,scs_u32_t i,TelemetryJob* job){
        job->sourceCityId = std::string(v->value_string.value);}},
    {SCS_TELEMETRY_CONFIG_ATTRIBUTE_destination_company,[](const scs_value_t* v,scs_u32_t i,TelemetryJob* job){
        job->destinationCompany = std::string(v->value_string.value);}},
    {SCS_TELEMETRY_CONFIG_ATTRIBUTE_destination_company_id,[](const scs_value_t* v,scs_u32_t i,TelemetryJob* job){
        job->destinationCompanyId = std::string(v->value_string.value);}},
    {SCS_TELEMETRY_CONFIG_ATTRIBUTE_source_company,[](const scs_value_t* v,scs_u32_t i,TelemetryJob* job){
        job->sourceCompany = std::string(v->value_string.value);}},
    {SCS_TELEMETRY_CONFIG_ATTRIBUTE_source_company_id,[](const scs_value_t* v,scs_u32_t i,TelemetryJob* job){
        job->sourceCompanyId = std::string(v->value_string.value);}},
    {SCS_TELEMETRY_CONFIG_ATTRIBUTE_income,[](const scs_value_t* v,scs_u32_t i,TelemetryJob* job){
        job->income = v->value_u64.value;}},
    {SCS_TELEMETRY_CONFIG_ATTRIBUTE_delivery_time,[](const scs_value_t* v,scs_u32_t i,TelemetryJob* job){
        job->deliveryTime = v->value_u32.value;}},
    {SCS_TELEMETRY_CONFIG_ATTRIBUTE_is_cargo_loaded,[](const scs_value_t* v,scs_u32_t i,TelemetryJob* job){
        job->isCargoLoaded = v->value_bool.value != 0;}},
    {SCS_TELEMETRY_CONFIG_ATTRIBUTE_job_market,[](const scs_value_t* v,scs_u32_t i,TelemetryJob* job){
        job->jobMarket = std::string(v->value_string.value);}},
    {SCS_TELEMETRY_CONFIG_ATTRIBUTE_special_job,[](const scs_value_t* v,scs_u32_t i,TelemetryJob* job){
        job->isSpecialJob = v->value_bool.value != 0;}},
    {SCS_TELEMETRY_CONFIG_ATTRIBUTE_planned_distance_km,[](const scs_value_t* v,scs_u32_t i,TelemetryJob* job){
        job->plannedDistance = v->value_u32.value;}}
};

std::map<std::string,void(*)(const scs_value_t*,scs_u32_t,TelemetryTruck*)> controlConfigHandlerTable = {
    {SCS_TELEMETRY_CONFIG_ATTRIBUTE_shifter_type,[](const scs_value_t* v,scs_u32_t i,TelemetryTruck* tr){
        tr->config.shifterType = std::string(v->value_string.value);}}
};



void ConfigHandler::HandleTruckConfig(const scs_named_value_t* attributes,TelemetryTruck* context){
    for(auto attr = attributes;attr->name;++attr){
        if(truckConfigHandlerTable.count(attr->name) > 0){
            truckConfigHandlerTable[std::string(attr->name)](&attr->value,attr->index,context);
        }
    }
}

void ConfigHandler::HandleTrailerConfig(const scs_named_value_t* attributes,TelemetryTrailer* context){
    for(auto attr = attributes;attr->name;++attr){
        if(trailerConfigHandlerTable.count(attr->name) > 0){
            trailerConfigHandlerTable[std::string(attr->name)](&attr->value,attr->index,context);
        }
    }
}

void ConfigHandler::HandleJobConfig(const scs_named_value_t* attributes,TelemetryJob* context){
    for(auto attr = attributes;attr->name;++attr){
        if(jobConfigHandlerTable.count(attr->name) > 0){
            jobConfigHandlerTable[std::string(attr->name)](&attr->value,attr->index,context);
        }
    }
}

void ConfigHandler::HandleControlConfig(const scs_named_value_t* attributes,TelemetryTruck* context){
    for(auto attr = attributes;attr->name;++attr){
        if(controlConfigHandlerTable.count(attr->name) > 0){
            controlConfigHandlerTable[std::string(attr->name)](&attr->value,attr->index,context);
        }
    }
}