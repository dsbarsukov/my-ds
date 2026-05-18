from __future__ import annotations

import math
from typing import Sequence


Point = tuple[float, ...]


class KDNode:
    def __init__(self, point: Point, axis: int) -> None:
        self.point = point
        self.axis = axis
        self.left = None
        self.right = None


class KDTree:
    def __init__(self, points: Sequence[Point] | None = None, auto_rebuild: bool = False, rebuild_factor: float = 2.0) -> None:
        if rebuild_factor <= 1.0:
            raise ValueError("rebuild_factor must be greater than 1.0")

        self._root = None
        self._size = 0
        self._dimensions = None
        self._height = 0

        self._auto_rebuild = auto_rebuild
        self._rebuild_factor = rebuild_factor

        self._distance_calls = 0
        self._visited_nodes = 0

        if points is not None:
            self.build(points)

    def build(self, points: Sequence[Point]) -> None:
        points = list(points)

        if len(points) == 0:
            self._root = None
            self._size = 0
            self._dimensions = None
            self._height = 0
            return

        self._dimensions = len(points[0])
        self._validate_points(points)

        self._root, self._height = self._build_recursive(points, depth=0)
        self._size = len(points)

    def _build_recursive(self, points: list[Point], depth: int) -> tuple[KDNode | None, int]:
        if len(points) == 0:
            return None, 0

        axis = depth % self._dimensions

        points.sort(key=lambda point: point[axis])
        median_index = len(points) // 2

        node = KDNode(points[median_index], axis)

        left_points = points[:median_index]
        right_points = points[median_index + 1:]

        node.left, left_height = self._build_recursive(left_points, depth + 1)
        node.right, right_height = self._build_recursive(right_points, depth + 1)

        height = max(left_height, right_height) + 1

        return node, height

    def insert(self, point: Point) -> None:
        if self._root is None:
            self._dimensions = len(point)
            self._validate_point(point)

            self._root = KDNode(point, axis=0)
            self._size = 1
            self._height = 1
            return

        self._validate_point(point)

        current_node = self._root
        depth = 0

        while True:
            axis = current_node.axis
            next_axis = (axis + 1) % self._dimensions

            if point[axis] < current_node.point[axis]:
                if current_node.left is None:
                    current_node.left = KDNode(point, axis=next_axis)
                    inserted_depth = depth + 1
                    break

                current_node = current_node.left

            else:
                if current_node.right is None:
                    current_node.right = KDNode(point, axis=next_axis)
                    inserted_depth = depth + 1
                    break

                current_node = current_node.right

            depth += 1

        self._size += 1
        self._height = max(self._height, inserted_depth + 1)

        if self._auto_rebuild and self._should_rebuild():
            self.rebuild()

    def _should_rebuild(self) -> bool:
        if self._size <= 2:
            return False

        max_allowed_height = self._rebuild_factor * math.ceil(math.log2(self._size + 1))

        return self._height > max_allowed_height

    def rebuild(self) -> None:
        points = self.to_list()
        self.build(points)

    def nearest_neighbor(self, target: Point) -> tuple[Point | None, float]:
        if self._root is None:
            return None, math.inf

        self._validate_point(target)
        self._reset_metrics()

        best_point = self._root.point
        best_distance = self._squared_distance(target, best_point)

        best_point, best_distance = self._nearest_neighbor_recursive(node=self._root, target=target, best_point=best_point, best_distance=best_distance)

        return best_point, math.sqrt(best_distance)

    def _nearest_neighbor_recursive(self, node: KDNode | None, target: Point, best_point: Point, best_distance: float) -> tuple[Point, float]:

        if node is None:
            return best_point, best_distance

        self._visited_nodes += 1

        current_distance = self._squared_distance(target, node.point)

        if current_distance < best_distance:
            best_point = node.point
            best_distance = current_distance

        axis = node.axis

        if target[axis] < node.point[axis]:
            first_branch = node.left
            second_branch = node.right
        else:
            first_branch = node.right
            second_branch = node.left

        best_point, best_distance = self._nearest_neighbor_recursive(first_branch, target, best_point, best_distance)

        axis_distance = target[axis] - node.point[axis]

        if axis_distance * axis_distance < best_distance:
            best_point, best_distance = self._nearest_neighbor_recursive(
                second_branch,
                target,
                best_point,
                best_distance
            )

        return best_point, best_distance

    def range_search(self, lower_bound: Point, upper_bound: Point) -> list[Point]:
        if self._root is None:
            return []

        self._validate_point(lower_bound)
        self._validate_point(upper_bound)
        self._validate_bounds(lower_bound, upper_bound)

        self._reset_metrics()

        result = []

        self._range_search_recursive(node=self._root, lower_bound=lower_bound, upper_bound=upper_bound, result=result)

        return result

    def _range_search_recursive(self, node: KDNode | None, lower_bound: Point, upper_bound: Point, result: list[Point]) -> None:

        if node is None:
            return

        self._visited_nodes += 1

        if self._is_point_inside_range(node.point, lower_bound, upper_bound):
            result.append(node.point)

        axis = node.axis

        if lower_bound[axis] <= node.point[axis]:
            self._range_search_recursive(node.left, lower_bound, upper_bound, result)

        if upper_bound[axis] >= node.point[axis]:
            self._range_search_recursive(node.right, lower_bound, upper_bound, result)

    def contains(self, point: Point) -> bool:
        if self._root is None:
            return False

        self._validate_point(point)

        current_node = self._root

        while current_node is not None:
            if current_node.point == point:
                return True

            axis = current_node.axis

            if point[axis] < current_node.point[axis]:
                current_node = current_node.left
            else:
                current_node = current_node.right

        return False

    def height(self) -> int:
        return self._height


    def to_list(self) -> list[Point]:
        result = []
        self._to_list_recursive(self._root, result)
        return result

    def _to_list_recursive(self, node: KDNode | None, result: list[Point]) -> None:
        if node is None:
            return

        result.append(node.point)
        self._to_list_recursive(node.left, result)
        self._to_list_recursive(node.right, result)

    def copy(self) -> "KDTree":
        new_tree = KDTree(auto_rebuild=self._auto_rebuild, rebuild_factor=self._rebuild_factor)
        new_tree._dimensions = self._dimensions
        new_tree._size = self._size
        new_tree._height = self._height
        new_tree._root = self._copy_recursive(self._root)
        return new_tree

    def _copy_recursive(self, node: KDNode | None) -> KDNode | None:
        if node is None:
            return None

        new_node = KDNode(node.point, node.axis)
        new_node.left = self._copy_recursive(node.left)
        new_node.right = self._copy_recursive(node.right)

        return new_node

    def _squared_distance(self, first_point: Point, second_point: Point) -> float:
        self._distance_calls += 1

        distance = 0.0

        for index in range(self._dimensions):
            difference = first_point[index] - second_point[index]
            distance += difference * difference

        return distance

    def _is_point_inside_range(self, point: Point, lower_bound: Point, upper_bound: Point) -> bool:

        for index in range(self._dimensions):
            if point[index] < lower_bound[index]:
                return False

            if point[index] > upper_bound[index]:
                return False

        return True

    def _validate_points(self, points: Sequence[Point]) -> None:
        for point in points:
            self._validate_point(point)

    def _validate_point(self, point: Point) -> None:
        if len(point) == 0:
            raise ValueError("point must have at least one dimension")

        if self._dimensions is not None and len(point) != self._dimensions:
            raise ValueError(f"point must have {self._dimensions} dimensions, but got {len(point)}")

    def _validate_bounds(self, lower_bound: Point, upper_bound: Point) -> None:
        for index in range(self._dimensions):
            if lower_bound[index] > upper_bound[index]:
                raise ValueError("lower_bound values must be less than or equal to upper_bound values")

    def _reset_metrics(self) -> None:
        self._distance_calls = 0
        self._visited_nodes = 0

    def get_distance_calls(self) -> int:
        return self._distance_calls

    def get_visited_nodes(self) -> int:
        return self._visited_nodes

    def brute_force_nearest(self, points: Sequence[Point], target: Point) -> tuple[Point | None, float]:
        """
        Finds the nearest point by checking all points one by one.
        This method is used as a baseline for tests and benchmarks.
        """
        points = list(points)

        if len(points) == 0:
            return None, math.inf

        self._validate_point(target)

        for point in points:
            self._validate_point(point)

        self._reset_metrics()

        best_point = points[0]
        best_distance = self._squared_distance(target, best_point)

        for point in points[1:]:
            current_distance = self._squared_distance(target, point)

            if current_distance < best_distance:
                best_point = point
                best_distance = current_distance

        return best_point, math.sqrt(best_distance)

    def brute_force_range_search(self, points: Sequence[Point], lower_bound: Point, upper_bound: Point) -> list[Point]:
        """
        Finds all points inside the given range by checking all points one by one.
        This method is used as a baseline for tests and benchmarks.
        """
        points = list(points)

        if len(points) == 0:
            return []

        self._validate_point(lower_bound)
        self._validate_point(upper_bound)
        self._validate_bounds(lower_bound, upper_bound)

        for point in points:
            self._validate_point(point)

        self._reset_metrics()

        result = []

        for point in points:
            if self._is_point_inside_range(point, lower_bound, upper_bound):
                result.append(point)

        return result

    def __len__(self) -> int:
        return self._size

    def __contains__(self, point: Point) -> bool:
        return self.contains(point)

    def __bool__(self) -> bool:
        return self._size > 0