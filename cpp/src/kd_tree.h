#pragma once

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <limits>
#include <memory>
#include <stdexcept>
#include <utility>
#include <vector>

class KDTree {
public:
    using Point = std::vector<double>;

private:
    struct KDNode {
        Point point;
        std::size_t axis;
        std::unique_ptr<KDNode> left;
        std::unique_ptr<KDNode> right;

        KDNode(const Point& point_value, std::size_t axis_value) : point(point_value), axis(axis_value), left(nullptr), right(nullptr) {}
    };

    std::unique_ptr<KDNode> root_;
    std::size_t size_ = 0;
    std::size_t dimensions_ = 0;
    bool has_dimensions_ = false;
    std::size_t height_ = 0;

    bool auto_rebuild_ = false;
    double rebuild_factor_ = 2.0;

    std::size_t distance_calls_ = 0;
    std::size_t visited_nodes_ = 0;

    static std::unique_ptr<KDNode> copy_recursive(const std::unique_ptr<KDNode>& node) {
        if (!node) {
            return nullptr;
        }

        auto new_node = std::make_unique<KDNode>(node->point, node->axis);
        new_node->left = copy_recursive(node->left);
        new_node->right = copy_recursive(node->right);
        return new_node;
    }

    std::pair<std::unique_ptr<KDNode>, std::size_t> build_recursive(std::vector<Point>& points, std::size_t left, std::size_t right, std::size_t depth) {
        if (left >= right) {
            return {nullptr, 0};
        }

        std::size_t axis = depth % dimensions_;
        std::size_t median_index = left + (right - left) / 2;

        std::sort(points.begin() + static_cast<std::ptrdiff_t>(left), points.begin() + static_cast<std::ptrdiff_t>(right),
                  [axis](const Point& first, const Point& second) {
                      return first[axis] < second[axis];
                  });

        auto node = std::make_unique<KDNode>(points[median_index], axis);

        auto [left_child, left_height] = build_recursive(points, left, median_index, depth + 1);
        auto [right_child, right_height] = build_recursive(points, median_index + 1, right, depth + 1);

        node->left = std::move(left_child);
        node->right = std::move(right_child);

        std::size_t node_height = std::max(left_height, right_height) + 1;
        return {std::move(node), node_height};
    }

    bool should_rebuild() const {
        if (size_ <= 2) {
            return false;
        }

        double max_allowed_height = rebuild_factor_ * std::ceil(std::log2(static_cast<double>(size_ + 1)));
        return static_cast<double>(height_) > max_allowed_height;
    }

    void nearest_neighbor_recursive(const KDNode* node, const Point& target, Point& best_point, double& best_distance) {
        if (node == nullptr) {
            return;
        }

        ++visited_nodes_;

        double current_distance = squared_distance(target, node->point);

        if (current_distance < best_distance) {
            best_point = node->point;
            best_distance = current_distance;
        }

        std::size_t axis = node->axis;

        const KDNode* first_branch = nullptr;
        const KDNode* second_branch = nullptr;

        if (target[axis] < node->point[axis]) {
            first_branch = node->left.get();
            second_branch = node->right.get();
        } else {
            first_branch = node->right.get();
            second_branch = node->left.get();
        }

        nearest_neighbor_recursive(first_branch, target, best_point, best_distance);

        double axis_distance = target[axis] - node->point[axis];

        if (axis_distance * axis_distance < best_distance) {
            nearest_neighbor_recursive(second_branch, target, best_point, best_distance);
        }
    }

    void range_search_recursive(const KDNode* node, const Point& lower_bound, const Point& upper_bound, std::vector<Point>& result) {
        if (node == nullptr) {
            return;
        }

        ++visited_nodes_;

        if (is_point_inside_range(node->point, lower_bound, upper_bound)) {
            result.push_back(node->point);
        }

        std::size_t axis = node->axis;

        if (lower_bound[axis] <= node->point[axis]) {
            range_search_recursive(node->left.get(), lower_bound, upper_bound, result);
        }

        if (upper_bound[axis] >= node->point[axis]) {
            range_search_recursive(node->right.get(), lower_bound, upper_bound, result);
        }
    }

