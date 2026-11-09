#ifndef TEST_HELPERS_H
#define TEST_HELPERS_H

#include "../dataTypes.h"
#include <cassert>
#include <climits>
#include <cmath>
#include <map>
#include <random>
#include <set>
#include <stdexcept>
#include <utility>
#include <vector>

#ifdef NDEBUG
#error Tests require enabled assert
#endif

using Tree = IntegerTree;
using Order = Tree::TraversalOrder;

struct Compare {
    bool reverse;
    int group_size;
    Compare(bool r = false, int g = 1) : reverse(r), group_size(g) {}
    bool operator()(int a, int b) const {
        return reverse ? a / group_size > b / group_size : a / group_size < b / group_size;
    }
};
using CustomTree = RedBlackTree<int, int, Compare>;

template <typename T>
void fill_tree(T& tree, const int* keys, int count) {
    for (int i = 0; i < count; ++i) tree.insert(keys[i], keys[i] * 10);
}

template <typename T>
void fill_b(T& tree) {
    const int keys[] = {4,2,6,1,3,5,7};
    fill_tree(tree, keys, 7);
}

template <typename T>
void fill_a(T& tree) {
    const int keys[] = {4,2,6,1,3};
    fill_tree(tree, keys, 5);
}

template <typename T>
void fill_c(T& tree) {
    const int keys[] = {8,4,12,2,6,10,14,1,3,5,7,9,11,13,15};
    fill_tree(tree, keys, 15);
}

template <typename T, typename K>
int check_node(const T& tree, typename T::Node* node, typename T::Node* parent,
               const K* lower, const K* upper, std::set<typename T::Node*>& seen) {
    if (node == tree.get_nil()) return 1;
    if (!node || !seen.insert(node).second || node->parent != parent) return 0;
    if (lower && !tree.get_compare()(*lower, node->key)) return 0;
    if (upper && !tree.get_compare()(node->key, *upper)) return 0;
    if (!node->left || !node->right) return 0;
    if (!node->color && (!node->left->color || !node->right->color)) return 0;
    int left = check_node(tree, node->left, node, lower, &node->key, seen);
    if (left == 0) return 0;
    int right = check_node(tree, node->right, node, &node->key, upper, seen);
    if (left != right) return 0;
    return left + (node->color ? 1 : 0);
}

template <typename T>
bool check_tree(const T& tree) {
    typename T::Node* nil = tree.get_nil();
    typename T::Node* root = tree.get_root();
    if (!nil || !root || !nil->color || !root->color) return false;
    if (nil->left != nil || nil->right != nil) return false;
    if (root != nil && root->parent != nil) return false;
    std::set<typename T::Node*> seen;
    return check_node(tree, root, nil, decltype(&root->key)(nullptr),
                      decltype(&root->key)(nullptr), seen) > 0;
}

template <typename T>
int node_count(const T& tree, typename T::Node* node) {
    if (node == tree.get_nil()) return 0;
    return 1 + node_count(tree, node->left) + node_count(tree, node->right);
}
template <typename T>
int node_count(const T& tree) { return node_count(tree, tree.get_root()); }

template <typename T>
int tree_height(const T& tree, typename T::Node* node) {
    if (node == tree.get_nil()) return 0;
    int left = tree_height(tree, node->left);
    int right = tree_height(tree, node->right);
    return 1 + (left > right ? left : right);
}

template <typename T>
void assert_same(const T& a, typename T::Node* x, const T& b, typename T::Node* y,
                 bool independent, bool colors = true) {
    if (x == a.get_nil() || y == b.get_nil()) {
        assert(x == a.get_nil() && y == b.get_nil());
        return;
    }
    assert(!a.get_compare()(x->key, y->key) && !a.get_compare()(y->key, x->key));
    assert(x->value == y->value);
    if (colors) assert(x->color == y->color);
    if (independent) assert(x != y);
    assert_same(a, x->left, b, y->left, independent, colors);
    assert_same(a, x->right, b, y->right, independent, colors);
}
template <typename T>
void assert_copy(const T& a, const T& b) {
    assert(a.get_nil() != b.get_nil());
    assert(check_tree(a) && check_tree(b));
    assert_same(a, a.get_root(), b, b.get_root(), true);
}

