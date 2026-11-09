#include "rbTree.h"
#include <algorithm>
#include <chrono>
#include <fstream>
#include <iomanip>
#include <random>
#include <stdexcept>
#include <vector>

using Tree = RedBlackTree<int, int>;
using Order = Tree::TraversalOrder;
using Clock = std::chrono::steady_clock;

const char* order_names[] = {"NLR", "NRL", "LNR", "RNL", "LRN", "RLN"};

void require(bool condition, const char* message) {
    if (!condition) throw std::runtime_error(message);
}

long long expected_sum(long long count) {
    return count * (count - 1) / 2;
}

double elapsed_ms(Clock::time_point start) {
    return std::chrono::duration<double, std::milli>(Clock::now() - start).count();
}

// Validation is always performed outside the measured interval.
int check_node(const Tree::Node* node, const Tree::Node* nil,
               const Tree::Node* parent, long long low, long long high,
               long long& count, long long& sum) {
    if (node == nil) return 1;
    require(node != nullptr && node->parent == parent, "Invalid parent");
    require(low < node->key && node->key < high, "Invalid key order");
    if (!node->color)
        require(node->left->color && node->right->color, "Adjacent red nodes");
    ++count;
    sum += node->value;
    int left = check_node(node->left, nil, node, low, node->key, count, sum);
    int right = check_node(node->right, nil, node, node->key, high, count, sum);
    require(left == right, "Different black heights");
    return left + (node->color ? 1 : 0);
}

void check_tree(const Tree& tree, long long expected_count, long long sum) {
    long long count = 0;
    long long actual_sum = 0;
    require(tree.get_nil()->color && tree.get_root()->color, "Invalid root or nil");
    check_node(tree.get_root(), tree.get_nil(), tree.get_nil(),
               -2147483649LL, 2147483648LL, count, actual_sum);
    require(count == expected_count && actual_sum == sum, "Wrong tree contents");
}

long long count_nodes(const Tree::Node* node, const Tree::Node* nil) {
    if (node == nil) return 0;
    return 1 + count_nodes(node->left, nil) + count_nodes(node->right, nil);
}

long long sum_values(const Tree::Node* node, const Tree::Node* nil) {
    if (node == nil) return 0;
    return node->value + sum_values(node->left, nil) + sum_values(node->right, nil);
}

// MemAvailable excludes swap. The cgroup limit is also respected, if present.
unsigned long long available_memory() {
    std::ifstream input("/proc/meminfo");
    std::string name, rest;
    unsigned long long value = 0, available = 0;
    while (input >> name >> value) {
        std::getline(input, rest);
        if (name == "MemAvailable:") {
            available = value * 1024;
            break;
        }
    }
    std::ifstream limit_file("/sys/fs/cgroup/memory.max");
    std::ifstream used_file("/sys/fs/cgroup/memory.current");
    unsigned long long limit = 0, used = 0;
    if (limit_file >> limit && used_file >> used && limit > used) {
        if (!available || limit - used < available) available = limit - used;
    }
    return available;
}

struct Measurement {
    std::string operation, order;
    int size, queries, repeat;
    long long subtree;
    double time;
};

struct Report {
    std::ofstream file;
    std::vector<Measurement> values;

    explicit Report(const std::string& path) : file(path) {
        require(bool(file), "Cannot open CSV file");
        file << "operation;size;order;queries;subtree_size;repeat;time_ms;"
                "ns_per_query;status;detail\n";
        file << std::fixed << std::setprecision(6);
        std::cout << std::fixed << std::setprecision(3);
    }

    void add(const std::string& operation, int size, const std::string& order,
             int queries, long long subtree, int repeat, double time) {
        values.push_back({operation, order, size, queries, repeat, subtree, time});
        file << operation << ';' << size << ';' << order << ';' << queries << ';'
             << subtree << ';' << repeat << ';' << time << ';'
             << time * 1000000 / queries << ";OK;\n";
        file.flush();
        require(bool(file), "Cannot write CSV file");
        std::cout << operation << " N=" << size << " " << order
                  << " repeat=" << repeat << " " << time << " ms\n";
    }

