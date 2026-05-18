import pytest

from binary_heap import BinaryHeap


def is_min_heap(data: list) -> bool:
    for index in range(len(data)):
        left_child_index = 2 * index + 1
        right_child_index = 2 * index + 2

        if left_child_index < len(data) and data[index] > data[left_child_index]:
            return False

        if right_child_index < len(data) and data[index] > data[right_child_index]:
            return False

    return True


def test_push_and_peek_basic():
    heap = BinaryHeap()

    heap.push(5)
    heap.push(2)
    heap.push(8)

    assert heap.peek() == 2
    assert len(heap) == 3


def test_pop_returns_min_element():
    heap = BinaryHeap()

    heap.push(5)
    heap.push(2)
    heap.push(8)

    assert heap.pop() == 2
    assert len(heap) == 2
    assert heap.peek() == 5


def test_pop_all_returns_sorted_order():
    heap = BinaryHeap()

    values = [5, 1, 8, 3, 2, 10]

    for value in values:
        heap.push(value)

    result = []

    while len(heap) > 0:
        result.append(heap.pop())

    assert result == sorted(values)
    assert len(heap) == 0


def test_push_reverse_order():
    heap = BinaryHeap()

    values = [10, 9, 8, 7, 6, 5, 4, 3, 2, 1]

    for value in values:
        heap.push(value)

    result = []

    while len(heap) > 0:
        result.append(heap.pop())

    assert result == sorted(values)


def test_push_with_duplicates():
    heap = BinaryHeap()

    values = [5, 1, 3, 1, 5, 2]

    for value in values:
        heap.push(value)

    result = []

    while len(heap) > 0:
        result.append(heap.pop())

    assert result == sorted(values)


def test_heapify_builds_valid_heap():
    values = [5, 1, 8, 3, 2, 10]

    heap = BinaryHeap(values)

    assert is_min_heap(heap.to_list())
    assert len(heap) == len(values)


def test_heapify_pop_all_returns_sorted_order():
    values = [5, 1, 8, 3, 2, 10]

    heap = BinaryHeap(values)

    result = []

    while len(heap) > 0:
        result.append(heap.pop())

    assert result == sorted(values)


def test_heapify_empty_list():
    heap = BinaryHeap([])

    assert len(heap) == 0
    assert heap.to_list() == []


def test_heapify_single_element():
    heap = BinaryHeap([42])

    assert len(heap) == 1
    assert heap.peek() == 42
    assert heap.pop() == 42
    assert len(heap) == 0


def test_peek_empty_heap_raises_indexerror():
    heap = BinaryHeap()

    with pytest.raises(IndexError):
        heap.peek()


def test_pop_empty_heap_raises_indexerror():
    heap = BinaryHeap()

    with pytest.raises(IndexError):
        heap.pop()


def test_len_tracks_number_of_elements():
    heap = BinaryHeap()

    assert len(heap) == 0

    heap.push(10)
    heap.push(20)
    heap.push(5)

    assert len(heap) == 3

    heap.pop()

    assert len(heap) == 2

    heap.pop()
    heap.pop()

    assert len(heap) == 0


def test_contains_existing_value():
    heap = BinaryHeap()

    heap.push(10)
    heap.push(5)
    heap.push(20)

    assert 5 in heap
    assert 10 in heap
    assert 20 in heap


def test_contains_missing_value():
    heap = BinaryHeap()

    heap.push(10)
    heap.push(5)

    assert 99 not in heap


def test_bool_empty_heap_is_false():
    heap = BinaryHeap()

    assert not heap


def test_bool_non_empty_heap_is_true():
    heap = BinaryHeap()

    heap.push(1)

    assert heap


def test_to_list_returns_copy():
    heap = BinaryHeap()

    heap.push(3)
    heap.push(1)
    heap.push(2)

    data = heap.to_list()
    data.append(100)

    assert len(heap) == 3
    assert 100 not in heap


def test_copy_preserves_all_elements():
    heap = BinaryHeap()

    values = [5, 1, 8, 3, 2]

    for value in values:
        heap.push(value)

    heap_copy = heap.copy()

    result = []

    while len(heap_copy) > 0:
        result.append(heap_copy.pop())

    assert result == sorted(values)
    assert len(heap) == len(values)


def test_copy_is_independent_from_original():
    heap = BinaryHeap()

    heap.push(5)
    heap.push(1)
    heap.push(3)

    heap_copy = heap.copy()

    heap.pop()
    heap_copy.push(0)

    assert heap.peek() == 3
    assert heap_copy.peek() == 0


def test_mixed_push_pop_operations():
    heap = BinaryHeap()

    heap.push(10)
    heap.push(4)
    heap.push(7)

    assert heap.pop() == 4

    heap.push(1)
    heap.push(9)

    assert heap.pop() == 1
    assert heap.pop() == 7
    assert heap.pop() == 9
    assert heap.pop() == 10
    assert len(heap) == 0


def test_heap_property_after_each_push():
    heap = BinaryHeap()

    values = [9, 4, 7, 1, 3, 8, 2]

    for value in values:
        heap.push(value)
        assert is_min_heap(heap.to_list())


def test_heap_property_after_each_pop():
    heap = BinaryHeap([9, 4, 7, 1, 3, 8, 2])

    while len(heap) > 0:
        heap.pop()
        assert is_min_heap(heap.to_list())