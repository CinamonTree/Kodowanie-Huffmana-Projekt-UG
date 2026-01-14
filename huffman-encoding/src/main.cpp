#include "arg_paraser.h"
#include "file_handler.h"
#include "huffman_tree.h"
#include "priority_queue.h"

#include <cstdint>
#include <cstring>
#include <iostream>
#include <memory>
#include <map>
#include <utility>
#include <windows.h>

int main(int argc, char* argv[]) {
    SetConsoleOutputCP(CP_UTF8);
    Arguments args = ArgumentParaser::parse_args(argc, argv);
    MinPriorityQueue<HuffmanTree> priority_queue;
    if (args.mode == MODE::COMPRESS) {
        std::map<std::byte, uint32_t> bytes_map = FileHandler::count_bytes(args.input_path);
        for (const auto& frequency_pair : bytes_map) {
            priority_queue.push(frequency_pair.second, std::make_unique<HuffmanTree>(frequency_pair.first, frequency_pair.second));
        }
        while (priority_queue.size() > 1) {
            auto [freq1, tree1] = priority_queue.pop();
            auto [freq2, tree2] = priority_queue.pop();
            HuffmanTree::TreePointer new_tree = HuffmanTree::rebuild(std::move(tree1), std::move(tree2));
            uint32_t new_freq = freq1 + freq2;
            priority_queue.push(new_freq, std::move(new_tree));
        }
        auto [priority, full_tree_ptr] = priority_queue.pop();
        HuffmanTree::Dict codes = (*full_tree_ptr).build_huffman_codes();
    }
};