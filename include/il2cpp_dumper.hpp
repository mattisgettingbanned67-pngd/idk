#pragma once

#include <cstdint>
#include <filesystem>
#include <string>
#include <string_view>
#include <vector>

namespace il2cpp_dumper {

struct MethodRecord {
    std::string type_namespace;
    std::string type_name;
    std::string method_name;
    std::uint32_t token = 0;
    std::string rva;
};

struct DumpResult {
    std::vector<MethodRecord> methods;
    std::vector<std::pair<std::string, std::uint32_t>> exports;
};

DumpResult dump(const std::filesystem::path& metadata_path, const std::filesystem::path& game_assembly_path);
bool write_csv(const std::filesystem::path& out_path, const DumpResult& result);

} // namespace il2cpp_dumper
