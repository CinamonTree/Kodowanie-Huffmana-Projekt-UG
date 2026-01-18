#pragma once

#include "huffman_tree.h"

#include <fstream>
#include <string>
#include <unordered_map>
#include <vector>
#include <cstdint>
#include <stdexcept>
#include <map>
#include <sstream>
#include <iterator>

class FileHandler {
public:
    // Zliczanie wystąpień bajtów i zapis do mapy
    static std::map<std::byte, uint32_t> count_bytes(const std::string& file_path) {
        std::ifstream file_stream(file_path, std::ios::binary);
        if (!file_stream.is_open()) {
            throw std::runtime_error("Nie mo��na otworzy�A pliku.");
        }

        std::map<std::byte, uint32_t> frequencies;
        std::vector<unsigned char> buffer(16 * 1024);

        while (file_stream) {
            file_stream.read(reinterpret_cast<char*>(buffer.data()),
                             static_cast<std::streamsize>(buffer.size()));
            std::streamsize n = file_stream.gcount();
            for (std::streamsize i = 0; i < n; ++i) {
                std::byte b = static_cast<std::byte>(buffer[static_cast<size_t>(i)]);
                frequencies[b]++;
            }
        }

        if (file_stream.bad()) {
            throw std::runtime_error("Wystąpił błąd odczytu pliku.");
        }
        return frequencies;
    }

    // Zapisuje do pliku nagłówek i skompresowane dane
    static void write_bits(const std::string& input_path, 
                           const std::string& output_path,
                           const std::unordered_map<std::byte, std::string>& codes,
                           const std::map<std::byte, uint32_t>& bytes_frequencies)
    {
        std::ifstream in(input_path, std::ios::binary);
        if (!in) { 
            throw std::runtime_error("Nie mozna otworzyc pliku wejsciowego");
        }
        std::ofstream out(output_path, std::ios::binary | std::ios::trunc);
        if (!out) { 
            throw std::runtime_error("Nie mozna otworzyc pliku wwyjściowego");
        }

        write_header(out, bytes_frequencies);

        uint8_t buffer = 0;
        int bitCount = 0;

        auto flush_byte = [&]() {
            out.put(static_cast<char>(buffer));
            buffer = 0;
            bitCount = 0;
        };
        auto writeBit = [&](bool bit) {
            buffer = static_cast<uint8_t>((buffer << 1) | (bit ? 1 : 0));
            if (++bitCount == 8) flush_byte();
        };

        char ch;
        while (in.get(ch)) {
            std::byte b = static_cast<std::byte>(static_cast<unsigned char>(ch));
            auto itc = codes.find(b);
            if (itc == codes.end()) {
                throw std::runtime_error("Brak kodu Huffmana dla bajtu");
            }
            const std::string& codeBits = itc->second;
            for (size_t i = 0; i < codeBits.size(); ++i) {
                writeBit(codeBits[i] == '1');
            }
        }

        // padding
        uint8_t padding = 0;
        if (bitCount != 0) {
            padding = static_cast<uint8_t>(8 - bitCount);
            buffer <<= padding;
            out.put(static_cast<char>(buffer));
        }
        out.put(static_cast<char>(padding));
    }

    
    // Odczytuje czestotliwosci z naglowka pliku.
    static std::map<std::byte, uint32_t> get_frequencies(const std::string& file_path) {
        std::ifstream in(file_path, std::ios::binary);
        if (!in) throw std::runtime_error("Nie mo��na otworzy�A pliku");

        return read_header(in);
    }