    void skip(const std::string& operation, int size, int repeat,
              const std::string& reason) {
        file << operation << ';' << size << ";;;;" << repeat
             << ";;;SKIPPED;" << reason << '\n';
        file.flush();
        std::cout << "SKIPPED " << operation << " N=" << size << ": " << reason << '\n';
    }

    bool memory(const std::string& operation, int size, int repeat,
                unsigned long long extra_bytes) {
        unsigned long long available = available_memory();
        if (available && extra_bytes > available * 3 / 4) {
            skip(operation, size, repeat, "estimated extra memory exceeds 75% available");
            return false;
        }
        return true;
    }

    void medians() {
        std::cout << "\nMEDIANS\n";
        for (std::size_t i = 0; i < values.size(); ++i) {
            const Measurement& row = values[i];
            bool first = true;
            std::vector<double> times;
            for (std::size_t j = 0; j < values.size(); ++j) {
                const Measurement& other = values[j];
                if (row.operation == other.operation && row.size == other.size &&
                    row.order == other.order && row.queries == other.queries &&
                    row.subtree == other.subtree) {
                    if (j < i) first = false;
                    times.push_back(other.time);
                }
            }
            if (!first) continue;
            std::sort(times.begin(), times.end());
            double median = times[times.size() / 2];
            if (times.size() % 2 == 0)
                median = (median + times[times.size() / 2 - 1]) / 2;
            file << row.operation << ';' << row.size << ';' << row.order << ';'
                 << row.queries << ';' << row.subtree << ";median;" << median
                 << ';' << median * 1000000 / row.queries << ";OK;"
                 << times.size() << " repetitions\n";
            std::cout << row.operation << " N=" << row.size << " " << row.order
                      << " median=" << median << " ms (" << times.size() << " runs)\n";
        }
        file.flush();
        require(bool(file), "Cannot write medians");
    }
};

bool selected(const std::string& selection, const std::string& operation) {
    if (selection == "all" || selection == operation) return true;
    return selection == "basic" &&
           (operation == "insert" || operation == "find" || operation == "erase" ||
            operation == "traverse" || operation == "reduce" || operation == "path");
}

unsigned long long node_bytes() {
    return (sizeof(Tree::Node) + 16 + 15) / 16 * 16;
}

void fill_tree(Tree& tree, const std::vector<int>& keys, int offset = 0) {
    for (int key : keys) tree.insert(key + offset, key + offset);
}

struct Counter {
    long long& count;
    long long& sum;
    void operator()(const int&, const int& value) const {
        ++count;
        sum += value;
    }
};

int increment(const int& value) { return value + 1; }
bool even(const int& value) { return value % 2 == 0; }
long long add(const long long& result, const int& value) { return result + value; }

void test_insert(Report& report, int size, int repeat) {
    if (!report.memory("insert", size, repeat, size * (node_bytes() + 4))) return;
    std::vector<int> keys(size);
    const char* names[] = {"ascending", "descending", "random"};
    for (int order = 0; order < 3; ++order) {
        for (int i = 0; i < size; ++i) keys[i] = i;
        if (order == 1) std::reverse(keys.begin(), keys.end());
        if (order == 2) {
            std::mt19937 generator(42);
            std::shuffle(keys.begin(), keys.end(), generator);
        }
        Tree tree;
        Clock::time_point start = Clock::now();
        fill_tree(tree, keys);
        double time = elapsed_ms(start);
        check_tree(tree, size, expected_sum(size));
        report.add("insert", size, names[order], size, 0, repeat, time);
    }
}

void test_find(Report& report, const Tree& tree, int size, int repeat) {
    int queries = std::min(size, 100000);
    std::vector<int> keys(queries);
    for (int i = 0; i < queries; ++i) keys[i] = int(7919LL * i % size);
    for (int missing = 0; missing < 2; ++missing) {
        long long found = 0, sum = 0, expected = 0;
        for (int key : keys) expected += key;
        Clock::time_point start = Clock::now();
        for (int key : keys) {
            Tree::Node* node = tree.find(key + (missing ? size : 0));
            if (node) { ++found; sum += node->value; }
        }
        double time = elapsed_ms(start);
        require(found == (missing ? 0 : queries), "Wrong find count");
        require(sum == (missing ? 0 : expected), "Wrong find values");
        report.add(missing ? "find_missing" : "find_present", size, "random",
                   queries, 0, repeat, time);
    }
}

