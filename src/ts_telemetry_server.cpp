#include "telemetry.h"
#include "telemetry_config.h"
#include "telemetry_channels.h"

#include <cstring>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>

#define SHM_NAME "/ets2_telemetry"
#define UNUSED(x) (void)(x)

TelemetryFrame telemetryData = {};
void* shm_ptr = nullptr;

SCSAPI_VOID telemetry_frame_end(const scs_event_t UNUSED(event),
                                const void* UNUSED(event_info),
                                scs_context_t UNUSED(context)) {
    if (shm_ptr) {
        std::memcpy(shm_ptr, &telemetryData, sizeof(telemetryData));
    }
}

SCSAPI_RESULT scs_telemetry_init(const scs_u32_t version,
                                 const scs_telemetry_init_params_t* params) {
    if (version != SCS_TELEMETRY_VERSION_1_01) return SCS_RESULT_unsupported;
    const auto* vparams = static_cast<const scs_telemetry_init_params_v101_t*>(params);

    if (vparams->register_for_event(SCS_TELEMETRY_EVENT_frame_end, telemetry_frame_end, nullptr) != SCS_RESULT_ok) {
        return SCS_RESULT_generic_error;
    }

    // 註冊通道，保持原邏輯
    register_channels(vparams->register_for_channel);

    // 建立 shared memory
    int shm_fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666);
    if (shm_fd < 0 || ftruncate(shm_fd, sizeof(TelemetryFrame)) != 0) {
        return SCS_RESULT_generic_error;
    }
    shm_ptr = mmap(nullptr, sizeof(TelemetryFrame), PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (shm_ptr == MAP_FAILED) {
        shm_ptr = nullptr;
        return SCS_RESULT_generic_error;
    }

    return SCS_RESULT_ok;
}

SCSAPI_VOID scs_telemetry_shutdown() {
    if (shm_ptr) {
        munmap(shm_ptr, sizeof(TelemetryFrame));
        shm_ptr = nullptr;
    }
    shm_unlink(SHM_NAME);  // 可選：若你希望在 ETS2 結束時清除記憶體
}

#ifdef _WIN32
BOOL APIENTRY DllMain(HMODULE, DWORD reason, LPVOID) {
    return TRUE;
}
#else
__attribute__((destructor)) void unload() {
    scs_telemetry_shutdown();
}
#endif
