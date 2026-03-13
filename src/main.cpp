#include "il2cpp_dumper.hpp"

#include <filesystem>
#include <iostream>
#include <string>

namespace {

std::string strip_quotes(std::string value) {
    if (value.size() >= 2 && value.front() == '"' && value.back() == '"') {
        return value.substr(1, value.size() - 2);
    }
    return value;
}

std::filesystem::path ask_path(const std::string& prompt) {
    std::cout << prompt << "\n> ";
    std::string input;
    std::getline(std::cin, input);
    return std::filesystem::path(strip_quotes(input));
}

} // namespace

int main(int argc, char** argv) {
    std::cout << "=== IL2CPP Offset Dumper (CLI) ===\n";
    std::cout << "Tip: Drag and drop files into this window to auto-fill paths.\n\n";

    std::filesystem::path metadata;
    std::filesystem::path game_assembly;

    if (argc >= 3) {
        metadata = strip_quotes(argv[1]);
        game_assembly = strip_quotes(argv[2]);
    } else {
        metadata = ask_path("Drop global-metadata.dat path:");
        game_assembly = ask_path("Drop GameAssembly.dll path:");
    }

    if (!std::filesystem::exists(metadata)) {
        std::cerr << "[!] Metadata file not found: " << metadata << "\n";
        return 1;
    }
    if (!std::filesystem::exists(game_assembly)) {
        std::cerr << "[!] GameAssembly file not found: " << game_assembly << "\n";
        return 1;
    }

    try {
        const auto result = il2cpp_dumper::dump(metadata, game_assembly);

        const auto output = std::filesystem::current_path() / "il2cpp_dump.csv";
        if (!il2cpp_dumper::write_csv(output, result)) {
            std::cerr << "[!] Failed to write output CSV.\n";
            return 1;
        }

        std::cout << "[+] Dump complete.\n";
        std::cout << "    Methods discovered: " << result.methods.size() << "\n";
        std::cout << "    Exports discovered: " << result.exports.size() << "\n";
        std::cout << "    Output: " << output << "\n";
        std::cout << "\nNote: RVA auto-matching for metadata methods depends on exported-symbol name matches.\n";
    } catch (const std::exception& ex) {
        std::cerr << "[!] Dump failed: " << ex.what() << "\n";
        return 1;
    }

    return 0;
}