struct PathQuery {
    std::string path, up;
    int key;
};

void test_paths(Report& report, const Tree& tree, int size, int repeat) {
    const char* names[] = {"shallow", "middle", "deep"};
    for (int level = 0; level < 3; ++level) {
        std::vector<PathQuery> paths;
        for (int i = 0; i < 256; ++i) {
            Tree::Node* node = tree.find(int(7919LL * i % size));
            std::string path;
            while (node->parent != tree.get_nil()) {
                path += node == node->parent->left ? 'L' : 'R';
                node = node->parent;
            }
            std::reverse(path.begin(), path.end());
            std::size_t depth = path.size();
            if (level == 0) depth = std::min<std::size_t>(2, depth);
            if (level == 1) depth /= 2;
            path.resize(depth);
            node = tree.get_root();
            for (char direction : path) node = direction == 'L' ? node->left : node->right;
            paths.push_back({path, std::string(depth, 'U'), node->key});
        }
        const int queries = 100000;
        long long sum = 0, expected = 0;
        int found = 0;
        for (int i = 0; i < queries; ++i) expected += paths[i % paths.size()].key;
        Clock::time_point start = Clock::now();
        for (int i = 0; i < queries; ++i) {
            Tree::Node* node = tree.find_by_path(paths[i % paths.size()].path.c_str());
            if (node) { sum += node->key; ++found; }
        }
        double time = elapsed_ms(start);
        require(found == queries && sum == expected, "Wrong absolute path result");
        report.add("path_absolute", size, names[level], queries, 0, repeat, time);

        found = 0;
        sum = 0;
        start = Clock::now();
        for (int i = 0; i < queries; ++i) {
            const PathQuery& query = paths[i % paths.size()];
            Tree::Node* node = tree.find_by_path(query.key, query.up.c_str());
            if (node) { sum += node->key; ++found; }
        }
        time = elapsed_ms(start);
        require(found == queries && sum == 1LL * queries * tree.get_root()->key,
                "Wrong relative path result");
        report.add("path_relative_up", size, names[level], queries, 0, repeat, time);
    }
}

void test_extract(Report& report, const Tree& tree, int size, int repeat) {
    const Tree::Node* small = tree.get_root();
    while (small->left != tree.get_nil()) small = small->left;
    const Tree::Node* roots[] = {small, tree.get_root()};
    const char* names[] = {"extract_small", "extract_root"};
    for (int i = 0; i < 2; ++i) {
        long long count = count_nodes(roots[i], tree.get_nil());
        long long sum = sum_values(roots[i], tree.get_nil());
        if (!report.memory(names[i], size, repeat, count * node_bytes())) continue;
        Clock::time_point start = Clock::now();
        Tree result = tree.extract_subtree(roots[i]->key);
        double time = elapsed_ms(start);
        check_tree(result, count, sum);
        report.add(names[i], size, "random", 1, count, repeat, time);
    }
}

void test_contains(Report& report, const Tree& tree, int size, int repeat) {
    if (!report.memory("contains", size, repeat, size * node_bytes())) return;
    {
        Tree candidate = tree.extract_subtree(tree.get_root()->key);
        Clock::time_point start = Clock::now();
        bool found = tree.contains_subtree(candidate);
        double time = elapsed_ms(start);
        require(found, "Existing subtree not found");
        report.add("contains_present", size, "random", 1, size, repeat, time);

        // A late value mismatch requires visiting almost the whole candidate.
        Tree::Node* last = candidate.get_root();
        while (last->right != candidate.get_nil()) last = last->right;
        ++last->value;
        start = Clock::now();
        found = tree.contains_subtree(candidate);
        time = elapsed_ms(start);
        require(!found, "Mismatching subtree found");
        report.add("contains_mismatch", size, "random", 1, size, repeat, time);
    }
    std::vector<Tree> missing(256);
    for (int i = 0; i < 256; ++i) missing[i].insert(size + i, size + i);
    const int queries = 100000;
    int found = 0;
    Clock::time_point start = Clock::now();
    for (int i = 0; i < queries; ++i)
        if (tree.contains_subtree(missing[i % missing.size()])) ++found;
    double time = elapsed_ms(start);
    require(found == 0, "Missing subtree found");
    report.add("contains_missing_key", size, "random", queries, 1, repeat, time);
}

