#ifndef PERFORMANCE_TESTS_H
#define PERFORMANCE_TESTS_H

#include <iostream>
#include <iomanip>
#include <chrono>
#include <vector>
#include <random>
#include <string>
#include <map>
#include "dataTypes.h"

// ====================== НАСТРОЙКИ РАЗМЕРОВ ======================
const std::size_t SMALL_SIZE = 10000;
const std::size_t MEDIUM_SIZE = 100000;
const std::size_t LARGE_SIZE = 1000000;
const std::size_t VERY_LARGE_SIZE = 10000000;

// ====================== УТИЛИТЫ ИЗМЕРЕНИЯ ======================
struct BenchmarkResult {
    std::string operation;
    std::size_t size;
    double time_ms;
    double ops_per_sec;
};

std::vector<BenchmarkResult> benchmark_results;

#define BENCHMARK_START \
    { \
    auto _bench_start = std::chrono::high_resolution_clock::now();

#define BENCHMARK_END(op_name, sz) \
    auto _bench_end = std::chrono::high_resolution_clock::now(); \
    double _bench_elapsed = std::chrono::duration<double, std::milli>(_bench_end - _bench_start).count(); \
    double _bench_ops = (sz > 0) ? (sz / (_bench_elapsed / 1000.0)) : 0; \
    benchmark_results.push_back({op_name, sz, _bench_elapsed, _bench_ops}); \
    }

// ====================== ГЕНЕРАЦИЯ ДАННЫХ ======================
std::vector<int> generate_random_ints(std::size_t count, int seed = 42) {
    std::vector<int> data;
    data.reserve(count);
    std::mt19937 gen(seed);
    std::uniform_int_distribution<int> dist(1, count * 10);
    for (std::size_t i = 0; i < count; ++i) {
        data.push_back(dist(gen));
    }
    return data;
}

std::vector<int> generate_sorted_ints(std::size_t count) {
    std::vector<int> data;
    data.reserve(count);
    for (std::size_t i = 0; i < count; ++i) {
        data.push_back(static_cast<int>(i));
    }
    return data;
}

std::vector<int> generate_reverse_ints(std::size_t count) {
    std::vector<int> data;
    data.reserve(count);
    for (std::size_t i = 0; i < count; ++i) {
        data.push_back(static_cast<int>(count - i));
    }
    return data;
}

std::vector<std::string> generate_random_strings(std::size_t count, std::size_t length = 10) {
    std::vector<std::string> data;
    data.reserve(count);
    std::mt19937 gen(42);
    std::uniform_int_distribution<int> char_dist('a', 'z');
    for (std::size_t i = 0; i < count; ++i) {
        std::string s;
        s.reserve(length);
        for (std::size_t j = 0; j < length; ++j) {
            s += static_cast<char>(char_dist(gen));
        }
        data.push_back(s);
    }
    return data;
}

// ====================== ФУНКТОРЫ ДЛЯ ТЕСТОВ ======================
struct CountAction {
    mutable std::size_t count = 0;
    void operator()(const int&, const int&) const { ++count; }
};

struct PerfSumAction {
    long long operator()(long long acc, const int& v) const { return acc + v; }
};

// ====================== ТЕСТЫ INSERT ======================
void benchmark_insert_random(std::size_t size) {
    std::cout << "  Insert (random order, " << size << " elements)... ";
    std::vector<int> data = generate_random_ints(size);
    IntegerTree tree;
    
    BENCHMARK_START;
    for (int key : data) {
        tree.insert(key, key * 2);
    }
    BENCHMARK_END("Insert (random)", size);
    
    std::cout << benchmark_results.back().time_ms << " ms\n";
}

void benchmark_insert_sorted(std::size_t size) {
    std::cout << "  Insert (sorted order, " << size << " elements)... ";
    std::vector<int> data = generate_sorted_ints(size);
    IntegerTree tree;
    
    BENCHMARK_START;
    for (int key : data) {
        tree.insert(key, key * 2);
    }
    BENCHMARK_END("Insert (sorted)", size);
    
    std::cout << benchmark_results.back().time_ms << " ms\n";
}

void benchmark_insert_reverse(std::size_t size) {
    std::cout << "  Insert (reverse order, " << size << " elements)... ";
    std::vector<int> data = generate_reverse_ints(size);
    IntegerTree tree;
    
    BENCHMARK_START;
    for (int key : data) {
        tree.insert(key, key * 2);
    }
    BENCHMARK_END("Insert (reverse)", size);
    
    std::cout << benchmark_results.back().time_ms << " ms\n";
}

