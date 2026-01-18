#pragma once

#include "priority_queue.h"

#include <memory>
#include <cstddef>
#include <vector>
#include <cstdint>
#include <unordered_map>
#include <string>
#include <map>
#include <type_traits>
#include <stdexcept>
#include <ostream>
#include <istream>
#include <algorithm>

class HuffmanTree {
private:
    // Węzeł
    struct Node {
        std::unique_ptr<Node> left_child;
        std::unique_ptr<Node> right_child;
        std::uint32_t frequency = 0;
        std::vector<std::byte> headers;
    };

    std::unique_ptr<Node> root;

public:
    // Konstruktor standardowy
    HuffmanTree(std::byte header, std::uint32_t frequency) {
        root = std::make_unique<Node>();
        root->frequency = frequency;
        root->headers.push_back(header);
    }

    // Budowa drzewa z dwóch poddrzew
    static std::unique_ptr<HuffmanTree> rebuild(std::unique_ptr<HuffmanTree> tree_left,std::unique_ptr<HuffmanTree> tree_right) {
        std::unique_ptr<Node> left_node_ptr = tree_left->take_root();
        std::unique_ptr<Node> right_node_ptr = tree_right->take_root();

        tree_left.reset();
        tree_right.reset();

        const Node& left_node_ref = *left_node_ptr;
        const Node& right_node_ref = *right_node_ptr;

        std::vector<std::byte> new_headers = build_headers(left_node_ref.headers, right_node_ref.headers);

        std::unique_ptr<Node> new_node = std::make_unique<Node>();
        Node& new_node_reference = *new_node;
        new_node_reference.left_child = std::move(left_node_ptr);
        new_node_reference.right_child = std::move(right_node_ptr);
        new_node_reference.frequency = left_node_ref.frequency + right_node_ref.frequency;
        new_node_reference.headers = std::move(new_headers);

        return std::unique_ptr<HuffmanTree>(new HuffmanTree(std::move(new_node)));
    }

    // Buduje mapy kodow Huffmana dla lisci.
    std::unordered_map<std::byte, std::string> build_huffman_codes() const {
        std::unordered_map<std::byte, std::string> codes;
        if (!root) return codes;

        std::string code;
        build_codes(*root, code, codes);

        if (is_leaf(*root) && !root->headers.empty() && codes[root->headers[0]].empty()) {
            codes[root->headers[0]] = "0";
        }
        return codes;
    }

    // Budowa drzewa ze słownika częstotliwości
    static std::unique_ptr<HuffmanTree> from_frequencies(const std::map<std::byte, uint32_t>& frequencies) {
        MinPriorityQueue<HuffmanTree> pq;
        for (auto it = frequencies.begin(); it != frequencies.end(); ++it) {
            std::byte b = it->first;
            uint32_t f = it->second;

            unsigned int tie = std::to_integer<unsigned int>(b);
            pq.push(f, tie, std::make_unique<HuffmanTree>(b, f));
        }

        while (pq.size() > 1) {
            auto p1 = pq.pop();
            auto p2 = pq.pop();

            unsigned int f1 = p1.first.first;
            unsigned int t1 = p1.first.second;
            std::unique_ptr<HuffmanTree> tree1 = std::move(p1.second);

            unsigned int f2 = p2.first.first;
            unsigned int t2 = p2.first.second;
            std::unique_ptr<HuffmanTree> tree2 = std::move(p2.second);

            std::unique_ptr<HuffmanTree> new_tree = HuffmanTree::rebuild(std::move(tree1), std::move(tree2));
            unsigned int new_freq = f1 + f2;
            unsigned int new_tie  = (t1 < t2) ? t1 : t2;

            pq.push(new_freq, new_tie, std::move(new_tree));
        }

        auto last = pq.pop();
        return std::move(last.second);
    }

    // dekoduje
    void decode_payload_to_stream(const std::vector<uint8_t>& payload,uint8_t padding, std::ostream& out) const {
        if (!root) return;
        if (padding > 7) throw std::runtime_error("Niepoprawny padding");

        const Node* node = root.get();

        for (size_t i = 0; i < payload.size(); ++i) {
            uint8_t byte = payload[i];

            int bits = 8;
            if (i == payload.size() - 1) bits -= padding;
            if (bits < 0) bits = 0;

            for (int b = 7; b >= 8 - bits; --b) {
                bool bit = ((byte >> b) & 1) != 0;
                node = bit ? node->right_child.get() : node->left_child.get();

                if (is_leaf(*node)) {
                    out.put(static_cast<char>(std::to_integer<unsigned char>(node->headers[0])));
                    node = root.get();
                }
            }
        }
    }

private:
    // Rekurencyjna budowa kodów jako stringi
    static void build_codes(const Node& node, std::string& code, std::unordered_map<std::byte, std::string>& codes) {
        if (is_leaf(node)) {
            if (!node.headers.empty()) codes[node.headers[0]] = code;
            return;
        }
        if (node.left_child) {
            code.push_back('0');
            build_codes(*node.left_child, code, codes);
            code.pop_back();
        }
        if (node.right_child) {
            code.push_back('1');
            build_codes(*node.right_child, code, codes);
            code.pop_back();
        }
    }

    // Konstruktor prywatny drzewa z węzła
    explicit HuffmanTree(std::unique_ptr<Node> root_node) {
        root = std::move(root_node);
    }

    // Budowa nowych nagłówków w trakcie łączenia węzłów
    static std::vector<std::byte> build_headers(const std::vector<std::byte>& headers_left, const std::vector<std::byte>& headers_right){
        std::vector<std::byte> new_headers;
        new_headers.reserve(headers_left.size() + headers_right.size());
        new_headers.insert(new_headers.end(), headers_left.begin(), headers_left.end());
        new_headers.insert(new_headers.end(), headers_right.begin(), headers_right.end());
        return new_headers;
    }

    // Test czy węzeł jest liściem
    static bool is_leaf(const Node& n) {
        return !n.left_child && !n.right_child;
    }

    // Oddaje orginalny wskaźnik na korzeń drzewa
    std::unique_ptr<Node> take_root() {
        return std::move(root);
    }
};
