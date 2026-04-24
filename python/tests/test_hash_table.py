import pytest

from hash_table import HashTable

class BadHash:
    """
    Ключ с управляемым hash, чтобы гарантированно создавать коллизии
    """
    def __init__(self, value, h=42):
        self.value = value
        self.h = h

    def __hash__(self):
        return self.h

    def __eq__(self, other):
        return isinstance(other, BadHash) and self.value == other.value and self.h == other.h

    def __repr__(self):
        return f"BadHash(value={self.value!r}, h={self.h})"


def test_put_and_get_basic():
    ht = HashTable()

    ht.put("a", 10)

    assert ht.get("a") == 10
    assert len(ht) == 1
    assert "a" in ht

def test_update_existing_key_does_not_increase_size():
    ht = HashTable()

    ht.put("a", 10)
    ht.put("a", 20)

    assert ht.get("a") == 20
    assert len(ht) == 1

def test_get_missing_key_returns_none():
    ht = HashTable()

    assert ht.get("missing") is None
    assert "missing" not in ht

def test_getitem_returns_value_for_existing_key():
    ht = HashTable()
    ht["x"] = 99

    assert ht["x"] == 99

def test_getitem_raises_keyerror_for_missing_key():
    ht = HashTable()

    with pytest.raises(KeyError):
        _ = ht["missing"]

def test_delete_existing_key():
    ht = HashTable()

    ht.put("a", 10)
    ht.delete("a")

    assert ht.get("a") is None
    assert "a" not in ht
    assert len(ht) == 0

def test_delete_missing_key_raises_keyerror():
    ht = HashTable()

    with pytest.raises(KeyError):
        ht.delete("missing")

def test_delitem_raises_keyerror_for_missing_key():
    ht = HashTable()

    with pytest.raises(KeyError):
        del ht["missing"]

def test_len_tracks_number_of_live_elements():
    ht = HashTable()

    ht.put("a", 1)
    ht.put("b", 2)
    ht.put("a", 3)
    ht.delete("b")

    assert len(ht) == 1

def test_collision_handling():
    ht = HashTable(capacity=8)

    k1 = BadHash("k1", h=1)
    k2 = BadHash("k2", h=1)
    k3 = BadHash("k3", h=1)

    ht.put(k1, 100)
    ht.put(k2, 200)
    ht.put(k3, 300)

    assert ht.get(k1) == 100
    assert ht.get(k2) == 200
    assert ht.get(k3) == 300
    assert len(ht) == 3

def test_tombstone_does_not_break_search_chain():
    ht = HashTable(capacity=8)

    k1 = BadHash("k1", h=1)
    k2 = BadHash("k2", h=1)
    k3 = BadHash("k3", h=1)

    ht.put(k1, 10)
    ht.put(k2, 20)
    ht.put(k3, 30)

    ht.delete(k2)

    assert ht.get(k3) == 30
    assert k3 in ht
    assert len(ht) == 2

def test_insert_reuses_deleted_slot():
    ht = HashTable(capacity=8)

    k1 = BadHash("k1", h=1)
    k2 = BadHash("k2", h=1)
    k3 = BadHash("k3", h=1)

    ht.put(k1, 10)
    ht.put(k2, 20)
    ht.delete(k1)

    deleted_before = ht._deleted_count

    ht.put(k3, 30)

    assert ht.get(k2) == 20
    assert ht.get(k3) == 30
    assert len(ht) == 2
    assert ht._deleted_count == deleted_before - 1

def test_resize_preserves_all_elements():
    ht = HashTable(capacity=4)

    initial_capacity = ht._capacity

    for i in range(10):
        ht.put(f"key{i}", i)

    assert ht._capacity > initial_capacity

    for i in range(10):
        assert ht.get(f"key{i}") == i

    assert len(ht) == 10
    assert ht._capacity == 32

def test_rebuild_on_deleted_ratio_preserves_remaining_elements():
    ht = HashTable(capacity=8)

    keys = [f"k{i}" for i in range(4)]
    for i, key in enumerate(keys):
        ht.put(key, i)

    ht.delete("k0")
    ht.delete("k1")
    ht.delete("k2")

    assert ht._deleted_count == 0
    assert ht.get("k3") == 3
    assert len(ht) == 1


def test_setitem_and_delitem_work():
    ht = HashTable()

    ht["a"] = 1
    ht["b"] = 2
    del ht["a"]

    assert "a" not in ht
    assert ht.get("a") is None
    assert ht["b"] == 2
    assert len(ht) == 1

def test_none_value():
    ht = HashTable()

    ht.put("a", None)

    assert ht.get("a") is None
    assert "a" in ht

def test_multiple_resizes():
    ht = HashTable(capacity=2)

    for i in range(50):
        ht.put(i, i)

    for i in range(50):
        assert ht.get(i) == i

    assert len(ht) == 50

def test_copy_preserves_all_elements():
    ht = HashTable()

    ht.put("a", 1)
    ht.put("b", 2)
    ht.put("c", 3)

    ht_copy = ht.copy()

    assert len(ht_copy) == 3
    assert ht_copy.get("a") == 1
    assert ht_copy.get("b") == 2
    assert ht_copy.get("c") == 3

def test_copy_is_independent_from_original():
    ht = HashTable()

    ht.put("a", 1)
    ht.put("b", 2)

    ht_copy = ht.copy()

    ht.delete("a")
    ht_copy.put("c", 3)

    assert "a" not in ht
    assert ht_copy.get("a") == 1

    assert ht.get("c") is None
    assert ht_copy.get("c") == 3