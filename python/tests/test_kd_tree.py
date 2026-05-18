import math
import random
import pytest

from kd_tree import KDTree


def assert_same_nearest_result(kd_result, brute_result) -> None:
    kd_point, kd_distance = kd_result
    brute_point, brute_distance = brute_result

    assert kd_point is not None
    assert brute_point is not None
    assert math.isclose(kd_distance, brute_distance)


def test_build_empty_tree() -> None:
    tree = KDTree([])

    assert len(tree) == 0
    assert tree.height() == 0
    assert bool(tree) is False
    assert tree.to_list() == []


def test_build_tree_size() -> None:
    points = [(2, 3), (5, 4), (9, 6), (4, 7), (8, 1), (7, 2)]

    tree = KDTree(points)

    assert len(tree) == len(points)
    assert bool(tree) is True
    assert set(tree.to_list()) == set(points)


def test_build_one_point_tree() -> None:
    tree = KDTree([(2, 3)])

    assert len(tree) == 1
    assert tree.height() == 1
    assert tree.to_list() == [(2, 3)]


def test_insert_into_empty_tree() -> None:
    tree = KDTree()

    tree.insert((2, 3))

    assert len(tree) == 1
    assert tree.height() == 1
    assert (2, 3) in tree


def test_insert_into_non_empty_tree() -> None:
    tree = KDTree([(2, 3), (5, 4)])

    tree.insert((9, 6))

    assert len(tree) == 3
    assert (9, 6) in tree
    assert set(tree.to_list()) == {(2, 3), (5, 4), (9, 6)}


def test_contains_existing_point() -> None:
    points = [(2, 3), (5, 4), (9, 6)]

    tree = KDTree(points)

    assert tree.contains((5, 4)) is True
    assert (5, 4) in tree


def test_contains_missing_point() -> None:
    points = [(2, 3), (5, 4), (9, 6)]

    tree = KDTree(points)

    assert tree.contains((100, 100)) is False
    assert (100, 100) not in tree


def test_contains_in_empty_tree() -> None:
    tree = KDTree()

    assert tree.contains((1, 2)) is False


def test_nearest_neighbor_empty_tree() -> None:
    tree = KDTree()

    nearest_point, distance = tree.nearest_neighbor((1, 2))

    assert nearest_point is None
    assert distance == math.inf


def test_nearest_neighbor_simple_case() -> None:
    points = [(2, 3), (5, 4), (9, 6), (4, 7), (8, 1), (7, 2)]

    tree = KDTree(points)

    nearest_point, distance = tree.nearest_neighbor((6, 3))

    assert nearest_point in {(5, 4), (7, 2)}
    assert math.isclose(distance, math.sqrt(2))


def test_nearest_neighbor_exact_match() -> None:
    points = [(2, 3), (5, 4), (9, 6)]

    tree = KDTree(points)

    nearest_point, distance = tree.nearest_neighbor((5, 4))

    assert nearest_point == (5, 4)
    assert distance == 0


def test_nearest_neighbor_matches_brute_force() -> None:
    points = [(2, 3), (5, 4), (9, 6), (4, 7), (8, 1), (7, 2)]

    target = (6, 3)

    tree = KDTree(points)

    kd_result = tree.nearest_neighbor(target)
    brute_result = tree.brute_force_nearest(points, target)

    assert_same_nearest_result(kd_result, brute_result)


def test_nearest_neighbor_matches_brute_force_random_points() -> None:
    random.seed(42)

    points = []

    for _ in range(100):
        x = random.randint(-100, 100)
        y = random.randint(-100, 100)
        points.append((x, y))

    tree = KDTree(points)

    for _ in range(30):
        target = (
            random.randint(-100, 100),
            random.randint(-100, 100),
        )

        kd_result = tree.nearest_neighbor(target)
        brute_result = tree.brute_force_nearest(points, target)

        assert_same_nearest_result(kd_result, brute_result)


def test_nearest_neighbor_3d_points() -> None:
    points = [(1, 2, 3), (4, 5, 6), (7, 8, 9), (2, 2, 2)]

    target = (3, 3, 3)

    tree = KDTree(points)

    kd_result = tree.nearest_neighbor(target)
    brute_result = tree.brute_force_nearest(points, target)

    assert_same_nearest_result(kd_result, brute_result)


def test_range_search_empty_tree() -> None:
    tree = KDTree()

    result = tree.range_search((0, 0), (10, 10))

    assert result == []


def test_range_search_simple_case() -> None:
    points = [(2, 3), (5, 4), (9, 6), (4, 7), (8, 1), (7, 2)]

    tree = KDTree(points)

    result = tree.range_search(lower_bound=(3, 1), upper_bound=(8, 5))

    assert set(result) == {(5, 4), (8, 1), (7, 2)}


def test_range_search_matches_brute_force() -> None:
    points = [(2, 3), (5, 4), (9, 6), (4, 7), (8, 1), (7, 2)]

    lower_bound = (3, 1)
    upper_bound = (8, 5)

    tree = KDTree(points)

    kd_result = tree.range_search(lower_bound, upper_bound)
    brute_result = tree.brute_force_range_search(points, lower_bound, upper_bound)

    assert set(kd_result) == set(brute_result)


