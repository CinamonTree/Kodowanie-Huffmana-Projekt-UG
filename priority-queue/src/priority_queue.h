#pragma once

#include <vector>
#include <memory>
#include <utility>
#include <cstddef>

template <typename T>
class MinPriorityQueue {

public:
    void push(unsigned int priority, std::unique_ptr<T> value) {
        heap.push_back({priority, std::move(value)});
        build_up(heap.size() - 1);
    }

    std::pair<unsigned int, std::unique_ptr<T>> pop() {
        if (heap.empty())
            return {0u, nullptr};

        auto result = std::make_pair(heap[0].first, std::move(heap[0].second));
        if (heap.size() == 1) {
            heap.pop_back();
            return result;
        }
        heap[0] = std::move(heap.back());
        heap.pop_back();
        build_down(0);
        return result;
    }
    
    void build(const std::vector<std::pair<unsigned int, T>>& items) {
        heap.clear();
        heap.reserve(items.size());
        for (const auto& [priority, value] : items) {
            heap.push_back(std::pair<unsigned int, std::unique_ptr<T>>{priority, std::make_unique<T>(value)});
        }
        for (size_t i = heap.size() / 2; i-- > 0; ) {
            build_down(i);
        }
    }

    bool isEmpty() const {
        return heap.empty();
    };

    // zwraca kolejkę z kopią priorytetu i wskaźnikiem tylko do wglądu.
    std::vector<std::pair<unsigned int, const T*>> get_view() {
        std::vector<std::pair<unsigned int, const T*>> view_array;
        view_array.reserve(heap.size());
        for (const auto& [priority, value] : heap) {
            view_array.push_back({priority, value.get()});
        }
        return view_array;
    }

private:
    std::vector<std::pair<unsigned int, std::unique_ptr<T>>> heap;

    void build_up(size_t index) {
        while (index > 0) {
            size_t parent_i = parent(index);
            if (heap[parent_i].first <= heap[index].first)
                break;
            std::swap(heap[parent_i], heap[index]);
            index = parent_i;
        }
    }

    void build_down(size_t index) {
        size_t smallest = index;
        size_t left = left_son(index);
        size_t right = right_son(index);
        if (left < heap.size() && heap[left].first < heap[smallest].first)
            smallest = left;

        if (right < heap.size() && heap[right].first < heap[smallest].first)
            smallest = right;

        if (smallest != index) {
            std::swap(heap[index], heap[smallest]);
            build_down(smallest);
        }
    }

    constexpr size_t left_son(size_t index) const {
        return 2 * index + 1;
    }

    constexpr size_t right_son(size_t index) const {
        return 2 * index + 2;
    }

    constexpr size_t parent(size_t index) const {
        return  (index - 1) / 2;
    }
};