    void to_list_recursive(const KDNode* node, std::vector<Point>& result) const {
        if (node == nullptr) {
            return;
        }

        result.push_back(node->point);
        to_list_recursive(node->left.get(), result);
        to_list_recursive(node->right.get(), result);
    }

    double squared_distance(const Point& first_point, const Point& second_point) {
        ++distance_calls_;

        double distance = 0.0;

        for (std::size_t index = 0; index < dimensions_; ++index) {
            double difference = first_point[index] - second_point[index];
            distance += difference * difference;
        }

        return distance;
    }

    bool is_point_inside_range(const Point& point, const Point& lower_bound, const Point& upper_bound) const {
        for (std::size_t index = 0; index < dimensions_; ++index) {
            if (point[index] < lower_bound[index]) {
                return false;
            }

            if (point[index] > upper_bound[index]) {
                return false;
            }
        }

        return true;
    }

    void validate_points(const std::vector<Point>& points) const {
        for (const Point& point : points) {
            validate_point(point);
        }
    }

    void validate_point(const Point& point) const {
        if (point.empty()) {
            throw std::invalid_argument("point must have at least one dimension");
        }

        if (has_dimensions_ && point.size() != dimensions_) {
            throw std::invalid_argument("point has incorrect number of dimensions");
        }
    }

    void validate_bounds(const Point& lower_bound, const Point& upper_bound) const {
        for (std::size_t index = 0; index < dimensions_; ++index) {
            if (lower_bound[index] > upper_bound[index]) {
                throw std::invalid_argument("lower_bound values must be less than or equal to upper_bound values");
            }
        }
    }

    void reset_metrics() {
        distance_calls_ = 0;
        visited_nodes_ = 0;
    }

public:
    KDTree(bool auto_rebuild = false, double rebuild_factor = 2.0) : auto_rebuild_(auto_rebuild), rebuild_factor_(rebuild_factor) {
        if (rebuild_factor_ <= 1.0) {
            throw std::invalid_argument("rebuild_factor must be greater than 1.0");
        }
    }

    explicit KDTree(const std::vector<Point>& points, bool auto_rebuild = false, double rebuild_factor = 2.0) : KDTree(auto_rebuild, rebuild_factor) {
        build(points);
    }

    KDTree(const KDTree& other)
        : root_(copy_recursive(other.root_)),
          size_(other.size_),
          dimensions_(other.dimensions_),
          has_dimensions_(other.has_dimensions_),
          height_(other.height_),
          auto_rebuild_(other.auto_rebuild_),
          rebuild_factor_(other.rebuild_factor_),
          distance_calls_(other.distance_calls_),
          visited_nodes_(other.visited_nodes_) {}

    KDTree& operator=(const KDTree& other) {
        if (this == &other) {
            return *this;
        }

        root_ = copy_recursive(other.root_);
        size_ = other.size_;
        dimensions_ = other.dimensions_;
        has_dimensions_ = other.has_dimensions_;
        height_ = other.height_;
        auto_rebuild_ = other.auto_rebuild_;
        rebuild_factor_ = other.rebuild_factor_;
        distance_calls_ = other.distance_calls_;
        visited_nodes_ = other.visited_nodes_;
        return *this;
    }

    KDTree(KDTree&& other) noexcept = default;
    KDTree& operator=(KDTree&& other) noexcept = default;
    ~KDTree() = default;

    void build(const std::vector<Point>& input_points) {
        std::vector<Point> points = input_points;

        if (points.empty()) {
            root_ = nullptr;
            size_ = 0;
            dimensions_ = 0;
            has_dimensions_ = false;
            height_ = 0;
            return;
        }

        dimensions_ = points[0].size();
        has_dimensions_ = true;
        validate_points(points);

        auto [new_root, new_height] = build_recursive(points, 0, points.size(), 0);
        root_ = std::move(new_root);
        height_ = new_height;
        size_ = points.size();
    }

