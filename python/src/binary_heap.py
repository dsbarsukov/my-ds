class BinaryHeap:
    def __init__(self, values=None) -> None:
        self._data = []

        if values is not None:
            self._data = list(values)
            self._heapify()

    def _parent_index(self, index: int) -> int:
        return (index - 1) // 2

    def _left_child_index(self, index: int) -> int:
        return 2 * index + 1

    def _right_child_index(self, index: int) -> int:
        return 2 * index + 2

    def _swap(self, first_index: int, second_index: int) -> None:
        self._data[first_index], self._data[second_index] = self._data[second_index], self._data[first_index]

    def _sift_up(self, index: int) -> None:
        while index > 0:
            parent_index = self._parent_index(index)

            if self._data[parent_index] <= self._data[index]:
                break

            self._swap(parent_index, index)
            index = parent_index

    def _sift_down(self, index: int) -> None:
        size = len(self._data)

        while True:
            left_child_index = self._left_child_index(index)
            right_child_index = self._right_child_index(index)

            smallest_index = index

            if left_child_index < size and self._data[left_child_index] < self._data[smallest_index]:
                smallest_index = left_child_index

            if right_child_index < size and self._data[right_child_index] < self._data[smallest_index]:
                smallest_index = right_child_index

            if smallest_index == index:
                break

            self._swap(index, smallest_index)
            index = smallest_index

    def _heapify(self) -> None:
        last_parent_index = len(self._data) // 2 - 1

        for index in range(last_parent_index, -1, -1):
            self._sift_down(index)

    def push(self, value) -> None:
        self._data.append(value)
        self._sift_up(len(self._data) - 1)

    def pop(self):
        if len(self._data) == 0:
            raise IndexError("pop from empty heap")

        root = self._data[0]
        last_value = self._data.pop()

        if len(self._data) > 0:
            self._data[0] = last_value
            self._sift_down(0)

        return root

    def peek(self):
        if len(self._data) == 0:
            raise IndexError("peek from empty heap")

        return self._data[0]

    def copy(self) -> "BinaryHeap":
        new_heap = BinaryHeap()
        new_heap._data = self._data[:]
        return new_heap

    def to_list(self) -> list:
        return self._data[:]

    def __len__(self) -> int:
        return len(self._data)

    def __contains__(self, value) -> bool:
        return value in self._data

    def __bool__(self) -> bool:
        return len(self._data) > 0