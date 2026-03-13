#include "il2cpp_dumper.hpp"

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <stdexcept>
#include <unordered_map>

namespace il2cpp_dumper {
namespace {

#pragma pack(push, 1)
struct Il2CppGlobalMetadataHeader {
    std::uint32_t sanity;
    std::uint32_t version;
    std::uint32_t string_literal_offset;
    std::uint32_t string_literal_count;
    std::uint32_t string_literal_data_offset;
    std::uint32_t string_literal_data_count;
    std::uint32_t string_offset;
    std::uint32_t string_count;
    std::uint32_t events_offset;
    std::uint32_t events_count;
    std::uint32_t properties_offset;
    std::uint32_t properties_count;
    std::uint32_t methods_offset;
    std::uint32_t methods_count;
    std::uint32_t parameter_default_values_offset;
    std::uint32_t parameter_default_values_count;
    std::uint32_t field_default_values_offset;
    std::uint32_t field_default_values_count;
    std::uint32_t field_and_parameter_default_value_data_offset;
    std::uint32_t field_and_parameter_default_value_data_count;
    std::uint32_t field_marshaled_sizes_offset;
    std::uint32_t field_marshaled_sizes_count;
    std::uint32_t parameters_offset;
    std::uint32_t parameters_count;
    std::uint32_t fields_offset;
    std::uint32_t fields_count;
    std::uint32_t generic_parameters_offset;
    std::uint32_t generic_parameters_count;
    std::uint32_t generic_parameter_constraints_offset;
    std::uint32_t generic_parameter_constraints_count;
    std::uint32_t generic_containers_offset;
    std::uint32_t generic_containers_count;
    std::uint32_t nested_types_offset;
    std::uint32_t nested_types_count;
    std::uint32_t interfaces_offset;
    std::uint32_t interfaces_count;
    std::uint32_t vtable_methods_offset;
    std::uint32_t vtable_methods_count;
    std::uint32_t interface_offsets_offset;
    std::uint32_t interface_offsets_count;
    std::uint32_t type_definitions_offset;
    std::uint32_t type_definitions_count;
};

struct Il2CppTypeDefinition {
    std::uint32_t name_index;
    std::uint32_t namespace_index;
    std::int32_t byval_type_index;
    std::int32_t byref_type_index;
    std::int32_t declaring_type_index;
    std::int32_t parent_index;
    std::int32_t element_type_index;
    std::int32_t generic_container_index;
    std::uint32_t flags;
    std::int32_t field_start;
    std::int32_t method_start;
    std::int32_t event_start;
    std::int32_t property_start;
    std::int32_t nested_types_start;
    std::int32_t interfaces_start;
    std::int32_t vtable_start;
    std::int32_t interface_offsets_start;
    std::uint16_t method_count;
    std::uint16_t property_count;
    std::uint16_t field_count;
    std::uint16_t event_count;
    std::uint16_t nested_type_count;
    std::uint16_t vtable_count;
    std::uint16_t interfaces_count;
    std::uint16_t interface_offsets_count;
    std::uint32_t bitfield;
    std::uint32_t token;
};

struct Il2CppMethodDefinition {
    std::int32_t name_index;
    std::int32_t declaring_type;
    std::int32_t return_type;
    std::int32_t parameter_start;
    std::int32_t generic_container_index;
    std::uint32_t token;
    std::uint16_t flags;
    std::uint16_t iflags;
    std::uint16_t slot;
    std::uint16_t parameter_count;
};

struct ImageDosHeader {
    std::uint16_t e_magic;
    std::uint8_t pad[58];
    std::int32_t e_lfanew;
};

struct ImageFileHeader {
    std::uint16_t machine;
    std::uint16_t number_of_sections;
    std::uint32_t time_date_stamp;
    std::uint32_t pointer_to_symbol_table;
    std::uint32_t number_of_symbols;
    std::uint16_t size_of_optional_header;
    std::uint16_t characteristics;
};

struct ImageDataDirectory {
    std::uint32_t virtual_address;
    std::uint32_t size;
};

struct ImageOptionalHeader64 {
    std::uint16_t magic;
    std::uint8_t pad[110];
    std::uint32_t number_of_rva_and_sizes;
    ImageDataDirectory data_directory[16];
};

struct ImageNtHeaders64 {
    std::uint32_t signature;
    ImageFileHeader file_header;
    ImageOptionalHeader64 optional_header;
};

struct ImageSectionHeader {
    char name[8];
    union {
        std::uint32_t physical_address;
        std::uint32_t virtual_size;
    } misc;
    std::uint32_t virtual_address;
    std::uint32_t size_of_raw_data;
    std::uint32_t pointer_to_raw_data;
    std::uint32_t pointer_to_relocations;
    std::uint32_t pointer_to_linenumbers;
    std::uint16_t number_of_relocations;
    std::uint16_t number_of_linenumbers;
    std::uint32_t characteristics;
};

struct ImageExportDirectory {
    std::uint32_t characteristics;
    std::uint32_t time_date_stamp;
    std::uint16_t major_version;
    std::uint16_t minor_version;
    std::uint32_t name;
    std::uint32_t base;
    std::uint32_t number_of_functions;
    std::uint32_t number_of_names;
    std::uint32_t address_of_functions;
    std::uint32_t address_of_names;
    std::uint32_t address_of_name_ordinals;
};
#pragma pack(pop)

std::vector<std::byte> read_file(const std::filesystem::path& path) {
    std::ifstream file(path, std::ios::binary);
    if (!file) {
        throw std::runtime_error("Failed to open file: " + path.string());
    }

    file.seekg(0, std::ios::end);
    const auto size = file.tellg();
    file.seekg(0, std::ios::beg);

    std::vector<std::byte> data(static_cast<std::size_t>(size));
    if (!data.empty()) {
        file.read(reinterpret_cast<char*>(data.data()), static_cast<std::streamsize>(data.size()));
    }
    return data;
}

template <typename T>
const T* view_at(const std::vector<std::byte>& bytes, std::size_t offset) {
    if (offset + sizeof(T) > bytes.size()) {
        return nullptr;
    }
    return reinterpret_cast<const T*>(bytes.data() + offset);
}

std::string metadata_string(const std::vector<std::byte>& bytes, const Il2CppGlobalMetadataHeader& header, std::uint32_t index) {
    if (index >= header.string_count) {
        return {};
    }

    const auto start = static_cast<std::size_t>(header.string_offset) + index;
    if (start >= bytes.size()) {
        return {};
    }

    const char* cstr = reinterpret_cast<const char*>(bytes.data() + start);
    const std::size_t max_len = bytes.size() - start;
    std::size_t len = 0;
    while (len < max_len && cstr[len] != '\0') {
        ++len;
    }
    return std::string(cstr, len);
}

std::string hex_rva(std::uint32_t rva) {
    std::ostringstream oss;
    oss << "0x" << std::uppercase << std::hex << rva;
    return oss.str();
}

std::size_t rva_to_raw(std::uint32_t rva, const std::vector<ImageSectionHeader>& sections) {
    for (const auto& section : sections) {
        const auto section_va = section.virtual_address;
        const auto section_size = std::max(section.misc.virtual_size, section.size_of_raw_data);
        if (rva >= section_va && rva < section_va + section_size) {
            return static_cast<std::size_t>(section.pointer_to_raw_data + (rva - section_va));
        }
    }
    throw std::runtime_error("Could not map RVA to raw file offset.");
}

std::vector<std::pair<std::string, std::uint32_t>> parse_exports(const std::vector<std::byte>& pe_bytes) {
    const auto* dos = view_at<ImageDosHeader>(pe_bytes, 0);
    if (!dos || dos->e_magic != 0x5A4D) {
        throw std::runtime_error("Invalid DOS header in GameAssembly.");
    }

    const auto* nt = view_at<ImageNtHeaders64>(pe_bytes, static_cast<std::size_t>(dos->e_lfanew));
    if (!nt || nt->signature != 0x00004550) {
        throw std::runtime_error("Invalid NT header in GameAssembly.");
    }

    const std::size_t sections_offset = static_cast<std::size_t>(dos->e_lfanew) + sizeof(std::uint32_t) + sizeof(ImageFileHeader) + nt->file_header.size_of_optional_header;
    std::vector<ImageSectionHeader> sections;
    sections.reserve(nt->file_header.number_of_sections);
    for (std::size_t i = 0; i < nt->file_header.number_of_sections; ++i) {
        const auto* section = view_at<ImageSectionHeader>(pe_bytes, sections_offset + i * sizeof(ImageSectionHeader));
        if (!section) {
            throw std::runtime_error("Invalid section table in GameAssembly.");
        }
        sections.push_back(*section);
    }

    const auto export_dir = nt->optional_header.data_directory[0];
    if (export_dir.virtual_address == 0 || export_dir.size == 0) {
        return {};
    }

    const auto export_raw = rva_to_raw(export_dir.virtual_address, sections);
    const auto* exports = view_at<ImageExportDirectory>(pe_bytes, export_raw);
    if (!exports) {
        throw std::runtime_error("Invalid export directory.");
    }

    const auto names_raw = rva_to_raw(exports->address_of_names, sections);
    const auto ordinals_raw = rva_to_raw(exports->address_of_name_ordinals, sections);
    const auto functions_raw = rva_to_raw(exports->address_of_functions, sections);

    std::vector<std::pair<std::string, std::uint32_t>> result;
    for (std::size_t i = 0; i < exports->number_of_names; ++i) {
        const auto* name_rva_ptr = view_at<std::uint32_t>(pe_bytes, names_raw + i * sizeof(std::uint32_t));
        const auto* ordinal_ptr = view_at<std::uint16_t>(pe_bytes, ordinals_raw + i * sizeof(std::uint16_t));
        if (!name_rva_ptr || !ordinal_ptr) {
            continue;
        }

        const auto name_raw = rva_to_raw(*name_rva_ptr, sections);
        const char* name_cstr = reinterpret_cast<const char*>(pe_bytes.data() + name_raw);
        const auto* fn_rva_ptr = view_at<std::uint32_t>(pe_bytes, functions_raw + *ordinal_ptr * sizeof(std::uint32_t));
        if (!fn_rva_ptr) {
            continue;
        }

        result.emplace_back(std::string(name_cstr), *fn_rva_ptr);
    }

    std::sort(result.begin(), result.end(), [](const auto& a, const auto& b) { return a.first < b.first; });
    return result;
}

} // namespace

DumpResult dump(const std::filesystem::path& metadata_path, const std::filesystem::path& game_assembly_path) {
    const auto metadata_bytes = read_file(metadata_path);
    const auto game_assembly_bytes = read_file(game_assembly_path);

    const auto* header = view_at<Il2CppGlobalMetadataHeader>(metadata_bytes, 0);
    if (!header || header->sanity != 0xFAB11BAF) {
        throw std::runtime_error("Invalid or unsupported global-metadata.dat header.");
    }

    const auto* type_definitions = view_at<Il2CppTypeDefinition>(metadata_bytes, header->type_definitions_offset);
    const auto* method_definitions = view_at<Il2CppMethodDefinition>(metadata_bytes, header->methods_offset);
    if (!type_definitions || !method_definitions) {
        throw std::runtime_error("Metadata tables are out of range.");
    }

    const std::size_t type_count = header->type_definitions_count / sizeof(Il2CppTypeDefinition);
    const std::size_t method_count = header->methods_count / sizeof(Il2CppMethodDefinition);

    std::unordered_map<std::uint32_t, std::uint32_t> export_map;
    DumpResult result;
    result.exports = parse_exports(game_assembly_bytes);

    for (const auto& [name, rva] : result.exports) {
        export_map.emplace(static_cast<std::uint32_t>(std::hash<std::string>{}(name)), rva);
    }

    for (std::size_t i = 0; i < type_count; ++i) {
        const auto& type = type_definitions[i];
        const auto type_name = metadata_string(metadata_bytes, *header, type.name_index);
        const auto type_ns = metadata_string(metadata_bytes, *header, type.namespace_index);

        const auto start = std::max(0, type.method_start);
        const auto end = std::min<std::size_t>(method_count, static_cast<std::size_t>(start + type.method_count));
        for (std::size_t method_index = static_cast<std::size_t>(start); method_index < end; ++method_index) {
            const auto& method = method_definitions[method_index];
            const auto method_name = metadata_string(metadata_bytes, *header, static_cast<std::uint32_t>(method.name_index));

            MethodRecord record;
            record.type_namespace = type_ns;
            record.type_name = type_name;
            record.method_name = method_name;
            record.token = method.token;

            const std::string fq_name = type_name + "::" + method_name;
            const auto it = export_map.find(static_cast<std::uint32_t>(std::hash<std::string>{}(fq_name)));
            record.rva = (it != export_map.end()) ? hex_rva(it->second) : "N/A";

            result.methods.push_back(std::move(record));
        }
    }

    return result;
}

bool write_csv(const std::filesystem::path& out_path, const DumpResult& result) {
    std::ofstream out(out_path);
    if (!out) {
        return false;
    }

    out << "Namespace,Class,Method,Token,RVA\n";
    for (const auto& m : result.methods) {
        out << '"' << m.type_namespace << "\","
            << m.type_name << "\","
            << m.method_name << "\","
            << "0x" << std::uppercase << std::hex << m.token << std::dec << "\"," << m.rva << "\n";
    }

    out << "\nExport,ExportRVA\n";
    for (const auto& [name, rva] : result.exports) {
        out << '"' << name << "\"," << hex_rva(rva) << "\n";
    }

    return true;
}

} // namespace il2cpp_dumper