    void insert(const Point& point) {
        if (!root_) {
            dimensions_ = point.size();
            has_dimensions_ = true;
            validate_point(point);

            root_ = std::make_unique<KDNode>(point, 0);
            size_ = 1;
            height_ = 1;
            return;
        }

        validate_point(point);

        KDNode* current_node = root_.get();
        std::size_t depth = 0;
        std::size_t inserted_depth = 0;

        while (true) {
            std::size_t axis = current_node->axis;
            std::size_t next_axis = (axis + 1) % dimensions_;

            if (point[axis] < current_node->point[axis]) {
                if (!current_node->left) {
                    current_node->left = std::make_unique<KDNode>(point, next_axis);
                    inserted_depth = depth + 1;
                    break;
                }

                current_node = current_node->left.get();
            } else {
                if (!current_node->right) {
                    current_node->right = std::make_unique<KDNode>(point, next_axis);
                    inserted_depth = depth + 1;
                    break;
                }

                current_node = current_node->right.get();
            }

            ++depth;
        }

        ++size_;
        height_ = std::max(height_, inserted_depth + 1);

        if (auto_rebuild_ && should_rebuild()) {
            rebuild();
        }
    }

    void rebuild() {
        std::vector<Point> points = to_list();
        build(points);
    }

    std::pair<Point, double> nearest_neighbor(const Point& target) {
        if (!root_) {
            return {Point{}, std::numeric_limits<double>::infinity()};
        }

        validate_point(target);
        reset_metrics();

        Point best_point = root_->point;
        double best_distance = squared_distance(target, best_point);

        nearest_neighbor_recursive(root_.get(), target, best_point, best_distance);

        return {best_point, std::sqrt(best_distance)};
    }

    std::vector<Point> range_search(const Point& lower_bound, const Point& upper_bound) {
        if (!root_) {
            return {};
        }

        validate_point(lower_bound);
        validate_point(upper_bound);
        validate_bounds(lower_bound, upper_bound);

        reset_metrics();

        std::vector<Point> result;
        range_search_recursive(root_.get(), lower_bound, upper_bound, result);
        return result;
    }

    bool contains(const Point& point) const {
        if (!root_) {
            return false;
        }

        validate_point(point);

        const KDNode* current_node = root_.get();

        while (current_node != nullptr) {
            if (current_node->point == point) {
                return true;
            }

            std::size_t axis = current_node->axis;

            if (point[axis] < current_node->point[axis]) {
                current_node = current_node->left.get();
            } else {
                current_node = current_node->right.get();
            }
        }

        return false;
    }

    std::size_t height() const {
        return height_;
    }

    std::vector<Point> to_list() const {
        std::vector<Point> result;
        to_list_recursive(root_.get(), result);
        return result;
    }

    KDTree copy() const {
        return KDTree(*this);
    }

    std::size_t get_distance_calls() const {
        return distance_calls_;
    }

    std::size_t get_visited_nodes() const {
        return visited_nodes_;
    }

    std::pair<Point, double> brute_force_nearest(const std::vector<Point>& input_points, const Point& target) {
        std::vector<Point> points = input_points;

        if (points.empty()) {
            return {Point{}, std::numeric_limits<double>::infinity()};
        }

        validate_point(target);

        for (const Point& point : points) {
            validate_point(point);
        }

        reset_metrics();

        Point best_point = points[0];
        double best_distance = squared_distance(target, best_point);

        for (std::size_t index = 1; index < points.size(); ++index) {
            double current_distance = squared_distance(target, points[index]);

            if (current_distance < best_distance) {
                best_point = points[index];
                best_distance = current_distance;
            }
        }

        return {best_point, std::sqrt(best_distance)};
    }

    std::vector<Point> brute_force_range_search(const std::vector<Point>& input_points, const Point& lower_bound, const Point& upper_bound) {
        std::vector<Point> points = input_points;

        if (points.empty()) {
            return {};
        }

        validate_point(lower_bound);
        validate_point(upper_bound);
        validate_bounds(lower_bound, upper_bound);

        for (const Point& point : points) {
            validate_point(point);
        }

        reset_metrics();

        std::vector<Point> result;

        for (const Point& point : points) {
            if (is_point_inside_range(point, lower_bound, upper_bound)) {
                result.push_back(point);
            }
        }

        return result;
    }

    std::size_t size() const {
        return size_;
    }

    bool empty() const {
        return size_ == 0;
    }

    explicit operator bool() const {
        return size_ > 0;
    }
};