// ====================== ТЕСТЫ FIND ======================
void benchmark_find_success(std::size_t size) {
    std::cout << "  Find (successful, " << size << " elements)... ";
    IntegerTree tree;
    for (std::size_t i = 0; i < size; ++i) {
        tree.insert(static_cast<int>(i), static_cast<int>(i * 2));
    }
    
    BENCHMARK_START;
    for (std::size_t i = 0; i < size; ++i) {
        tree.find(static_cast<int>(i));
    }
    BENCHMARK_END("Find (success)", size);
    
    std::cout << benchmark_results.back().time_ms << " ms\n";
}

void benchmark_find_failure(std::size_t size) {
    std::cout << "  Find (unsuccessful, " << size << " elements)... ";
    IntegerTree tree;
    for (std::size_t i = 0; i < size; ++i) {
        tree.insert(static_cast<int>(i * 2), static_cast<int>(i));
    }
    
    BENCHMARK_START;
    for (std::size_t i = 0; i < size; ++i) {
        tree.find(static_cast<int>(i * 2 + 1));
    }
    BENCHMARK_END("Find (failure)", size);
    
    std::cout << benchmark_results.back().time_ms << " ms\n";
}

// ====================== ТЕСТЫ ERASE ======================
void benchmark_erase_all(std::size_t size) {
    std::cout << "  Erase (all elements, " << size << " elements)... ";
    IntegerTree tree;
    for (std::size_t i = 0; i < size; ++i) {
        tree.insert(static_cast<int>(i), static_cast<int>(i * 2));
    }
    
    BENCHMARK_START;
    for (std::size_t i = 0; i < size; ++i) {
        tree.erase(static_cast<int>(i));
    }
    BENCHMARK_END("Erase (all)", size);
    
    std::cout << benchmark_results.back().time_ms << " ms\n";
}

// ====================== ТЕСТЫ TRAVERSE ======================
void benchmark_traverse_lnr(std::size_t size) {
    std::cout << "  Traverse (LNR, " << size << " elements)... ";
    IntegerTree tree;
    for (std::size_t i = 0; i < size; ++i) {
        tree.insert(static_cast<int>(i), static_cast<int>(i * 2));
    }
    
    CountAction action;
    BENCHMARK_START;
    tree.traverse(traversal::LNR{}, action);
    BENCHMARK_END("Traverse (LNR)", size);
    
    std::cout << benchmark_results.back().time_ms << " ms\n";
}

void benchmark_traverse_nlr(std::size_t size) {
    std::cout << "  Traverse (NLR, " << size << " elements)... ";
    IntegerTree tree;
    for (std::size_t i = 0; i < size; ++i) {
        tree.insert(static_cast<int>(i), static_cast<int>(i * 2));
    }
    
    CountAction action;
    BENCHMARK_START;
    tree.traverse(traversal::NLR{}, action);
    BENCHMARK_END("Traverse (NLR)", size);
    
    std::cout << benchmark_results.back().time_ms << " ms\n";
}

// ====================== ТЕСТЫ REDUCE ======================
void benchmark_reduce(std::size_t size) {
    std::cout << "  Reduce (sum, " << size << " elements)... ";
    IntegerTree tree;
    for (std::size_t i = 0; i < size; ++i) {
        tree.insert(static_cast<int>(i), static_cast<int>(i));
    }
    
    long long result = 0;
    BENCHMARK_START;
    result = tree.reduce(traversal::LNR{}, 0LL, PerfSumAction{});
    BENCHMARK_END("Reduce (sum)", size);
    
    std::cout << benchmark_results.back().time_ms << " ms (result: " << result << ")\n";
}

// ====================== ТЕСТЫ СЕРИАЛИЗАЦИИ ======================
void benchmark_to_string(std::size_t size) {
    std::cout << "  to_string (" << size << " elements)... ";
    IntegerTree tree;
    for (std::size_t i = 0; i < size; ++i) {
        tree.insert(static_cast<int>(i), static_cast<int>(i * 2));
    }
    
    std::string serialized;
    BENCHMARK_START;
    serialized = tree.to_string();
    BENCHMARK_END("to_string", size);
    
    std::cout << benchmark_results.back().time_ms << " ms (size: " << serialized.size() << " bytes)\n";
}

