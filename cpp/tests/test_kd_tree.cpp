#include <gtest/gtest.h>

#include <algorithm>
#include <cmath>
#include <limits>
#include <random>
#include <set>
#include <vector>

#include "kd_tree.h"

namespace {

using Point = KDTree::Point;

std::set<Point> point_set(const std::vector<Point>& points) {
    return std::set<Point>(points.begin(), points.end());
}

void expect_same_points(const std::vector<Point>& actual, const std::vector<Point>& expected) {
    EXPECT_EQ(point_set(actual), point_set(expected));
}

void expect_same_nearest_result(const std::pair<Point, double>& kd_result,
                                const std::pair<Point, double>& brute_result) {
    EXPECT_FALSE(kd_result.first.empty());
    EXPECT_FALSE(brute_result.first.empty());
    EXPECT_NEAR(kd_result.second, brute_result.second, 1e-9);
}

} // namespace

TEST(KDTreeTest, BuildEmptyTree) {
    std::vector<Point> points;
    KDTree tree(points);

    std::vector<Point> expected;

    EXPECT_EQ(tree.size(), 0u);
    EXPECT_EQ(tree.height(), 0u);
    EXPECT_FALSE(static_cast<bool>(tree));
    EXPECT_EQ(tree.to_list(), expected);
}

TEST(KDTreeTest, BuildTreeSize) {
    std::vector<Point> points = {
        Point{2, 3}, Point{5, 4}, Point{9, 6},
        Point{4, 7}, Point{8, 1}, Point{7, 2}
    };

    KDTree tree(points);

    EXPECT_EQ(tree.size(), points.size());
    EXPECT_TRUE(static_cast<bool>(tree));
    expect_same_points(tree.to_list(), points);
}

TEST(KDTreeTest, BuildOnePointTree) {
    std::vector<Point> points = {Point{2, 3}};
    KDTree tree(points);

    EXPECT_EQ(tree.size(), 1u);
    EXPECT_EQ(tree.height(), 1u);
    EXPECT_EQ(tree.to_list(), points);
}

TEST(KDTreeTest, InsertIntoEmptyTree) {
    KDTree tree;

    Point point = {2, 3};
    tree.insert(point);

    EXPECT_EQ(tree.size(), 1u);
    EXPECT_EQ(tree.height(), 1u);
    EXPECT_TRUE(tree.contains(point));
}

TEST(KDTreeTest, InsertIntoNonEmptyTree) {
    std::vector<Point> points = {Point{2, 3}, Point{5, 4}};
    KDTree tree(points);

    Point new_point = {9, 6};
    tree.insert(new_point);

    std::vector<Point> expected = {Point{2, 3}, Point{5, 4}, Point{9, 6}};

    EXPECT_EQ(tree.size(), 3u);
    EXPECT_TRUE(tree.contains(new_point));
    expect_same_points(tree.to_list(), expected);
}

TEST(KDTreeTest, ContainsExistingPoint) {
    std::vector<Point> points = {Point{2, 3}, Point{5, 4}, Point{9, 6}};
    KDTree tree(points);

    Point target = {5, 4};

    EXPECT_TRUE(tree.contains(target));
}

TEST(KDTreeTest, ContainsMissingPoint) {
    std::vector<Point> points = {Point{2, 3}, Point{5, 4}, Point{9, 6}};
    KDTree tree(points);

    Point target = {100, 100};

    EXPECT_FALSE(tree.contains(target));
}

TEST(KDTreeTest, ContainsInEmptyTree) {
    KDTree tree;

    Point target = {1, 2};

    EXPECT_FALSE(tree.contains(target));
}

TEST(KDTreeTest, NearestNeighborEmptyTree) {
    KDTree tree;

    Point target = {1, 2};
    auto result = tree.nearest_neighbor(target);

    EXPECT_TRUE(result.first.empty());
    EXPECT_EQ(result.second, std::numeric_limits<double>::infinity());
}