void test_merge(Report& report, const Tree& tree, const std::vector<int>& keys,
                int size, int repeat) {
    for (int overlap = 0; overlap < 2; ++overlap) {
        const char* name = overlap ? "merge_overlap" : "merge_disjoint";
        int offset = overlap ? size / 2 : size;
        long long count = size + offset;
        if (!report.memory(name, size, repeat, (size + count) * node_bytes())) continue;
        Tree other;
        fill_tree(other, keys, offset);
        Clock::time_point start = Clock::now();
        Tree result = tree.merge(other);
        double time = elapsed_ms(start);
        check_tree(result, count, expected_sum(count));
        report.add(name, size, "random", 1, size, repeat, time);
    }
}

void test_strings(Report& report, const Tree& tree, int size, int repeat) {
    // Includes stream buffers, path strings, the serialized text and restored nodes.
    if (!report.memory("string", size, repeat, 512ULL * size)) return;
    for (int i = 0; i < 6; ++i) {
        Clock::time_point start = Clock::now();
        std::string text = tree.to_string(Order(i));
        double time = elapsed_ms(start);
        require(text.size() > std::size_t(size), "Empty serialized tree");
        report.add("to_string", size, order_names[i], 1, 0, repeat, time);
        Tree restored;
        start = Clock::now();
        bool success = restored.from_string(text);
        time = elapsed_ms(start);
        require(success, "Cannot restore tree");
        check_tree(restored, size, expected_sum(size));
        report.add("from_string", size, order_names[i], 1, 0, repeat, time);
    }
}

void run_size(Report& report, int size, int repeats, const std::string& selection) {
    for (int repeat = 1; repeat <= repeats; ++repeat) {
        if (selected(selection, "insert")) test_insert(report, size, repeat);
        if (selection == "insert") continue;
        if (!report.memory("source", size, repeat, size * (node_bytes() + 4))) continue;
        std::vector<int> keys(size);
        for (int i = 0; i < size; ++i) keys[i] = i;
        std::mt19937 generator(42);
        std::shuffle(keys.begin(), keys.end(), generator);
        Tree tree;
        fill_tree(tree, keys);
        check_tree(tree, size, expected_sum(size));

        if (selected(selection, "find")) test_find(report, tree, size, repeat);
        if (selected(selection, "traverse")) {
            for (int i = 0; i < 6; ++i) {
                long long count = 0, sum = 0;
                std::function<void(const int&, const int&)> action = Counter{count, sum};
                Clock::time_point start = Clock::now();
                tree.traverse(Order(i), action);
                double time = elapsed_ms(start);
                require(count == size && sum == expected_sum(size), "Wrong traversal");
                report.add("traverse", size, order_names[i], size, 0, repeat, time);
            }
        }
        if (selected(selection, "reduce")) {
            for (int i = 0; i < 6; ++i) {
                Clock::time_point start = Clock::now();
                long long sum = tree.reduce(0LL, add, Order(i));
                double time = elapsed_ms(start);
                require(sum == expected_sum(size), "Wrong reduce");
                report.add("reduce", size, order_names[i], size, 0, repeat, time);
            }
        }
        if (selected(selection, "map") &&
            report.memory("map", size, repeat, size * node_bytes())) {
            std::function<int(const int&)> action = increment;
            Clock::time_point start = Clock::now();
            Tree result = tree.map(action);
            double time = elapsed_ms(start);
            check_tree(result, size, expected_sum(size) + size);
            report.add("map", size, "random", size, 0, repeat, time);
        }
        if (selected(selection, "where") &&
            report.memory("where", size, repeat, size / 2 * node_bytes())) {
            std::function<bool(const int&)> predicate = even;
            Clock::time_point start = Clock::now();
            Tree result = tree.where(predicate);
            double time = elapsed_ms(start);
            check_tree(result, size / 2, 1LL * (size / 2) * (size / 2 - 1));
            report.add("where_half", size, "random", size, 0, repeat, time);
        }
        if (selected(selection, "merge")) test_merge(report, tree, keys, size, repeat);
        if (selected(selection, "extract")) test_extract(report, tree, size, repeat);
        if (selected(selection, "contains")) test_contains(report, tree, size, repeat);
        if (selected(selection, "path")) test_paths(report, tree, size, repeat);
        if (selected(selection, "string")) test_strings(report, tree, size, repeat);
        if (selected(selection, "erase")) {
            long long erased = 0;
            Clock::time_point start = Clock::now();
            for (int key : keys) if (tree.erase(key)) ++erased;
            double time = elapsed_ms(start);
            require(erased == size, "Wrong erase count");
            check_tree(tree, 0, 0);
            report.add("erase", size, "random", size, 0, repeat, time);
        }
    }
}

