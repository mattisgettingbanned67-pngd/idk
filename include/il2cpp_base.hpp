#pragma once

#include <cstdint>
#include <string_view>

namespace il2cpp_base {

using il2cpp_domain_get_t = void* (*)();
using il2cpp_thread_attach_t = void* (*)(void* domain);
using il2cpp_string_new_t = void* (*)(const char* str);

struct Api {
    il2cpp_domain_get_t domain_get = nullptr;
    il2cpp_thread_attach_t thread_attach = nullptr;
    il2cpp_string_new_t string_new = nullptr;
};

bool initialize();
void shutdown();

const Api& api();
void* attach_current_thread();
void* create_managed_string(std::string_view value);

} // namespace il2cpp_base