void benchmark_from_string(std::size_t size) {
    std::cout << "  from_string (" << size << " elements)... ";
    IntegerTree tree;
    for (std::size_t i = 0; i < size; ++i) {
        tree.insert(static_cast<int>(i), static_cast<int>(i * 2));
    }
    std::string serialized = tree.to_string();
    
    IntegerTree restored;
    bool ok = false;
    BENCHMARK_START;
    ok = restored.from_string(serialized);
    BENCHMARK_END("from_string", size);
    
    std::cout << benchmark_results.back().time_ms << " ms (success: " << (ok ? "yes" : "no") << ")\n";
}

// ====================== ТЕСТЫ КОПИРОВАНИЯ ======================
void benchmark_copy(std::size_t size) {
    std::cout << "  Copy constructor (" << size << " elements)... ";
    IntegerTree tree;
    for (std::size_t i = 0; i < size; ++i) {
        tree.insert(static_cast<int>(i), static_cast<int>(i * 2));
    }
    
    BENCHMARK_START;
    IntegerTree copy(tree);
    BENCHMARK_END("Copy ctor", size);
    
    std::cout << benchmark_results.back().time_ms << " ms\n";
}

void benchmark_move(std::size_t size) {
    std::cout << "  Move constructor (" << size << " elements)... ";
    IntegerTree tree;
    for (std::size_t i = 0; i < size; ++i) {
        tree.insert(static_cast<int>(i), static_cast<int>(i * 2));
    }
    
    BENCHMARK_START;
    IntegerTree moved(std::move(tree));
    BENCHMARK_END("Move ctor", size);
    
    std::cout << benchmark_results.back().time_ms << " ms\n";
}

// ====================== ТЕСТЫ СРАВНЕНИЯ С std::map ======================
void benchmark_compare_with_std_map(std::size_t size) {
    std::cout << "  Comparison with std::map (" << size << " elements)...\n";
    std::vector<int> data = generate_random_ints(size);
    
    IntegerTree rb_tree;
    BENCHMARK_START;
    for (int key : data) {
        rb_tree.insert(key, key * 2);
    }
    BENCHMARK_END("RedBlackTree insert", size);
    double rb_insert_time = benchmark_results.back().time_ms;
    
    std::map<int, int> std_map;
    BENCHMARK_START;
    for (int key : data) {
        std_map[key] = key * 2;
    }
    BENCHMARK_END("std::map insert", size);
    double std_insert_time = benchmark_results.back().time_ms;
    
    std::cout << "    Insert: RedBlackTree " << rb_insert_time << " ms vs std::map " 
              << std_insert_time << " ms (ratio: " << std::fixed << std::setprecision(2) 
              << (rb_insert_time / std_insert_time) << "x)\n";
    
    BENCHMARK_START;
    for (int key : data) {
        rb_tree.find(key);
    }
    BENCHMARK_END("RedBlackTree find", size);
    double rb_find_time = benchmark_results.back().time_ms;
    
    BENCHMARK_START;
    for (int key : data) {
        std_map.find(key);
    }
    BENCHMARK_END("std::map find", size);
    double std_find_time = benchmark_results.back().time_ms;
    
    std::cout << "    Find: RedBlackTree " << rb_find_time << " ms vs std::map " 
              << std_find_time << " ms (ratio: " << std::fixed << std::setprecision(2) 
              << (rb_find_time / std_find_time) << "x)\n";
}

// ====================== ТЕСТЫ ДЛЯ РАЗНЫХ ТИПОВ ======================
void benchmark_string_tree(std::size_t size) {
    std::cout << "  StringTree operations (" << size << " elements)...\n";
    std::vector<std::string> keys = generate_random_strings(size, 10);
    std::vector<std::string> values = generate_random_strings(size, 20);
    
    StringTree tree;
    BENCHMARK_START;
    for (std::size_t i = 0; i < size; ++i) {
        tree.insert(keys[i], values[i]);
    }
    BENCHMARK_END("StringTree insert", size);
    std::cout << "    Insert: " << benchmark_results.back().time_ms << " ms\n";
    
    BENCHMARK_START;
    for (std::size_t i = 0; i < size; ++i) {
        tree.find(keys[i]);
    }
    BENCHMARK_END("StringTree find", size);
    std::cout << "    Find: " << benchmark_results.back().time_ms << " ms\n";
}