def test_range_search_matches_brute_force_random_points() -> None:
    random.seed(42)

    points = []

    for _ in range(100):
        x = random.randint(-100, 100)
        y = random.randint(-100, 100)
        points.append((x, y))

    tree = KDTree(points)

    for _ in range(30):
        x1 = random.randint(-100, 100)
        x2 = random.randint(-100, 100)
        y1 = random.randint(-100, 100)
        y2 = random.randint(-100, 100)

        lower_bound = (min(x1, x2), min(y1, y2))
        upper_bound = (max(x1, x2), max(y1, y2))

        kd_result = tree.range_search(lower_bound, upper_bound)
        brute_result = tree.brute_force_range_search(points, lower_bound, upper_bound)

        assert set(kd_result) == set(brute_result)


def test_range_search_3d_points() -> None:
    points = [(1, 2, 3), (4, 5, 6), (7, 8, 9), (2, 2, 2), (5, 5, 5)]

    lower_bound = (2, 2, 2)
    upper_bound = (5, 5, 6)

    tree = KDTree(points)

    kd_result = tree.range_search(lower_bound, upper_bound)
    brute_result = tree.brute_force_range_search(points, lower_bound, upper_bound)

    assert set(kd_result) == set(brute_result)


def test_rebuild_keeps_all_points() -> None:
    points = [(1, 1), (2, 2), (3, 3), (4, 4), (5, 5)]

    tree = KDTree()

    for point in points:
        tree.insert(point)

    tree.rebuild()

    assert len(tree) == len(points)
    assert set(tree.to_list()) == set(points)

    for point in points:
        assert point in tree


def test_rebuild_reduces_bad_height() -> None:
    points = []

    for value in range(20):
        points.append((value, value))

    tree = KDTree()

    for point in points:
        tree.insert(point)

    height_before = tree.height()

    tree.rebuild()

    height_after = tree.height()

    assert height_after < height_before
    assert set(tree.to_list()) == set(points)


def test_auto_rebuild_keeps_tree_correct() -> None:
    points = []

    for value in range(50):
        points.append((value, value))

    tree = KDTree(auto_rebuild=True, rebuild_factor=2.0)

    for point in points:
        tree.insert(point)

    assert len(tree) == len(points)
    assert set(tree.to_list()) == set(points)

    for point in points:
        assert point in tree


def test_auto_rebuild_height_is_reasonable() -> None:
    points = []

    for value in range(100):
        points.append((value, value))

    tree = KDTree(auto_rebuild=True, rebuild_factor=2.0)

    for point in points:
        tree.insert(point)

    max_allowed_height = 2.0 * math.ceil(math.log2(len(points) + 1))

    assert tree.height() <= max_allowed_height


def test_copy_has_same_points() -> None:
    points = [(2, 3), (5, 4), (9, 6)]

    tree = KDTree(points)
    copied_tree = tree.copy()

    assert len(copied_tree) == len(tree)
    assert copied_tree.height() == tree.height()
    assert set(copied_tree.to_list()) == set(tree.to_list())


def test_copy_is_independent() -> None:
    points = [(2, 3), (5, 4), (9, 6)]

    tree = KDTree(points)
    copied_tree = tree.copy()

    copied_tree.insert((100, 100))

    assert (100, 100) in copied_tree
    assert (100, 100) not in tree
    assert len(copied_tree) == len(tree) + 1


def test_invalid_empty_point_in_build() -> None:
    with pytest.raises(ValueError):
        KDTree([()])


def test_invalid_dimensions_in_build() -> None:
    points = [(1, 2), (3, 4, 5)]

    with pytest.raises(ValueError):
        KDTree(points)


def test_invalid_dimensions_in_insert() -> None:
    tree = KDTree([(1, 2)])

    with pytest.raises(ValueError):
        tree.insert((1, 2, 3))


def test_invalid_dimensions_in_contains() -> None:
    tree = KDTree([(1, 2)])

    with pytest.raises(ValueError):
        tree.contains((1, 2, 3))


def test_invalid_dimensions_in_nearest_neighbor() -> None:
    tree = KDTree([(1, 2)])

    with pytest.raises(ValueError):
        tree.nearest_neighbor((1, 2, 3))


def test_invalid_dimensions_in_range_search() -> None:
    tree = KDTree([(1, 2)])

    with pytest.raises(ValueError):
        tree.range_search((0, 0, 0), (10, 10, 10))


def test_invalid_bounds_in_range_search() -> None:
    tree = KDTree([(1, 2), (3, 4)])

    with pytest.raises(ValueError):
        tree.range_search((10, 0), (0, 10))


def test_invalid_rebuild_factor() -> None:
    with pytest.raises(ValueError):
        KDTree(rebuild_factor=1.0)


def test_metrics_after_nearest_neighbor() -> None:
    points = [(2, 3), (5, 4), (9, 6), (4, 7), (8, 1), (7, 2)]

    tree = KDTree(points)

    tree.nearest_neighbor((6, 3))

    assert tree.get_distance_calls() > 0
    assert tree.get_visited_nodes() > 0


def test_metrics_after_brute_force_nearest() -> None:
    points = [(2, 3), (5, 4), (9, 6), (4, 7), (8, 1), (7, 2)]

    tree = KDTree(points)

    tree.brute_force_nearest(points, (6, 3))

    assert tree.get_distance_calls() == len(points)