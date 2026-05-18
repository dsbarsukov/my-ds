#pragma once

#include <cstddef>
#include <stdexcept>
#include <vector>
#include <algorithm>

template<typename T>
class BinaryHeap {
private:
    std::vector<T> data_;

    std::size_t parent_index(std::size_t index) const {
        return (index - 1) / 2;
    }

    std::size_t left_child_index(std::size_t index) const {
        return 2 * index + 1;
    }

    std::size_t right_child_index(std::size_t index) const {
        return 2 * index + 2;
    }

    void swap_values(std::size_t first_index, std::size_t second_index) {
        std::swap(data_[first_index], data_[second_index]);
    }

    void sift_up(std::size_t index) {
        while (index > 0) {
            std::size_t parent = parent_index(index);

            if (data_[parent] <= data_[index]) {
                break;
            }

            swap_values(parent, index);
            index = parent;
        }
    }

    void sift_down(std::size_t index) {
        std::size_t heap_size = data_.size();

        while (true) {
            std::size_t left = left_child_index(index);
            std::size_t right = right_child_index(index);
            std::size_t smallest = index;

            if (left < heap_size && data_[left] < data_[smallest]) {
                smallest = left;
            }

            if (right < heap_size && data_[right] < data_[smallest]) {
                smallest = right;
            }

            if (smallest == index) {
                break;
            }

            swap_values(index, smallest);
            index = smallest;
        }
    }

    void heapify() {
        if (data_.empty()) {
            return;
        }

        for (std::size_t index = data_.size() / 2; index > 0; --index) {
            sift_down(index - 1);
        }
    }

public:
    BinaryHeap() = default;

    explicit BinaryHeap(const std::vector<T>& values): data_(values) {
        heapify();
    }

    void push(const T& value) {
        data_.push_back(value);
        sift_up(data_.size() - 1);
    }

    T pop() {
        if (data_.empty()) {
            throw std::out_of_range("pop from empty heap");
        }

        T root = data_[0];
        T last_value = data_.back();
        data_.pop_back();

        if (!data_.empty()) {
            data_[0] = last_value;
            sift_down(0);
        }

        return root;
    }

    const T& peek() const {
        if (data_.empty()) {
            throw std::out_of_range("peek from empty heap");
        }

        return data_[0];
    }

    BinaryHeap copy() const {
        BinaryHeap new_heap;
        new_heap.data_ = data_;
        return new_heap;
    }

    std::vector<T> to_list() const {
        return data_;
    }

    std::size_t size() const {
        return data_.size();
    }

    bool empty() const {
        return data_.empty();
    }

    bool contains(const T& value) const {
        for (const T& current_value : data_) {
            if (current_value == value) {
                return true;
            }
        }
        return false;
    }
};