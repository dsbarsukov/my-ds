#include <gtest/gtest.h>
#include <string>
#include <optional>
#include <stdexcept>
#include <functional>

#include "hash_table.h"

using namespace std;

struct BadHash {
    string value;
    size_t h;

    BadHash() : value(""), h(42) {}
    BadHash(const string& value, size_t h = 42) : value(value), h(h) {}

    bool operator==(const BadHash& other) const {
        return value == other.value && h == other.h;
    }
};

namespace std {
    template<>
    struct hash<BadHash> {
        size_t operator()(const BadHash& obj) const {
            return obj.h;
        }
    };
}

TEST(HashTableTest, PutAndGetBasic) {
    HashTable<string, int> ht;

    ht.put("a", 10);

    ASSERT_TRUE(ht.get("a").has_value());
    EXPECT_EQ(ht.get("a").value(), 10);
    EXPECT_EQ(ht.size(), 1);
    EXPECT_TRUE(ht.contains("a"));
}

TEST(HashTableTest, UpdateExistingKeyDoesNotIncreaseSize) {
    HashTable<string, int> ht;

    ht.put("a", 10);
    ht.put("a", 20);

    ASSERT_TRUE(ht.get("a").has_value());
    EXPECT_EQ(ht.get("a").value(), 20);
    EXPECT_EQ(ht.size(), 1);
}

TEST(HashTableTest, GetMissingKeyReturnsNullopt) {
    HashTable<string, int> ht;

    EXPECT_FALSE(ht.get("missing").has_value());
    EXPECT_FALSE(ht.contains("missing"));
}

TEST(HashTableTest, EraseExistingKey) {
    HashTable<string, int> ht;

    ht.put("a", 10);
    ht.erase("a");

    EXPECT_FALSE(ht.get("a").has_value());
    EXPECT_FALSE(ht.contains("a"));
    EXPECT_EQ(ht.size(), 0);
}

TEST(HashTableTest, EraseMissingKeyThrows) {
    HashTable<string, int> ht;

    EXPECT_THROW(ht.erase("missing"), out_of_range);
}

TEST(HashTableTest, SizeTracksNumberOfLiveElements) {
    HashTable<string, int> ht;

    ht.put("a", 1);
    ht.put("b", 2);
    ht.put("a", 3);
    ht.erase("b");

    EXPECT_EQ(ht.size(), 1);
}

TEST(HashTableTest, CollisionHandling) {
    HashTable<BadHash, int> ht(8);

    BadHash k1("k1", 1);
    BadHash k2("k2", 1);
    BadHash k3("k3", 1);

    ht.put(k1, 100);
    ht.put(k2, 200);
    ht.put(k3, 300);

    ASSERT_TRUE(ht.get(k1).has_value());
    ASSERT_TRUE(ht.get(k2).has_value());
    ASSERT_TRUE(ht.get(k3).has_value());

    EXPECT_EQ(ht.get(k1).value(), 100);
    EXPECT_EQ(ht.get(k2).value(), 200);
    EXPECT_EQ(ht.get(k3).value(), 300);
    EXPECT_EQ(ht.size(), 3);
}

TEST(HashTableTest, TombstoneDoesNotBreakSearchChain) {
    HashTable<BadHash, int> ht(8);

    BadHash k1("k1", 1);
    BadHash k2("k2", 1);
    BadHash k3("k3", 1);

    ht.put(k1, 10);
    ht.put(k2, 20);
    ht.put(k3, 30);

    ht.erase(k2);

    ASSERT_TRUE(ht.get(k3).has_value());
    EXPECT_EQ(ht.get(k3).value(), 30);
    EXPECT_TRUE(ht.contains(k3));
    EXPECT_EQ(ht.size(), 2);
}

