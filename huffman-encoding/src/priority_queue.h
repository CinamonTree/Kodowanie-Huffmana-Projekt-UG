#pragma once

#include <vector>
#include <memory>
#include <utility>
#include <cstddef>

template <typename T>
class MinPriorityQueue {

private:

public:

    struct Item {
        unsigned int priority;
        unsigned int tie;
        std::unique_ptr<T> value;
    };

private:

    std::vector<Item> heap;

public:
    // Dodaje element z priorytetem i wartoscia.
    void push(Item item) {
        heap.push_back(std::move(item));
        build_up(heap.size() - 1);
    }

    // Usuwa i zwraca element o najmniejszym priorytecie.
    std::pair<std::pair<unsigned int, unsigned int>, std::unique_ptr<T>> pop() {
        auto result = std::make_pair(std::make_pair(heap[0].priority, heap[0].tie), std::move(heap[0].value));

        if (heap.size() == 1) {
            heap.pop_back();
            return result;
        }

        heap[0] = std::move(heap.back());
        heap.pop_back();
        build_down(0);
        return result;
    }

    // Sprawdza, czy kolejka jest pusta.
    bool isEmpty() const {
        return heap.empty();
    }

    // Zwraca liczbe elementow w kolejce.
    size_t size() const {
        return heap.size();
    }

    // Zwraca widok danych bez przenoszenia wlasnosci.
    std::vector<std::pair<std::pair<unsigned int, unsigned int>, const T*>> get_view() const {
        std::vector<std::pair<std::pair<unsigned int, unsigned int>, const T*>> view_array;
        view_array.reserve(heap.size());
        for (const auto& it : heap) {
            view_array.push_back({{it.priority, it.tie}, it.value.get()});
        }
        return view_array;
    }

    static MinPriorityQueue<T> linear_build(std::vector<Item> items) {
        MinPriorityQueue<T> queue;
        queue.heap.clear();
        queue.heap.reserve(items.size());
        for (auto& item : items) {
            queue.heap.push_back(std::move(item));
        }
        if (!queue.heap.empty()) {
            for (size_t i = queue.heap.size() / 2; i > 0; i--) {
                queue.build_down(i);
            }
        }
        return queue;
    }

private:

    // Porownuje elementy po priorytecie i tie-breaku.
    static bool less(const Item& a, const Item& b) {
        if (a.priority != b.priority) return a.priority < b.priority;
        return a.tie < b.tie;
    }

    // Przywraca kopiec, przesuwajac element w gore.
    void build_up(size_t index) {
        while (index > 0) {
            size_t parent_i = parent(index);
            if (!less(heap[index], heap[parent_i]))
                break;
            std::swap(heap[parent_i], heap[index]);
            index = parent_i;
        }
    }

    // Przywraca kopiec, przesuwajac element w dol.
    void build_down(size_t index) {
        size_t smallest = index;
        size_t left = left_son(index);
        size_t right = right_son(index);

        if (left < heap.size() && less(heap[left], heap[smallest]))
            smallest = left;

        if (right < heap.size() && less(heap[right], heap[smallest]))
            smallest = right;

        if (smallest != index) {
            std::swap(heap[index], heap[smallest]);
            build_down(smallest);
        }
    }

    // Zwraca indeks lewego dziecka.
    constexpr size_t left_son(size_t index) const {
        return 2 * index + 1;
    }

    // Zwraca indeks prawego dziecka.
    constexpr size_t right_son(size_t index) const {
        return 2 * index + 2;
    }

    // Zwraca indeks rodzica.
    constexpr size_t parent(size_t index) const {
        return (index - 1) / 2;
    }
};
