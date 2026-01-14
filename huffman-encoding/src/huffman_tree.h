#include <memory>
#include <cstddef>
#include <vector>
#include <cstdint>
#include <unordered_map>

class HuffmanTree{

private:

    struct Node;
    using NodePointer = std::unique_ptr<Node>;
    using Headers = std::vector<std::byte>;

    struct Node {
        NodePointer left_child;
        NodePointer right_child;
        uint32_t frequency;
        Headers headers;
    };

    NodePointer root;

public:

    using Code = std::vector<bool>;
    using Dict = std::unordered_map<std::byte, Code>;
    using TreePointer = std::unique_ptr<HuffmanTree>;

    HuffmanTree(std::byte header, uint32_t frequency) {
        root = std::make_unique<Node>();
        (*root).frequency = frequency;
        (*root).headers.push_back(header);
    }

    static TreePointer rebuild(TreePointer tree_left, TreePointer tree_right) {
        NodePointer left_node_ptr = (*tree_left).take_root();
        NodePointer right_node_ptr = (*tree_right).take_root();

        tree_left.reset();
        tree_right.reset();

        const Node& left_node_ref = (*left_node_ptr);
        const Node& right_node_ref = (*right_node_ptr);

        Headers new_headers = build_headers(left_node_ref.headers, right_node_ref.headers);

        NodePointer new_node = std::make_unique<Node>();
        Node& new_node_reference = (*new_node);
        new_node_reference.left_child = std::move(left_node_ptr);
        new_node_reference.right_child = std::move(right_node_ptr);
        new_node_reference.frequency = left_node_ref.frequency + right_node_ref.frequency;
        new_node_reference.headers = std::move(new_headers);

        TreePointer new_tree(new HuffmanTree(std::move(new_node)));
        return new_tree;
    }

    Dict build_huffman_codes() const {
        Dict dict;
        if (!root) return dict;
        Code code;
        build_dict(*root, code, dict);
        if (is_leaf(*root) && !root->headers.empty() && dict[root->headers[0]].empty()) {
            dict[root->headers[0]] = Code{false};
        }
        return dict;
    }

private:

    HuffmanTree(NodePointer root) {
        this->root = std::move(root);
    }

    static void build_dict(const Node& node, Code& code, Dict& dict) {
        if (is_leaf(node)) {
            if (!node.headers.empty()) dict[node.headers[0]] = code;
            return;
        }
        if (node.left_child) {
            code.push_back(false);
            build_dict(*node.left_child, code, dict);
            code.pop_back();
        }
        if (node.right_child) {
            code.push_back(true);
            build_dict(*node.right_child, code, dict);
            code.pop_back();
        }
    }

    static std::vector<std::byte> build_headers(const std::vector<std::byte>& headers_left, const std::vector<std::byte>& headers_right) {
        std::vector<std::byte> new_headers;
        new_headers.reserve(headers_left.size() + headers_right.size());
        new_headers.insert(new_headers.end(), headers_left.begin(), headers_left.end());
        new_headers.insert(new_headers.end(), headers_right.begin(), headers_right.end());
        return new_headers;
    }

    static bool is_leaf(const Node& n) {
        return !n.left_child && !n.right_child;
    }

    NodePointer take_root() {
        return std::move(root);
    }
    
};