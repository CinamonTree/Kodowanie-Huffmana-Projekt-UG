#include "arg_paraser.h"
#include "file_handler.h"
#include "huffman_tree.h"
#include "priority_queue.h"

#include <windows.h>
#include <cstdint>
#include <map>
#include <memory>
#include <unordered_map>
#include <string>
#include <type_traits>
#include <stdexcept>

// Uruchamia kompresje lub dekompresje na podstawie argumentow.
int main(int argc, char* argv[]) {
    SetConsoleOutputCP(CP_UTF8);

    Arguments args = ArgumentParaser::parse_args(argc, argv);

    if (args.mode == MODE::COMPRESS) {
        std::map<std::byte, uint32_t> byte_frequencies = FileHandler::count_bytes(args.input_path);
        if (byte_frequencies.empty()) {
            throw std::runtime_error("Plik jest pusty.");
        }
        std::unique_ptr<HuffmanTree> tree = HuffmanTree::from_frequencies(byte_frequencies);
        std::unordered_map<std::byte, std::string> codes = tree->build_huffman_codes();
        FileHandler::write_bits(args.input_path, args.output_path, codes, byte_frequencies);
    }
    else if (args.mode == MODE::DECOMPRESS) {
        FileHandler::decompress_file(args.input_path, args.output_path);
    }

    return 0;
}
