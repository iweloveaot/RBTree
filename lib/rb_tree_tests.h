#ifndef RB_TREE_TESTS_H
#define RB_TREE_TESTS_H

#include <iostream>
#include <string>
#include <vector>
#include <cmath>
#include <complex>
#include <stdexcept>
#include "dataTypes.h"
#include "traversals.h"


int rb_tests_passed = 0;
int rb_tests_failed = 0;

#define TEST_CASE(name) void test_##name()

#define RUN_TEST(name) \
    try { \
        std::cout << "[RUN] " << #name << "... "; \
        test_##name(); \
        std::cout << "\033[32mPASSED\033[0m\n"; \
        rb_tests_passed++; \
    } catch (const std::exception& e) { \
        std::cout << "\033[31mFAILED\033[0m\n"; \
        std::cerr << "  Error: " << e.what() << "\n"; \
        rb_tests_failed++; \
    }

#define ASSERT_TRUE(expr) \
    if (!(expr)) throw std::runtime_error(std::string("ASSERT_TRUE failed: ") + #expr)

#define ASSERT_FALSE(expr) \
    if ((expr)) throw std::runtime_error(std::string("ASSERT_FALSE failed: ") + #expr)

#define ASSERT_EQ(a, b) \
    if (!((a) == (b))) throw std::runtime_error(std::string("ASSERT_EQ failed: ") + #a + " != " + #b)

#define ASSERT_NOT_NULL(ptr) \
    if ((ptr) == nullptr) throw std::runtime_error(std::string("ASSERT_NOT_NULL failed: ") + #ptr)

#define ASSERT_NULL(ptr) \
    if ((ptr) != nullptr) throw std::runtime_error(std::string("ASSERT_NULL failed: ") + #ptr)

#define ASSERT_NEAR(a, b, eps) \
    if (std::abs((a) - (b)) > (eps)) throw std::runtime_error(std::string("ASSERT_NEAR failed: ") + #a + " !~= " + #b)

#define ASSERT_THROW(expr, exc_type) \
    do { \
        bool caught = false; \
        try { expr; } catch (const exc_type&) { caught = true; } catch (...) {} \
        if (!caught) throw std::runtime_error(std::string("ASSERT_THROW failed: ") + #expr + " did not throw " + #exc_type); \
    } while (0)


struct PrintIntKV {
    void operator()(const int& k, const int& v) const {
        std::cout << k << ":" << v << " ";
    }
};

struct CollectKeys {
    std::vector<int>& keys;
    void operator()(const int& k, const int&) const {
        keys.push_back(k);
    }
};

struct CollectKeysComplex {
    std::vector<Complex>& keys;
    void operator()(const Complex& k, const Complex&) const {
        keys.push_back(k);
    }
};

struct CollectValues {
    std::vector<int>& values;
    void operator()(const int&, const int& v) const {
        values.push_back(v);
    }
};

struct SumAction {
    int operator()(const int& acc, const int& v) const {
        return acc + v;
    }
};

struct ProductAction {
    int operator()(const int& acc, const int& v) const {
        return acc * v;
    }
};

struct IsEvenPredicate {
    bool operator()(const int& v) const {
        return v % 2 == 0;
    }
};

struct DoubleValue {
    int operator()(const int& v) const {
        return v * 2;
    }
};



template <typename Tree, typename K, typename V>
std::vector<std::pair<K, V>> collect_lnr(const Tree& tree) {
    std::vector<K> keys;
    std::vector<V> values;
    CollectKeys key_collector{keys};
    CollectValues value_collector{values};
    tree.traverse(traversal::LNR{}, key_collector);
    tree.traverse(traversal::LNR{}, value_collector);
    std::vector<std::pair<K, V>> result;
    for (std::size_t i = 0; i < keys.size(); ++i) {
        result.emplace_back(keys[i], values[i]);
    }
    return result;
}

template <typename Vec>
void assert_vec_eq(const Vec& a, const Vec& b) {
    if (a.size() != b.size())
        throw std::runtime_error("Vectors differ in size");
    for (std::size_t i = 0; i < a.size(); ++i)
        if (!(a[i] == b[i]))
            throw std::runtime_error("Vectors differ at index " + std::to_string(i));
}

// Round-trip тест: to_string -> from_string -> сравнение
template <typename Tree>
void assert_roundtrip(const Tree& original) {
    std::string serialized = original.to_string();
    Tree restored;
    if (!restored.from_string(serialized))
        throw std::runtime_error("from_string returned false on valid serialization");
    std::string serialized2 = restored.to_string();
    if (serialized != serialized2)
        throw std::runtime_error("Round-trip failed: serialized strings differ");
}

// базовые операции

template <typename Tree, typename K, typename V>
void impl_default_ctor() {
    Tree t;
    ASSERT_TRUE(t.empty());
    ASSERT_NULL(t.find(K()));
}

template <typename Tree, typename K, typename V>
void impl_insert_single(K k, V v) {
    Tree t;
    t.insert(k, v);
    ASSERT_FALSE(t.empty());
}

template <typename Tree, typename K, typename V>
void impl_insert_multiple(K k1, V v1, K k2, V v2, K k3, V v3) {
    Tree t;
    t.insert(k1, v1);
    t.insert(k2, v2);
    t.insert(k3, v3);
    ASSERT_NOT_NULL(t.find(k1));
    ASSERT_NOT_NULL(t.find(k2));
    ASSERT_NOT_NULL(t.find(k3));
}

template <typename Tree, typename K, typename V>
void impl_insert_duplicate(K k, V v1, V v2) {
    Tree t;
    t.insert(k, v1);
    t.insert(k, v2);
    ASSERT_EQ(t.find(k)->value, v1);
}

template <typename Tree, typename K, typename V>
void impl_find_existing(K k, V v) {
    Tree t;
    t.insert(k, v);
    auto* node = t.find(k);
    ASSERT_NOT_NULL(node);
    ASSERT_EQ(node->value, v);
}

template <typename Tree, typename K, typename V>
void impl_find_non_existing(K k_present, K k_absent) {
    Tree t;
    t.insert(k_present, V());
    ASSERT_NULL(t.find(k_absent));
}

template <typename Tree, typename K, typename V>
void impl_erase_existing(K k1, V v1, K k2, V v2) {
    Tree t;
    t.insert(k1, v1);
    t.insert(k2, v2);
    bool erased = t.erase(k1);
    ASSERT_TRUE(erased);
    ASSERT_NULL(t.find(k1));
    ASSERT_NOT_NULL(t.find(k2));
}

template <typename Tree, typename K, typename V>
void impl_erase_non_existing(K k) {
    Tree t;
    t.insert(k, V());
    ASSERT_FALSE(t.erase(K()));
}

template <typename Tree, typename K, typename V>
void impl_erase_from_empty(K k) {
    Tree t;
    ASSERT_FALSE(t.erase(k));
}

template <typename Tree, typename K, typename V>
void impl_copy_ctor(K k, V v) {
    Tree original;
    original.insert(k, v);
    Tree copy(original);
    ASSERT_EQ(copy.find(k)->value, v);
    original.erase(k);
    ASSERT_NOT_NULL(copy.find(k));
}

template <typename Tree, typename K, typename V>
void impl_copy_assign(K k1, V v1, K k2, V v2) {
    Tree a, b;
    a.insert(k1, v1);
    b.insert(k2, v2);
    b = a;
    ASSERT_NOT_NULL(b.find(k1));
    ASSERT_NULL(b.find(k2));
}

template <typename Tree, typename K, typename V>
void impl_move_ctor(K k, V v) {
    Tree original;
    original.insert(k, v);
    Tree moved(std::move(original));
    ASSERT_NOT_NULL(moved.find(k));
    ASSERT_TRUE(original.empty());
}

template <typename Tree, typename K, typename V>
void impl_move_assign(K k1, V v1, K k2, V v2) {
    Tree a, b;
    a.insert(k1, v1);
    b.insert(k2, v2);
    b = std::move(a);
    ASSERT_NOT_NULL(b.find(k1));
    ASSERT_NULL(b.find(k2));
    ASSERT_TRUE(a.empty());
}

// конкретные тесты для каждого типа 

// --- IntegerTree ---
TEST_CASE(IntegerTree_DefaultCtor)         { impl_default_ctor<IntegerTree, int, int>(); }
TEST_CASE(IntegerTree_InsertSingle)        { impl_insert_single<IntegerTree, int, int>(42, 100); }
TEST_CASE(IntegerTree_InsertMultiple)      { impl_insert_multiple<IntegerTree, int, int>(1, 10, 5, 50, 3, 30); }
TEST_CASE(IntegerTree_InsertDuplicate)     { impl_insert_duplicate<IntegerTree, int, int>(7, 100, 999); }
TEST_CASE(IntegerTree_FindExisting)        { impl_find_existing<IntegerTree, int, int>(11, 121); }
TEST_CASE(IntegerTree_FindNonExisting)     { impl_find_non_existing<IntegerTree, int, int>(1, 9999); }
TEST_CASE(IntegerTree_EraseExisting)       { impl_erase_existing<IntegerTree, int, int>(1, 10, 2, 20); }
TEST_CASE(IntegerTree_EraseNonExisting)    { impl_erase_non_existing<IntegerTree, int, int>(1); }
TEST_CASE(IntegerTree_EraseFromEmpty)      { impl_erase_from_empty<IntegerTree, int, int>(1); }
TEST_CASE(IntegerTree_CopyCtor)            { impl_copy_ctor<IntegerTree, int, int>(5, 25); }
TEST_CASE(IntegerTree_CopyAssign)          { impl_copy_assign<IntegerTree, int, int>(1, 10, 2, 20); }
TEST_CASE(IntegerTree_MoveCtor)            { impl_move_ctor<IntegerTree, int, int>(9, 81); }
TEST_CASE(IntegerTree_MoveAssign)          { impl_move_assign<IntegerTree, int, int>(1, 10, 2, 20); }

// --- RealTree ---
TEST_CASE(RealTree_DefaultCtor)            { impl_default_ctor<RealTree, double, double>(); }
TEST_CASE(RealTree_InsertSingle)           { impl_insert_single<RealTree, double, double>(1.5, 2.25); }
TEST_CASE(RealTree_InsertMultiple)         { impl_insert_multiple<RealTree, double, double>(0.1, 1.0, 0.5, 2.0, 0.9, 3.0); }
TEST_CASE(RealTree_InsertDuplicate)        { impl_insert_duplicate<RealTree, double, double>(3.14, 1.0, 9.9); }
TEST_CASE(RealTree_FindExisting) {
    RealTree t; t.insert(2.5, 6.25);
    auto* n = t.find(2.5);
    ASSERT_NOT_NULL(n);
    ASSERT_NEAR(n->value, 6.25, 1e-12);
}
TEST_CASE(RealTree_FindNonExisting)        { impl_find_non_existing<RealTree, double, double>(1.0, 99.0); }
TEST_CASE(RealTree_EraseExisting)          { impl_erase_existing<RealTree, double, double>(1.0, 1.0, 2.0, 4.0); }
TEST_CASE(RealTree_EraseNonExisting)       { impl_erase_non_existing<RealTree, double, double>(1.0); }
TEST_CASE(RealTree_EraseFromEmpty)         { impl_erase_from_empty<RealTree, double, double>(1.0); }
TEST_CASE(RealTree_CopyCtor)               { impl_copy_ctor<RealTree, double, double>(1.5, 2.25); }
TEST_CASE(RealTree_CopyAssign)             { impl_copy_assign<RealTree, double, double>(1.0, 1.0, 2.0, 4.0); }
TEST_CASE(RealTree_MoveCtor)               { impl_move_ctor<RealTree, double, double>(2.5, 6.25); }
TEST_CASE(RealTree_MoveAssign)             { impl_move_assign<RealTree, double, double>(1.0, 1.0, 2.0, 4.0); }

// --- ComplexTree ---
TEST_CASE(ComplexTree_DefaultCtor)         { impl_default_ctor<ComplexTree, Complex, Complex>(); }
TEST_CASE(ComplexTree_InsertSingle)        { impl_insert_single<ComplexTree, Complex, Complex>(Complex(1, 2), Complex(3, 4)); }
TEST_CASE(ComplexTree_InsertMultiple) {
    ComplexTree t;
    t.insert(Complex(1, 1), Complex(0, 1));
    t.insert(Complex(1, 2), Complex(0, 2));
    t.insert(Complex(2, 0), Complex(0, 3));
    ASSERT_NOT_NULL(t.find(Complex(1, 1)));
    ASSERT_NOT_NULL(t.find(Complex(1, 2)));
    ASSERT_NOT_NULL(t.find(Complex(2, 0)));
}
TEST_CASE(ComplexTree_CustomComparator) {
    ComplexTree t(ComplexCompare{});
    t.insert(Complex(1, 5), Complex(10, 10));
    t.insert(Complex(1, 2), Complex(20, 20));
    t.insert(Complex(2, 0), Complex(30, 30));
    std::vector<Complex> keys;
    CollectKeysComplex collector{keys};
    t.traverse(traversal::LNR{}, collector);
    ASSERT_EQ(keys.size(), (std::size_t)3);
    ASSERT_EQ(keys[0], Complex(1, 2));
    ASSERT_EQ(keys[1], Complex(1, 5));
    ASSERT_EQ(keys[2], Complex(2, 0));
}
TEST_CASE(ComplexTree_EraseExisting)       { impl_erase_existing<ComplexTree, Complex, Complex>(Complex(1,1), Complex(0,0), Complex(2,2), Complex(0,0)); }
TEST_CASE(ComplexTree_CopyCtor)            { impl_copy_ctor<ComplexTree, Complex, Complex>(Complex(1,1), Complex(2,2)); }
TEST_CASE(ComplexTree_MoveCtor)            { impl_move_ctor<ComplexTree, Complex, Complex>(Complex(3,4), Complex(5,6)); }

// --- StringTree ---
TEST_CASE(StringTree_DefaultCtor)          { impl_default_ctor<StringTree, std::string, std::string>(); }
TEST_CASE(StringTree_InsertSingle)         { impl_insert_single<StringTree, std::string, std::string>("alpha", "first"); }
TEST_CASE(StringTree_InsertMultiple)       { impl_insert_multiple<StringTree, std::string, std::string>("a", "1", "m", "2", "z", "3"); }
TEST_CASE(StringTree_InsertDuplicate)      { impl_insert_duplicate<StringTree, std::string, std::string>("key", "v1", "v2"); }
TEST_CASE(StringTree_FindExisting)         { impl_find_existing<StringTree, std::string, std::string>("hello", "world"); }
TEST_CASE(StringTree_FindNonExisting)      { impl_find_non_existing<StringTree, std::string, std::string>("present", "absent"); }
TEST_CASE(StringTree_EraseExisting)        { impl_erase_existing<StringTree, std::string, std::string>("a", "1", "b", "2"); }
TEST_CASE(StringTree_EraseNonExisting)     { impl_erase_non_existing<StringTree, std::string, std::string>("x"); }
TEST_CASE(StringTree_EraseFromEmpty)       { impl_erase_from_empty<StringTree, std::string, std::string>("x"); }
TEST_CASE(StringTree_CopyCtor)             { impl_copy_ctor<StringTree, std::string, std::string>("k", "v"); }
TEST_CASE(StringTree_MoveCtor)             { impl_move_ctor<StringTree, std::string, std::string>("k", "v"); }
TEST_CASE(StringTree_SpecialCharacters) {
    StringTree t;
    t.insert("key with spaces", "value\twith\ttabs");
    t.insert("key\"quote", "value\nnewline");
    t.insert("", "empty key");
    ASSERT_EQ(t.find("key with spaces")->value, std::string("value\twith\ttabs"));
    ASSERT_EQ(t.find("key\"quote")->value, std::string("value\nnewline"));
    ASSERT_EQ(t.find("")->value, std::string("empty key"));
    assert_roundtrip(t);
}

// --- FunctionTree ---
TEST_CASE(FunctionTree_DefaultCtor)        { impl_default_ctor<FunctionTree, int, FunctionValue>(); }
TEST_CASE(FunctionTree_InsertSingle)       { impl_insert_single<FunctionTree, int, FunctionValue>(1, FunctionValue(2)); }
TEST_CASE(FunctionTree_InsertMultiple) {
    FunctionTree t;
    t.insert(1, FunctionValue(1));
    t.insert(2, FunctionValue(2));
    t.insert(3, FunctionValue(3));
    ASSERT_EQ(t.find(1)->value.call(42), 42);
    ASSERT_EQ(t.find(2)->value.call(4), 1);
    ASSERT_EQ(t.find(2)->value.call(5), 0);
    ASSERT_EQ(t.find(3)->value.call(-7), -1);
}
TEST_CASE(FunctionTree_EmptyCallThrows) {
    FunctionTree t;
    t.insert(1, FunctionValue(0));
    ASSERT_THROW(t.find(1)->value.call(10), std::logic_error);
}
TEST_CASE(FunctionTree_InvalidIdThrows) {
    ASSERT_THROW(FunctionValue(99), std::invalid_argument);
}
TEST_CASE(FunctionTree_CopyCtor)           { impl_copy_ctor<FunctionTree, int, FunctionValue>(1, FunctionValue(2)); }
TEST_CASE(FunctionTree_MoveCtor)           { impl_move_ctor<FunctionTree, int, FunctionValue>(1, FunctionValue(3)); }

// --- StudentTree ---
TEST_CASE(StudentTree_DefaultCtor)         { impl_default_ctor<StudentTree, PersonID, Student>(); }
TEST_CASE(StudentTree_InsertSingle) {
    Student s(Person("001", "Ivan", "Ivanovich", "Ivanov", Date(15, 3, 2008)), "BPI-21", 2);
    impl_insert_single<StudentTree, PersonID, Student>("001", s);
}
TEST_CASE(StudentTree_FindExisting) {
    Student s(Person("002", "Anna", "", "Petrova", Date(1, 1, 2009)), "BPI-22", 1);
    StudentTree t;
    t.insert("002", s);
    auto* n = t.find("002");
    ASSERT_NOT_NULL(n);
    ASSERT_EQ(n->value, s);
}
TEST_CASE(StudentTree_EraseExisting) {
    Student s1(Person("001", "A", "", "B", Date(1,1,2000)), "G1", 1);
    Student s2(Person("002", "C", "", "D", Date(1,1,2001)), "G2", 2);
    impl_erase_existing<StudentTree, PersonID, Student>("001", s1, "002", s2);
}
TEST_CASE(StudentTree_CopyCtor) {
    Student s(Person("001", "X", "", "Y", Date(1,1,2000)), "G", 3);
    impl_copy_ctor<StudentTree, PersonID, Student>("001", s);
}
TEST_CASE(StudentTree_MoveCtor) {
    Student s(Person("001", "X", "", "Y", Date(1,1,2000)), "G", 3);
    impl_move_ctor<StudentTree, PersonID, Student>("001", s);
}
TEST_CASE(StudentTree_RoundTrip) {
    StudentTree t;
    t.insert("001", Student(Person("001", "John", "Fitzgerald", "Kennedy", Date(29, 5, 1917)), "HS-1", 1));
    t.insert("002", Student(Person("002", "Jane", "", "Doe", Date(1, 1, 2000)), "UNI-3", 3));
    assert_roundtrip(t);
}

// --- TeacherTree ---
TEST_CASE(TeacherTree_DefaultCtor)         { impl_default_ctor<TeacherTree, PersonID, Teacher>(); }
TEST_CASE(TeacherTree_InsertSingle) {
    Teacher t(Person("T1", "Marie", "", "Curie", Date(7, 11, 1867)), "Physics", "Professor");
    impl_insert_single<TeacherTree, PersonID, Teacher>("T1", t);
}
TEST_CASE(TeacherTree_CopyCtor) {
    Teacher t(Person("T1", "A", "", "B", Date(1,1,1970)), "Math", "Docent");
    impl_copy_ctor<TeacherTree, PersonID, Teacher>("T1", t);
}
TEST_CASE(TeacherTree_MoveCtor) {
    Teacher t(Person("T1", "A", "", "B", Date(1,1,1970)), "Math", "Docent");
    impl_move_ctor<TeacherTree, PersonID, Teacher>("T1", t);
}
TEST_CASE(TeacherTree_RoundTrip) {
    TeacherTree t;
    t.insert("T1", Teacher(Person("T1", "Albert", "", "Einstein", Date(14, 3, 1879)), "Physics", "Professor"));
    t.insert("T2", Teacher(Person("T2", "Nikola", "", "Tesla", Date(10, 7, 1856)), "Electrical Eng.", "Inventor"));
    assert_roundtrip(t);
}

//  СЕРИАЛИЗАЦИЯ 

TEST_CASE(IntegerTree_RoundTrip) {
    IntegerTree t;
    for (int i = 1; i <= 15; ++i) t.insert(i, i * i);
    assert_roundtrip(t);
}

TEST_CASE(RealTree_RoundTrip_Precision) {
    RealTree t;
    t.insert(1.0/3.0, 1.0);
    t.insert(3.14159265358979323846, 2.0);
    t.insert(2.718281828459045, 3.0);
    assert_roundtrip(t);
}

TEST_CASE(FromString_InvalidHeader) {
    IntegerTree t;
    ASSERT_FALSE(t.from_string("XXX\n"));
    ASSERT_FALSE(t.from_string(""));
    ASSERT_FALSE(t.from_string("RBT"));
}

TEST_CASE(FromString_InvalidStructure) {
    IntegerTree t;
    ASSERT_TRUE(t.from_string("RBT\n"));
    ASSERT_TRUE(t.empty());
}

TEST_CASE(FromString_RootMustBeBlack) {
    std::string bad = "RBT\n. 0 1:1 1:1\n";
    IntegerTree t;
    ASSERT_FALSE(t.from_string(bad));
}

// ФУНКЦИОНАЛЬНЫЕ ОПЕРАЦИИ 

TEST_CASE(IntegerTree_Map) {
    IntegerTree t;
    t.insert(1, 10); t.insert(2, 20); t.insert(3, 30);
    auto mapped = t.map(DoubleValue{});
    ASSERT_EQ(mapped.find(1)->value, 20);
    ASSERT_EQ(mapped.find(2)->value, 40);
    ASSERT_EQ(mapped.find(3)->value, 60);
    ASSERT_EQ(t.find(1)->value, 10);
}

TEST_CASE(IntegerTree_Where) {
    IntegerTree t;
    for (int i = 1; i <= 10; ++i) t.insert(i, i);
    auto filtered = t.where(IsEvenPredicate{});
    for (int i = 1; i <= 10; ++i) {
        auto* n = filtered.find(i);
        if (i % 2 == 0) { ASSERT_NOT_NULL(n); }
        else            { ASSERT_NULL(n); }
    }
}

TEST_CASE(IntegerTree_Reduce_LNR) {
    IntegerTree t;
    for (int i = 1; i <= 5; ++i) t.insert(i, i);
    int sum = t.reduce(traversal::LNR{}, 0, SumAction{});
    ASSERT_EQ(sum, 15);
}

TEST_CASE(IntegerTree_Reduce_NLR) {
    IntegerTree t;
    for (int i = 1; i <= 5; ++i) t.insert(i, i);
    int sum = t.reduce(traversal::NLR{}, 0, SumAction{});
    ASSERT_EQ(sum, 15);
}

TEST_CASE(IntegerTree_Reduce_Product) {
    IntegerTree t;
    for (int i = 1; i <= 4; ++i) t.insert(i, i);
    int product = t.reduce(traversal::LNR{}, 1, ProductAction{});
    ASSERT_EQ(product, 24);
}

TEST_CASE(IntegerTree_Merge) {
    IntegerTree a, b;
    a.insert(1, 10); a.insert(2, 20); a.insert(3, 30);
    b.insert(3, 99); b.insert(4, 40); b.insert(5, 50);
    auto merged = a.merge(b);
    ASSERT_NOT_NULL(merged.find(1));
    ASSERT_NOT_NULL(merged.find(2));
    ASSERT_NOT_NULL(merged.find(3));
    ASSERT_NOT_NULL(merged.find(4));
    ASSERT_NOT_NULL(merged.find(5));
}

TEST_CASE(IntegerTree_ExtractSubtree) {
    IntegerTree t;
    t.insert(5, 50); t.insert(3, 30); t.insert(7, 70);
    t.insert(2, 20); t.insert(4, 40);
    auto sub = t.extract_subtree(3);
    ASSERT_NOT_NULL(sub.find(3));
    ASSERT_NOT_NULL(sub.find(2));
    ASSERT_NOT_NULL(sub.find(4));
    ASSERT_NULL(sub.find(5));
    ASSERT_NULL(sub.find(7));
}

TEST_CASE(IntegerTree_ContainsSubtree) {
    IntegerTree big;
    big.insert(1, 10); big.insert(2, 20); big.insert(3, 30); big.insert(4, 40);
    IntegerTree small(big);
    ASSERT_TRUE(big.contains_subtree(small));
    small.insert(99, 99);
    ASSERT_FALSE(big.contains_subtree(small));
}

TEST_CASE(IntegerTree_FindByPath) {
    IntegerTree t;
    t.insert(5, 50); t.insert(3, 30); t.insert(7, 70); t.insert(2, 20);
    auto* root = t.find_by_path("");
    ASSERT_NOT_NULL(root);
    ASSERT_EQ(root->key, 5);
    auto* left = t.find_by_path("L");
    ASSERT_NOT_NULL(left);
    ASSERT_EQ(left->key, 3);
    auto* ll = t.find_by_path("LL");
    ASSERT_NOT_NULL(ll);
    ASSERT_EQ(ll->key, 2);
    ASSERT_NULL(t.find_by_path("RRR"));
}

TEST_CASE(IntegerTree_Traverse_LNR) {
    IntegerTree t;
    t.insert(2, 20); t.insert(1, 10); t.insert(3, 30);
    std::vector<int> values;
    CollectValues collector{values};
    t.traverse(traversal::LNR{}, collector);
    std::vector<int> expected = {10, 20, 30};
    assert_vec_eq(values, expected);
}

TEST_CASE(IntegerTree_Traverse_NLR) {
    IntegerTree t;
    t.insert(2, 20); t.insert(1, 10); t.insert(3, 30);
    std::vector<int> values;
    CollectValues collector{values};
    t.traverse(traversal::NLR{}, collector);
    ASSERT_EQ(values[0], 20);
}

// ИНВАРИАНТЫ КЧ-ДЕРЕВА 

TEST_CASE(IntegerTree_BalanceAfterManyInserts) {
    IntegerTree t;
    for (int i = 1; i <= 200; ++i) t.insert(i, i);
    assert_roundtrip(t);
}

TEST_CASE(IntegerTree_BalanceAfterInsertsAndErases) {
    IntegerTree t;
    for (int i = 1; i <= 100; ++i) t.insert(i, i);
    for (int i = 1; i <= 100; i += 2) t.erase(i);
    for (int i = 101; i <= 150; ++i) t.insert(i, i);
    assert_roundtrip(t);
}

TEST_CASE(IntegerTree_SelfAssignment) {
    IntegerTree t;
    t.insert(1, 10); t.insert(2, 20);
    t = t;
    ASSERT_NOT_NULL(t.find(1));
    ASSERT_NOT_NULL(t.find(2));
    ASSERT_EQ(t.find(1)->value, 10);
}

// утечки памяти
class TreeTestObject {
public:
    static int alive_count;
    static int total_created;
    int value;

    TreeTestObject(int v = 0) : value(v) { ++alive_count; ++total_created; }
    TreeTestObject(const TreeTestObject& o) : value(o.value) { ++alive_count; ++total_created; }
    TreeTestObject(TreeTestObject&& o) noexcept : value(o.value) { ++alive_count; o.value = -1; }
    TreeTestObject& operator=(const TreeTestObject& o) { value = o.value; return *this; }
    TreeTestObject& operator=(TreeTestObject&& o) noexcept { value = o.value; o.value = -1; return *this; }
    ~TreeTestObject() { --alive_count; }

    bool operator==(const TreeTestObject& o) const { return value == o.value; }
    bool operator<(const TreeTestObject& o) const { return value < o.value; }

    static void reset() { alive_count = 0; total_created = 0; }
};
int TreeTestObject::alive_count = 0;
int TreeTestObject::total_created = 0;

inline std::ostream& operator<<(std::ostream& s, const TreeTestObject& o) { return s << o.value; }
inline std::istream& operator>>(std::istream& s, TreeTestObject& o) { return s >> o.value; }

using MemTree = RedBlackTree<int, TreeTestObject>;

TEST_CASE(Memory_DefaultCtorNoLeak) {
    TreeTestObject::reset();
    { MemTree t; }
    ASSERT_EQ(TreeTestObject::alive_count, 0);
}

TEST_CASE(Memory_InsertAndDestroy) {
    TreeTestObject::reset();
    {
        MemTree t;
        for (int i = 0; i < 100; ++i) t.insert(i, TreeTestObject(i));
    }
    ASSERT_EQ(TreeTestObject::alive_count, 0);
}

TEST_CASE(Memory_InsertAndEraseAll) {
    TreeTestObject::reset();
    {
        MemTree t;
        for (int i = 0; i < 50; ++i) t.insert(i, TreeTestObject(i));
        for (int i = 0; i < 50; ++i) t.erase(i);
    }
    ASSERT_EQ(TreeTestObject::alive_count, 0);
}

TEST_CASE(Memory_CopyCtorNoLeak) {
    TreeTestObject::reset();
    {
        MemTree a;
        for (int i = 0; i < 30; ++i) a.insert(i, TreeTestObject(i));
        MemTree b(a);
    }
    ASSERT_EQ(TreeTestObject::alive_count, 0);
}

TEST_CASE(Memory_CopyAssignNoLeak) {
    TreeTestObject::reset();
    {
        MemTree a, b;
        for (int i = 0; i < 20; ++i) a.insert(i, TreeTestObject(i));
        for (int i = 10; i < 40; ++i) b.insert(i, TreeTestObject(i));
        b = a;
    }
    ASSERT_EQ(TreeTestObject::alive_count, 0);
}

TEST_CASE(Memory_MoveCtorNoLeak) {
    TreeTestObject::reset();
    {
        MemTree a;
        for (int i = 0; i < 40; ++i) a.insert(i, TreeTestObject(i));
        MemTree b(std::move(a));
    }
    ASSERT_EQ(TreeTestObject::alive_count, 0);
}

TEST_CASE(Memory_MoveAssignNoLeak) {
    TreeTestObject::reset();
    {
        MemTree a, b;
        for (int i = 0; i < 25; ++i) a.insert(i, TreeTestObject(i));
        for (int i = 0; i < 15; ++i) b.insert(i, TreeTestObject(i));
        b = std::move(a);
    }
    ASSERT_EQ(TreeTestObject::alive_count, 0);
}

TEST_CASE(Memory_FromStringNoLeak) {
    TreeTestObject::reset();
    {
        MemTree a;
        for (int i = 0; i < 20; ++i) a.insert(i, TreeTestObject(i));
        std::string s = a.to_string();
        MemTree b;
        if (!b.from_string(s)) throw std::runtime_error("from_string failed");
    }
    ASSERT_EQ(TreeTestObject::alive_count, 0);
}

TEST_CASE(Memory_FromStringInvalidNoLeak) {
    TreeTestObject::reset();
    {
        MemTree t;
        std::string bad = "RBT\n. 1 1:5 1:0\nL 1 1:7 1:0\n";
        (void)t.from_string(bad);
    }
    ASSERT_EQ(TreeTestObject::alive_count, 0);
}

TEST_CASE(Memory_MapWhereReduceNoLeak) {
    TreeTestObject::reset();
    {
        MemTree t;
        for (int i = 0; i < 10; ++i) t.insert(i, TreeTestObject(i));
        auto m = t.map([](const TreeTestObject& v){ return TreeTestObject(v.value * 2); });
        auto w = t.where([](const TreeTestObject& v){ return v.value % 2 == 0; });
        int sum = t.reduce(traversal::LNR{}, 0, [](int acc, const TreeTestObject& v){ return acc + v.value; });
        (void)sum;
    }
    ASSERT_EQ(TreeTestObject::alive_count, 0);
}

TEST_CASE(Memory_MergeNoLeak) {
    TreeTestObject::reset();
    {
        MemTree a, b;
        for (int i = 0; i < 15; ++i) a.insert(i, TreeTestObject(i));
        for (int i = 10; i < 25; ++i) b.insert(i, TreeTestObject(i));
        auto m = a.merge(b);
    }
    ASSERT_EQ(TreeTestObject::alive_count, 0);
}

TEST_CASE(Memory_ExtractSubtreeNoLeak) {
    TreeTestObject::reset();
    {
        MemTree t;
        for (int i = 0; i < 20; ++i) t.insert(i, TreeTestObject(i));
        auto sub = t.extract_subtree(10);
    }
    ASSERT_EQ(TreeTestObject::alive_count, 0);
}

TEST_CASE(Memory_StressInsertErase) {
    TreeTestObject::reset();
    {
        MemTree t;
        for (int cycle = 0; cycle < 5; ++cycle) {
            for (int i = 0; i < 200; ++i) t.insert(i, TreeTestObject(i));
            for (int i = 0; i < 200; ++i) t.erase(i);
        }
    }
    ASSERT_EQ(TreeTestObject::alive_count, 0);
}

// НЕГАТИВНЫЕ ТЕСТЫ from_string 

TEST_CASE(FromString_BSTViolation_LeftChildGreater) {
    std::string bad = "RBT\n. 1 1:5 1:0\nL 1 1:7 1:0\n";
    IntegerTree t;
    ASSERT_FALSE(t.from_string(bad));
}

TEST_CASE(FromString_BSTViolation_RightChildLess) {
    std::string bad = "RBT\n. 1 1:5 1:0\nR 1 1:3 1:0\n";
    IntegerTree t;
    ASSERT_FALSE(t.from_string(bad));
}

TEST_CASE(FromString_TwoRedsInARow) {
    std::string bad = "RBT\n. 1 1:5 1:0\nL 0 1:3 1:0\nLL 0 1:1 1:0\n";
    IntegerTree t;
    ASSERT_FALSE(t.from_string(bad));
}

TEST_CASE(FromString_UnequalBlackHeights) {
    std::string bad = "RBT\n. 1 1:5 1:0\nL 1 1:3 1:0\nLL 1 1:1 1:0\nR 0 1:7 1:0\n";
    IntegerTree t;
    ASSERT_FALSE(t.from_string(bad));
}

TEST_CASE(FromString_InvalidKeyForType) {
    std::string bad = "RBT\n. 1 3:abc 1:0\n";
    IntegerTree t;
    ASSERT_FALSE(t.from_string(bad));
}

TEST_CASE(FromString_LengthMismatch_TooShort) {
    std::string bad = "RBT\n. 1 10:12345 1:0\n";
    IntegerTree t;
    ASSERT_FALSE(t.from_string(bad));
}

TEST_CASE(FromString_InvalidPathCharacter) {
    std::string bad = "RBT\nX 1 1:5 1:0\n";
    IntegerTree t;
    ASSERT_FALSE(t.from_string(bad));
}

TEST_CASE(FromString_TruncatedRecord) {
    std::string bad = "RBT\n. 1 1:5 \n";
    IntegerTree t;
    ASSERT_FALSE(t.from_string(bad));
}

TEST_CASE(FromString_UnknownHeader) {
    std::string bad = "XYZ\n. 1 1:5 1:0\n";
    IntegerTree t;
    ASSERT_FALSE(t.from_string(bad));
}

TEST_CASE(FromString_EmptyString) {
    IntegerTree t;
    ASSERT_FALSE(t.from_string(""));
}

TEST_CASE(FromString_OnlyHeader) {
    IntegerTree t;
    ASSERT_FALSE(t.from_string("RBT"));
    IntegerTree t2;
    ASSERT_TRUE(t2.from_string("RBT\n"));
    ASSERT_TRUE(t2.empty());
}

TEST_CASE(FromString_NonBlackRoot) {
    std::string bad = "RBT\n. 0 1:5 1:0\n";
    IntegerTree t;
    ASSERT_FALSE(t.from_string(bad));
}

// find_by_path С allow_parent 

TEST_CASE(FindByPath_WithParent) {
    IntegerTree t;
    t.insert(5, 50);
    t.insert(3, 30);
    auto* node = t.find_by_path(3, "U");
    ASSERT_NOT_NULL(node);
    ASSERT_EQ(node->key, 5);
    auto* node2 = t.find_by_path(5, "U");
    ASSERT_NULL(node2);
}

TEST_CASE(FindByPath_ParentFromRoot) {
    IntegerTree t;
    t.insert(5, 50);
    auto* node = t.find_by_path(5, "U");
    ASSERT_NULL(node);
}

TEST_CASE(FindByPath_InvalidCharWithoutAllowParent) {
    IntegerTree t;
    t.insert(5, 50); t.insert(3, 30);
    auto* node = t.find_by_path("LU");
    ASSERT_NULL(node);
}

TEST_CASE(FindByPath_NullPath) {
    IntegerTree t;
    t.insert(5, 50);
    auto* node = t.find_by_path(nullptr);
    ASSERT_NULL(node);
}

TEST_CASE(FindByPath_EmptyPath) {
    IntegerTree t;
    t.insert(5, 50);
    auto* node = t.find_by_path("");
    ASSERT_NOT_NULL(node);
    ASSERT_EQ(node->key, 5);
}

// КОНСТРУКТОР С КОМПАРАТОРОМ 

TEST_CASE(Ctor_WithComparator) {
    // Обратный компаратор для int
    auto rev = [](const int& a, const int& b){ return a > b; };
    RedBlackTree<int, int> t(rev);  // передаём компаратор через конструктор
    t.insert(1, 10); t.insert(5, 50); t.insert(3, 30);
    // LNR с обратным компаратором даст: 5, 3, 1
    std::vector<int> keys;
    CollectKeys collector{keys};
    t.traverse(traversal::LNR{}, collector);
    ASSERT_EQ(keys.size(), (std::size_t)3);
    ASSERT_EQ(keys[0], 5);
    ASSERT_EQ(keys[1], 3);
    ASSERT_EQ(keys[2], 1);
}

// ГЕТТЕРЫ 

TEST_CASE(Getters_RootNilCompare) {
    IntegerTree t;
    ASSERT_TRUE(t.empty());
    ASSERT_TRUE(t.get_root() == t.get_nil());
    ASSERT_NOT_NULL(t.get_nil());
    const auto& cmp = t.get_compare();
    (void)cmp;
    
    t.insert(1, 10);
    ASSERT_FALSE(t.empty());
    ASSERT_NOT_NULL(t.get_root());
    ASSERT_FALSE(t.get_root() == t.get_nil());
}

// СЛОЖНЫЙ ERASE 

TEST_CASE(Erase_NodeWithTwoChildren) {
    IntegerTree t;
    t.insert(10, 100); t.insert(5, 50); t.insert(15, 150);
    t.insert(3, 30); t.insert(7, 70); t.insert(12, 120); t.insert(20, 200);
    bool erased = t.erase(10);
    ASSERT_TRUE(erased);
    ASSERT_NULL(t.find(10));
    ASSERT_NOT_NULL(t.find(5));
    ASSERT_NOT_NULL(t.find(15));
    ASSERT_NOT_NULL(t.find(3));
    ASSERT_NOT_NULL(t.find(7));
    ASSERT_NOT_NULL(t.find(12));
    ASSERT_NOT_NULL(t.find(20));
    assert_roundtrip(t);
}

TEST_CASE(Erase_Root) {
    IntegerTree t;
    t.insert(5, 50); t.insert(3, 30); t.insert(7, 70);
    bool erased = t.erase(5);
    ASSERT_TRUE(erased);
    ASSERT_NULL(t.find(5));
    ASSERT_NOT_NULL(t.find(3));
    ASSERT_NOT_NULL(t.find(7));
    assert_roundtrip(t);
}

TEST_CASE(Erase_UntilEmpty) {
    IntegerTree t;
    for (int i = 1; i <= 10; ++i) t.insert(i, i);
    for (int i = 1; i <= 10; ++i) {
        ASSERT_TRUE(t.erase(i));
    }
    ASSERT_TRUE(t.empty());
    ASSERT_NULL(t.find(1));
}

// MERGE ПРИ ПЕРЕСЕЧЕНИИ 

TEST_CASE(Merge_OverlappingKeysKeepsThis) {
    IntegerTree a, b;
    a.insert(1, 100); a.insert(2, 200);
    b.insert(2, 999); b.insert(3, 300);
    auto m = a.merge(b);
    ASSERT_EQ(m.find(1)->value, 100);
    ASSERT_EQ(m.find(2)->value, 200);
    ASSERT_EQ(m.find(3)->value, 300);
}

// CONTAINS_SUBTREE ПУСТОЙ 

TEST_CASE(ContainsSubtree_EmptyOther) {
    IntegerTree big;
    big.insert(1, 10); big.insert(2, 20);
    IntegerTree empty;
    ASSERT_TRUE(big.contains_subtree(empty));
}

TEST_CASE(ContainsSubtree_EqualTrees) {
    IntegerTree a, b;
    a.insert(1, 10); a.insert(2, 20); a.insert(3, 30);
    b.insert(1, 10); b.insert(2, 20); b.insert(3, 30);
    ASSERT_TRUE(a.contains_subtree(b));
}

TEST_CASE(ContainsSubtree_ValueMismatch) {
    IntegerTree big;
    big.insert(1, 10); big.insert(2, 20);
    IntegerTree small(big);
    small.erase(2);
    small.insert(2, 999);
    ASSERT_FALSE(big.contains_subtree(small));
}

// FIND НА ПУСТОМ ДЕРЕВЕ 

TEST_CASE(Find_OnEmptyTree) {
    IntegerTree t;
    ASSERT_NULL(t.find(42));
}

//  SELF-MOVE-ASSIGNMENT 

TEST_CASE(SelfMoveAssignment) {
    IntegerTree t;
    t.insert(1, 10); t.insert(2, 20);
    IntegerTree& ref = t;
    t = std::move(ref);
    ASSERT_NOT_NULL(t.find(1));
    ASSERT_NOT_NULL(t.find(2));
}

// DELETE_FIXUP STRESS 

TEST_CASE(DeleteFixup_StressSequential) {
    IntegerTree t;
    for (int i = 1; i <= 50; ++i) t.insert(i, i);
    for (int i = 1; i <= 50; ++i) {
        ASSERT_TRUE(t.erase(i));
        assert_roundtrip(t);
    }
    ASSERT_TRUE(t.empty());
}

TEST_CASE(DeleteFixup_RandomPattern) {
    IntegerTree t;
    for (int i = 1; i <= 100; ++i) t.insert(i, i);
    for (int i = 50; i >= 1; --i) {
        int k = (i * 37) % 100 + 1;
        t.erase(k);
    }
    assert_roundtrip(t);
}

// CHAR AS KEY/VALUE 

TEST_CASE(CharTree_RoundTrip) {
    RedBlackTree<char, char> t;
    t.insert('a', 'A');
    t.insert('b', 'B');
    t.insert('z', 'Z');
    std::string s = t.to_string();
    RedBlackTree<char, char> t2;
    ASSERT_TRUE(t2.from_string(s));
    ASSERT_EQ(t2.find('a')->value, 'A');
    ASSERT_EQ(t2.find('z')->value, 'Z');
}

//  ДОПОЛНИТЕЛЬНЫЕ ТЕСТЫ ДЛЯ СТРАТЕГИЙ 

TEST_CASE(IntegerTree_Traverse_AllStrategies) {
    IntegerTree t;
    t.insert(2, 20); t.insert(1, 10); t.insert(3, 30);
    
    std::vector<int> lnr_values;
    CollectValues lnr_collector{lnr_values};
    t.traverse(traversal::LNR{}, lnr_collector);
    ASSERT_EQ(lnr_values.size(), (std::size_t)3);
    
    std::vector<int> nlr_values;
    CollectValues nlr_collector{nlr_values};
    t.traverse(traversal::NLR{}, nlr_collector);
    ASSERT_EQ(nlr_values.size(), (std::size_t)3);
    
    std::vector<int> lrn_values;
    CollectValues lrn_collector{lrn_values};
    t.traverse(traversal::LRN{}, lrn_collector);
    ASSERT_EQ(lrn_values.size(), (std::size_t)3);
}


template <typename Strategy>
void check_enumerator_order(Strategy strategy, const int* expected) {
    IntegerTree tree;
    const int keys[] = {4, 2, 6, 1, 3, 5, 7};
    for (int key : keys) tree.insert(key, key * 10);
    IntegerTree::TreeEnumerator<Strategy> enumerator(tree, strategy);
    ASSERT_THROW(enumerator.GetCurrent(), std::out_of_range);
    for (int i = 0; i < 7; ++i) {
        ASSERT_TRUE(enumerator.MoveNext());
        const IntegerTree::Node& node = enumerator.GetCurrent();
        ASSERT_EQ(node.key, expected[i]);
        ASSERT_EQ(node.value, expected[i] * 10);
        ASSERT_TRUE(&node == tree.find(expected[i]));
        ASSERT_TRUE(&node == &enumerator.GetCurrent());
    }
    ASSERT_FALSE(enumerator.MoveNext());
    ASSERT_FALSE(enumerator.MoveNext());
    ASSERT_THROW(enumerator.GetCurrent(), std::out_of_range);
    enumerator.Reset();
    ASSERT_THROW(enumerator.GetCurrent(), std::out_of_range);
    for (int i = 0; i < 7; ++i) {
        ASSERT_TRUE(enumerator.MoveNext());
        ASSERT_EQ(enumerator.GetCurrent().key, expected[i]);
    }
    ASSERT_FALSE(enumerator.MoveNext());
}

TEST_CASE(TreeEnumerator_AllOrders) {
    const int nlr[] = {4, 2, 1, 3, 6, 5, 7};
    const int nrl[] = {4, 6, 7, 5, 2, 3, 1};
    const int lnr[] = {1, 2, 3, 4, 5, 6, 7};
    const int rnl[] = {7, 6, 5, 4, 3, 2, 1};
    const int lrn[] = {1, 3, 2, 5, 7, 6, 4};
    const int rln[] = {7, 5, 6, 3, 1, 2, 4};
    check_enumerator_order(traversal::NLR{}, nlr);
    check_enumerator_order(traversal::NRL{}, nrl);
    check_enumerator_order(traversal::LNR{}, lnr);
    check_enumerator_order(traversal::RNL{}, rnl);
    check_enumerator_order(traversal::LRN{}, lrn);
    check_enumerator_order(traversal::RLN{}, rln);
}

TEST_CASE(TreeEnumerator_Empty) {
    IntegerTree tree;
    IntegerTree::TreeEnumerator<> enumerator(tree);
    ASSERT_THROW(enumerator.GetCurrent(), std::out_of_range);
    ASSERT_FALSE(enumerator.MoveNext());
    enumerator.Reset();
    ASSERT_FALSE(enumerator.MoveNext());
    ASSERT_THROW(enumerator.GetCurrent(), std::out_of_range);
}

TEST_CASE(TreeEnumerator_ResetDuringTraversal) {
    IntegerTree tree;
    tree.insert(2, 20);
    tree.insert(1, 10);
    tree.insert(3, 30);
    IntegerTree::TreeEnumerator<> enumerator(tree);
    ASSERT_TRUE(enumerator.MoveNext());
    ASSERT_TRUE(enumerator.MoveNext());
    enumerator.Reset();
    ASSERT_TRUE(enumerator.MoveNext());
    ASSERT_EQ(enumerator.GetCurrent().key, 1);
}

TEST_CASE(TreeEnumerator_Single) {
    IntegerTree tree;
    tree.insert(1, 10);
    IntegerTree::TreeEnumerator<> enumerator(tree);
    ASSERT_TRUE(enumerator.MoveNext());
    ASSERT_EQ(enumerator.GetCurrent().value, 10);
    ASSERT_FALSE(enumerator.MoveNext());
    enumerator.Reset();
    ASSERT_TRUE(enumerator.MoveNext());
    ASSERT_EQ(enumerator.GetCurrent().key, 1);
}

TEST_CASE(TreeEnumerator_Independent) {
    IntegerTree tree;
    tree.insert(2, 20);
    tree.insert(1, 10);
    tree.insert(3, 30);
    IntegerTree::TreeEnumerator<> first(tree);
    IntegerTree::TreeEnumerator<traversal::RNL> second(tree);
    ASSERT_TRUE(first.MoveNext());
    ASSERT_TRUE(second.MoveNext());
    ASSERT_EQ(first.GetCurrent().key, 1);
    ASSERT_EQ(second.GetCurrent().key, 3);
    first.Reset();
    ASSERT_TRUE(second.MoveNext());
    ASSERT_EQ(second.GetCurrent().key, 2);
}

TEST_CASE(TreeEnumerator_Interface) {
    IntegerTree tree;
    tree.insert(2, 20);
    tree.insert(1, 10);
    tree.insert(3, 30);
    IEnumerator<IntegerTree::Node>* enumerator = tree.GetEnumerator();
    std::vector<int> keys;
    while (enumerator->MoveNext()) keys.push_back(enumerator->GetCurrent().key);
    enumerator->Reset();
    bool restarted = enumerator->MoveNext();
    delete enumerator;
    ASSERT_TRUE(restarted);
    ASSERT_TRUE(keys == std::vector<int>({1, 2, 3}));

    enumerator = tree.GetEnumerator(traversal::NLR{});
    keys.clear();
    while (enumerator->MoveNext()) keys.push_back(enumerator->GetCurrent().key);
    delete enumerator;
    ASSERT_TRUE(keys == std::vector<int>({2, 1, 3}));
}

struct EnumeratorCountingOrder {
    int& calls;
    template <typename V, typename L, typename R>
    void operator()(const V& visit, const L& left, const R& right) const {
        ++calls;
        visit();
        left();
        right();
    }
};

TEST_CASE(TreeEnumerator_Lazy) {
    IntegerTree tree;
    for (int i = 0; i < 100; ++i) tree.insert(i, i);
    int calls = 0;
    IntegerTree::TreeEnumerator<EnumeratorCountingOrder> enumerator(tree, EnumeratorCountingOrder{calls});
    ASSERT_EQ(calls, 0);
    ASSERT_TRUE(enumerator.MoveNext());
    ASSERT_EQ(calls, 1);
    ASSERT_TRUE(&enumerator.GetCurrent() == tree.get_root());
}

struct EnumeratorThrowVisit {
    int& calls;
    void operator()(const int&, const int&) const {
        if (++calls == 2) throw std::runtime_error("visit");
    }
};

struct EnumeratorThrowReduce {
    int& calls;
    int operator()(const int& result, const int& value) const {
        if (++calls == 2) throw std::runtime_error("reduce");
        return result + value;
    }
};

TEST_CASE(TreeEnumerator_ActionException) {
    IntegerTree tree;
    tree.insert(2, 20);
    tree.insert(1, 10);
    tree.insert(3, 30);
    std::string before = tree.to_string();
    int calls = 0;
    ASSERT_THROW(tree.traverse(traversal::NLR{}, EnumeratorThrowVisit{calls}), std::runtime_error);
    ASSERT_EQ(calls, 2);
    calls = 0;
    ASSERT_THROW(tree.reduce(traversal::LNR{}, 0, EnumeratorThrowReduce{calls}), std::runtime_error);
    ASSERT_EQ(calls, 2);
    ASSERT_EQ(tree.to_string(), before);
}

TEST_CASE(TreeEnumerator_SelectedOrder) {
    IntegerTree tree;
    const int keys[] = {4, 2, 6, 1, 3, 5, 7};
    for (int key : keys) tree.insert(key, key * 10);
    std::vector<std::string> paths = {"R", "", "LL", "R"};
    IntegerTree::TreeEnumerator<> enumerator(tree, paths);
    const int expected[] = {6, 4, 1, 6};
    ASSERT_THROW(enumerator.GetCurrent(), std::out_of_range);
    for (int i = 0; i < 4; ++i) {
        ASSERT_TRUE(enumerator.MoveNext());
        const IntegerTree::Node& node = enumerator.GetCurrent();
        ASSERT_EQ(node.key, expected[i]);
        ASSERT_EQ(node.value, expected[i] * 10);
        ASSERT_TRUE(&node == tree.find(expected[i]));
    }
    ASSERT_FALSE(enumerator.MoveNext());
    ASSERT_FALSE(enumerator.MoveNext());
    ASSERT_THROW(enumerator.GetCurrent(), std::out_of_range);
}

TEST_CASE(TreeEnumerator_SelectedReset) {
    IntegerTree tree;
    tree.insert(2, 20);
    tree.insert(1, 10);
    tree.insert(3, 30);
    std::vector<std::string> paths = {"R", "L"};
    IntegerTree::TreeEnumerator<> enumerator(tree, paths);
    for (int repeat = 0; repeat < 2; ++repeat) {
        ASSERT_TRUE(enumerator.MoveNext());
        enumerator.Reset();
        ASSERT_THROW(enumerator.GetCurrent(), std::out_of_range);
        ASSERT_TRUE(enumerator.MoveNext());
        ASSERT_EQ(enumerator.GetCurrent().key, 3);
        ASSERT_TRUE(enumerator.MoveNext());
        ASSERT_EQ(enumerator.GetCurrent().key, 1);
        ASSERT_FALSE(enumerator.MoveNext());
        enumerator.Reset();
    }
}

TEST_CASE(TreeEnumerator_SelectedEmpty) {
    IntegerTree tree;
    std::vector<std::string> paths;
    IntegerTree::TreeEnumerator<> empty(tree, paths);
    ASSERT_FALSE(empty.MoveNext());
    empty.Reset();
    ASSERT_FALSE(empty.MoveNext());
    ASSERT_THROW(empty.GetCurrent(), std::out_of_range);
    IntegerTree nonempty_tree;
    nonempty_tree.insert(1, 10);
    IntegerTree::TreeEnumerator<> nonempty(nonempty_tree, paths);
    ASSERT_FALSE(nonempty.MoveNext());
    nonempty.Reset();
    ASSERT_FALSE(nonempty.MoveNext());
}

TEST_CASE(TreeEnumerator_SelectedInvalid) {
    IntegerTree tree;
    tree.insert(2, 20);
    tree.insert(1, 10);
    std::string before = tree.to_string();
    const std::string invalid[] = {"R", "LL", "U", "LX", ".", std::string("L\0R", 3)};
    for (const std::string& path : invalid) {
        std::vector<std::string> paths = {"", path, "L"};
        ASSERT_THROW(IntegerTree::TreeEnumerator<>(tree, paths), std::invalid_argument);
    }
    ASSERT_EQ(tree.to_string(), before);
    IntegerTree empty;
    std::vector<std::string> root = {""};
    ASSERT_THROW(IntegerTree::TreeEnumerator<>(empty, root), std::invalid_argument);
}

TEST_CASE(TreeEnumerator_SelectedIndependent) {
    IntegerTree tree;
    tree.insert(2, 20);
    tree.insert(1, 10);
    tree.insert(3, 30);
    std::vector<std::string> paths = {"R", ""};
    IntegerTree::TreeEnumerator<> selected(tree, paths);
    paths.clear();
    IntegerTree::TreeEnumerator<> standard(tree);
    ASSERT_TRUE(selected.MoveNext());
    ASSERT_EQ(selected.GetCurrent().key, 3);
    ASSERT_TRUE(standard.MoveNext());
    ASSERT_EQ(standard.GetCurrent().key, 1);
    standard.Reset();
    ASSERT_TRUE(selected.MoveNext());
    ASSERT_EQ(selected.GetCurrent().key, 2);
    ASSERT_FALSE(selected.MoveNext());
}

TEST_CASE(TreeEnumerator_SelectedInterface) {
    IntegerTree tree;
    tree.insert(2, 20);
    tree.insert(1, 10);
    tree.insert(3, 30);
    std::vector<std::string> paths = {"R", "L", "R"};
    IEnumerator<IntegerTree::Node>* enumerator = tree.GetEnumerator(paths);
    std::vector<int> keys;
    while (enumerator->MoveNext()) keys.push_back(enumerator->GetCurrent().key);
    enumerator->Reset();
    bool restarted = enumerator->MoveNext();
    int first = enumerator->GetCurrent().key;
    delete enumerator;
    ASSERT_TRUE(keys == std::vector<int>({3, 1, 3}));
    ASSERT_TRUE(restarted);
    ASSERT_EQ(first, 3);
}

TEST_CASE(TreeEnumerator_SelectedRootOnly) {
    IntegerTree tree;
    tree.insert(2, 20);
    tree.insert(1, 10);
    tree.insert(3, 30);
    int calls = 0;
    std::vector<std::string> paths = {""};
    IntegerTree::TreeEnumerator<EnumeratorCountingOrder> enumerator(tree, paths, EnumeratorCountingOrder{calls});
    ASSERT_TRUE(enumerator.MoveNext());
    ASSERT_TRUE(&enumerator.GetCurrent() == tree.get_root());
    ASSERT_FALSE(enumerator.MoveNext());
    ASSERT_EQ(calls, 0);
}

void run_rb_tree_tests() {
    RUN_TEST(TreeEnumerator_SelectedOrder);
    RUN_TEST(TreeEnumerator_SelectedReset);
    RUN_TEST(TreeEnumerator_SelectedEmpty);
    RUN_TEST(TreeEnumerator_SelectedInvalid);
    RUN_TEST(TreeEnumerator_SelectedIndependent);
    RUN_TEST(TreeEnumerator_SelectedInterface);
    RUN_TEST(TreeEnumerator_SelectedRootOnly);
    RUN_TEST(TreeEnumerator_AllOrders);
    RUN_TEST(TreeEnumerator_Empty);
    RUN_TEST(TreeEnumerator_ResetDuringTraversal);
    RUN_TEST(TreeEnumerator_Single);
    RUN_TEST(TreeEnumerator_Independent);
    RUN_TEST(TreeEnumerator_Interface);
    RUN_TEST(TreeEnumerator_Lazy);
    RUN_TEST(TreeEnumerator_ActionException);
    std::cout << "\n========================================\n";
    std::cout << "      RedBlackTree Unit Tests\n";
    std::cout << "========================================\n";

    // IntegerTree
    RUN_TEST(IntegerTree_DefaultCtor);
    RUN_TEST(IntegerTree_InsertSingle);
    RUN_TEST(IntegerTree_InsertMultiple);
    RUN_TEST(IntegerTree_InsertDuplicate);
    RUN_TEST(IntegerTree_FindExisting);
    RUN_TEST(IntegerTree_FindNonExisting);
    RUN_TEST(IntegerTree_EraseExisting);
    RUN_TEST(IntegerTree_EraseNonExisting);
    RUN_TEST(IntegerTree_EraseFromEmpty);
    RUN_TEST(IntegerTree_CopyCtor);
    RUN_TEST(IntegerTree_CopyAssign);
    RUN_TEST(IntegerTree_MoveCtor);
    RUN_TEST(IntegerTree_MoveAssign);
    RUN_TEST(IntegerTree_SelfAssignment);

    // RealTree
    RUN_TEST(RealTree_DefaultCtor);
    RUN_TEST(RealTree_InsertSingle);
    RUN_TEST(RealTree_InsertMultiple);
    RUN_TEST(RealTree_InsertDuplicate);
    RUN_TEST(RealTree_FindExisting);
    RUN_TEST(RealTree_FindNonExisting);
    RUN_TEST(RealTree_EraseExisting);
    RUN_TEST(RealTree_EraseNonExisting);
    RUN_TEST(RealTree_EraseFromEmpty);
    RUN_TEST(RealTree_CopyCtor);
    RUN_TEST(RealTree_CopyAssign);
    RUN_TEST(RealTree_MoveCtor);
    RUN_TEST(RealTree_MoveAssign);

    // ComplexTree
    RUN_TEST(ComplexTree_DefaultCtor);
    RUN_TEST(ComplexTree_InsertSingle);
    RUN_TEST(ComplexTree_InsertMultiple);
    RUN_TEST(ComplexTree_CustomComparator);
    RUN_TEST(ComplexTree_EraseExisting);
    RUN_TEST(ComplexTree_CopyCtor);
    RUN_TEST(ComplexTree_MoveCtor);

    // StringTree
    RUN_TEST(StringTree_DefaultCtor);
    RUN_TEST(StringTree_InsertSingle);
    RUN_TEST(StringTree_InsertMultiple);
    RUN_TEST(StringTree_InsertDuplicate);
    RUN_TEST(StringTree_FindExisting);
    RUN_TEST(StringTree_FindNonExisting);
    RUN_TEST(StringTree_EraseExisting);
    RUN_TEST(StringTree_EraseNonExisting);
    RUN_TEST(StringTree_EraseFromEmpty);
    RUN_TEST(StringTree_CopyCtor);
    RUN_TEST(StringTree_MoveCtor);
    RUN_TEST(StringTree_SpecialCharacters);

    // FunctionTree
    RUN_TEST(FunctionTree_DefaultCtor);
    RUN_TEST(FunctionTree_InsertSingle);
    RUN_TEST(FunctionTree_InsertMultiple);
    RUN_TEST(FunctionTree_EmptyCallThrows);
    RUN_TEST(FunctionTree_InvalidIdThrows);
    RUN_TEST(FunctionTree_CopyCtor);
    RUN_TEST(FunctionTree_MoveCtor);

    // StudentTree
    RUN_TEST(StudentTree_DefaultCtor);
    RUN_TEST(StudentTree_InsertSingle);
    RUN_TEST(StudentTree_FindExisting);
    RUN_TEST(StudentTree_EraseExisting);
    RUN_TEST(StudentTree_CopyCtor);
    RUN_TEST(StudentTree_MoveCtor);
    RUN_TEST(StudentTree_RoundTrip);

    // TeacherTree
    RUN_TEST(TeacherTree_DefaultCtor);
    RUN_TEST(TeacherTree_InsertSingle);
    RUN_TEST(TeacherTree_CopyCtor);
    RUN_TEST(TeacherTree_MoveCtor);
    RUN_TEST(TeacherTree_RoundTrip);

    // Сериализация
    RUN_TEST(IntegerTree_RoundTrip);
    RUN_TEST(RealTree_RoundTrip_Precision);
    RUN_TEST(FromString_InvalidHeader);
    RUN_TEST(FromString_InvalidStructure);
    RUN_TEST(FromString_RootMustBeBlack);

    // Функциональные операции
    RUN_TEST(IntegerTree_Map);
    RUN_TEST(IntegerTree_Where);
    RUN_TEST(IntegerTree_Reduce_LNR);
    RUN_TEST(IntegerTree_Reduce_NLR);
    RUN_TEST(IntegerTree_Reduce_Product);
    RUN_TEST(IntegerTree_Merge);
    RUN_TEST(IntegerTree_ExtractSubtree);
    RUN_TEST(IntegerTree_ContainsSubtree);
    RUN_TEST(IntegerTree_FindByPath);
    RUN_TEST(IntegerTree_Traverse_LNR);
    RUN_TEST(IntegerTree_Traverse_NLR);

    // Инварианты
    RUN_TEST(IntegerTree_BalanceAfterManyInserts);
    RUN_TEST(IntegerTree_BalanceAfterInsertsAndErases);

    // Утечки памяти
    RUN_TEST(Memory_DefaultCtorNoLeak);
    RUN_TEST(Memory_InsertAndDestroy);
    RUN_TEST(Memory_InsertAndEraseAll);
    RUN_TEST(Memory_CopyCtorNoLeak);
    RUN_TEST(Memory_CopyAssignNoLeak);
    RUN_TEST(Memory_MoveCtorNoLeak);
    RUN_TEST(Memory_MoveAssignNoLeak);
    RUN_TEST(Memory_FromStringNoLeak);
    RUN_TEST(Memory_FromStringInvalidNoLeak);
    RUN_TEST(Memory_MapWhereReduceNoLeak);
    RUN_TEST(Memory_MergeNoLeak);
    RUN_TEST(Memory_ExtractSubtreeNoLeak);
    RUN_TEST(Memory_StressInsertErase);

    // Негативные from_string
    RUN_TEST(FromString_BSTViolation_LeftChildGreater);
    RUN_TEST(FromString_BSTViolation_RightChildLess);
    RUN_TEST(FromString_TwoRedsInARow);
    RUN_TEST(FromString_UnequalBlackHeights);
    RUN_TEST(FromString_InvalidKeyForType);
    RUN_TEST(FromString_LengthMismatch_TooShort);
    RUN_TEST(FromString_InvalidPathCharacter);
    RUN_TEST(FromString_TruncatedRecord);
    RUN_TEST(FromString_UnknownHeader);
    RUN_TEST(FromString_EmptyString);
    RUN_TEST(FromString_OnlyHeader);
    RUN_TEST(FromString_NonBlackRoot);

    // find_by_path с родителем
    RUN_TEST(FindByPath_WithParent);
    RUN_TEST(FindByPath_ParentFromRoot);
    RUN_TEST(FindByPath_InvalidCharWithoutAllowParent);
    RUN_TEST(FindByPath_NullPath);
    RUN_TEST(FindByPath_EmptyPath);

    // Конструктор с компаратором и геттеры
    RUN_TEST(Ctor_WithComparator);
    RUN_TEST(Getters_RootNilCompare);

    // Сложный erase
    RUN_TEST(Erase_NodeWithTwoChildren);
    RUN_TEST(Erase_Root);
    RUN_TEST(Erase_UntilEmpty);

    // Merge и contains_subtree
    RUN_TEST(Merge_OverlappingKeysKeepsThis);
    RUN_TEST(ContainsSubtree_EmptyOther);
    RUN_TEST(ContainsSubtree_EqualTrees);
    RUN_TEST(ContainsSubtree_ValueMismatch);

    // Прочее
    RUN_TEST(Find_OnEmptyTree);
    RUN_TEST(SelfMoveAssignment);
    RUN_TEST(DeleteFixup_StressSequential);
    RUN_TEST(DeleteFixup_RandomPattern);
    RUN_TEST(CharTree_RoundTrip);
    RUN_TEST(IntegerTree_Traverse_AllStrategies);

    std::cout << "\n========================================\n";
    std::cout << "Results: \033[32m" << rb_tests_passed << " passed\033[0m, \033[31m" << rb_tests_failed << " failed\033[0m\n";
    std::cout << "========================================\n";
}

#endif