TEST(KDTreeTest, NearestNeighborSimpleCase) {
    std::vector<Point> points = {
        Point{2, 3}, Point{5, 4}, Point{9, 6},
        Point{4, 7}, Point{8, 1}, Point{7, 2}
    };
    KDTree tree(points);

    Point target = {6, 3};
    auto result = tree.nearest_neighbor(target);

    std::set<Point> allowed_points = {Point{5, 4}, Point{7, 2}};

    EXPECT_TRUE(allowed_points.count(result.first) > 0);
    EXPECT_NEAR(result.second, std::sqrt(2.0), 1e-9);
}

TEST(KDTreeTest, NearestNeighborExactMatch) {
    std::vector<Point> points = {Point{2, 3}, Point{5, 4}, Point{9, 6}};
    KDTree tree(points);

    Point target = {5, 4};
    auto result = tree.nearest_neighbor(target);

    EXPECT_EQ(result.first, target);
    EXPECT_DOUBLE_EQ(result.second, 0.0);
}

TEST(KDTreeTest, NearestNeighborMatchesBruteForce) {
    std::vector<Point> points = {
        Point{2, 3}, Point{5, 4}, Point{9, 6},
        Point{4, 7}, Point{8, 1}, Point{7, 2}
    };
    Point target = {6, 3};

    KDTree tree(points);

    auto kd_result = tree.nearest_neighbor(target);
    auto brute_result = tree.brute_force_nearest(points, target);

    expect_same_nearest_result(kd_result, brute_result);
}

TEST(KDTreeTest, NearestNeighborMatchesBruteForceRandomPoints) {
    std::mt19937 generator(42);
    std::uniform_int_distribution<int> distribution(-100, 100);

    std::vector<Point> points;

    for (int index = 0; index < 100; ++index) {
        points.push_back(Point{
            static_cast<double>(distribution(generator)),
            static_cast<double>(distribution(generator))
        });
    }

    KDTree tree(points);

    for (int index = 0; index < 30; ++index) {
        Point target = {
            static_cast<double>(distribution(generator)),
            static_cast<double>(distribution(generator))
        };

        auto kd_result = tree.nearest_neighbor(target);
        auto brute_result = tree.brute_force_nearest(points, target);

        expect_same_nearest_result(kd_result, brute_result);
    }
}

TEST(KDTreeTest, NearestNeighbor3DPoints) {
    std::vector<Point> points = {
        Point{1, 2, 3}, Point{4, 5, 6}, Point{7, 8, 9}, Point{2, 2, 2}
    };
    Point target = {3, 3, 3};

    KDTree tree(points);

    auto kd_result = tree.nearest_neighbor(target);
    auto brute_result = tree.brute_force_nearest(points, target);

    expect_same_nearest_result(kd_result, brute_result);
}

TEST(KDTreeTest, RangeSearchEmptyTree) {
    KDTree tree;

    Point lower_bound = {0, 0};
    Point upper_bound = {10, 10};
    std::vector<Point> expected;

    EXPECT_EQ(tree.range_search(lower_bound, upper_bound), expected);
}

TEST(KDTreeTest, RangeSearchSimpleCase) {
    std::vector<Point> points = {
        Point{2, 3}, Point{5, 4}, Point{9, 6},
        Point{4, 7}, Point{8, 1}, Point{7, 2}
    };
    KDTree tree(points);

    Point lower_bound = {3, 1};
    Point upper_bound = {8, 5};

    std::vector<Point> expected = {Point{5, 4}, Point{8, 1}, Point{7, 2}};
    std::vector<Point> result = tree.range_search(lower_bound, upper_bound);

    expect_same_points(result, expected);
}

TEST(KDTreeTest, RangeSearchMatchesBruteForce) {
    std::vector<Point> points = {
        Point{2, 3}, Point{5, 4}, Point{9, 6},
        Point{4, 7}, Point{8, 1}, Point{7, 2}
    };
    Point lower_bound = {3, 1};
    Point upper_bound = {8, 5};

    KDTree tree(points);

    std::vector<Point> kd_result = tree.range_search(lower_bound, upper_bound);
    std::vector<Point> brute_result = tree.brute_force_range_search(points, lower_bound, upper_bound);

    expect_same_points(kd_result, brute_result);
}

