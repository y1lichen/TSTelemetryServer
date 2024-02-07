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


#ifndef ABSTRACT_TELEMETRY_SERIALIZER_H
#define ABSTRACT_TELEMETRY_SERIALIZER_H
#include <string>
#include "telemetry.h"

class AbstractTelemetrySerializer{
        public:
                virtual std::string SerializeFrame(TelemetryFrame*) = 0;
                virtual std::string SerializeEvent(TelemetryGameplayEvent*) = 0;
                virtual ~AbstractTelemetrySerializer(){}
};

#endif