void benchmark_complex_tree(std::size_t size) {
    std::cout << "  ComplexTree operations (" << size << " elements)...\n";
    std::vector<Complex> keys;
    keys.reserve(size);
    std::mt19937 gen(42);
    std::uniform_real_distribution<double> dist(-1000.0, 1000.0);
    for (std::size_t i = 0; i < size; ++i) {
        keys.emplace_back(dist(gen), dist(gen));
    }
    
    ComplexTree tree;
    BENCHMARK_START;
    for (const auto& key : keys) {
        tree.insert(key, key);
    }
    BENCHMARK_END("ComplexTree insert", size);
    std::cout << "    Insert: " << benchmark_results.back().time_ms << " ms\n";
    
    BENCHMARK_START;
    for (const auto& key : keys) {
        tree.find(key);
    }
    BENCHMARK_END("ComplexTree find", size);
    std::cout << "    Find: " << benchmark_results.back().time_ms << " ms\n";
}

// ====================== АГРЕГАТОР ======================
void run_performance_tests() {
    std::cout << "\n========================================\n";
    std::cout << "   Performance Tests (Load Testing)\n";
    std::cout << "========================================\n\n";
    
    std::cout << "=== SMALL SIZE (" << SMALL_SIZE << " elements) ===\n";
    benchmark_insert_random(SMALL_SIZE);
    benchmark_insert_sorted(SMALL_SIZE);
    benchmark_insert_reverse(SMALL_SIZE);
    benchmark_find_success(SMALL_SIZE);
    benchmark_find_failure(SMALL_SIZE);
    benchmark_erase_all(SMALL_SIZE);
    benchmark_traverse_lnr(SMALL_SIZE);
    benchmark_reduce(SMALL_SIZE);
    benchmark_to_string(SMALL_SIZE);
    benchmark_from_string(SMALL_SIZE);
    benchmark_copy(SMALL_SIZE);
    benchmark_move(SMALL_SIZE);
    
    std::cout << "\n=== MEDIUM SIZE (" << MEDIUM_SIZE << " elements) ===\n";
    benchmark_insert_random(MEDIUM_SIZE);
    benchmark_insert_sorted(MEDIUM_SIZE);
    benchmark_find_success(MEDIUM_SIZE);
    benchmark_erase_all(MEDIUM_SIZE);
    benchmark_traverse_lnr(MEDIUM_SIZE);
    benchmark_reduce(MEDIUM_SIZE);
    benchmark_to_string(MEDIUM_SIZE);
    benchmark_from_string(MEDIUM_SIZE);
    benchmark_copy(MEDIUM_SIZE);
    
    std::cout << "\n=== LARGE SIZE (" << LARGE_SIZE << " elements) ===\n";
    benchmark_insert_random(LARGE_SIZE);
    benchmark_insert_sorted(LARGE_SIZE);
    benchmark_find_success(LARGE_SIZE);
    benchmark_erase_all(LARGE_SIZE);
    benchmark_reduce(LARGE_SIZE);
    
    std::cout << "\n=== VERY LARGE SIZE (" << VERY_LARGE_SIZE << " elements) ===\n";
    std::cout << "WARNING: This may take several minutes and require significant memory.\n";
    benchmark_insert_random(VERY_LARGE_SIZE);
    benchmark_find_success(VERY_LARGE_SIZE);
    
    std::cout << "\n=== COMPARISON WITH std::map ===\n";
    benchmark_compare_with_std_map(MEDIUM_SIZE);
    
    std::cout << "\n=== DIFFERENT DATA TYPES ===\n";
    benchmark_string_tree(MEDIUM_SIZE);
    benchmark_complex_tree(MEDIUM_SIZE);
    
    std::cout << "\n========================================\n";
    std::cout << "Performance Tests Completed\n";
    std::cout << "========================================\n";
    
    std::cout << "\n=== SUMMARY TABLE ===\n";
    std::cout << std::left << std::setw(30) << "Operation" 
              << std::right << std::setw(12) << "Size" 
              << std::setw(15) << "Time (ms)" 
              << std::setw(18) << "Ops/sec" << "\n";
    std::cout << std::string(75, '-') << "\n";
    
    for (const auto& result : benchmark_results) {
        std::cout << std::left << std::setw(30) << result.operation
                  << std::right << std::setw(12) << result.size
                  << std::setw(15) << std::fixed << std::setprecision(2) << result.time_ms
                  << std::setw(18) << std::scientific << std::setprecision(2) << result.ops_per_sec
                  << "\n";
    }
    
    std::cout << "========================================\n";
}

#endif