TEST(KDTreeTest, RangeSearchMatchesBruteForceRandomPoints) {
    std::mt19937 generator(42);
    std::uniform_int_distribution<int> distribution(-100, 100);

    std::vector<Point> points;

    for (int index = 0; index < 100; ++index) {
        points.push_back(Point{
            static_cast<double>(distribution(generator)),
            static_cast<double>(distribution(generator))
        });
    }

    KDTree tree(points);

    for (int index = 0; index < 30; ++index) {
        int x1 = distribution(generator);
        int x2 = distribution(generator);
        int y1 = distribution(generator);
        int y2 = distribution(generator);

        Point lower_bound = {
            static_cast<double>(std::min(x1, x2)),
            static_cast<double>(std::min(y1, y2))
        };
        Point upper_bound = {
            static_cast<double>(std::max(x1, x2)),
            static_cast<double>(std::max(y1, y2))
        };

        std::vector<Point> kd_result = tree.range_search(lower_bound, upper_bound);
        std::vector<Point> brute_result = tree.brute_force_range_search(points, lower_bound, upper_bound);

        expect_same_points(kd_result, brute_result);
    }
}

TEST(KDTreeTest, RangeSearch3DPoints) {
    std::vector<Point> points = {
        Point{1, 2, 3}, Point{4, 5, 6}, Point{7, 8, 9}, Point{2, 2, 2}, Point{5, 5, 5}
    };
    Point lower_bound = {2, 2, 2};
    Point upper_bound = {5, 5, 6};

    KDTree tree(points);

    std::vector<Point> kd_result = tree.range_search(lower_bound, upper_bound);
    std::vector<Point> brute_result = tree.brute_force_range_search(points, lower_bound, upper_bound);

    expect_same_points(kd_result, brute_result);
}

TEST(KDTreeTest, RebuildKeepsAllPoints) {
    std::vector<Point> points = {Point{1, 1}, Point{2, 2}, Point{3, 3}, Point{4, 4}, Point{5, 5}};
    KDTree tree;

    for (const Point& point : points) {
        tree.insert(point);
    }

    tree.rebuild();

    EXPECT_EQ(tree.size(), points.size());
    expect_same_points(tree.to_list(), points);

    for (const Point& point : points) {
        EXPECT_TRUE(tree.contains(point));
    }
}

TEST(KDTreeTest, RebuildReducesBadHeight) {
    std::vector<Point> points;

    for (int value = 0; value < 20; ++value) {
        points.push_back(Point{static_cast<double>(value), static_cast<double>(value)});
    }

    KDTree tree;

    for (const Point& point : points) {
        tree.insert(point);
    }

    std::size_t height_before = tree.height();

    tree.rebuild();

    std::size_t height_after = tree.height();

    EXPECT_LT(height_after, height_before);
    expect_same_points(tree.to_list(), points);
}

TEST(KDTreeTest, AutoRebuildKeepsTreeCorrect) {
    std::vector<Point> points;

    for (int value = 0; value < 50; ++value) {
        points.push_back(Point{static_cast<double>(value), static_cast<double>(value)});
    }

    KDTree tree(true, 2.0);

    for (const Point& point : points) {
        tree.insert(point);
    }

    EXPECT_EQ(tree.size(), points.size());
    expect_same_points(tree.to_list(), points);

    for (const Point& point : points) {
        EXPECT_TRUE(tree.contains(point));
    }
}

TEST(KDTreeTest, AutoRebuildHeightIsReasonable) {
    std::vector<Point> points;

    for (int value = 0; value < 100; ++value) {
        points.push_back(Point{static_cast<double>(value), static_cast<double>(value)});
    }

    KDTree tree(true, 2.0);

    for (const Point& point : points) {
        tree.insert(point);
    }

    double max_allowed_height = 2.0 * std::ceil(std::log2(static_cast<double>(points.size() + 1)));

    EXPECT_LE(static_cast<double>(tree.height()), max_allowed_height);
}

