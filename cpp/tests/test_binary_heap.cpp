#include <gtest/gtest.h>

#include <algorithm>
#include <stdexcept>
#include <vector>

#include "binary_heap.h"

namespace {

template <typename T>
bool is_min_heap(const std::vector<T>& data) {
    for (std::size_t index = 0; index < data.size(); ++index) {
        std::size_t left_child_index = 2 * index + 1;
        std::size_t right_child_index = 2 * index + 2;

        if (left_child_index < data.size() && data[index] > data[left_child_index]) {
            return false;
        }

        if (right_child_index < data.size() && data[index] > data[right_child_index]) {
            return false;
        }
    }

    return true;
}

std::vector<int> pop_all(BinaryHeap<int>& heap) {
    std::vector<int> result;

    while (!heap.empty()) {
        result.push_back(heap.pop());
    }

    return result;
}

} // namespace

TEST(BinaryHeapTest, PushAndPeekBasic) {
    BinaryHeap<int> heap;

    heap.push(5);
    heap.push(2);
    heap.push(8);

    EXPECT_EQ(heap.peek(), 2);
    EXPECT_EQ(heap.size(), 3);
}

TEST(BinaryHeapTest, PopReturnsMinElement) {
    BinaryHeap<int> heap;

    heap.push(5);
    heap.push(2);
    heap.push(8);

    EXPECT_EQ(heap.pop(), 2);
    EXPECT_EQ(heap.size(), 2);
    EXPECT_EQ(heap.peek(), 5);
}

TEST(BinaryHeapTest, PopAllReturnsSortedOrder) {
    BinaryHeap<int> heap;
    std::vector<int> values = {5, 1, 8, 3, 2, 10};

    for (int value : values) {
        heap.push(value);
    }

    std::vector<int> result = pop_all(heap);
    std::sort(values.begin(), values.end());

    EXPECT_EQ(result, values);
    EXPECT_EQ(heap.size(), 0);
}

TEST(BinaryHeapTest, PushReverseOrder) {
    BinaryHeap<int> heap;
    std::vector<int> values = {10, 9, 8, 7, 6, 5, 4, 3, 2, 1};

    for (int value : values) {
        heap.push(value);
    }

    std::vector<int> result = pop_all(heap);
    std::sort(values.begin(), values.end());

    EXPECT_EQ(result, values);
}

TEST(BinaryHeapTest, PushWithDuplicates) {
    BinaryHeap<int> heap;
    std::vector<int> values = {5, 1, 3, 1, 5, 2};

    for (int value : values) {
        heap.push(value);
    }

    std::vector<int> result = pop_all(heap);
    std::sort(values.begin(), values.end());

    EXPECT_EQ(result, values);
}

TEST(BinaryHeapTest, HeapifyBuildsValidHeap) {
    std::vector<int> values = {5, 1, 8, 3, 2, 10};

    BinaryHeap<int> heap(values);

    EXPECT_TRUE(is_min_heap(heap.to_list()));
    EXPECT_EQ(heap.size(), values.size());
}

TEST(BinaryHeapTest, HeapifyPopAllReturnsSortedOrder) {
    std::vector<int> values = {5, 1, 8, 3, 2, 10};

    BinaryHeap<int> heap(values);

    std::vector<int> result = pop_all(heap);
    std::sort(values.begin(), values.end());

    EXPECT_EQ(result, values);
}

TEST(BinaryHeapTest, HeapifyEmptyList) {
    BinaryHeap<int> heap(std::vector<int>{});

    EXPECT_EQ(heap.size(), 0);
    EXPECT_TRUE(heap.to_list().empty());
}

TEST(BinaryHeapTest, HeapifySingleElement) {
    BinaryHeap<int> heap(std::vector<int>{42});

    EXPECT_EQ(heap.size(), 1);
    EXPECT_EQ(heap.peek(), 42);
    EXPECT_EQ(heap.pop(), 42);
    EXPECT_EQ(heap.size(), 0);
}

TEST(BinaryHeapTest, PeekEmptyHeapThrows) {
    BinaryHeap<int> heap;

    EXPECT_THROW(heap.peek(), std::out_of_range);
}

