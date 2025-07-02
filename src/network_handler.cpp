#include "network_handler.h"
#include <cstring>
#include <iostream>

#ifdef _WIN32
#include <windows.h>
static HANDLE hMapFile = nullptr;
static LPVOID pBuf = nullptr;
#else
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
static int shm_fd = -1;
static void* pBuf = nullptr;
#endif

constexpr const char* SHM_NAME = "TSTelemetrySharedMemory";
constexpr size_t SHM_SIZE = sizeof(TelemetryFrame);

bool SharedMemoryWriter::Init() {
#ifdef _WIN32
    hMapFile = CreateFileMappingA(
        INVALID_HANDLE_VALUE,
        nullptr,
        PAGE_READWRITE,
        0,
        SHM_SIZE,
        SHM_NAME
    );
    if (hMapFile == nullptr) {
        std::cerr << "Failed to create file mapping" << std::endl;
        return false;
    }

    pBuf = MapViewOfFile(hMapFile, FILE_MAP_ALL_ACCESS, 0, 0, SHM_SIZE);
    if (pBuf == nullptr) {
        CloseHandle(hMapFile);
        hMapFile = nullptr;
        std::cerr << "Failed to map view of file" << std::endl;
        return false;
    }
#else
    shm_fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666);
    if (shm_fd == -1) {
        std::cerr << "Failed to create shared memory" << std::endl;
        return false;
    }

    if (ftruncate(shm_fd, SHM_SIZE) == -1) {
        std::cerr << "Failed to set shared memory size" << std::endl;
        return false;
    }

    pBuf = mmap(nullptr, SHM_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (pBuf == MAP_FAILED) {
        std::cerr << "Failed to mmap shared memory" << std::endl;
        return false;
    }
#endif
    return true;
}

void SharedMemoryWriter::WriteFrame(const TelemetryFrame* data) {
    if (pBuf != nullptr && data != nullptr) {
        std::memcpy(pBuf, data, sizeof(TelemetryFrame));
    }
}

void SharedMemoryWriter::Cleanup() {
#ifdef _WIN32
    if (pBuf) {
        UnmapViewOfFile(pBuf);
        pBuf = nullptr;
    }
    if (hMapFile) {
        CloseHandle(hMapFile);
        hMapFile = nullptr;
    }
#else
    if (pBuf && pBuf != MAP_FAILED) {
        munmap(pBuf, SHM_SIZE);
        pBuf = nullptr;
    }
    if (shm_fd != -1) {
        close(shm_fd);
        shm_fd = -1;
    }
    shm_unlink(SHM_NAME);
#endif
}