TEST(HashTableTest, InsertReusesDeletedSlot) {
    HashTable<BadHash, int> ht(8);

    BadHash k1("k1", 1);
    BadHash k2("k2", 1);
    BadHash k3("k3", 1);

    ht.put(k1, 10);
    ht.put(k2, 20);
    ht.erase(k1);
    ht.put(k3, 30);

    ASSERT_TRUE(ht.get(k2).has_value());
    ASSERT_TRUE(ht.get(k3).has_value());

    EXPECT_EQ(ht.get(k2).value(), 20);
    EXPECT_EQ(ht.get(k3).value(), 30);
    EXPECT_EQ(ht.size(), 2);
}

TEST(HashTableTest, ResizePreservesAllElements) {
    HashTable<string, int> ht(4);

    for (int i = 0; i < 10; ++i) {
        ht.put("key" + to_string(i), i);
    }

    for (int i = 0; i < 10; ++i) {
        auto value = ht.get("key" + to_string(i));
        ASSERT_TRUE(value.has_value());
        EXPECT_EQ(value.value(), i);
    }

    EXPECT_EQ(ht.size(), 10);
}

TEST(HashTableTest, MultipleResizes) {
    HashTable<int, int> ht(2);

    for (int i = 0; i < 50; ++i) {
        ht.put(i, i);
    }

    for (int i = 0; i < 50; ++i) {
        auto value = ht.get(i);
        ASSERT_TRUE(value.has_value());
        EXPECT_EQ(value.value(), i);
    }

    EXPECT_EQ(ht.size(), 50);
}

TEST(HashTableTest, ValueCanBeNulloptEquivalent) {
    HashTable<string, optional<int>> ht;

    ht.put("a", nullopt);

    ASSERT_TRUE(ht.get("a").has_value());
    EXPECT_FALSE(ht.get("a").value().has_value());
    EXPECT_TRUE(ht.contains("a"));
}

TEST(HashTableTest, ResizeChangesCapacityAsExpected) {
    HashTable<string, int> ht(4);

    size_t initial_capacity = ht.capacity();

    for (int i = 0; i < 10; ++i) {
        ht.put("key" + to_string(i), i);
    }

    EXPECT_GT(ht.capacity(), initial_capacity);
    EXPECT_EQ(ht.capacity(), 32);
}

TEST(HashTableTest, RebuildOnDeletedRatioPreservesRemainingElements) {
    HashTable<string, int> ht(8);

    ht.put("k0", 0);
    ht.put("k1", 1);
    ht.put("k2", 2);
    ht.put("k3", 3);

    ht.erase("k0");
    ht.erase("k1");
    ht.erase("k2");

    EXPECT_EQ(ht.deleted_count(), 0);

    auto value = ht.get("k3");
    ASSERT_TRUE(value.has_value());
    EXPECT_EQ(value.value(), 3);
    EXPECT_EQ(ht.size(), 1);
}

TEST(HashTableTest, CopyPreservesAllElements) {
    HashTable<string, int> ht;

    ht.put("a", 1);
    ht.put("b", 2);
    ht.put("c", 3);

    auto ht_copy = ht.copy();

    EXPECT_EQ(ht_copy.size(), 3);

    ASSERT_TRUE(ht_copy.get("a").has_value());
    ASSERT_TRUE(ht_copy.get("b").has_value());
    ASSERT_TRUE(ht_copy.get("c").has_value());

    EXPECT_EQ(ht_copy.get("a").value(), 1);
    EXPECT_EQ(ht_copy.get("b").value(), 2);
    EXPECT_EQ(ht_copy.get("c").value(), 3);
}

TEST(HashTableTest, CopyIsIndependentFromOriginal) {
    HashTable<string, int> ht;

    ht.put("a", 1);
    ht.put("b", 2);

    auto ht_copy = ht.copy();

    ht.erase("a");
    ht_copy.put("c", 3);

    EXPECT_FALSE(ht.contains("a"));

    ASSERT_TRUE(ht_copy.get("a").has_value());
    EXPECT_EQ(ht_copy.get("a").value(), 1);

    EXPECT_FALSE(ht.get("c").has_value());

    ASSERT_TRUE(ht_copy.get("c").has_value());
    EXPECT_EQ(ht_copy.get("c").value(), 3);
}