#include "il2cpp_base.hpp"

#include <atomic>
#include <mutex>
#include <string>

#ifdef _WIN32
#include <Windows.h>
#endif

namespace il2cpp_base {
namespace {

Api g_api{};
std::atomic<bool> g_initialized{false};
std::mutex g_init_mutex;

#ifdef _WIN32
void* resolve_export(HMODULE module, const char* name) {
    return reinterpret_cast<void*>(GetProcAddress(module, name));
}
#endif

} // namespace

bool initialize() {
    std::scoped_lock lock(g_init_mutex);

    if (g_initialized.load(std::memory_order_acquire)) {
        return true;
    }

#ifdef _WIN32
    HMODULE game_assembly = GetModuleHandleW(L"GameAssembly.dll");
    if (!game_assembly) {
        return false;
    }

    g_api.domain_get = reinterpret_cast<il2cpp_domain_get_t>(resolve_export(game_assembly, "il2cpp_domain_get"));
    g_api.thread_attach = reinterpret_cast<il2cpp_thread_attach_t>(resolve_export(game_assembly, "il2cpp_thread_attach"));
    g_api.string_new = reinterpret_cast<il2cpp_string_new_t>(resolve_export(game_assembly, "il2cpp_string_new"));

    if (!g_api.domain_get || !g_api.thread_attach || !g_api.string_new) {
        g_api = {};
        return false;
    }

    g_initialized.store(true, std::memory_order_release);
    return true;
#else
    return false;
#endif
}

void shutdown() {
    std::scoped_lock lock(g_init_mutex);
    g_api = {};
    g_initialized.store(false, std::memory_order_release);
}

const Api& api() {
    return g_api;
}

void* attach_current_thread() {
    if (!g_initialized.load(std::memory_order_acquire) || !g_api.domain_get || !g_api.thread_attach) {
        return nullptr;
    }

    void* domain = g_api.domain_get();
    if (!domain) {
        return nullptr;
    }

    return g_api.thread_attach(domain);
}

void* create_managed_string(std::string_view value) {
    if (!g_initialized.load(std::memory_order_acquire) || !g_api.string_new) {
        return nullptr;
    }

    std::string owned(value);
    return g_api.string_new(owned.c_str());
}

} // namespace il2cpp_base