    // Dekompresuje plik Huffmana do postaci binarnej.
    static void decompress_file(const std::string& input_path,
                                const std::string& output_path)
    {
        std::ifstream in(input_path, std::ios::binary);
        if (!in) throw std::runtime_error("Nie mozna otworzyc pliku wejsciowego");

        std::ofstream out(output_path, std::ios::binary | std::ios::trunc);
        if (!out) throw std::runtime_error("Nie mozna otworzyc pliku wyjsciowego");

        std::map<std::byte, uint32_t> freq = read_header(in);

        std::unique_ptr<HuffmanTree> tree = HuffmanTree::from_frequencies(freq);
        std::vector<unsigned char> tail((std::istreambuf_iterator<char>(in)),std::istreambuf_iterator<char>());

        if (tail.empty()) return;

        uint8_t padding = static_cast<uint8_t>(tail.back());
        tail.pop_back();

        std::vector<uint8_t> payload;
        payload.assign(tail.begin(), tail.end());

        tree->decode_payload_to_stream(payload, padding, out);
    }

private:
    // Koduje bajt do postaci tokenu naglowka.
    static std::string encode_symbol(std::byte symbol) {
        unsigned char c = std::to_integer<unsigned char>(symbol);
        if (c == '\\') return "\\\\";
        if (c == '\n') return "\\n";
        if (c == '\r') return "\\r";
        if (c == '\t') return "\\t";
        if (c == ':') return "\\:";
        if (c == ' ') return "\\s";
        if (c >= 32 && c <= 126) return std::string(1, static_cast<char>(c)); // możliwe do druku ASCII

        const char* hex = "0123456789ABCDEF";
        std::string out = "\\x";
        out.push_back(hex[(c >> 4) & 0xF]);
        out.push_back(hex[c & 0xF]);
        return out;
    }

    // Dekoduje token naglowka do bajtu.
    static std::byte decode_symbol_token(const std::string& token) {
        if (token == "\\n") return std::byte{static_cast<unsigned char>('\n')};
        if (token == "\\r") return std::byte{static_cast<unsigned char>('\r')};
        if (token == "\\t") return std::byte{static_cast<unsigned char>('\t')};
        if (token == "\\:") return std::byte{static_cast<unsigned char>(':')};
        if (token == "\\\\") return std::byte{static_cast<unsigned char>('\\')};
        if (token == "\\s") return std::byte{static_cast<unsigned char>(' ')};

        if (token.size() == 4 && token[0] == '\\' && token[1] == 'x') {
            auto hex_value = [](char h) -> int {
                if (h >= '0' && h <= '9') return h - '0';
                if (h >= 'A' && h <= 'F') return 10 + (h - 'A');
                if (h >= 'a' && h <= 'f') return 10 + (h - 'a');
                return -1;
            };
            int hi = hex_value(token[2]);
            int lo = hex_value(token[3]);
            if (hi < 0 || lo < 0) {
                throw std::runtime_error("Niepoprawny znak w naglowku");
            }
            unsigned char c = static_cast<unsigned char>((hi << 4) | lo);
            return std::byte{c};
        }

        if (token.size() == 1) {
            return std::byte{static_cast<unsigned char>(token[0])};
        }

        throw std::runtime_error("Niepoprawny znak w naglowku");
    }

    // Zapisuje naglowek jako czyste ASCII, a dane pozostawia binarne.
    static void write_header(std::ostream& out, const std::map<std::byte, uint32_t>& bytes_frequencies) {
        std::string header;
        header.reserve(bytes_frequencies.size() * 16);
        bool first = true;
        for (auto it = bytes_frequencies.begin(); it != bytes_frequencies.end(); ++it) {
            if (!first) header += " ";
            first = false;
            header += encode_symbol(it->first);
            header += ":";
            header += std::to_string(it->second);
        }
        header += "\n";
        header += "\n";
        out.write(header.data(), static_cast<std::streamsize>(header.size()));
    }

    // Czyta naglowek i zwraca mape czestotliwosci.
    static std::map<std::byte, uint32_t> read_header(std::istream& in) {
        std::map<std::byte, uint32_t> freq;
        std::string line;
        bool saw_blank = false;

        while (std::getline(in, line)) {
            if (line.empty()) {
                saw_blank = true;
                break;
            }
            std::istringstream iss(line);
            std::string pair_token;
            while (iss >> pair_token) {
                size_t colon = pair_token.find(':');
                if (colon == std::string::npos || colon == 0) {
                    throw std::runtime_error("Niepoprawny naglowek pliku");
                }

                std::string token = pair_token.substr(0, colon);
                std::string number = pair_token.substr(colon + 1);
                if (number.empty()) {
                    throw std::runtime_error("Niepoprawny naglowek pliku");
                }

                uint32_t frequency = static_cast<uint32_t>(std::stoul(number));
                std::byte symbol = decode_symbol_token(token);
                freq[symbol] = frequency;
            }
        }

        if (!in && !saw_blank) {
            throw std::runtime_error("Niepoprawny naglowek pliku");
        }

        return freq;
    }
};
