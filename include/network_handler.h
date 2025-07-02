#pragma once

#include "telemetry.h"

class SharedMemoryWriter {
public:
    static bool Init();
    static void WriteFrame(const TelemetryFrame* data);
    static void Cleanup();
};
