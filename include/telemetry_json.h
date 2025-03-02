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

#ifndef TELEMETRY_JSON_H
#define TELEMETRY_JSON_H
#include "telemetry.h"
#include <nlohmann/json.hpp>

/* Common types */
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(TelemetryVec3D, x, y, z)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(TelemetryOrientation, heading, pitch, roll)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(TelemetryPlacement, position, orientation)

/* Wheels */
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(TelemetryWheelConfig, isLiftable, position,
                                   isPowered, radius, isSimulated, isSteerable)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(TelemetryWheel, lift, liftOffset, isOnGround,
                                   rotation, steering, substance,
                                   suspensionDeflection, velocity, config)

/* Truck */
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(TelemetryTruckCabin, offset,
                                   angularAcceleration, angularVelocity)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(TelemetryTruckInput, brake, throttle, clutch,
                                   steering)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(TelemetryTruckBrake, retarder, parking,
                                   motor, airPressure, airPressureWarning,
                                   airPressureEmergency, temperature)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(TelemetryTruckFuel, amount, range,
                                   averageConsumption, warning)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(TelemetryTruckEngine, rpm, gear, enabled)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(TelemetryTruckOil, pressure, temperature,
                                   pressureWarning)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(TelemetryTruckAdblue, amount,
                                   averageConsumption, warning)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(TelemetryTruckLight, leftBlinker,
                                   rightBlinker, parking, lowBeam, highBeam,
                                   auxFront, auxRoof, beacon, brake, reverse)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(TelemetryTruckWear, engine, transmission,
                                   cabin, chassis, wheels)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(TelemetryTruckNavigation, distance, time,
                                   speed_limit)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(
    TelemetryTruckConfig, brand, brandId, id, name, fuelCapacity,
    fuelWarningFactor, adblueCapacity, adblueWarningFactor, airPressureWarning,
    airPressureEmergency, oilPressureWarning, waterTemperatureWarning,
    batteryVoltageWarning, rpmLimit, forwardGearCount, reverseGearCount,
    forwardGearRatios, reverseGearRatios, retarderStepCount, cabinPosition,
    hookPosition, headPosition, licensePlate, licensePlateCountry,
    licensePlateCountryId, wheelCount, shifterType)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(
    TelemetryTruck, config, worldPlacement, localLinearVelocity,
    localLinearAcceleration, localAngularVelocity, localAngularAcceleration,
    cabin, headOffset, speed, engine, displayedGear, input, effective,
    cruiseControl, brake, fuel, adblue, oil, waterTemperature,
    waterTemperatureWarning, batteryVoltage, batteryVoltageWarning,
    electricEnabled, leftBlinker, rightBlinker, hazardWarning, differentialLock,
    light, wipers, dashboardBacklight, liftAxle, liftAxleIndicator,
    trailerLiftAxle, trailerLiftAxleIndicator, wheels, wear, odometer,
    navigation)

/* Trailer */
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(TelemetryTrailerConfig, id, cargoAccessoryId,
                                   hookPosition, brandId, brand, name,
                                   licensePlate, licensePlateCountry,
                                   licensePlateCountryId, chainType, bodyType,
                                   wheelCount)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(TelemetryTrailerWear, body, chassis, wheels)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(TelemetryTrailer, config, worldPlacement,
                                   localLinearVelocity, localLinearAcceleration,
                                   localAngularVelocity,
                                   localAngularAcceleration, wear, connected,
                                   cargoDamage, wheels)

/* Job */
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(TelemetryJob, cargo, cargoId, cargoUnitCount,
                                   cargoMass, cargoUnitMass, deliveryTime,
                                   plannedDistance, income, destinationCity,
                                   destinationCityId, sourceCity, sourceCityId,
                                   destinationCompany, destinationCompanyId,
                                   sourceCompany, sourceCompanyId, jobMarket,
                                   isCargoLoaded, isSpecialJob, cargoDamage)

/* Frame and gameplay events */
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(TelemetryFrame, gameTime, localScale,
                                   multiplayerTimeOffset, restStop, paused,
                                   truck, trailer, job)

/* Gameplay events are ugly */
void to_json(nlohmann::json &j, const TelemetryGameplayEvent &gameplayEvent) {
  j = nlohmann::json();
  j["eventType"] = gameplayEvent.eventType;
  nlohmann::json attributes = nlohmann::json();
  for (auto const &[name, value] : gameplayEvent.attributes) {
    if (std::holds_alternative<std::string>(value)) {
      attributes[name] = std::get<std::string>(value);
    } else if (std::holds_alternative<scs_float_t>(value)) {
      attributes[name] = std::get<scs_float_t>(value);
    } else if (std::holds_alternative<scs_double_t>(value)) {
      attributes[name] = std::get<scs_double_t>(value);
    } else if (std::holds_alternative<scs_s32_t>(value)) {
      attributes[name] = std::get<scs_s32_t>(value);
    } else if (std::holds_alternative<scs_s64_t>(value)) {
      attributes[name] = std::get<scs_s64_t>(value);
    } else if (std::holds_alternative<scs_u32_t>(value)) {
      attributes[name] = std::get<scs_u32_t>(value);
    } else if (std::holds_alternative<scs_u64_t>(value)) {
      attributes[name] = std::get<scs_u64_t>(value);
    } else if (std::holds_alternative<bool>(value)) {
      attributes[name] = std::get<bool>(value);
    } else {
      attributes[name] = 0;
    }
  }
  j["attributes"] = attributes;
}

#endif