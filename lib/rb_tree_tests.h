#ifndef RB_TREE_TESTS_H
#define RB_TREE_TESTS_H

#include <iostream>
#include <string>
#include <vector>
#include <cmath>
#include <complex>
#include <stdexcept>
#include "dataTypes.h"

// ====================== СЧЁТЧИКИ И МАКРОСЫ ======================
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

// ====================== ХЕЛПЕРЫ ======================
template <typename Tree, typename K, typename V>
std::vector<std::pair<K, V>> collect_lnr(const Tree& tree) {
    std::vector<std::pair<K, V>> result;
    tree.traverse(Tree::TraversalOrder::LNR,
        [&](const K& key, const V& value) {
            result.emplace_back(key, value);
        });
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

// Универсальный round-trip тест — сравниваем через to_string,
// чтобы не зависеть от типов Key/Value
template <typename Tree>
void assert_roundtrip(const Tree& original,
                      typename Tree::TraversalOrder order = Tree::TraversalOrder::LNR) {
    std::string serialized = original.to_string(order);
    Tree restored;
    if (!restored.from_string(serialized))
        throw std::runtime_error("from_string returned false on valid serialization");
    std::string serialized2 = restored.to_string(order);
    if (serialized != serialized2)
        throw std::runtime_error("Round-trip failed: serialized strings differ");
}

// ====================== БАЗОВЫЕ ОПЕРАЦИИ (ШАБЛОНЫ) ======================

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
    t.insert(k, v2);  // должно игнорироваться
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
    ASSERT_FALSE(t.erase(K()));  // ключ, которого нет
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
    // независимость: меняем оригинал — копия не меняется
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

// ====================== DETECTOR УТЕЧЕК ПАМЯТИ ======================
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

// ====================== КОНКРЕТНЫЕ TEST_CASE ДЛЯ КАЖДОГО ТИПА ======================

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
    ComplexTree t;
    t.insert(Complex(1, 5), Complex(10, 10));
    t.insert(Complex(1, 2), Complex(20, 20));
    t.insert(Complex(2, 0), Complex(30, 30));
    auto vec = collect_lnr<ComplexTree, Complex, Complex>(t);
    ASSERT_EQ(vec.size(), (std::size_t)3);
    ASSERT_EQ(vec[0].first, Complex(1, 2));
    ASSERT_EQ(vec[1].first, Complex(1, 5));
    ASSERT_EQ(vec[2].first, Complex(2, 0));
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
    // Пробелы, кавычки, переносы строк — формат length:text должен переваривать
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
    t.insert(1, FunctionValue(1));  // identity
    t.insert(2, FunctionValue(2));  // is_even
    t.insert(3, FunctionValue(3));  // sign
    ASSERT_EQ(t.find(1)->value.call(42), 42);
    ASSERT_EQ(t.find(2)->value.call(4), 1);
    ASSERT_EQ(t.find(2)->value.call(5), 0);
    ASSERT_EQ(t.find(3)->value.call(-7), -1);
}
TEST_CASE(FunctionTree_EmptyCallThrows) {
    FunctionTree t;
    t.insert(1, FunctionValue(0));  // empty
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
    // std::quoted в операторе << должен корректно обрабатывать пробелы в именах
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

// ====================== СЕРИАЛИЗАЦИЯ ======================

TEST_CASE(IntegerTree_RoundTrip_AllOrders) {
    IntegerTree t;
    for (int i = 1; i <= 15; ++i) t.insert(i, i * i);
    assert_roundtrip(t, IntegerTree::TraversalOrder::NLR);
    assert_roundtrip(t, IntegerTree::TraversalOrder::NRL);
    assert_roundtrip(t, IntegerTree::TraversalOrder::LNR);
    assert_roundtrip(t, IntegerTree::TraversalOrder::RNL);
    assert_roundtrip(t, IntegerTree::TraversalOrder::LRN);
    assert_roundtrip(t, IntegerTree::TraversalOrder::RLN);
}

TEST_CASE(RealTree_RoundTrip_Precision) {
    // max_digits10 для long double должен гарантировать round-trip для double
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
    ASSERT_FALSE(t.from_string("LNR"));  // нет \n после заголовка
}

TEST_CASE(FromString_RootMustBeBlack) {
    // Сформируем строку, в которой корень красный — должно отвергнуться
    // Формат записи: "path color key_len:key value_len:value\n"
    // Путь "." = корень. Цвет "0" = красный.
    std::string bad = "LNR\n. 0 1:1 1:1\n";
    IntegerTree t;
    ASSERT_FALSE(t.from_string(bad));
}

// ====================== ФУНКЦИОНАЛЬНЫЕ ОПЕРАЦИИ ======================

TEST_CASE(IntegerTree_Map) {
    IntegerTree t;
    t.insert(1, 10); t.insert(2, 20); t.insert(3, 30);
    auto mapped = t.map([](const int& v) { return v * 2; });
    ASSERT_EQ(mapped.find(1)->value, 20);
    ASSERT_EQ(mapped.find(2)->value, 40);
    ASSERT_EQ(mapped.find(3)->value, 60);
    // оригинал не изменился
    ASSERT_EQ(t.find(1)->value, 10);
}

TEST_CASE(IntegerTree_Where) {
    IntegerTree t;
    for (int i = 1; i <= 10; ++i) t.insert(i, i);
    auto filtered = t.where([](const int& v) { return v % 2 == 0; });
    for (int i = 1; i <= 10; ++i) {
        auto* n = filtered.find(i);
        if (i % 2 == 0) { ASSERT_NOT_NULL(n); }
        else            { ASSERT_NULL(n); }
    }
}

TEST_CASE(IntegerTree_Reduce) {
    IntegerTree t;
    for (int i = 1; i <= 5; ++i) t.insert(i, i);  // значения 1,2,3,4,5
    int sum = t.reduce<int>(0, [](const int& acc, const int& v) { return acc + v; });
    ASSERT_EQ(sum, 15);
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
    // Построим дерево: корень 5, левый ребёнок 3, правый 7
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
    
    // small должен быть структурно равен поддереву big.
    // Самый простой способ: small = копия big.
    IntegerTree small(big);
    ASSERT_TRUE(big.contains_subtree(small));
    
    // Если добавить лишний ключ, структура изменится → false
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
    ASSERT_NULL(t.find_by_path("RRR"));  // нет такого пути
}

TEST_CASE(IntegerTree_Traverse_AllOrders) {
    IntegerTree t;
    t.insert(2, 20); t.insert(1, 10); t.insert(3, 30);
    std::vector<int> lnr;
    t.traverse(IntegerTree::TraversalOrder::LNR, [&](const int&, const int& v){ lnr.push_back(v); });
    std::vector<int> expected_lnr = {10, 20, 30};
    assert_vec_eq(lnr, expected_lnr);

    std::vector<int> nlr;
    t.traverse(IntegerTree::TraversalOrder::NLR, [&](const int&, const int& v){ nlr.push_back(v); });
    ASSERT_EQ(nlr[0], 20);  // корень первый
}

// ====================== ИНВАРИАНТЫ КЧ-ДЕРЕВА ======================

TEST_CASE(IntegerTree_BalanceAfterManyInserts) {
    IntegerTree t;
    // Вставляем 1..200 в отсортированном порядке — наивный BST выродился бы в список,
    // КЧ-дерево обязано сохранить баланс. Round-trip через from_string проверяет инварианты.
    for (int i = 1; i <= 200; ++i) t.insert(i, i);
    assert_roundtrip(t);
}

TEST_CASE(IntegerTree_BalanceAfterInsertsAndErases) {
    IntegerTree t;
    for (int i = 1; i <= 100; ++i) t.insert(i, i);
    for (int i = 1; i <= 100; i += 2) t.erase(i);  // удаляем нечётные
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

// ====================== УТЕЧКИ ПАМЯТИ ======================

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
        b = a;  // старое содержимое b должно быть уничтожено
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
    // Даже при откате из-за невалидной строки память не должна течь
    TreeTestObject::reset();
    {
        MemTree t;
        // Нарушение BST: корень 5, левый ребёнок 7
        std::string bad = "NLR\n. 1 1:5 1:0\nL 1 1:7 1:0\n";
        (void)t.from_string(bad);  // должно вернуть false
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
        int sum = t.reduce<int>(0, [](int acc, const TreeTestObject& v){ return acc + v.value; });
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

// ====================== НЕГАТИВНЫЕ ТЕСТЫ from_string ======================

TEST_CASE(FromString_BSTViolation_LeftChildGreater) {
    // NLR: корень 5, левый ребёнок 7 — нарушение BST
    std::string bad = "NLR\n. 1 1:5 1:0\nL 1 1:7 1:0\n";
    IntegerTree t;
    ASSERT_FALSE(t.from_string(bad));
}

TEST_CASE(FromString_BSTViolation_RightChildLess) {
    // NLR: корень 5, правый ребёнок 3 — нарушение BST
    std::string bad = "NLR\n. 1 1:5 1:0\nR 1 1:3 1:0\n";
    IntegerTree t;
    ASSERT_FALSE(t.from_string(bad));
}

TEST_CASE(FromString_TwoRedsInARow) {
    // NLR: корень красный (должен быть чёрный — уже ловится),
    // но проверим случай: корень чёрный, его ребёнок красный, и внук тоже красный
    // Путь: "." корень (чёрный), "L" красный, "LL" красный — нарушение
    std::string bad = "NLR\n. 1 1:5 1:0\nL 0 1:3 1:0\nLL 0 1:1 1:0\n";
    IntegerTree t;
    ASSERT_FALSE(t.from_string(bad));
}

TEST_CASE(FromString_UnequalBlackHeights) {
    // Левое поддерево имеет 2 чёрных узла, правое — 1.
    // NLR: корень 5 (чёрный), левый 3 (чёрный) с левым 1 (чёрный), правый 7 (красный)
    // Чёрная высота слева = 3, справа = 1
    std::string bad = "NLR\n. 1 1:5 1:0\nL 1 1:3 1:0\nLL 1 1:1 1:0\nR 0 1:7 1:0\n";
    IntegerTree t;
    ASSERT_FALSE(t.from_string(bad));
}

TEST_CASE(FromString_InvalidKeyForType) {
    // Пытаемся прочитать "abc" как int
    std::string bad = "NLR\n. 1 3:abc 1:0\n";
    IntegerTree t;
    ASSERT_FALSE(t.from_string(bad));
}

TEST_CASE(FromString_LengthMismatch_TooShort) {
    // Заявлена длина ключа 10, но текст "12345" — 5 символов
    std::string bad = "NLR\n. 1 10:12345 1:0\n";
    IntegerTree t;
    ASSERT_FALSE(t.from_string(bad));
}

TEST_CASE(FromString_InvalidPathCharacter) {
    // Путь содержит 'X'
    std::string bad = "NLR\nX 1 1:5 1:0\n";
    IntegerTree t;
    ASSERT_FALSE(t.from_string(bad));
}

TEST_CASE(FromString_TruncatedRecord) {
    // Обрезанная запись: нет значения
    std::string bad = "NLR\n. 1 1:5 \n";
    IntegerTree t;
    ASSERT_FALSE(t.from_string(bad));
}

TEST_CASE(FromString_DuplicateKeys) {
    // Два узла с ключом 1 — insert второй раз проигнорирует, но check_subtree
    // может не заметить; главное, что round-trip через to_string такого не выдаст.
    // Сформируем руками и проверим, что from_string либо отвергнет, либо построит
    // некорректное дерево. В нашей реализации insert молча пропустит дубликат,
    // поэтому узел "R 1 1:1 1:0" просто не вставится, и структура разойдётся с ожидаемой.
    // Проверим, что check_string_subtree всё равно пройдёт (дерево валидно, просто меньше узлов).
    // Поэтому этот тест — на то, что from_string не падает и возвращает осмысленный результат.
    std::string dup = "NLR\n. 1 1:5 1:0\nL 1 1:1 1:0\nR 1 1:1 1:0\n";
    IntegerTree t;
    // Ожидаем, что либо false (отвергнут дубликат), либо true, но дерево валидно.
    bool ok = t.from_string(dup);
    if (ok) {
        // Проверим, что дерево валидно через round-trip
        std::string s2 = t.to_string(IntegerTree::TraversalOrder::NLR);
        IntegerTree t2;
        ASSERT_TRUE(t2.from_string(s2));
    }
    // Если false — тоже нормально, просто дубликат отвергнут
}

TEST_CASE(FromString_UnknownTraversalOrder) {
    std::string bad = "XYZ\n. 1 1:5 1:0\n";
    IntegerTree t;
    ASSERT_FALSE(t.from_string(bad));
}

TEST_CASE(FromString_EmptyString) {
    IntegerTree t;
    ASSERT_FALSE(t.from_string(""));
}

TEST_CASE(FromString_InvalidStructure) {
    IntegerTree t;
    // Только заголовок без записей — это валидная сериализация пустого дерева
    ASSERT_TRUE(t.from_string("LNR\n"));
    ASSERT_TRUE(t.empty());
}

TEST_CASE(FromString_OnlyHeader) {
    IntegerTree t;
    ASSERT_FALSE(t.from_string("LNR"));  // нет \n — невалидно
    IntegerTree t2;
    ASSERT_TRUE(t2.from_string("LNR\n"));  // валидно: пустое дерево
    ASSERT_TRUE(t2.empty());
}

TEST_CASE(FromString_PathDoesNotMatchOrder) {
    // При LNR: сначала левое (путь "L"), потом корень (путь ""), потом правое (путь "R").
    // Если первая запись — корень ".", то левое поддерево пустое (nil), корень читается, правое пустое.
    // Это валидное дерево из одного узла.
    IntegerTree t;
    ASSERT_TRUE(t.from_string("LNR\n. 1 1:5 1:0\n"));
    ASSERT_NOT_NULL(t.find(5));
}

TEST_CASE(FromString_NonBlackRoot) {
    // Корень красный — отдельный тест, уже есть, но продублируем для LNR
    std::string bad = "LNR\n. 0 1:5 1:0\n";
    IntegerTree t;
    ASSERT_FALSE(t.from_string(bad));
}

// ====================== find_by_path С allow_parent ======================

TEST_CASE(FindByPath_WithParent) {
    IntegerTree t;
    t.insert(5, 50);
    t.insert(3, 30);
    // Теперь 3 — левый ребёнок 5, parent у 3 = 5
    
    auto* node = t.find_by_path(3, "U");  // parent 3 → 5
    ASSERT_NOT_NULL(node);
    ASSERT_EQ(node->key, 5);
    
    // U от корня → parent у 5 это nil → nullptr
    auto* node2 = t.find_by_path(5, "U");
    ASSERT_NULL(node2);
}

TEST_CASE(FindByPath_ParentFromRoot) {
    IntegerTree t;
    t.insert(5, 50);
    // U от корня — parent у корня это nil, follow_path вернёт nullptr
    auto* node = t.find_by_path(5, "U");
    ASSERT_NULL(node);
}

TEST_CASE(FindByPath_InvalidCharWithoutAllowParent) {
    IntegerTree t;
    t.insert(5, 50); t.insert(3, 30);
    // Первая перегрузка (allow_parent = false): 'U' должен вернуть nullptr
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

// ====================== КОНСТРУКТОР С КОМПАРАТОРОМ ======================

TEST_CASE(Ctor_WithComparator) {
    // Обратный компаратор для int
    auto rev = [](const int& a, const int& b){ return a > b; };
    RedBlackTree<int, int, decltype(rev)> t(rev);
    t.insert(1, 10); t.insert(5, 50); t.insert(3, 30);
    // LNR с обратным компаратором даст: 5, 3, 1
    std::vector<int> keys;
    t.traverse(RedBlackTree<int,int,decltype(rev)>::TraversalOrder::LNR,
        [&](const int& k, const int&){ keys.push_back(k); });
    ASSERT_EQ(keys.size(), (std::size_t)3);
    ASSERT_EQ(keys[0], 5);
    ASSERT_EQ(keys[1], 3);
    ASSERT_EQ(keys[2], 1);
}

// ====================== ГЕТТЕРЫ ======================

TEST_CASE(Getters_RootNilCompare) {
    IntegerTree t;
    ASSERT_TRUE(t.empty());
    ASSERT_TRUE(t.get_root() == t.get_nil());  // root == nil
    ASSERT_NOT_NULL(t.get_nil());
    const auto& cmp = t.get_compare();
    (void)cmp;
    
    t.insert(1, 10);
    ASSERT_FALSE(t.empty());
    ASSERT_NOT_NULL(t.get_root());
    ASSERT_FALSE(t.get_root() == t.get_nil());  // root != nil
}

// ====================== СЛОЖНЫЙ ERASE ======================

TEST_CASE(Erase_NodeWithTwoChildren) {
    IntegerTree t;
    t.insert(10, 100); t.insert(5, 50); t.insert(15, 150);
    t.insert(3, 30); t.insert(7, 70); t.insert(12, 120); t.insert(20, 200);
    // Удаляем 10 — у него два ребёнка. В коде используется maximum(z->left) = 7.
    bool erased = t.erase(10);
    ASSERT_TRUE(erased);
    ASSERT_NULL(t.find(10));
    // Остальные должны остаться
    ASSERT_NOT_NULL(t.find(5));
    ASSERT_NOT_NULL(t.find(15));
    ASSERT_NOT_NULL(t.find(3));
    ASSERT_NOT_NULL(t.find(7));
    ASSERT_NOT_NULL(t.find(12));
    ASSERT_NOT_NULL(t.find(20));
    // Инварианты
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

// ====================== MERGE ПРИ ПЕРЕСЕЧЕНИИ ======================

TEST_CASE(Merge_OverlappingKeysKeepsThis) {
    // В коде: сначала merge_subtree(this), потом merge_subtree(other).
    // insert при совпадении ключа игнорирует новое значение → остаётся значение из this.
    IntegerTree a, b;
    a.insert(1, 100); a.insert(2, 200);
    b.insert(2, 999); b.insert(3, 300);
    auto m = a.merge(b);
    ASSERT_EQ(m.find(1)->value, 100);
    ASSERT_EQ(m.find(2)->value, 200);  // из a, не из b
    ASSERT_EQ(m.find(3)->value, 300);
}

// ====================== CONTAINS_SUBTREE ПУСТОЙ ======================

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
    IntegerTree big, small;
    big.insert(1, 10); big.insert(2, 20);
    small.insert(1, 10); small.insert(2, 999);  // значение другое
    ASSERT_FALSE(big.contains_subtree(small));
}

// ====================== FIND НА ПУСТОМ ДЕРЕВЕ ======================

TEST_CASE(Find_OnEmptyTree) {
    IntegerTree t;
    ASSERT_NULL(t.find(42));
}

// ====================== SELF-MOVE-ASSIGNMENT ======================

TEST_CASE(SelfMoveAssignment) {
    IntegerTree t;
    t.insert(1, 10); t.insert(2, 20);
    IntegerTree& ref = t;
    t = std::move(ref);  // self-move
    // В коде есть проверка if (this == &other) return *this;
    ASSERT_NOT_NULL(t.find(1));
    ASSERT_NOT_NULL(t.find(2));
}

// ====================== DELETE_FIXUP STRESS ======================

TEST_CASE(DeleteFixup_StressSequential) {
    // Удаляем по одному в отсортированном порядке — это провоцирует
    // разные ветки delete_fixup (красный sibling, чёрный sibling с красным ребёнком и т.д.)
    IntegerTree t;
    for (int i = 1; i <= 50; ++i) t.insert(i, i);
    for (int i = 1; i <= 50; ++i) {
        ASSERT_TRUE(t.erase(i));
        assert_roundtrip(t);  // после каждого удаления проверяем инварианты
    }
    ASSERT_TRUE(t.empty());
}

TEST_CASE(DeleteFixup_RandomPattern) {
    IntegerTree t;
    for (int i = 1; i <= 100; ++i) t.insert(i, i);
    // Удаляем в псевдослучайном порядке
    for (int i = 50; i >= 1; --i) {
        int k = (i * 37) % 100 + 1;
        t.erase(k);
    }
    assert_roundtrip(t);
}

// ====================== CHAR AS KEY/VALUE ======================

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

// ====================== АГРЕГАТОР ======================

void run_rb_tree_tests() {
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
    RUN_TEST(IntegerTree_RoundTrip_AllOrders);
    RUN_TEST(RealTree_RoundTrip_Precision);
    RUN_TEST(FromString_InvalidHeader);
    RUN_TEST(FromString_InvalidStructure);
    RUN_TEST(FromString_RootMustBeBlack);

    // Функциональные операции
    RUN_TEST(IntegerTree_Map);
    RUN_TEST(IntegerTree_Where);
    RUN_TEST(IntegerTree_Reduce);
    RUN_TEST(IntegerTree_Merge);
    RUN_TEST(IntegerTree_ExtractSubtree);
    RUN_TEST(IntegerTree_ContainsSubtree);
    RUN_TEST(IntegerTree_FindByPath);
    RUN_TEST(IntegerTree_Traverse_AllOrders);

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
    RUN_TEST(FromString_DuplicateKeys);
    RUN_TEST(FromString_UnknownTraversalOrder);
    RUN_TEST(FromString_EmptyString);
    RUN_TEST(FromString_OnlyHeader);
    RUN_TEST(FromString_PathDoesNotMatchOrder);
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

    std::cout << "\n========================================\n";
    std::cout << "Results: \033[32m" << rb_tests_passed << " passed\033[0m, \033[31m" << rb_tests_failed << " failed\033[0m\n";
    std::cout << "========================================\n";
}

#endif