struct Collect {
    std::vector<int>& keys;
    std::vector<int>& values;
    void operator()(const int& key, const int& value) const {
        keys.push_back(key);
        values.push_back(value);
    }
};
struct Calls {
    int& count;
    void operator()(const int&, const int&) const { ++count; }
};
struct MapCalls {
    int& count;
    int operator()(const int& value) const { ++count; return value + 1; }
};
struct PredicateCalls {
    int& count;
    bool operator()(const int&) const { ++count; return true; }
};
struct ThrowVisit {
    int& count;
    void operator()(const int&, const int&) const {
        if (++count == 3) throw std::runtime_error("visit");
    }
};
struct ThrowMap {
    int& count;
    int operator()(const int& value) const {
        if (++count == 3) throw std::runtime_error("map");
        return value;
    }
};
struct ThrowPredicate {
    int& count;
    bool operator()(const int&) const {
        if (++count == 3) throw std::runtime_error("predicate");
        return true;
    }
};
struct ThrowReduce {
    int& count;
    int operator()(const int& result, const int& value) const {
        if (++count == 3) throw std::runtime_error("reduce");
        return result + value;
    }
};

inline int twice(const int& value) { return value * 2; }
inline int plus_one(const int& value) { return value + 1; }
inline int five(const int&) { return 5; }
inline bool positive(const int& value) { return value > 0; }
inline bool negative(const int& value) { return value < 0; }
inline bool even(const int& value) { return value % 2 == 0; }
inline bool accept_all(const int&) { return true; }
inline bool accept_none(const int&) { return false; }
inline int add(const int& result, const int& value) { return result + value; }
inline int digits(const int& result, const int& value) { return result * 10 + value / 10; }
inline int length_sum(const int& result, const std::string& value) { return result + int(value.size()); }
struct Accumulator {
    int total;
    explicit Accumulator(int value) : total(value) {}
};
inline Accumulator accumulate(const Accumulator& result, const int& value) {
    return Accumulator(result.total + value);
}
struct Track {
    static int alive;
    static int copies_left;
    static int reads_left;
    static bool throw_output;
    static bool throw_equality;
    int value;
    Track(int n = 0) : value(n) { ++alive; }
    Track(const Track& other) : value(other.value) {
        if (copies_left == 0) throw std::runtime_error("copy");
        if (copies_left > 0) --copies_left;
        ++alive;
    }
    bool operator==(const Track& other) const {
        if (throw_equality) throw std::runtime_error("equality");
        return value == other.value;
    }
    ~Track() { --alive; }
};
inline std::ostream& operator<<(std::ostream& stream, const Track& value) {
    if (Track::throw_output) throw std::runtime_error("output");
    return stream << value.value;
}
inline std::istream& operator>>(std::istream& stream, Track& value) {
    if (Track::reads_left == 0) throw std::runtime_error("input");
    if (Track::reads_left > 0) --Track::reads_left;
    return stream >> value.value;
}
struct TrackCompare {
    static int calls_left;
    static int copies_left;
    bool reverse;
    TrackCompare(bool r = false) : reverse(r) {}
    TrackCompare(const TrackCompare& other) : reverse(other.reverse) {
        if (copies_left == 0) throw std::runtime_error("compare copy");
        if (copies_left > 0) --copies_left;
    }
    TrackCompare& operator=(const TrackCompare& other) {
        reverse = other.reverse;
        return *this;
    }
    bool operator()(int a, int b) const {
        if (calls_left == 0) throw std::runtime_error("compare");
        if (calls_left > 0) --calls_left;
        return reverse ? a > b : a < b;
    }
};
struct TrackKeyCompare {
    bool operator()(const Track& a, const Track& b) const { return a.value < b.value; }
};
using TrackTree = RedBlackTree<int, Track>;
using CompareTrackTree = RedBlackTree<int, Track, TrackCompare>;

void fail_next_allocation();
void allow_allocations();

inline std::string record_text(const std::string& path, int color,
                               const std::string& key, const std::string& value) {
    std::ostringstream stream;
    stream << path << ' ' << color << ' ' << key.size() << ':' << key
           << ' ' << value.size() << ':' << value << '\n';
    return stream.str();
}
std::string fixture_text(Order order);

template <typename T>
std::string output_text(const T& value) {
    std::ostringstream stream;
    stream.precision(std::numeric_limits<long double>::max_digits10);
    stream << value;
    return stream.str();
}

template <typename T>
void reject_input(const std::string& text, const T& initial) {
    T value = initial;
    std::istringstream stream(text);
    stream >> value;
    assert(stream.fail());
    assert(value == initial);
}

inline void reject_tree_text(const std::string& text) {
    Tree tree;
    tree.insert(99, 990);
    Tree::Node* root = tree.get_root();
    Tree::Node* nil = tree.get_nil();
    bool success = tree.from_string(text);
    assert(!success);
    assert(tree.get_root() == root && tree.get_nil() == nil);
    assert(tree.find(99)->value == 990 && node_count(tree) == 1);
    assert(check_tree(tree));
}
#endif

