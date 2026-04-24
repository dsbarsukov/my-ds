#pragma once

#include <vector>
#include <stdexcept>
#include <functional>
#include <optional>
#include <tuple>

template<typename Key, typename Value>
class HashTable {
private:
    enum State {
        EMPTY,
        OCCUPIED,
        DELETED
    };

    size_t capacity_;
    size_t size_ = 0;
    size_t deleted_count_ = 0;

    double load_factor_threshold_ = 0.6;
    double deleted_threshold_ = 0.3;

    std::vector<State> states_;
    std::vector<Key> keys_;
    std::vector<Value> values_;

    mutable size_t find_slot_calls_ = 0;
    mutable size_t find_slot_total_steps_ = 0;

    size_t initial_index(const Key& key) const {
        return std::hash<Key>{}(key) % capacity_;
    }

    size_t probe_step(const Key& key) const {
        size_t step = std::hash<Key>{}(key) % (capacity_ - 1) + 1;
        if (step % 2 == 1) {
            return step;
        }
        return step + 1;
    }

    std::pair<std::optional<size_t>, std::optional<size_t>> find_slot(const Key& key) const {
        ++find_slot_calls_;

        size_t init_index = initial_index(key);
        size_t index = init_index;
        std::optional<size_t> insert_index = std::nullopt;
        size_t step = probe_step(key);
        size_t cnt_steps = 0;

        while (cnt_steps < capacity_) {
            if (states_[index] == DELETED && !insert_index) {
                insert_index = index;
            }
            else if (states_[index] == OCCUPIED && keys_[index] == key) {
                find_slot_total_steps_ += cnt_steps;
                return {index, std::nullopt};
            }
            else if (states_[index] == EMPTY) {
                find_slot_total_steps_ += cnt_steps;
                if (!insert_index) {
                    return {std::nullopt, index};
                } else {
                    return {std::nullopt, insert_index};
                }
            }
            ++cnt_steps;
            index = (init_index + cnt_steps * step) % capacity_;

        }
        find_slot_total_steps_ += cnt_steps;
        return {std::nullopt, insert_index};
    }

    void insert_no_resize(const Key& key, const Value& value, const std::optional<size_t> found_index, const std::optional<size_t> insert_index) {
        if (!found_index && !insert_index) {
            throw std::overflow_error("HashTable is full. Cannot insert new key-value pair");
        }

        if (!found_index) {
            if (states_[*insert_index] == DELETED) {
                --deleted_count_;
            }

            ++size_;
            states_[*insert_index] = OCCUPIED;
            keys_[*insert_index] = key;
            values_[*insert_index] = value;
        }

        else {
            values_[*found_index] = value;
        }
    }

    void resize(const size_t new_capacity) {
        const size_t old_capacity = capacity_;
        const std::vector<State> old_states = states_;
        const std::vector<Key> old_keys = keys_;
        const std::vector<Value> old_values = values_;

        capacity_ = new_capacity;
        size_ = 0;
        deleted_count_ = 0;
        states_ = std::vector<State>(capacity_, EMPTY);
        keys_ = std::vector<Key>(capacity_);
        values_ = std::vector<Value>(capacity_);

        for (size_t index = 0; index < old_capacity; ++index) {
            if (old_states[index] == OCCUPIED) {
                const Key& key = old_keys[index];
                const Value& value = old_values[index];
                auto [found_index, insert_index] = find_slot(key);
                insert_no_resize(key, value, found_index, insert_index);
            }
        }
    }

public:
    HashTable(size_t capacity = 8): capacity_(capacity) {
        if (capacity < 2) {
            throw std::invalid_argument("capacity must be at least 2");
        }
        if ((capacity & (capacity - 1)) != 0) {
            throw std::invalid_argument("capacity must be a power of two");
        }

        states_ = std::vector<State>(capacity, EMPTY);
        keys_ = std::vector<Key>(capacity);
        values_ = std::vector<Value>(capacity);
    }

    void put(const Key& key, const Value& value) {
        auto [found_index, insert_index] = find_slot(key);

        if (!found_index) {
            if (static_cast<double>(size_ + 1) / capacity_ >= load_factor_threshold_) {
                resize(capacity_ * 2);
            }
            if (static_cast<double>(size_ + deleted_count_) / capacity_ > 0.7){
                resize(capacity_);
            }

            std::tie(found_index, insert_index) = find_slot(key);

        }

        insert_no_resize(key, value, found_index, insert_index);
    }

    std::optional<Value> get(const Key& key) const {
        auto [found_index, _] = find_slot(key);

        if (found_index) {
            return values_[*found_index];
        }
        return std::nullopt;
    }

    void erase(const Key& key) {
        auto [found_index, _] = find_slot(key);

        if (!found_index) {
            throw std::out_of_range("Key not found");
        }

        --size_;
        ++deleted_count_;
        states_[*found_index] = DELETED;

        if (static_cast<double>(deleted_count_) / capacity_ > deleted_threshold_) {
            resize(capacity_);
        }
    }

    HashTable copy() const{
        HashTable new_ht = HashTable<Key, Value>(capacity_);
        new_ht.size_ = size_;
        new_ht.deleted_count_ = deleted_count_;
        new_ht.load_factor_threshold_ = load_factor_threshold_;
        new_ht.deleted_threshold_ = deleted_threshold_;
        new_ht.states_ = states_;
        new_ht.keys_ = keys_;
        new_ht.values_ = values_;
        new_ht.find_slot_calls_ = 0;
        new_ht.find_slot_total_steps_ = 0;
        return new_ht;
    }

    size_t size() const {
        return size_;
    }

    size_t capacity() const {
        return capacity_;
    }

    size_t deleted_count() const {
        return deleted_count_;
    }

    size_t find_slot_calls() const {
        return find_slot_calls_;
    }

    size_t find_slot_total_steps() const {
        return find_slot_total_steps_;
    }

    void reset_probe_stats() {
        find_slot_calls_ = 0;
        find_slot_total_steps_ = 0;
    }

    bool contains(const Key& key) const {
        auto [found_index, _] = find_slot(key);
        return found_index.has_value();
    }
};