bool valid_selection(const std::string& value) {
    const char* names[] = {"all", "basic", "insert", "find", "erase", "traverse",
                          "map", "where", "reduce", "merge", "extract",
                          "contains", "path", "string"};
    for (const char* name : names) if (value == name) return true;
    return false;
}

int read_number(const char* text) {
    std::istringstream input(text);
    int value = 0;
    char extra;
    require(bool(input >> value) && !(input >> extra), "Invalid numeric argument");
    return value;
}

void show_help() {
    std::cout << "Usage: load_test [max_size] [repeats] [operation] [csv_file]\n"
                 "Defaults: 1000000 3 all build/wsl/load_results.csv\n"
                 "Sizes: 10000, 100000, 1000000, 10000000, 100000000 (up to max_size)\n"
                 "Operations: all basic insert find erase traverse map where reduce\n"
                 "            merge extract contains path string\n"
                 "basic: insert, find, erase, six traversals, six reductions, paths\n"
                 "Seed: 42; values equal keys; time excludes preparation/checks/output.\n"
                 "CSV is overwritten. SKIPPED rows document memory limitations.\n";
}

int main(int argc, char** argv) {
    try {
        if (argc == 2 && std::string(argv[1]) == "--help") {
            show_help();
            return 0;
        }
        require(argc <= 5, "Too many arguments; use --help");
        int maximum = argc > 1 ? read_number(argv[1]) : 1000000;
        int repeats = argc > 2 ? read_number(argv[2]) : 3;
        std::string selection = argc > 3 ? argv[3] : "all";
        std::string path = argc > 4 ? argv[4] : "build/wsl/load_results.csv";
        require(maximum >= 10000 && maximum <= 100000000, "max_size must be 10000..100000000");
        require(repeats >= 1 && repeats <= 10, "repeats must be 1..10");
        require(valid_selection(selection), "Unknown operation; use --help");
        Report report(path);
        std::cout << "Tree<int,int>, seed=42, sizeof(Node)=" << sizeof(Tree::Node)
                  << ", estimated bytes/node=" << node_bytes()
                  << ", available memory=" << available_memory() / 1024 / 1024 << " MiB\n";
        const int sizes[] = {10000, 100000, 1000000, 10000000, 100000000};
        try {
            for (int size : sizes)
                if (size <= maximum) run_size(report, size, repeats, selection);
        } catch (const std::exception& error) {
            report.file << "run;;;;;;;;ERROR;" << error.what() << '\n';
            report.file.flush();
            throw;
        }
        report.medians();
        std::cout << "\nCSV: " << path << '\n';
        return 0;
    } catch (const std::exception& error) {
        std::cerr << "Load test failed: " << error.what() << '\n';
        return 1;
    }
}
