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

#ifndef TELEMETRY_TRUCK_H
#define TELEMETRY_TRUCK_H
#include "scs_sdk/scssdk.h"
#include "telemetry_common.h"
#include <vector>

struct TelemetryTruckCabin {
  TelemetryPlacement offset = {};
  TelemetryVec3D angularAcceleration = {};
  TelemetryVec3D angularVelocity = {};
};

struct TelemetryTruckInput {
  scs_double_t brake = scs_double_t(0.0);
  scs_double_t throttle = scs_double_t(0.0);
  scs_double_t clutch = scs_double_t(0.0);
  scs_double_t steering = scs_double_t(0.0);
};

struct TelemetryTruckBrake {
  scs_u32_t retarder = scs_u32_t(0);
  bool parking = false;
  bool motor = false;
  scs_double_t airPressure = scs_double_t(0.0);
  bool airPressureWarning = false;
  bool airPressureEmergency = false;
  scs_double_t temperature = scs_double_t(0.0);
};

struct TelemetryTruckFuel {
  scs_double_t amount = scs_double_t(0.0);
  scs_double_t range = scs_double_t(0.0);
  scs_double_t averageConsumption = scs_double_t(0.0);
  bool warning = false;
};

struct TelemetryTruckEngine {
  scs_double_t rpm = scs_double_t(0.0);
  scs_s32_t gear = scs_s32_t(0);
  bool enabled = false;
};

struct TelemetryTruckOil {
  scs_double_t pressure = scs_double_t(0.0);
  scs_double_t temperature = scs_double_t(0.0);
  bool pressureWarning = false;
};

struct TelemetryTruckAdblue {
  scs_double_t amount = scs_double_t(0.0);
  scs_double_t averageConsumption = scs_double_t(0.0);
  bool warning = false;
};

struct TelemetryTruckLight {
  bool leftBlinker = false;
  bool rightBlinker = false;
  bool parking = false;
  bool lowBeam = false;
  bool highBeam = false;
  scs_u32_t auxFront = scs_u32_t(0);
  scs_u32_t auxRoof = scs_u32_t(0);
  bool beacon = false;
  bool brake = false;
  bool reverse = false;
};

struct TelemetryTruckWear {
  scs_double_t engine = scs_double_t(0.0);
  scs_double_t transmission = scs_double_t(0.0);
  scs_double_t cabin = scs_double_t(0.0);
  scs_double_t chassis = scs_double_t(0.0);
  scs_double_t wheels = scs_double_t(0.0);
};

struct TelemetryTruckNavigation {
  scs_double_t distance = scs_double_t(0.0);
  scs_double_t time = scs_double_t(0.0);
  scs_double_t speed_limit = scs_double_t(0.0);
};

struct TelemetryTruckConfig {
  std::string brandId;
  std::string brand;
  std::string id;
  std::string name;
  scs_double_t fuelCapacity = scs_double_t(0.0);
  scs_double_t fuelWarningFactor = scs_double_t(0.0);
  scs_double_t adblueCapacity = scs_double_t(0.0);
  scs_double_t adblueWarningFactor = scs_double_t(0.0);
  scs_double_t airPressureWarning = scs_double_t(0.0);
  scs_double_t airPressureEmergency = scs_double_t(0.0);
  scs_double_t oilPressureWarning = scs_double_t(0.0);
  scs_double_t waterTemperatureWarning = scs_double_t(0.0);
  scs_double_t batteryVoltageWarning = scs_double_t(0.0);
  scs_double_t rpmLimit = scs_double_t(0.0);
  scs_u32_t forwardGearCount = scs_u32_t(0);
  scs_u32_t reverseGearCount = scs_u32_t(0);
  scs_u32_t retarderStepCount = scs_u32_t(0);
  TelemetryVec3D cabinPosition = {};
  TelemetryVec3D hookPosition = {};
  TelemetryVec3D headPosition = {};
  std::string licensePlate;
  std::string licensePlateCountry;
  std::string licensePlateCountryId;
  scs_u32_t wheelCount = scs_u32_t(0);
  std::string shifterType;
  std::array<scs_double_t, 24> forwardGearRatios = {};
  std::array<scs_double_t, 8> reverseGearRatios = {};
  scs_double_t differentialRation = scs_double_t(0.0);
};

struct TelemetryTruck {
  TelemetryTruckConfig config = {};
  TelemetryPlacement worldPlacement = {};
  TelemetryVec3D localLinearVelocity = {};
  TelemetryVec3D localAngularVelocity = {};
  TelemetryVec3D localLinearAcceleration = {};
  TelemetryVec3D localAngularAcceleration = {};
  TelemetryTruckCabin cabin = {};
  TelemetryPlacement headOffset = {};
  scs_double_t speed = scs_double_t(0.0);
  TelemetryTruckEngine engine = {};
  scs_s32_t displayedGear = scs_s32_t(0);
  TelemetryTruckInput input = {};
  TelemetryTruckInput effective = {};
  scs_double_t cruiseControl = scs_double_t(0.0);
  TelemetryTruckBrake brake = {};
  TelemetryTruckFuel fuel = {};
  TelemetryTruckAdblue adblue = {};
  TelemetryTruckOil oil = {};
  scs_double_t waterTemperature = scs_double_t(0.0);
  bool waterTemperatureWarning = false;
  scs_double_t batteryVoltage = scs_double_t(0.0);
  bool batteryVoltageWarning = false;
  bool electricEnabled = false;
  bool leftBlinker = false;
  bool rightBlinker = false;
  bool hazardWarning = false;
  bool differentialLock = false;
  TelemetryTruckLight light = {};
  bool wipers = false;
  scs_double_t dashboardBacklight = scs_double_t(0.0);
  bool liftAxle = false;
  bool liftAxleIndicator = false;
  bool trailerLiftAxle = false;
  bool trailerLiftAxleIndicator = false;
  TelemetryWheel wheels[MAX_WHEEL_COUNT];
  TelemetryTruckWear wear = {};
  scs_double_t odometer = scs_double_t(0.0);
  TelemetryTruckNavigation navigation = {};
};
#endif
