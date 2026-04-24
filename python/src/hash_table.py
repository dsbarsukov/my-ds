from typing import Tuple

EMPTY = 0
OCCUPIED = 1
DELETED = 2

class HashTable:

    def __init__(self, capacity: int = 8) -> None:
        if capacity < 2:
            raise ValueError("capacity must be at least 2")
        if capacity & (capacity - 1) != 0:
            raise ValueError("capacity must be a power of two")

        self._capacity = capacity
        self._size = 0
        self._deleted_count = 0

        self._load_factor_threshold = 0.6
        self._deleted_threshold = 0.3

        self._states = [EMPTY] * capacity
        self._keys = [None] * capacity
        self._values = [None] * capacity

        self._find_slot_calls = 0
        self._find_slot_total_steps = 0

    def _initial_index(self, key) -> int:
        return hash(key) % self._capacity

    def _probe_step(self, key) -> int:
        step = hash(key) % (self._capacity - 1) + 1
        return step if step % 2 == 1 else step + 1

    def _find_slot(self, key) -> Tuple[int | None, int | None]:
        self._find_slot_calls += 1

        initial_index = self._initial_index(key)
        index = initial_index
        insert_index = None
        step = self._probe_step(key)
        cnt_steps = 0

        while cnt_steps < self._capacity:

            if self._states[index] == DELETED and insert_index is None:
                insert_index = index

            elif self._states[index] == OCCUPIED and self._keys[index] == key:
                self._find_slot_total_steps += cnt_steps
                return index, None

            elif self._states[index] == EMPTY:
                self._find_slot_total_steps += cnt_steps
                if insert_index is None:
                    return None, index
                return None, insert_index

            cnt_steps += 1
            # index = (index + 1) % self._capacity
            # index = (initial_index + cnt_steps**2) % self._capacity
            index = (initial_index + cnt_steps * step) % self._capacity

        self._find_slot_total_steps += cnt_steps
        return None, insert_index

    def _insert_no_resize(self, key, value, found_index: int | None, insert_index: int | None) -> None:
        if found_index is None and insert_index is None:
            raise OverflowError('HashTable is full. Cannot insert new key-value pair')

        if found_index is None:
            if self._states[insert_index] == DELETED:
                self._deleted_count -= 1

            self._size += 1
            self._states[insert_index] = OCCUPIED
            self._values[insert_index] = value
            self._keys[insert_index] = key

        else:
            self._values[found_index] = value

    def _resize(self, new_capacity: int) -> None:
        old_capacity = self._capacity
        old_states = self._states
        old_keys = self._keys
        old_values = self._values

        self._capacity = new_capacity
        self._size = 0
        self._deleted_count = 0
        self._states = [EMPTY] * self._capacity
        self._keys = [None] * self._capacity
        self._values = [None] * self._capacity

        for index in range(old_capacity):
            if old_states[index] == OCCUPIED:
                key, value = old_keys[index], old_values[index]
                found_index, insert_index = self._find_slot(key)
                self._insert_no_resize(key, value, found_index, insert_index)

    def put(self, key, value) -> None:
        found_index, insert_index = self._find_slot(key)

        if found_index is None:
            if ((self._size + 1) / self._capacity) >= self._load_factor_threshold:
                self._resize(self._capacity * 2)

            if (self._size + self._deleted_count) / self._capacity > 0.7:
                self._resize(self._capacity)

            found_index, insert_index = self._find_slot(key)

        self._insert_no_resize(key, value, found_index, insert_index)

    def get(self, key, default=None):
        found_index, _ = self._find_slot(key)

        if found_index is not None:
            return self._values[found_index]

        return default

    def delete(self, key) -> None:
        found_index, _ = self._find_slot(key)

        if found_index is None:
            raise KeyError(key)

        self._size -= 1
        self._deleted_count += 1
        self._states[found_index] = DELETED

        if self._deleted_count / self._capacity > self._deleted_threshold:
            self._resize(self._capacity)

    def copy(self) -> "HashTable":
        new_ht = HashTable(self._capacity)
        new_ht._size = self._size
        new_ht._deleted_count = self._deleted_count
        new_ht._load_factor_threshold = self._load_factor_threshold
        new_ht._deleted_threshold = self._deleted_threshold
        new_ht._states = self._states[:]
        new_ht._keys = self._keys[:]
        new_ht._values = self._values[:]
        new_ht._find_slot_calls = 0
        new_ht._find_slot_total_steps = 0
        return new_ht

    def __len__(self) -> int:
        return self._size

    def __contains__(self, key) -> bool:
        found_index, _ = self._find_slot(key)
        return found_index is not None

    def __getitem__(self, key):
        found_index, _ = self._find_slot(key)

        if found_index is not None:
            return self._values[found_index]
        raise KeyError(key)

    def __setitem__(self, key, value) -> None:
        self.put(key, value)

    def __delitem__(self, key) -> None:
        self.delete(key)