TEST(KDTreeTest, CopyHasSamePoints) {
    std::vector<Point> points = {Point{2, 3}, Point{5, 4}, Point{9, 6}};

    KDTree tree(points);
    KDTree copied_tree = tree.copy();

    EXPECT_EQ(copied_tree.size(), tree.size());
    EXPECT_EQ(copied_tree.height(), tree.height());
    expect_same_points(copied_tree.to_list(), tree.to_list());
}

TEST(KDTreeTest, CopyIsIndependent) {
    std::vector<Point> points = {Point{2, 3}, Point{5, 4}, Point{9, 6}};

    KDTree tree(points);
    KDTree copied_tree = tree.copy();

    Point new_point = {100, 100};
    copied_tree.insert(new_point);

    EXPECT_TRUE(copied_tree.contains(new_point));
    EXPECT_FALSE(tree.contains(new_point));
    EXPECT_EQ(copied_tree.size(), tree.size() + 1);
}

TEST(KDTreeTest, InvalidEmptyPointInBuildThrowsInvalidArgument) {
    std::vector<Point> points = {Point{}};

    EXPECT_THROW(KDTree tree(points), std::invalid_argument);
}

TEST(KDTreeTest, InvalidDimensionsInBuildThrowsInvalidArgument) {
    std::vector<Point> points = {Point{1, 2}, Point{3, 4, 5}};

    EXPECT_THROW(KDTree tree(points), std::invalid_argument);
}

TEST(KDTreeTest, InvalidDimensionsInInsertThrowsInvalidArgument) {
    std::vector<Point> points = {Point{1, 2}};
    KDTree tree(points);

    Point invalid_point = {1, 2, 3};

    EXPECT_THROW(tree.insert(invalid_point), std::invalid_argument);
}

TEST(KDTreeTest, InvalidDimensionsInContainsThrowsInvalidArgument) {
    std::vector<Point> points = {Point{1, 2}};
    KDTree tree(points);

    Point invalid_point = {1, 2, 3};

    EXPECT_THROW(tree.contains(invalid_point), std::invalid_argument);
}

TEST(KDTreeTest, InvalidDimensionsInNearestNeighborThrowsInvalidArgument) {
    std::vector<Point> points = {Point{1, 2}};
    KDTree tree(points);

    Point invalid_point = {1, 2, 3};

    EXPECT_THROW(tree.nearest_neighbor(invalid_point), std::invalid_argument);
}

TEST(KDTreeTest, InvalidDimensionsInRangeSearchThrowsInvalidArgument) {
    std::vector<Point> points = {Point{1, 2}};
    KDTree tree(points);

    Point lower_bound = {0, 0, 0};
    Point upper_bound = {10, 10, 10};

    EXPECT_THROW(tree.range_search(lower_bound, upper_bound), std::invalid_argument);
}

TEST(KDTreeTest, InvalidBoundsInRangeSearchThrowsInvalidArgument) {
    std::vector<Point> points = {Point{1, 2}, Point{3, 4}};
    KDTree tree(points);

    Point lower_bound = {10, 0};
    Point upper_bound = {0, 10};

    EXPECT_THROW(tree.range_search(lower_bound, upper_bound), std::invalid_argument);
}

TEST(KDTreeTest, InvalidRebuildFactorThrowsInvalidArgument) {
    EXPECT_THROW(KDTree tree(false, 1.0), std::invalid_argument);
}

TEST(KDTreeTest, MetricsAfterNearestNeighbor) {
    std::vector<Point> points = {
        Point{2, 3}, Point{5, 4}, Point{9, 6},
        Point{4, 7}, Point{8, 1}, Point{7, 2}
    };

    KDTree tree(points);

    Point target = {6, 3};
    tree.nearest_neighbor(target);

    EXPECT_GT(tree.get_distance_calls(), 0u);
    EXPECT_GT(tree.get_visited_nodes(), 0u);
}

TEST(KDTreeTest, MetricsAfterBruteForceNearest) {
    std::vector<Point> points = {
        Point{2, 3}, Point{5, 4}, Point{9, 6},
        Point{4, 7}, Point{8, 1}, Point{7, 2}
    };

    KDTree tree(points);

    Point target = {6, 3};
    tree.brute_force_nearest(points, target);

    EXPECT_EQ(tree.get_distance_calls(), points.size());
}
