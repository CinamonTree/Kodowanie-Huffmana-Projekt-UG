#include <memory>
#include <cstddef>
#include <vector>
#include <cstdint>

class HuffmanTree{

private:

    struct Node {
        std::unique_ptr<Node> left_child;
        std::unique_ptr<Node> right_child;
        uint64_t frequency;
        std::vector<std::byte> headers;
    };

    std::unique_ptr<Node> root;

    HuffmanTree(std::unique_ptr<Node> root) {
        this->root = std::move(root);
    }

    std::vector<std::byte> build_headers(const std::vector<std::byte>& headers_left, const std::vector<std::byte>& headers_right) const {
        std::vector<std::byte> new_headers;
        new_headers.reserve(headers_left.size() + headers_right.size());
        new_headers.insert(new_headers.end(), headers_left.begin(), headers_left.end());
        new_headers.insert(new_headers.end(), headers_right.begin(), headers_right.end());
        return new_headers;
    }

    std::unique_ptr<Node> take_root() {
        std::unique_ptr<Node> odr_root = std::move(root);
    }

public:

    HuffmanTree(std::byte header, uint64_t frequency) {
        root = std::make_unique<Node>();
        (*root).frequency = frequency;
        (*root).headers.push_back(header);
    }

    const Node& get_root() const {
        return *root;
    }

    std::unique_ptr<HuffmanTree> rebuild(std::unique_ptr<HuffmanTree> tree_left, std::unique_ptr<HuffmanTree> tree_right) {
        std::unique_ptr<Node> left_node_ptr = (*tree_left).take_root();
        std::unique_ptr<Node> right_node_ptr = (*tree_right).take_root();

        tree_left.reset();
        tree_right.reset();

        const Node& left_node_ref = (*left_node_ptr);
        const Node& right_node_ref = (*right_node_ptr);

        std::vector<std::byte> new_headers = build_headers(left_node_ref.headers, right_node_ref.headers);

        std::unique_ptr<Node> new_node = std::make_unique<Node>();
        Node& new_node_reference = (*new_node);
        new_node_reference.left_child = std::move(left_node_ptr);
        new_node_reference.right_child = std::move(right_node_ptr);
        new_node_reference.frequency = left_node_ref.frequency + right_node_ref.frequency;
        new_node_reference.headers = new_headers;

        std::unique_ptr<HuffmanTree> new_tree = std::make_unique<HuffmanTree>(std::move(new_node));
        return new_tree;
    }

};