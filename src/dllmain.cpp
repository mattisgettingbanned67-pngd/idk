#include "il2cpp_base.hpp"

#include <atomic>
#include <thread>

#ifdef _WIN32
#include <Windows.h>
#endif

namespace {
#ifdef _WIN32
std::atomic<bool> g_running{true};

void mod_entry() {
    if (!il2cpp_base::initialize()) {
        return;
    }

    void* thread = il2cpp_base::attach_current_thread();
    if (!thread) {
        il2cpp_base::shutdown();
        return;
    }

    // TODO: Install your hooks here (for example via MinHook + metadata lookup).
    // Example placeholder call to verify basic IL2CPP API resolution:
    [[maybe_unused]] void* hello = il2cpp_base::create_managed_string("IL2CPP base initialized");

    while (g_running.load(std::memory_order_acquire)) {
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
    }

    il2cpp_base::shutdown();
}
#endif
} // namespace

#ifdef _WIN32
BOOL APIENTRY DllMain(HMODULE module, DWORD reason, LPVOID reserved) {
    switch (reason) {
        case DLL_PROCESS_ATTACH:
            DisableThreadLibraryCalls(module);
            std::thread(mod_entry).detach();
            break;
        case DLL_PROCESS_DETACH:
            g_running.store(false, std::memory_order_release);
            break;
        default:
            break;
    }

    return TRUE;
}
#else
int not_windows_build_placeholder() {
    return 0;
}
#endif