TEST(BinaryHeapTest, PopEmptyHeapThrows) {
    BinaryHeap<int> heap;

    EXPECT_THROW(heap.pop(), std::out_of_range);
}

TEST(BinaryHeapTest, SizeTracksNumberOfElements) {
    BinaryHeap<int> heap;

    EXPECT_EQ(heap.size(), 0);

    heap.push(10);
    heap.push(20);
    heap.push(5);

    EXPECT_EQ(heap.size(), 3);

    heap.pop();

    EXPECT_EQ(heap.size(), 2);

    heap.pop();
    heap.pop();

    EXPECT_EQ(heap.size(), 0);
}

TEST(BinaryHeapTest, ContainsExistingValue) {
    BinaryHeap<int> heap;

    heap.push(10);
    heap.push(5);
    heap.push(20);

    EXPECT_TRUE(heap.contains(5));
    EXPECT_TRUE(heap.contains(10));
    EXPECT_TRUE(heap.contains(20));
}

TEST(BinaryHeapTest, ContainsMissingValue) {
    BinaryHeap<int> heap;

    heap.push(10);
    heap.push(5);

    EXPECT_FALSE(heap.contains(99));
}

TEST(BinaryHeapTest, EmptyHeapIsEmpty) {
    BinaryHeap<int> heap;

    EXPECT_TRUE(heap.empty());
}

TEST(BinaryHeapTest, NonEmptyHeapIsNotEmpty) {
    BinaryHeap<int> heap;

    heap.push(1);

    EXPECT_FALSE(heap.empty());
}

TEST(BinaryHeapTest, ToListReturnsCopy) {
    BinaryHeap<int> heap;

    heap.push(3);
    heap.push(1);
    heap.push(2);

    std::vector<int> data = heap.to_list();
    data.push_back(100);

    EXPECT_EQ(heap.size(), 3);
    EXPECT_FALSE(heap.contains(100));
}

TEST(BinaryHeapTest, CopyPreservesAllElements) {
    BinaryHeap<int> heap;
    std::vector<int> values = {5, 1, 8, 3, 2};

    for (int value : values) {
        heap.push(value);
    }

    BinaryHeap<int> heap_copy = heap.copy();

    std::vector<int> result = pop_all(heap_copy);
    std::sort(values.begin(), values.end());

    EXPECT_EQ(result, values);
    EXPECT_EQ(heap.size(), values.size());
}

TEST(BinaryHeapTest, CopyIsIndependentFromOriginal) {
    BinaryHeap<int> heap;

    heap.push(5);
    heap.push(1);
    heap.push(3);

    BinaryHeap<int> heap_copy = heap.copy();

    heap.pop();
    heap_copy.push(0);

    EXPECT_EQ(heap.peek(), 3);
    EXPECT_EQ(heap_copy.peek(), 0);
}

TEST(BinaryHeapTest, MixedPushPopOperations) {
    BinaryHeap<int> heap;

    heap.push(10);
    heap.push(4);
    heap.push(7);

    EXPECT_EQ(heap.pop(), 4);

    heap.push(1);
    heap.push(9);

    EXPECT_EQ(heap.pop(), 1);
    EXPECT_EQ(heap.pop(), 7);
    EXPECT_EQ(heap.pop(), 9);
    EXPECT_EQ(heap.pop(), 10);
    EXPECT_EQ(heap.size(), 0);
}

TEST(BinaryHeapTest, HeapPropertyAfterEachPush) {
    BinaryHeap<int> heap;
    std::vector<int> values = {9, 4, 7, 1, 3, 8, 2};

    for (int value : values) {
        heap.push(value);
        EXPECT_TRUE(is_min_heap(heap.to_list()));
    }
}

TEST(BinaryHeapTest, HeapPropertyAfterEachPop) {
    BinaryHeap<int> heap(std::vector<int>{9, 4, 7, 1, 3, 8, 2});

    while (!heap.empty()) {
        heap.pop();
        EXPECT_TRUE(is_min_heap(heap.to_list()));
    }
}
