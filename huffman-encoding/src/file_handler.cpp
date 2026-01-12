#include <fstream>
#include <string>
#include <map>
#include <vector>
#include <cstdint>
#include <stdexcept>

class FileHandler {
private:

    std::ifstream file_stream;

public:

    FileHandler(const std::string& path) {
        file_stream.open(path, std::ios::in | std::ios::binary);
        if (!file_stream.is_open()) {
            throw std::runtime_error("Nie można otworzyć pliku");
        }
        if (!file_stream.good()) {
            throw std::runtime_error("Wystąpił błąd podczas otwierania pliku");
        }
    }

    std::map<std::byte, uint32_t> count_bytes() {
        file_stream.clear();
        file_stream.seekg(0, std::ios::beg);
        std::map<std::byte, uint32_t> frequencies;
        std::vector<char> buffer(16 * 1024);
        while (true) {
            file_stream.read(buffer.data(), static_cast<std::streamsize>(buffer.size()));
            std::streamsize readed_count =  file_stream.gcount();
            if (readed_count <= 0)
                break;
            for (size_t i = 0; i < static_cast<size_t>(readed_count); ++i) {
                std::byte byte = std::byte{ static_cast<unsigned char>(buffer[i]) };
                frequencies[byte]++;
            }
        }
        return frequencies;
    }
    
};
