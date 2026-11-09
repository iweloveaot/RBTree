#include "test_helpers.h"
#include "type_fixtures.h"

// MATRIX
void test_matrix_integer_empty() {
    using D = IntegerData;
    using T = D::Tree;
    T tree;
    assert(tree.empty() && check_tree(tree));
}

// MATRIX
void test_matrix_integer_single() {
    using D = IntegerData;
    using T = D::Tree;
    T tree;
    tree.insert(D::key(1),D::value(1));
    assert(tree.find(D::key(1))->value == D::value(1) && node_count(tree) == 1 && check_tree(tree));
}

// MATRIX
void test_matrix_integer_find() {
    using D = IntegerData;
    using T = D::Tree;
    T tree;
    fill_data<D>(tree);
    assert_data<D>(tree);
}

// MATRIX
void test_matrix_integer_missing() {
    using D = IntegerData;
    using T = D::Tree;
    T tree;
    fill_data<D>(tree);
    assert(!tree.find(D::key(99)));
}

// MATRIX
void test_matrix_integer_duplicate() {
    using D = IntegerData;
    using T = D::Tree;
    T tree;
    fill_data<D>(tree);
    tree.insert(D::key(1),D::transform(D::value(1)));
    assert_data<D>(tree);
}

// MATRIX
void test_matrix_integer_erase() {
    using D = IntegerData;
    using T = D::Tree;
    T tree;
    fill_data<D>(tree);
    bool erased = tree.erase(D::key(2));
    assert(erased && !tree.find(D::key(2)) && node_count(tree) == 2 && check_tree(tree));
    assert(tree.find(D::key(1))->value == D::value(1) && tree.find(D::key(3))->value == D::value(3));
}

// MATRIX
void test_matrix_integer_copy() {
    using D = IntegerData;
    using T = D::Tree;
    T tree;
    fill_data<D>(tree);
    T copy(tree);
    assert_copy(tree,copy);
}

// MATRIX
void test_matrix_integer_copy_assign() {
    using D = IntegerData;
    using T = D::Tree;
    T tree;
    fill_data<D>(tree);
    T copy;
    copy.insert(D::key(99),D::value(1));
    copy = tree;
    assert_copy(tree,copy);
}

// MATRIX
void test_matrix_integer_move() {
    using D = IntegerData;
    using T = D::Tree;
    T tree;
    fill_data<D>(tree);
    auto root = tree.get_root();
    T moved(std::move(tree));
    assert(tree.empty() && moved.get_root() == root);
    assert_data<D>(moved);
    tree.insert(D::key(1),D::value(1));
    assert(check_tree(tree));
}

// MATRIX
void test_matrix_integer_move_assign() {
    using D = IntegerData;
    using T = D::Tree;
    T tree;
    fill_data<D>(tree);
    T moved;
    moved.insert(D::key(99),D::value(1));
    moved = std::move(tree);
    assert(tree.empty());
    assert_data<D>(moved);
}

// MATRIX
void test_matrix_integer_copy_independent() {
    using D = IntegerData;
    using T = D::Tree;
    T tree;
    fill_data<D>(tree);
    T copy(tree);
    copy.find(D::key(1))->value = D::transform(D::value(1));
    bool erased = copy.erase(D::key(2));
    assert(erased);
    assert_data<D>(tree);
}

// MATRIX
void test_matrix_integer_map() {
    using D = IntegerData;
    using T = D::Tree;
    T tree;
    fill_data<D>(tree);
    T result = tree.map(D::transform);
    for (int i=1;i<=3;++i) assert(result.find(D::key(i))->value == D::transform(D::value(i)) && result.find(D::key(i)) != tree.find(D::key(i)));
    assert(check_tree(result));
    assert_data<D>(tree);
}

// MATRIX
void test_matrix_integer_where() {
    using D = IntegerData;
    using T = D::Tree;
    T tree;
    fill_data<D>(tree);
    T result = tree.where(D::keep);
    assert(node_count(result) == 2 && !result.find(D::key(1)) && check_tree(result));
    assert(result.find(D::key(2))->value == D::value(2) && result.find(D::key(3))->value == D::value(3));
    assert_data<D>(tree);
}

// MATRIX
void test_matrix_integer_reduce() {
    using D = IntegerData;
    using T = D::Tree;
    T tree;
    fill_data<D>(tree);
    D::Result result = tree.reduce(D::Result(),D::fold);
    assert_result(result,D::expected());
    assert_data<D>(tree);
}

// MATRIX
void test_matrix_integer_merge() {
    using D = IntegerData;
    using T = D::Tree;
    T first,second;
    fill_data<D>(first);
    second.insert(D::key(2),D::transform(D::value(2)));
    second.insert(D::key(4),D::value(4));
    T result = first.merge(second);
    assert(node_count(result) == 4 && check_tree(result));
    assert(result.find(D::key(2))->value == D::value(2) && result.find(D::key(4))->value == D::value(4));
    assert_data<D>(first);
    assert(second.find(D::key(2))->value == D::transform(D::value(2)));
}

// MATRIX
void test_matrix_integer_extract_root() {
    using D = IntegerData;
    using T = D::Tree;
    T tree;
    fill_data<D>(tree);
    T result = tree.extract_subtree(D::key(2));
    assert_copy(tree,result);
}

// MATRIX
void test_matrix_integer_extract_child() {
    using D = IntegerData;
    using T = D::Tree;
    T tree;
    fill_data<D>(tree);
    T result = tree.extract_subtree(D::key(1));
    assert(node_count(result) == 1 && result.get_root()->color && tree.contains_subtree(result));
    assert(result.find(D::key(1)) != tree.find(D::key(1)));
    assert_data<D>(tree);
}

// MATRIX
void test_matrix_integer_contains_mismatch() {
    using D = IntegerData;
    using T = D::Tree;
    T tree;
    fill_data<D>(tree);
    T result = tree.extract_subtree(D::key(1));
    result.get_root()->value = D::transform(D::value(1));
    assert(!tree.contains_subtree(result));
}

// MATRIX
void test_matrix_integer_absolute_path() {
    using D = IntegerData;
    using T = D::Tree;
    T tree;
    fill_data<D>(tree);
    assert(tree.find_by_path("L") == tree.find(D::key(1)));
    assert(tree.find_by_path("R") == tree.find(D::key(3)));
    assert(tree.find_by_path("LL") == nullptr);
}

// MATRIX
void test_matrix_integer_relative_path() {
    using D = IntegerData;
    using T = D::Tree;
    T tree;
    fill_data<D>(tree);
    assert(tree.find_by_path(D::key(1),"UR") == tree.find(D::key(3)));
    assert(tree.find_by_path(D::key(1),"UU") == nullptr);
}

// MATRIX
void test_matrix_integer_traverse_nlr() {
    using D = IntegerData;
    using T = D::Tree;
    T tree;
    fill_data<D>(tree);
    using K = decltype(D::key(1));
    using V = decltype(D::value(1));
    std::vector<K> keys;
    std::vector<V> values;
    tree.traverse(T::TraversalOrder::NLR,TypedCollect<K,V>{keys,values});
    const int expected[] = {2,1,3};
    assert(keys.size() == 3 && values.size() == 3);
    for (int j=0;j<3;++j) assert(keys[j] == D::key(expected[j]) && values[j] == D::value(expected[j]));
}

// LOAD-016
void test_load_roundtrip_types_integer_nlr() {
    using D = IntegerData;
    using T = D::Tree;
    T tree;
    fill_data<D>(tree);
    T restored;
    std::string text = tree.to_string(T::TraversalOrder::NLR);
    bool success = restored.from_string(text);
    assert(success);
    assert_copy(tree,restored);
    assert(restored.to_string(T::TraversalOrder::NLR) == text);
}

// MATRIX
void test_matrix_integer_traverse_nrl() {
    using D = IntegerData;
    using T = D::Tree;
    T tree;
    fill_data<D>(tree);
    using K = decltype(D::key(1));
    using V = decltype(D::value(1));
    std::vector<K> keys;
    std::vector<V> values;
    tree.traverse(T::TraversalOrder::NRL,TypedCollect<K,V>{keys,values});
    const int expected[] = {2,3,1};
    assert(keys.size() == 3 && values.size() == 3);
    for (int j=0;j<3;++j) assert(keys[j] == D::key(expected[j]) && values[j] == D::value(expected[j]));
}

// LOAD-016
void test_load_roundtrip_types_integer_nrl() {
    using D = IntegerData;
    using T = D::Tree;
    T tree;
    fill_data<D>(tree);
    T restored;
    std::string text = tree.to_string(T::TraversalOrder::NRL);
    bool success = restored.from_string(text);
    assert(success);
    assert_copy(tree,restored);
    assert(restored.to_string(T::TraversalOrder::NRL) == text);
}

// MATRIX
void test_matrix_integer_traverse_lnr() {
    using D = IntegerData;
    using T = D::Tree;
    T tree;
    fill_data<D>(tree);
    using K = decltype(D::key(1));
    using V = decltype(D::value(1));
    std::vector<K> keys;
    std::vector<V> values;
    tree.traverse(T::TraversalOrder::LNR,TypedCollect<K,V>{keys,values});
    const int expected[] = {1,2,3};
    assert(keys.size() == 3 && values.size() == 3);
    for (int j=0;j<3;++j) assert(keys[j] == D::key(expected[j]) && values[j] == D::value(expected[j]));
}

// LOAD-016
void test_load_roundtrip_types_integer_lnr() {
    using D = IntegerData;
    using T = D::Tree;
    T tree;
    fill_data<D>(tree);
    T restored;
    std::string text = tree.to_string(T::TraversalOrder::LNR);
    bool success = restored.from_string(text);
    assert(success);
    assert_copy(tree,restored);
    assert(restored.to_string(T::TraversalOrder::LNR) == text);
}

// MATRIX
void test_matrix_integer_traverse_rnl() {
    using D = IntegerData;
    using T = D::Tree;
    T tree;
    fill_data<D>(tree);
    using K = decltype(D::key(1));
    using V = decltype(D::value(1));
    std::vector<K> keys;
    std::vector<V> values;
    tree.traverse(T::TraversalOrder::RNL,TypedCollect<K,V>{keys,values});
    const int expected[] = {3,2,1};
    assert(keys.size() == 3 && values.size() == 3);
    for (int j=0;j<3;++j) assert(keys[j] == D::key(expected[j]) && values[j] == D::value(expected[j]));
}

// LOAD-016
void test_load_roundtrip_types_integer_rnl() {
    using D = IntegerData;
    using T = D::Tree;
    T tree;
    fill_data<D>(tree);
    T restored;
    std::string text = tree.to_string(T::TraversalOrder::RNL);
    bool success = restored.from_string(text);
    assert(success);
    assert_copy(tree,restored);
    assert(restored.to_string(T::TraversalOrder::RNL) == text);
}

// MATRIX
void test_matrix_integer_traverse_lrn() {
    using D = IntegerData;
    using T = D::Tree;
    T tree;
    fill_data<D>(tree);
    using K = decltype(D::key(1));
    using V = decltype(D::value(1));
    std::vector<K> keys;
    std::vector<V> values;
    tree.traverse(T::TraversalOrder::LRN,TypedCollect<K,V>{keys,values});
    const int expected[] = {1,3,2};
    assert(keys.size() == 3 && values.size() == 3);
    for (int j=0;j<3;++j) assert(keys[j] == D::key(expected[j]) && values[j] == D::value(expected[j]));
}

// LOAD-016
void test_load_roundtrip_types_integer_lrn() {
    using D = IntegerData;
    using T = D::Tree;
    T tree;
    fill_data<D>(tree);
    T restored;
    std::string text = tree.to_string(T::TraversalOrder::LRN);
    bool success = restored.from_string(text);
    assert(success);
    assert_copy(tree,restored);
    assert(restored.to_string(T::TraversalOrder::LRN) == text);
}

// MATRIX
void test_matrix_integer_traverse_rln() {
    using D = IntegerData;
    using T = D::Tree;
    T tree;
    fill_data<D>(tree);
    using K = decltype(D::key(1));
    using V = decltype(D::value(1));
    std::vector<K> keys;
    std::vector<V> values;
    tree.traverse(T::TraversalOrder::RLN,TypedCollect<K,V>{keys,values});
    const int expected[] = {3,1,2};
    assert(keys.size() == 3 && values.size() == 3);
    for (int j=0;j<3;++j) assert(keys[j] == D::key(expected[j]) && values[j] == D::value(expected[j]));
}

// LOAD-016
void test_load_roundtrip_types_integer_rln() {
    using D = IntegerData;
    using T = D::Tree;
    T tree;
    fill_data<D>(tree);
    T restored;
    std::string text = tree.to_string(T::TraversalOrder::RLN);
    bool success = restored.from_string(text);
    assert(success);
    assert_copy(tree,restored);
    assert(restored.to_string(T::TraversalOrder::RLN) == text);
}

// MATRIX
void test_matrix_real_empty() {
    using D = RealData;
    using T = D::Tree;
    T tree;
    assert(tree.empty() && check_tree(tree));
}

// MATRIX
void test_matrix_real_single() {
    using D = RealData;
    using T = D::Tree;
    T tree;
    tree.insert(D::key(1),D::value(1));
    assert(tree.find(D::key(1))->value == D::value(1) && node_count(tree) == 1 && check_tree(tree));
}

// MATRIX
void test_matrix_real_find() {
    using D = RealData;
    using T = D::Tree;
    T tree;
    fill_data<D>(tree);
    assert_data<D>(tree);
}

// MATRIX
void test_matrix_real_missing() {
    using D = RealData;
    using T = D::Tree;
    T tree;
    fill_data<D>(tree);
    assert(!tree.find(D::key(99)));
}

// MATRIX
void test_matrix_real_duplicate() {
    using D = RealData;
    using T = D::Tree;
    T tree;
    fill_data<D>(tree);
    tree.insert(D::key(1),D::transform(D::value(1)));
    assert_data<D>(tree);
}

// MATRIX
void test_matrix_real_erase() {
    using D = RealData;
    using T = D::Tree;
    T tree;
    fill_data<D>(tree);
    bool erased = tree.erase(D::key(2));
    assert(erased && !tree.find(D::key(2)) && node_count(tree) == 2 && check_tree(tree));
    assert(tree.find(D::key(1))->value == D::value(1) && tree.find(D::key(3))->value == D::value(3));
}

// MATRIX
void test_matrix_real_copy() {
    using D = RealData;
    using T = D::Tree;
    T tree;
    fill_data<D>(tree);
    T copy(tree);
    assert_copy(tree,copy);
}

// MATRIX
void test_matrix_real_copy_assign() {
    using D = RealData;
    using T = D::Tree;
    T tree;
    fill_data<D>(tree);
    T copy;
    copy.insert(D::key(99),D::value(1));
    copy = tree;
    assert_copy(tree,copy);
}

// MATRIX
void test_matrix_real_move() {
    using D = RealData;
    using T = D::Tree;
    T tree;
    fill_data<D>(tree);
    auto root = tree.get_root();
    T moved(std::move(tree));
    assert(tree.empty() && moved.get_root() == root);
    assert_data<D>(moved);
    tree.insert(D::key(1),D::value(1));
    assert(check_tree(tree));
}

// MATRIX
void test_matrix_real_move_assign() {
    using D = RealData;
    using T = D::Tree;
    T tree;
    fill_data<D>(tree);
    T moved;
    moved.insert(D::key(99),D::value(1));
    moved = std::move(tree);
    assert(tree.empty());
    assert_data<D>(moved);
}

// MATRIX
void test_matrix_real_copy_independent() {
    using D = RealData;
    using T = D::Tree;
    T tree;
    fill_data<D>(tree);
    T copy(tree);
    copy.find(D::key(1))->value = D::transform(D::value(1));
    bool erased = copy.erase(D::key(2));
    assert(erased);
    assert_data<D>(tree);
}

// MATRIX
void test_matrix_real_map() {
    using D = RealData;
    using T = D::Tree;
    T tree;
    fill_data<D>(tree);
    T result = tree.map(D::transform);
    for (int i=1;i<=3;++i) assert(result.find(D::key(i))->value == D::transform(D::value(i)) && result.find(D::key(i)) != tree.find(D::key(i)));
    assert(check_tree(result));
    assert_data<D>(tree);
}

// MATRIX
void test_matrix_real_where() {
    using D = RealData;
    using T = D::Tree;
    T tree;
    fill_data<D>(tree);
    T result = tree.where(D::keep);
    assert(node_count(result) == 2 && !result.find(D::key(1)) && check_tree(result));
    assert(result.find(D::key(2))->value == D::value(2) && result.find(D::key(3))->value == D::value(3));
    assert_data<D>(tree);
}

// MATRIX
void test_matrix_real_reduce() {
    using D = RealData;
    using T = D::Tree;
    T tree;
    fill_data<D>(tree);
    D::Result result = tree.reduce(D::Result(),D::fold);
    assert_result(result,D::expected());
    assert_data<D>(tree);
}

// MATRIX
void test_matrix_real_merge() {
    using D = RealData;
    using T = D::Tree;
    T first,second;
    fill_data<D>(first);
    second.insert(D::key(2),D::transform(D::value(2)));
    second.insert(D::key(4),D::value(4));
    T result = first.merge(second);
    assert(node_count(result) == 4 && check_tree(result));
    assert(result.find(D::key(2))->value == D::value(2) && result.find(D::key(4))->value == D::value(4));
    assert_data<D>(first);
    assert(second.find(D::key(2))->value == D::transform(D::value(2)));
}

// MATRIX
void test_matrix_real_extract_root() {
    using D = RealData;
    using T = D::Tree;
    T tree;
    fill_data<D>(tree);
    T result = tree.extract_subtree(D::key(2));
    assert_copy(tree,result);
}

// MATRIX
void test_matrix_real_extract_child() {
    using D = RealData;
    using T = D::Tree;
    T tree;
    fill_data<D>(tree);
    T result = tree.extract_subtree(D::key(1));
    assert(node_count(result) == 1 && result.get_root()->color && tree.contains_subtree(result));
    assert(result.find(D::key(1)) != tree.find(D::key(1)));
    assert_data<D>(tree);
}

// MATRIX
void test_matrix_real_contains_mismatch() {
    using D = RealData;
    using T = D::Tree;
    T tree;
    fill_data<D>(tree);
    T result = tree.extract_subtree(D::key(1));
    result.get_root()->value = D::transform(D::value(1));
    assert(!tree.contains_subtree(result));
}

// MATRIX
void test_matrix_real_absolute_path() {
    using D = RealData;
    using T = D::Tree;
    T tree;
    fill_data<D>(tree);
    assert(tree.find_by_path("L") == tree.find(D::key(1)));
    assert(tree.find_by_path("R") == tree.find(D::key(3)));
    assert(tree.find_by_path("LL") == nullptr);
}

// MATRIX
void test_matrix_real_relative_path() {
    using D = RealData;
    using T = D::Tree;
    T tree;
    fill_data<D>(tree);
    assert(tree.find_by_path(D::key(1),"UR") == tree.find(D::key(3)));
    assert(tree.find_by_path(D::key(1),"UU") == nullptr);
}

// MATRIX
void test_matrix_real_traverse_nlr() {
    using D = RealData;
    using T = D::Tree;
    T tree;
    fill_data<D>(tree);
    using K = decltype(D::key(1));
    using V = decltype(D::value(1));
    std::vector<K> keys;
    std::vector<V> values;
    tree.traverse(T::TraversalOrder::NLR,TypedCollect<K,V>{keys,values});
    const int expected[] = {2,1,3};
    assert(keys.size() == 3 && values.size() == 3);
    for (int j=0;j<3;++j) assert(keys[j] == D::key(expected[j]) && values[j] == D::value(expected[j]));
}

// LOAD-016
void test_load_roundtrip_types_real_nlr() {
    using D = RealData;
    using T = D::Tree;
    T tree;
    fill_data<D>(tree);
    T restored;
    std::string text = tree.to_string(T::TraversalOrder::NLR);
    bool success = restored.from_string(text);
    assert(success);
    assert_copy(tree,restored);
    assert(restored.to_string(T::TraversalOrder::NLR) == text);
}

// MATRIX
void test_matrix_real_traverse_nrl() {
    using D = RealData;
    using T = D::Tree;
    T tree;
    fill_data<D>(tree);
    using K = decltype(D::key(1));
    using V = decltype(D::value(1));
    std::vector<K> keys;
    std::vector<V> values;
    tree.traverse(T::TraversalOrder::NRL,TypedCollect<K,V>{keys,values});
    const int expected[] = {2,3,1};
    assert(keys.size() == 3 && values.size() == 3);
    for (int j=0;j<3;++j) assert(keys[j] == D::key(expected[j]) && values[j] == D::value(expected[j]));
}

// LOAD-016
void test_load_roundtrip_types_real_nrl() {
    using D = RealData;
    using T = D::Tree;
    T tree;
    fill_data<D>(tree);
    T restored;
    std::string text = tree.to_string(T::TraversalOrder::NRL);
    bool success = restored.from_string(text);
    assert(success);
    assert_copy(tree,restored);
    assert(restored.to_string(T::TraversalOrder::NRL) == text);
}

// MATRIX
void test_matrix_real_traverse_lnr() {
    using D = RealData;
    using T = D::Tree;
    T tree;
    fill_data<D>(tree);
    using K = decltype(D::key(1));
    using V = decltype(D::value(1));
    std::vector<K> keys;
    std::vector<V> values;
    tree.traverse(T::TraversalOrder::LNR,TypedCollect<K,V>{keys,values});
    const int expected[] = {1,2,3};
    assert(keys.size() == 3 && values.size() == 3);
    for (int j=0;j<3;++j) assert(keys[j] == D::key(expected[j]) && values[j] == D::value(expected[j]));
}

// LOAD-016
void test_load_roundtrip_types_real_lnr() {
    using D = RealData;
    using T = D::Tree;
    T tree;
    fill_data<D>(tree);
    T restored;
    std::string text = tree.to_string(T::TraversalOrder::LNR);
    bool success = restored.from_string(text);
    assert(success);
    assert_copy(tree,restored);
    assert(restored.to_string(T::TraversalOrder::LNR) == text);
}

// MATRIX
void test_matrix_real_traverse_rnl() {
    using D = RealData;
    using T = D::Tree;
    T tree;
    fill_data<D>(tree);
    using K = decltype(D::key(1));
    using V = decltype(D::value(1));
    std::vector<K> keys;
    std::vector<V> values;
    tree.traverse(T::TraversalOrder::RNL,TypedCollect<K,V>{keys,values});
    const int expected[] = {3,2,1};
    assert(keys.size() == 3 && values.size() == 3);
    for (int j=0;j<3;++j) assert(keys[j] == D::key(expected[j]) && values[j] == D::value(expected[j]));
}

// LOAD-016
void test_load_roundtrip_types_real_rnl() {
    using D = RealData;
    using T = D::Tree;
    T tree;
    fill_data<D>(tree);
    T restored;
    std::string text = tree.to_string(T::TraversalOrder::RNL);
    bool success = restored.from_string(text);
    assert(success);
    assert_copy(tree,restored);
    assert(restored.to_string(T::TraversalOrder::RNL) == text);
}

// MATRIX
void test_matrix_real_traverse_lrn() {
    using D = RealData;
    using T = D::Tree;
    T tree;
    fill_data<D>(tree);
    using K = decltype(D::key(1));
    using V = decltype(D::value(1));
    std::vector<K> keys;
    std::vector<V> values;
    tree.traverse(T::TraversalOrder::LRN,TypedCollect<K,V>{keys,values});
    const int expected[] = {1,3,2};
    assert(keys.size() == 3 && values.size() == 3);
    for (int j=0;j<3;++j) assert(keys[j] == D::key(expected[j]) && values[j] == D::value(expected[j]));
}

// LOAD-016
void test_load_roundtrip_types_real_lrn() {
    using D = RealData;
    using T = D::Tree;
    T tree;
    fill_data<D>(tree);
    T restored;
    std::string text = tree.to_string(T::TraversalOrder::LRN);
    bool success = restored.from_string(text);
    assert(success);
    assert_copy(tree,restored);
    assert(restored.to_string(T::TraversalOrder::LRN) == text);
}

// MATRIX
void test_matrix_real_traverse_rln() {
    using D = RealData;
    using T = D::Tree;
    T tree;
    fill_data<D>(tree);
    using K = decltype(D::key(1));
    using V = decltype(D::value(1));
    std::vector<K> keys;
    std::vector<V> values;
    tree.traverse(T::TraversalOrder::RLN,TypedCollect<K,V>{keys,values});
    const int expected[] = {3,1,2};
    assert(keys.size() == 3 && values.size() == 3);
    for (int j=0;j<3;++j) assert(keys[j] == D::key(expected[j]) && values[j] == D::value(expected[j]));
}

// LOAD-016
void test_load_roundtrip_types_real_rln() {
    using D = RealData;
    using T = D::Tree;
    T tree;
    fill_data<D>(tree);
    T restored;
    std::string text = tree.to_string(T::TraversalOrder::RLN);
    bool success = restored.from_string(text);
    assert(success);
    assert_copy(tree,restored);
    assert(restored.to_string(T::TraversalOrder::RLN) == text);
}

// MATRIX
void test_matrix_complex_empty() {
    using D = ComplexData;
    using T = D::Tree;
    T tree;
    assert(tree.empty() && check_tree(tree));
}

// MATRIX
void test_matrix_complex_single() {
    using D = ComplexData;
    using T = D::Tree;
    T tree;
    tree.insert(D::key(1),D::value(1));
    assert(tree.find(D::key(1))->value == D::value(1) && node_count(tree) == 1 && check_tree(tree));
}

// MATRIX
void test_matrix_complex_find() {
    using D = ComplexData;
    using T = D::Tree;
    T tree;
    fill_data<D>(tree);
    assert_data<D>(tree);
}

// MATRIX
void test_matrix_complex_missing() {
    using D = ComplexData;
    using T = D::Tree;
    T tree;
    fill_data<D>(tree);
    assert(!tree.find(D::key(99)));
}

// MATRIX
void test_matrix_complex_duplicate() {
    using D = ComplexData;
    using T = D::Tree;
    T tree;
    fill_data<D>(tree);
    tree.insert(D::key(1),D::transform(D::value(1)));
    assert_data<D>(tree);
}

// MATRIX
void test_matrix_complex_erase() {
    using D = ComplexData;
    using T = D::Tree;
    T tree;
    fill_data<D>(tree);
    bool erased = tree.erase(D::key(2));
    assert(erased && !tree.find(D::key(2)) && node_count(tree) == 2 && check_tree(tree));
    assert(tree.find(D::key(1))->value == D::value(1) && tree.find(D::key(3))->value == D::value(3));
}

// MATRIX
void test_matrix_complex_copy() {
    using D = ComplexData;
    using T = D::Tree;
    T tree;
    fill_data<D>(tree);
    T copy(tree);
    assert_copy(tree,copy);
}

// MATRIX
void test_matrix_complex_copy_assign() {
    using D = ComplexData;
    using T = D::Tree;
    T tree;
    fill_data<D>(tree);
    T copy;
    copy.insert(D::key(99),D::value(1));
    copy = tree;
    assert_copy(tree,copy);
}

// MATRIX
void test_matrix_complex_move() {
    using D = ComplexData;
    using T = D::Tree;
    T tree;
    fill_data<D>(tree);
    auto root = tree.get_root();
    T moved(std::move(tree));
    assert(tree.empty() && moved.get_root() == root);
    assert_data<D>(moved);
    tree.insert(D::key(1),D::value(1));
    assert(check_tree(tree));
}

// MATRIX
void test_matrix_complex_move_assign() {
    using D = ComplexData;
    using T = D::Tree;
    T tree;
    fill_data<D>(tree);
    T moved;
    moved.insert(D::key(99),D::value(1));
    moved = std::move(tree);
    assert(tree.empty());
    assert_data<D>(moved);
}

// MATRIX
void test_matrix_complex_copy_independent() {
    using D = ComplexData;
    using T = D::Tree;
    T tree;
    fill_data<D>(tree);
    T copy(tree);
    copy.find(D::key(1))->value = D::transform(D::value(1));
    bool erased = copy.erase(D::key(2));
    assert(erased);
    assert_data<D>(tree);
}

// MATRIX
void test_matrix_complex_map() {
    using D = ComplexData;
    using T = D::Tree;
    T tree;
    fill_data<D>(tree);
    T result = tree.map(D::transform);
    for (int i=1;i<=3;++i) assert(result.find(D::key(i))->value == D::transform(D::value(i)) && result.find(D::key(i)) != tree.find(D::key(i)));
    assert(check_tree(result));
    assert_data<D>(tree);
}

// MATRIX
void test_matrix_complex_where() {
    using D = ComplexData;
    using T = D::Tree;
    T tree;
    fill_data<D>(tree);
    T result = tree.where(D::keep);
    assert(node_count(result) == 2 && !result.find(D::key(1)) && check_tree(result));
    assert(result.find(D::key(2))->value == D::value(2) && result.find(D::key(3))->value == D::value(3));
    assert_data<D>(tree);
}

// MATRIX
void test_matrix_complex_reduce() {
    using D = ComplexData;
    using T = D::Tree;
    T tree;
    fill_data<D>(tree);
    D::Result result = tree.reduce(D::Result(),D::fold);
    assert_result(result,D::expected());
    assert_data<D>(tree);
}

// MATRIX
void test_matrix_complex_merge() {
    using D = ComplexData;
    using T = D::Tree;
    T first,second;
    fill_data<D>(first);
    second.insert(D::key(2),D::transform(D::value(2)));
    second.insert(D::key(4),D::value(4));
    T result = first.merge(second);
    assert(node_count(result) == 4 && check_tree(result));
    assert(result.find(D::key(2))->value == D::value(2) && result.find(D::key(4))->value == D::value(4));
    assert_data<D>(first);
    assert(second.find(D::key(2))->value == D::transform(D::value(2)));
}

// MATRIX
void test_matrix_complex_extract_root() {
    using D = ComplexData;
    using T = D::Tree;
    T tree;
    fill_data<D>(tree);
    T result = tree.extract_subtree(D::key(2));
    assert_copy(tree,result);
}

// MATRIX
void test_matrix_complex_extract_child() {
    using D = ComplexData;
    using T = D::Tree;
    T tree;
    fill_data<D>(tree);
    T result = tree.extract_subtree(D::key(1));
    assert(node_count(result) == 1 && result.get_root()->color && tree.contains_subtree(result));
    assert(result.find(D::key(1)) != tree.find(D::key(1)));
    assert_data<D>(tree);
}

// MATRIX
void test_matrix_complex_contains_mismatch() {
    using D = ComplexData;
    using T = D::Tree;
    T tree;
    fill_data<D>(tree);
    T result = tree.extract_subtree(D::key(1));
    result.get_root()->value = D::transform(D::value(1));
    assert(!tree.contains_subtree(result));
}

// MATRIX
void test_matrix_complex_absolute_path() {
    using D = ComplexData;
    using T = D::Tree;
    T tree;
    fill_data<D>(tree);
    assert(tree.find_by_path("L") == tree.find(D::key(1)));
    assert(tree.find_by_path("R") == tree.find(D::key(3)));
    assert(tree.find_by_path("LL") == nullptr);
}

// MATRIX
void test_matrix_complex_relative_path() {
    using D = ComplexData;
    using T = D::Tree;
    T tree;
    fill_data<D>(tree);
    assert(tree.find_by_path(D::key(1),"UR") == tree.find(D::key(3)));
    assert(tree.find_by_path(D::key(1),"UU") == nullptr);
}

// MATRIX
void test_matrix_complex_traverse_nlr() {
    using D = ComplexData;
    using T = D::Tree;
    T tree;
    fill_data<D>(tree);
    using K = decltype(D::key(1));
    using V = decltype(D::value(1));
    std::vector<K> keys;
    std::vector<V> values;
    tree.traverse(T::TraversalOrder::NLR,TypedCollect<K,V>{keys,values});
    const int expected[] = {2,1,3};
    assert(keys.size() == 3 && values.size() == 3);
    for (int j=0;j<3;++j) assert(keys[j] == D::key(expected[j]) && values[j] == D::value(expected[j]));
}

// LOAD-016
void test_load_roundtrip_types_complex_nlr() {
    using D = ComplexData;
    using T = D::Tree;
    T tree;
    fill_data<D>(tree);
    T restored;
    std::string text = tree.to_string(T::TraversalOrder::NLR);
    bool success = restored.from_string(text);
    assert(success);
    assert_copy(tree,restored);
    assert(restored.to_string(T::TraversalOrder::NLR) == text);
}

// MATRIX
void test_matrix_complex_traverse_nrl() {
    using D = ComplexData;
    using T = D::Tree;
    T tree;
    fill_data<D>(tree);
    using K = decltype(D::key(1));
    using V = decltype(D::value(1));
    std::vector<K> keys;
    std::vector<V> values;
    tree.traverse(T::TraversalOrder::NRL,TypedCollect<K,V>{keys,values});
    const int expected[] = {2,3,1};
    assert(keys.size() == 3 && values.size() == 3);
    for (int j=0;j<3;++j) assert(keys[j] == D::key(expected[j]) && values[j] == D::value(expected[j]));
}

// LOAD-016
void test_load_roundtrip_types_complex_nrl() {
    using D = ComplexData;
    using T = D::Tree;
    T tree;
    fill_data<D>(tree);
    T restored;
    std::string text = tree.to_string(T::TraversalOrder::NRL);
    bool success = restored.from_string(text);
    assert(success);
    assert_copy(tree,restored);
    assert(restored.to_string(T::TraversalOrder::NRL) == text);
}

// MATRIX
void test_matrix_complex_traverse_lnr() {
    using D = ComplexData;
    using T = D::Tree;
    T tree;
    fill_data<D>(tree);
    using K = decltype(D::key(1));
    using V = decltype(D::value(1));
    std::vector<K> keys;
    std::vector<V> values;
    tree.traverse(T::TraversalOrder::LNR,TypedCollect<K,V>{keys,values});
    const int expected[] = {1,2,3};
    assert(keys.size() == 3 && values.size() == 3);
    for (int j=0;j<3;++j) assert(keys[j] == D::key(expected[j]) && values[j] == D::value(expected[j]));
}

// LOAD-016
void test_load_roundtrip_types_complex_lnr() {
    using D = ComplexData;
    using T = D::Tree;
    T tree;
    fill_data<D>(tree);
    T restored;
    std::string text = tree.to_string(T::TraversalOrder::LNR);
    bool success = restored.from_string(text);
    assert(success);
    assert_copy(tree,restored);
    assert(restored.to_string(T::TraversalOrder::LNR) == text);
}

// MATRIX
void test_matrix_complex_traverse_rnl() {
    using D = ComplexData;
    using T = D::Tree;
    T tree;
    fill_data<D>(tree);
    using K = decltype(D::key(1));
    using V = decltype(D::value(1));
    std::vector<K> keys;
    std::vector<V> values;
    tree.traverse(T::TraversalOrder::RNL,TypedCollect<K,V>{keys,values});
    const int expected[] = {3,2,1};
    assert(keys.size() == 3 && values.size() == 3);
    for (int j=0;j<3;++j) assert(keys[j] == D::key(expected[j]) && values[j] == D::value(expected[j]));
}

// LOAD-016
void test_load_roundtrip_types_complex_rnl() {
    using D = ComplexData;
    using T = D::Tree;
    T tree;
    fill_data<D>(tree);
    T restored;
    std::string text = tree.to_string(T::TraversalOrder::RNL);
    bool success = restored.from_string(text);
    assert(success);
    assert_copy(tree,restored);
    assert(restored.to_string(T::TraversalOrder::RNL) == text);
}

// MATRIX
void test_matrix_complex_traverse_lrn() {
    using D = ComplexData;
    using T = D::Tree;
    T tree;
    fill_data<D>(tree);
    using K = decltype(D::key(1));
    using V = decltype(D::value(1));
    std::vector<K> keys;
    std::vector<V> values;
    tree.traverse(T::TraversalOrder::LRN,TypedCollect<K,V>{keys,values});
    const int expected[] = {1,3,2};
    assert(keys.size() == 3 && values.size() == 3);
    for (int j=0;j<3;++j) assert(keys[j] == D::key(expected[j]) && values[j] == D::value(expected[j]));
}

// LOAD-016
void test_load_roundtrip_types_complex_lrn() {
    using D = ComplexData;
    using T = D::Tree;
    T tree;
    fill_data<D>(tree);
    T restored;
    std::string text = tree.to_string(T::TraversalOrder::LRN);
    bool success = restored.from_string(text);
    assert(success);
    assert_copy(tree,restored);
    assert(restored.to_string(T::TraversalOrder::LRN) == text);
}

// MATRIX
void test_matrix_complex_traverse_rln() {
    using D = ComplexData;
    using T = D::Tree;
    T tree;
    fill_data<D>(tree);
    using K = decltype(D::key(1));
    using V = decltype(D::value(1));
    std::vector<K> keys;
    std::vector<V> values;
    tree.traverse(T::TraversalOrder::RLN,TypedCollect<K,V>{keys,values});
    const int expected[] = {3,1,2};
    assert(keys.size() == 3 && values.size() == 3);
    for (int j=0;j<3;++j) assert(keys[j] == D::key(expected[j]) && values[j] == D::value(expected[j]));
}

// LOAD-016
void test_load_roundtrip_types_complex_rln() {
    using D = ComplexData;
    using T = D::Tree;
    T tree;
    fill_data<D>(tree);
    T restored;
    std::string text = tree.to_string(T::TraversalOrder::RLN);
    bool success = restored.from_string(text);
    assert(success);
    assert_copy(tree,restored);
    assert(restored.to_string(T::TraversalOrder::RLN) == text);
}

// MATRIX
void test_matrix_string_empty() {
    using D = StringData;
    using T = D::Tree;
    T tree;
    assert(tree.empty() && check_tree(tree));
}

// MATRIX
void test_matrix_string_single() {
    using D = StringData;
    using T = D::Tree;
    T tree;
    tree.insert(D::key(1),D::value(1));
    assert(tree.find(D::key(1))->value == D::value(1) && node_count(tree) == 1 && check_tree(tree));
}

// MATRIX
void test_matrix_string_find() {
    using D = StringData;
    using T = D::Tree;
    T tree;
    fill_data<D>(tree);
    assert_data<D>(tree);
}

// MATRIX
void test_matrix_string_missing() {
    using D = StringData;
    using T = D::Tree;
    T tree;
    fill_data<D>(tree);
    assert(!tree.find(D::key(99)));
}

// MATRIX
void test_matrix_string_duplicate() {
    using D = StringData;
    using T = D::Tree;
    T tree;
    fill_data<D>(tree);
    tree.insert(D::key(1),D::transform(D::value(1)));
    assert_data<D>(tree);
}

// MATRIX
void test_matrix_string_erase() {
    using D = StringData;
    using T = D::Tree;
    T tree;
    fill_data<D>(tree);
    bool erased = tree.erase(D::key(2));
    assert(erased && !tree.find(D::key(2)) && node_count(tree) == 2 && check_tree(tree));
    assert(tree.find(D::key(1))->value == D::value(1) && tree.find(D::key(3))->value == D::value(3));
}

// MATRIX
void test_matrix_string_copy() {
    using D = StringData;
    using T = D::Tree;
    T tree;
    fill_data<D>(tree);
    T copy(tree);
    assert_copy(tree,copy);
}

// MATRIX
void test_matrix_string_copy_assign() {
    using D = StringData;
    using T = D::Tree;
    T tree;
    fill_data<D>(tree);
    T copy;
    copy.insert(D::key(99),D::value(1));
    copy = tree;
    assert_copy(tree,copy);
}

// MATRIX
void test_matrix_string_move() {
    using D = StringData;
    using T = D::Tree;
    T tree;
    fill_data<D>(tree);
    auto root = tree.get_root();
    T moved(std::move(tree));
    assert(tree.empty() && moved.get_root() == root);
    assert_data<D>(moved);
    tree.insert(D::key(1),D::value(1));
    assert(check_tree(tree));
}

// MATRIX
void test_matrix_string_move_assign() {
    using D = StringData;
    using T = D::Tree;
    T tree;
    fill_data<D>(tree);
    T moved;
    moved.insert(D::key(99),D::value(1));
    moved = std::move(tree);
    assert(tree.empty());
    assert_data<D>(moved);
}

// MATRIX
void test_matrix_string_copy_independent() {
    using D = StringData;
    using T = D::Tree;
    T tree;
    fill_data<D>(tree);
    T copy(tree);
    copy.find(D::key(1))->value = D::transform(D::value(1));
    bool erased = copy.erase(D::key(2));
    assert(erased);
    assert_data<D>(tree);
}

// MATRIX
void test_matrix_string_map() {
    using D = StringData;
    using T = D::Tree;
    T tree;
    fill_data<D>(tree);
    T result = tree.map(D::transform);
    for (int i=1;i<=3;++i) assert(result.find(D::key(i))->value == D::transform(D::value(i)) && result.find(D::key(i)) != tree.find(D::key(i)));
    assert(check_tree(result));
    assert_data<D>(tree);
}

// MATRIX
void test_matrix_string_where() {
    using D = StringData;
    using T = D::Tree;
    T tree;
    fill_data<D>(tree);
    T result = tree.where(D::keep);
    assert(node_count(result) == 2 && !result.find(D::key(1)) && check_tree(result));
    assert(result.find(D::key(2))->value == D::value(2) && result.find(D::key(3))->value == D::value(3));
    assert_data<D>(tree);
}

// MATRIX
void test_matrix_string_reduce() {
    using D = StringData;
    using T = D::Tree;
    T tree;
    fill_data<D>(tree);
    D::Result result = tree.reduce(D::Result(),D::fold);
    assert_result(result,D::expected());
    assert_data<D>(tree);
}

// MATRIX
void test_matrix_string_merge() {
    using D = StringData;
    using T = D::Tree;
    T first,second;
    fill_data<D>(first);
    second.insert(D::key(2),D::transform(D::value(2)));
    second.insert(D::key(4),D::value(4));
    T result = first.merge(second);
    assert(node_count(result) == 4 && check_tree(result));
    assert(result.find(D::key(2))->value == D::value(2) && result.find(D::key(4))->value == D::value(4));
    assert_data<D>(first);
    assert(second.find(D::key(2))->value == D::transform(D::value(2)));
}

// MATRIX
void test_matrix_string_extract_root() {
    using D = StringData;
    using T = D::Tree;
    T tree;
    fill_data<D>(tree);
    T result = tree.extract_subtree(D::key(2));
    assert_copy(tree,result);
}

// MATRIX
void test_matrix_string_extract_child() {
    using D = StringData;
    using T = D::Tree;
    T tree;
    fill_data<D>(tree);
    T result = tree.extract_subtree(D::key(1));
    assert(node_count(result) == 1 && result.get_root()->color && tree.contains_subtree(result));
    assert(result.find(D::key(1)) != tree.find(D::key(1)));
    assert_data<D>(tree);
}

// MATRIX
void test_matrix_string_contains_mismatch() {
    using D = StringData;
    using T = D::Tree;
    T tree;
    fill_data<D>(tree);
    T result = tree.extract_subtree(D::key(1));
    result.get_root()->value = D::transform(D::value(1));
    assert(!tree.contains_subtree(result));
}

// MATRIX
void test_matrix_string_absolute_path() {
    using D = StringData;
    using T = D::Tree;
    T tree;
    fill_data<D>(tree);
    assert(tree.find_by_path("L") == tree.find(D::key(1)));
    assert(tree.find_by_path("R") == tree.find(D::key(3)));
    assert(tree.find_by_path("LL") == nullptr);
}

// MATRIX
void test_matrix_string_relative_path() {
    using D = StringData;
    using T = D::Tree;
    T tree;
    fill_data<D>(tree);
    assert(tree.find_by_path(D::key(1),"UR") == tree.find(D::key(3)));
    assert(tree.find_by_path(D::key(1),"UU") == nullptr);
}

// MATRIX
void test_matrix_string_traverse_nlr() {
    using D = StringData;
    using T = D::Tree;
    T tree;
    fill_data<D>(tree);
    using K = decltype(D::key(1));
    using V = decltype(D::value(1));
    std::vector<K> keys;
    std::vector<V> values;
    tree.traverse(T::TraversalOrder::NLR,TypedCollect<K,V>{keys,values});
    const int expected[] = {2,1,3};
    assert(keys.size() == 3 && values.size() == 3);
    for (int j=0;j<3;++j) assert(keys[j] == D::key(expected[j]) && values[j] == D::value(expected[j]));
}

// LOAD-016
void test_load_roundtrip_types_string_nlr() {
    using D = StringData;
    using T = D::Tree;
    T tree;
    fill_data<D>(tree);
    T restored;
    std::string text = tree.to_string(T::TraversalOrder::NLR);
    bool success = restored.from_string(text);
    assert(success);
    assert_copy(tree,restored);
    assert(restored.to_string(T::TraversalOrder::NLR) == text);
}

// MATRIX
void test_matrix_string_traverse_nrl() {
    using D = StringData;
    using T = D::Tree;
    T tree;
    fill_data<D>(tree);
    using K = decltype(D::key(1));
    using V = decltype(D::value(1));
    std::vector<K> keys;
    std::vector<V> values;
    tree.traverse(T::TraversalOrder::NRL,TypedCollect<K,V>{keys,values});
    const int expected[] = {2,3,1};
    assert(keys.size() == 3 && values.size() == 3);
    for (int j=0;j<3;++j) assert(keys[j] == D::key(expected[j]) && values[j] == D::value(expected[j]));
}

// LOAD-016
void test_load_roundtrip_types_string_nrl() {
    using D = StringData;
    using T = D::Tree;
    T tree;
    fill_data<D>(tree);
    T restored;
    std::string text = tree.to_string(T::TraversalOrder::NRL);
    bool success = restored.from_string(text);
    assert(success);
    assert_copy(tree,restored);
    assert(restored.to_string(T::TraversalOrder::NRL) == text);
}

// MATRIX
void test_matrix_string_traverse_lnr() {
    using D = StringData;
    using T = D::Tree;
    T tree;
    fill_data<D>(tree);
    using K = decltype(D::key(1));
    using V = decltype(D::value(1));
    std::vector<K> keys;
    std::vector<V> values;
    tree.traverse(T::TraversalOrder::LNR,TypedCollect<K,V>{keys,values});
    const int expected[] = {1,2,3};
    assert(keys.size() == 3 && values.size() == 3);
    for (int j=0;j<3;++j) assert(keys[j] == D::key(expected[j]) && values[j] == D::value(expected[j]));
}

// LOAD-016
void test_load_roundtrip_types_string_lnr() {
    using D = StringData;
    using T = D::Tree;
    T tree;
    fill_data<D>(tree);
    T restored;
    std::string text = tree.to_string(T::TraversalOrder::LNR);
    bool success = restored.from_string(text);
    assert(success);
    assert_copy(tree,restored);
    assert(restored.to_string(T::TraversalOrder::LNR) == text);
}

// MATRIX
void test_matrix_string_traverse_rnl() {
    using D = StringData;
    using T = D::Tree;
    T tree;
    fill_data<D>(tree);
    using K = decltype(D::key(1));
    using V = decltype(D::value(1));
    std::vector<K> keys;
    std::vector<V> values;
    tree.traverse(T::TraversalOrder::RNL,TypedCollect<K,V>{keys,values});
    const int expected[] = {3,2,1};
    assert(keys.size() == 3 && values.size() == 3);
    for (int j=0;j<3;++j) assert(keys[j] == D::key(expected[j]) && values[j] == D::value(expected[j]));
}

// LOAD-016
void test_load_roundtrip_types_string_rnl() {
    using D = StringData;
    using T = D::Tree;
    T tree;
    fill_data<D>(tree);
    T restored;
    std::string text = tree.to_string(T::TraversalOrder::RNL);
    bool success = restored.from_string(text);
    assert(success);
    assert_copy(tree,restored);
    assert(restored.to_string(T::TraversalOrder::RNL) == text);
}

// MATRIX
void test_matrix_string_traverse_lrn() {
    using D = StringData;
    using T = D::Tree;
    T tree;
    fill_data<D>(tree);
    using K = decltype(D::key(1));
    using V = decltype(D::value(1));
    std::vector<K> keys;
    std::vector<V> values;
    tree.traverse(T::TraversalOrder::LRN,TypedCollect<K,V>{keys,values});
    const int expected[] = {1,3,2};
    assert(keys.size() == 3 && values.size() == 3);
    for (int j=0;j<3;++j) assert(keys[j] == D::key(expected[j]) && values[j] == D::value(expected[j]));
}

// LOAD-016
void test_load_roundtrip_types_string_lrn() {
    using D = StringData;
    using T = D::Tree;
    T tree;
    fill_data<D>(tree);
    T restored;
    std::string text = tree.to_string(T::TraversalOrder::LRN);
    bool success = restored.from_string(text);
    assert(success);
    assert_copy(tree,restored);
    assert(restored.to_string(T::TraversalOrder::LRN) == text);
}

// MATRIX
void test_matrix_string_traverse_rln() {
    using D = StringData;
    using T = D::Tree;
    T tree;
    fill_data<D>(tree);
    using K = decltype(D::key(1));
    using V = decltype(D::value(1));
    std::vector<K> keys;
    std::vector<V> values;
    tree.traverse(T::TraversalOrder::RLN,TypedCollect<K,V>{keys,values});
    const int expected[] = {3,1,2};
    assert(keys.size() == 3 && values.size() == 3);
    for (int j=0;j<3;++j) assert(keys[j] == D::key(expected[j]) && values[j] == D::value(expected[j]));
}

// LOAD-016
void test_load_roundtrip_types_string_rln() {
    using D = StringData;
    using T = D::Tree;
    T tree;
    fill_data<D>(tree);
    T restored;
    std::string text = tree.to_string(T::TraversalOrder::RLN);
    bool success = restored.from_string(text);
    assert(success);
    assert_copy(tree,restored);
    assert(restored.to_string(T::TraversalOrder::RLN) == text);
}

// MATRIX
void test_matrix_function_empty() {
    using D = FunctionData;
    using T = D::Tree;
    T tree;
    assert(tree.empty() && check_tree(tree));
}

// MATRIX
void test_matrix_function_single() {
    using D = FunctionData;
    using T = D::Tree;
    T tree;
    tree.insert(D::key(1),D::value(1));
    assert(tree.find(D::key(1))->value == D::value(1) && node_count(tree) == 1 && check_tree(tree));
}

// MATRIX
void test_matrix_function_find() {
    using D = FunctionData;
    using T = D::Tree;
    T tree;
    fill_data<D>(tree);
    assert_data<D>(tree);
}

// MATRIX
void test_matrix_function_missing() {
    using D = FunctionData;
    using T = D::Tree;
    T tree;
    fill_data<D>(tree);
    assert(!tree.find(D::key(99)));
}

// MATRIX
void test_matrix_function_duplicate() {
    using D = FunctionData;
    using T = D::Tree;
    T tree;
    fill_data<D>(tree);
    tree.insert(D::key(1),D::transform(D::value(1)));
    assert_data<D>(tree);
}

// MATRIX
void test_matrix_function_erase() {
    using D = FunctionData;
    using T = D::Tree;
    T tree;
    fill_data<D>(tree);
    bool erased = tree.erase(D::key(2));
    assert(erased && !tree.find(D::key(2)) && node_count(tree) == 2 && check_tree(tree));
    assert(tree.find(D::key(1))->value == D::value(1) && tree.find(D::key(3))->value == D::value(3));
}

// MATRIX
void test_matrix_function_copy() {
    using D = FunctionData;
    using T = D::Tree;
    T tree;
    fill_data<D>(tree);
    T copy(tree);
    assert_copy(tree,copy);
}

// MATRIX
void test_matrix_function_copy_assign() {
    using D = FunctionData;
    using T = D::Tree;
    T tree;
    fill_data<D>(tree);
    T copy;
    copy.insert(D::key(99),D::value(1));
    copy = tree;
    assert_copy(tree,copy);
}

// MATRIX
void test_matrix_function_move() {
    using D = FunctionData;
    using T = D::Tree;
    T tree;
    fill_data<D>(tree);
    auto root = tree.get_root();
    T moved(std::move(tree));
    assert(tree.empty() && moved.get_root() == root);
    assert_data<D>(moved);
    tree.insert(D::key(1),D::value(1));
    assert(check_tree(tree));
}

// MATRIX
void test_matrix_function_move_assign() {
    using D = FunctionData;
    using T = D::Tree;
    T tree;
    fill_data<D>(tree);
    T moved;
    moved.insert(D::key(99),D::value(1));
    moved = std::move(tree);
    assert(tree.empty());
    assert_data<D>(moved);
}

// MATRIX
void test_matrix_function_copy_independent() {
    using D = FunctionData;
    using T = D::Tree;
    T tree;
    fill_data<D>(tree);
    T copy(tree);
    copy.find(D::key(1))->value = D::transform(D::value(1));
    bool erased = copy.erase(D::key(2));
    assert(erased);
    assert_data<D>(tree);
}

// MATRIX
void test_matrix_function_map() {
    using D = FunctionData;
    using T = D::Tree;
    T tree;
    fill_data<D>(tree);
    T result = tree.map(D::transform);
    for (int i=1;i<=3;++i) assert(result.find(D::key(i))->value == D::transform(D::value(i)) && result.find(D::key(i)) != tree.find(D::key(i)));
    assert(check_tree(result));
    assert_data<D>(tree);
}

// MATRIX
void test_matrix_function_where() {
    using D = FunctionData;
    using T = D::Tree;
    T tree;
    fill_data<D>(tree);
    T result = tree.where(D::keep);
    assert(node_count(result) == 2 && !result.find(D::key(1)) && check_tree(result));
    assert(result.find(D::key(2))->value == D::value(2) && result.find(D::key(3))->value == D::value(3));
    assert_data<D>(tree);
}

// MATRIX
void test_matrix_function_reduce() {
    using D = FunctionData;
    using T = D::Tree;
    T tree;
    fill_data<D>(tree);
    D::Result result = tree.reduce(D::Result(),D::fold);
    assert_result(result,D::expected());
    assert_data<D>(tree);
}

// MATRIX
void test_matrix_function_merge() {
    using D = FunctionData;
    using T = D::Tree;
    T first,second;
    fill_data<D>(first);
    second.insert(D::key(2),D::transform(D::value(2)));
    second.insert(D::key(4),D::value(4));
    T result = first.merge(second);
    assert(node_count(result) == 4 && check_tree(result));
    assert(result.find(D::key(2))->value == D::value(2) && result.find(D::key(4))->value == D::value(4));
    assert_data<D>(first);
    assert(second.find(D::key(2))->value == D::transform(D::value(2)));
}

// MATRIX
void test_matrix_function_extract_root() {
    using D = FunctionData;
    using T = D::Tree;
    T tree;
    fill_data<D>(tree);
    T result = tree.extract_subtree(D::key(2));
    assert_copy(tree,result);
}

// MATRIX
void test_matrix_function_extract_child() {
    using D = FunctionData;
    using T = D::Tree;
    T tree;
    fill_data<D>(tree);
    T result = tree.extract_subtree(D::key(1));
    assert(node_count(result) == 1 && result.get_root()->color && tree.contains_subtree(result));
    assert(result.find(D::key(1)) != tree.find(D::key(1)));
    assert_data<D>(tree);
}

// MATRIX
void test_matrix_function_contains_mismatch() {
    using D = FunctionData;
    using T = D::Tree;
    T tree;
    fill_data<D>(tree);
    T result = tree.extract_subtree(D::key(1));
    result.get_root()->value = D::transform(D::value(1));
    assert(!tree.contains_subtree(result));
}

// MATRIX
void test_matrix_function_absolute_path() {
    using D = FunctionData;
    using T = D::Tree;
    T tree;
    fill_data<D>(tree);
    assert(tree.find_by_path("L") == tree.find(D::key(1)));
    assert(tree.find_by_path("R") == tree.find(D::key(3)));
    assert(tree.find_by_path("LL") == nullptr);
}

// MATRIX
void test_matrix_function_relative_path() {
    using D = FunctionData;
    using T = D::Tree;
    T tree;
    fill_data<D>(tree);
    assert(tree.find_by_path(D::key(1),"UR") == tree.find(D::key(3)));
    assert(tree.find_by_path(D::key(1),"UU") == nullptr);
}

// MATRIX
void test_matrix_function_traverse_nlr() {
    using D = FunctionData;
    using T = D::Tree;
    T tree;
    fill_data<D>(tree);
    using K = decltype(D::key(1));
    using V = decltype(D::value(1));
    std::vector<K> keys;
    std::vector<V> values;
    tree.traverse(T::TraversalOrder::NLR,TypedCollect<K,V>{keys,values});
    const int expected[] = {2,1,3};
    assert(keys.size() == 3 && values.size() == 3);
    for (int j=0;j<3;++j) assert(keys[j] == D::key(expected[j]) && values[j] == D::value(expected[j]));
}

// LOAD-016
void test_load_roundtrip_types_function_nlr() {
    using D = FunctionData;
    using T = D::Tree;
    T tree;
    fill_data<D>(tree);
    T restored;
    std::string text = tree.to_string(T::TraversalOrder::NLR);
    bool success = restored.from_string(text);
    assert(success);
    assert_copy(tree,restored);
    assert(restored.to_string(T::TraversalOrder::NLR) == text);
}

// MATRIX
void test_matrix_function_traverse_nrl() {
    using D = FunctionData;
    using T = D::Tree;
    T tree;
    fill_data<D>(tree);
    using K = decltype(D::key(1));
    using V = decltype(D::value(1));
    std::vector<K> keys;
    std::vector<V> values;
    tree.traverse(T::TraversalOrder::NRL,TypedCollect<K,V>{keys,values});
    const int expected[] = {2,3,1};
    assert(keys.size() == 3 && values.size() == 3);
    for (int j=0;j<3;++j) assert(keys[j] == D::key(expected[j]) && values[j] == D::value(expected[j]));
}

// LOAD-016
void test_load_roundtrip_types_function_nrl() {
    using D = FunctionData;
    using T = D::Tree;
    T tree;
    fill_data<D>(tree);
    T restored;
    std::string text = tree.to_string(T::TraversalOrder::NRL);
    bool success = restored.from_string(text);
    assert(success);
    assert_copy(tree,restored);
    assert(restored.to_string(T::TraversalOrder::NRL) == text);
}

// MATRIX
void test_matrix_function_traverse_lnr() {
    using D = FunctionData;
    using T = D::Tree;
    T tree;
    fill_data<D>(tree);
    using K = decltype(D::key(1));
    using V = decltype(D::value(1));
    std::vector<K> keys;
    std::vector<V> values;
    tree.traverse(T::TraversalOrder::LNR,TypedCollect<K,V>{keys,values});
    const int expected[] = {1,2,3};
    assert(keys.size() == 3 && values.size() == 3);
    for (int j=0;j<3;++j) assert(keys[j] == D::key(expected[j]) && values[j] == D::value(expected[j]));
}

// LOAD-016
void test_load_roundtrip_types_function_lnr() {
    using D = FunctionData;
    using T = D::Tree;
    T tree;
    fill_data<D>(tree);
    T restored;
    std::string text = tree.to_string(T::TraversalOrder::LNR);
    bool success = restored.from_string(text);
    assert(success);
    assert_copy(tree,restored);
    assert(restored.to_string(T::TraversalOrder::LNR) == text);
}

// MATRIX
void test_matrix_function_traverse_rnl() {
    using D = FunctionData;
    using T = D::Tree;
    T tree;
    fill_data<D>(tree);
    using K = decltype(D::key(1));
    using V = decltype(D::value(1));
    std::vector<K> keys;
    std::vector<V> values;
    tree.traverse(T::TraversalOrder::RNL,TypedCollect<K,V>{keys,values});
    const int expected[] = {3,2,1};
    assert(keys.size() == 3 && values.size() == 3);
    for (int j=0;j<3;++j) assert(keys[j] == D::key(expected[j]) && values[j] == D::value(expected[j]));
}

// LOAD-016
void test_load_roundtrip_types_function_rnl() {
    using D = FunctionData;
    using T = D::Tree;
    T tree;
    fill_data<D>(tree);
    T restored;
    std::string text = tree.to_string(T::TraversalOrder::RNL);
    bool success = restored.from_string(text);
    assert(success);
    assert_copy(tree,restored);
    assert(restored.to_string(T::TraversalOrder::RNL) == text);
}

// MATRIX
void test_matrix_function_traverse_lrn() {
    using D = FunctionData;
    using T = D::Tree;
    T tree;
    fill_data<D>(tree);
    using K = decltype(D::key(1));
    using V = decltype(D::value(1));
    std::vector<K> keys;
    std::vector<V> values;
    tree.traverse(T::TraversalOrder::LRN,TypedCollect<K,V>{keys,values});
    const int expected[] = {1,3,2};
    assert(keys.size() == 3 && values.size() == 3);
    for (int j=0;j<3;++j) assert(keys[j] == D::key(expected[j]) && values[j] == D::value(expected[j]));
}

// LOAD-016
void test_load_roundtrip_types_function_lrn() {
    using D = FunctionData;
    using T = D::Tree;
    T tree;
    fill_data<D>(tree);
    T restored;
    std::string text = tree.to_string(T::TraversalOrder::LRN);
    bool success = restored.from_string(text);
    assert(success);
    assert_copy(tree,restored);
    assert(restored.to_string(T::TraversalOrder::LRN) == text);
}

// MATRIX
void test_matrix_function_traverse_rln() {
    using D = FunctionData;
    using T = D::Tree;
    T tree;
    fill_data<D>(tree);
    using K = decltype(D::key(1));
    using V = decltype(D::value(1));
    std::vector<K> keys;
    std::vector<V> values;
    tree.traverse(T::TraversalOrder::RLN,TypedCollect<K,V>{keys,values});
    const int expected[] = {3,1,2};
    assert(keys.size() == 3 && values.size() == 3);
    for (int j=0;j<3;++j) assert(keys[j] == D::key(expected[j]) && values[j] == D::value(expected[j]));
}

// LOAD-016
void test_load_roundtrip_types_function_rln() {
    using D = FunctionData;
    using T = D::Tree;
    T tree;
    fill_data<D>(tree);
    T restored;
    std::string text = tree.to_string(T::TraversalOrder::RLN);
    bool success = restored.from_string(text);
    assert(success);
    assert_copy(tree,restored);
    assert(restored.to_string(T::TraversalOrder::RLN) == text);
}

// MATRIX
void test_matrix_student_empty() {
    using D = StudentData;
    using T = D::Tree;
    T tree;
    assert(tree.empty() && check_tree(tree));
}

// MATRIX
void test_matrix_student_single() {
    using D = StudentData;
    using T = D::Tree;
    T tree;
    tree.insert(D::key(1),D::value(1));
    assert(tree.find(D::key(1))->value == D::value(1) && node_count(tree) == 1 && check_tree(tree));
}

// MATRIX
void test_matrix_student_find() {
    using D = StudentData;
    using T = D::Tree;
    T tree;
    fill_data<D>(tree);
    assert_data<D>(tree);
}

// MATRIX
void test_matrix_student_missing() {
    using D = StudentData;
    using T = D::Tree;
    T tree;
    fill_data<D>(tree);
    assert(!tree.find(D::key(99)));
}

// MATRIX
void test_matrix_student_duplicate() {
    using D = StudentData;
    using T = D::Tree;
    T tree;
    fill_data<D>(tree);
    tree.insert(D::key(1),D::transform(D::value(1)));
    assert_data<D>(tree);
}

// MATRIX
void test_matrix_student_erase() {
    using D = StudentData;
    using T = D::Tree;
    T tree;
    fill_data<D>(tree);
    bool erased = tree.erase(D::key(2));
    assert(erased && !tree.find(D::key(2)) && node_count(tree) == 2 && check_tree(tree));
    assert(tree.find(D::key(1))->value == D::value(1) && tree.find(D::key(3))->value == D::value(3));
}

// MATRIX
void test_matrix_student_copy() {
    using D = StudentData;
    using T = D::Tree;
    T tree;
    fill_data<D>(tree);
    T copy(tree);
    assert_copy(tree,copy);
}

// MATRIX
void test_matrix_student_copy_assign() {
    using D = StudentData;
    using T = D::Tree;
    T tree;
    fill_data<D>(tree);
    T copy;
    copy.insert(D::key(99),D::value(1));
    copy = tree;
    assert_copy(tree,copy);
}

// MATRIX
void test_matrix_student_move() {
    using D = StudentData;
    using T = D::Tree;
    T tree;
    fill_data<D>(tree);
    auto root = tree.get_root();
    T moved(std::move(tree));
    assert(tree.empty() && moved.get_root() == root);
    assert_data<D>(moved);
    tree.insert(D::key(1),D::value(1));
    assert(check_tree(tree));
}

// MATRIX
void test_matrix_student_move_assign() {
    using D = StudentData;
    using T = D::Tree;
    T tree;
    fill_data<D>(tree);
    T moved;
    moved.insert(D::key(99),D::value(1));
    moved = std::move(tree);
    assert(tree.empty());
    assert_data<D>(moved);
}

// MATRIX
void test_matrix_student_copy_independent() {
    using D = StudentData;
    using T = D::Tree;
    T tree;
    fill_data<D>(tree);
    T copy(tree);
    copy.find(D::key(1))->value = D::transform(D::value(1));
    bool erased = copy.erase(D::key(2));
    assert(erased);
    assert_data<D>(tree);
}

// MATRIX
void test_matrix_student_map() {
    using D = StudentData;
    using T = D::Tree;
    T tree;
    fill_data<D>(tree);
    T result = tree.map(D::transform);
    for (int i=1;i<=3;++i) assert(result.find(D::key(i))->value == D::transform(D::value(i)) && result.find(D::key(i)) != tree.find(D::key(i)));
    assert(check_tree(result));
    assert_data<D>(tree);
}

// MATRIX
void test_matrix_student_where() {
    using D = StudentData;
    using T = D::Tree;
    T tree;
    fill_data<D>(tree);
    T result = tree.where(D::keep);
    assert(node_count(result) == 2 && !result.find(D::key(1)) && check_tree(result));
    assert(result.find(D::key(2))->value == D::value(2) && result.find(D::key(3))->value == D::value(3));
    assert_data<D>(tree);
}

// MATRIX
void test_matrix_student_reduce() {
    using D = StudentData;
    using T = D::Tree;
    T tree;
    fill_data<D>(tree);
    D::Result result = tree.reduce(D::Result(),D::fold);
    assert_result(result,D::expected());
    assert_data<D>(tree);
}

// MATRIX
void test_matrix_student_merge() {
    using D = StudentData;
    using T = D::Tree;
    T first,second;
    fill_data<D>(first);
    second.insert(D::key(2),D::transform(D::value(2)));
    second.insert(D::key(4),D::value(4));
    T result = first.merge(second);
    assert(node_count(result) == 4 && check_tree(result));
    assert(result.find(D::key(2))->value == D::value(2) && result.find(D::key(4))->value == D::value(4));
    assert_data<D>(first);
    assert(second.find(D::key(2))->value == D::transform(D::value(2)));
}

// MATRIX
void test_matrix_student_extract_root() {
    using D = StudentData;
    using T = D::Tree;
    T tree;
    fill_data<D>(tree);
    T result = tree.extract_subtree(D::key(2));
    assert_copy(tree,result);
}

// MATRIX
void test_matrix_student_extract_child() {
    using D = StudentData;
    using T = D::Tree;
    T tree;
    fill_data<D>(tree);
    T result = tree.extract_subtree(D::key(1));
    assert(node_count(result) == 1 && result.get_root()->color && tree.contains_subtree(result));
    assert(result.find(D::key(1)) != tree.find(D::key(1)));
    assert_data<D>(tree);
}

// MATRIX
void test_matrix_student_contains_mismatch() {
    using D = StudentData;
    using T = D::Tree;
    T tree;
    fill_data<D>(tree);
    T result = tree.extract_subtree(D::key(1));
    result.get_root()->value = D::transform(D::value(1));
    assert(!tree.contains_subtree(result));
}

// MATRIX
void test_matrix_student_absolute_path() {
    using D = StudentData;
    using T = D::Tree;
    T tree;
    fill_data<D>(tree);
    assert(tree.find_by_path("L") == tree.find(D::key(1)));
    assert(tree.find_by_path("R") == tree.find(D::key(3)));
    assert(tree.find_by_path("LL") == nullptr);
}

// MATRIX
void test_matrix_student_relative_path() {
    using D = StudentData;
    using T = D::Tree;
    T tree;
    fill_data<D>(tree);
    assert(tree.find_by_path(D::key(1),"UR") == tree.find(D::key(3)));
    assert(tree.find_by_path(D::key(1),"UU") == nullptr);
}

// MATRIX
void test_matrix_student_traverse_nlr() {
    using D = StudentData;
    using T = D::Tree;
    T tree;
    fill_data<D>(tree);
    using K = decltype(D::key(1));
    using V = decltype(D::value(1));
    std::vector<K> keys;
    std::vector<V> values;
    tree.traverse(T::TraversalOrder::NLR,TypedCollect<K,V>{keys,values});
    const int expected[] = {2,1,3};
    assert(keys.size() == 3 && values.size() == 3);
    for (int j=0;j<3;++j) assert(keys[j] == D::key(expected[j]) && values[j] == D::value(expected[j]));
}

// LOAD-016
void test_load_roundtrip_types_student_nlr() {
    using D = StudentData;
    using T = D::Tree;
    T tree;
    fill_data<D>(tree);
    T restored;
    std::string text = tree.to_string(T::TraversalOrder::NLR);
    bool success = restored.from_string(text);
    assert(success);
    assert_copy(tree,restored);
    assert(restored.to_string(T::TraversalOrder::NLR) == text);
}

// MATRIX
void test_matrix_student_traverse_nrl() {
    using D = StudentData;
    using T = D::Tree;
    T tree;
    fill_data<D>(tree);
    using K = decltype(D::key(1));
    using V = decltype(D::value(1));
    std::vector<K> keys;
    std::vector<V> values;
    tree.traverse(T::TraversalOrder::NRL,TypedCollect<K,V>{keys,values});
    const int expected[] = {2,3,1};
    assert(keys.size() == 3 && values.size() == 3);
    for (int j=0;j<3;++j) assert(keys[j] == D::key(expected[j]) && values[j] == D::value(expected[j]));
}

// LOAD-016
void test_load_roundtrip_types_student_nrl() {
    using D = StudentData;
    using T = D::Tree;
    T tree;
    fill_data<D>(tree);
    T restored;
    std::string text = tree.to_string(T::TraversalOrder::NRL);
    bool success = restored.from_string(text);
    assert(success);
    assert_copy(tree,restored);
    assert(restored.to_string(T::TraversalOrder::NRL) == text);
}

// MATRIX
void test_matrix_student_traverse_lnr() {
    using D = StudentData;
    using T = D::Tree;
    T tree;
    fill_data<D>(tree);
    using K = decltype(D::key(1));
    using V = decltype(D::value(1));
    std::vector<K> keys;
    std::vector<V> values;
    tree.traverse(T::TraversalOrder::LNR,TypedCollect<K,V>{keys,values});
    const int expected[] = {1,2,3};
    assert(keys.size() == 3 && values.size() == 3);
    for (int j=0;j<3;++j) assert(keys[j] == D::key(expected[j]) && values[j] == D::value(expected[j]));
}

// LOAD-016
void test_load_roundtrip_types_student_lnr() {
    using D = StudentData;
    using T = D::Tree;
    T tree;
    fill_data<D>(tree);
    T restored;
    std::string text = tree.to_string(T::TraversalOrder::LNR);
    bool success = restored.from_string(text);
    assert(success);
    assert_copy(tree,restored);
    assert(restored.to_string(T::TraversalOrder::LNR) == text);
}

// MATRIX
void test_matrix_student_traverse_rnl() {
    using D = StudentData;
    using T = D::Tree;
    T tree;
    fill_data<D>(tree);
    using K = decltype(D::key(1));
    using V = decltype(D::value(1));
    std::vector<K> keys;
    std::vector<V> values;
    tree.traverse(T::TraversalOrder::RNL,TypedCollect<K,V>{keys,values});
    const int expected[] = {3,2,1};
    assert(keys.size() == 3 && values.size() == 3);
    for (int j=0;j<3;++j) assert(keys[j] == D::key(expected[j]) && values[j] == D::value(expected[j]));
}

// LOAD-016
void test_load_roundtrip_types_student_rnl() {
    using D = StudentData;
    using T = D::Tree;
    T tree;
    fill_data<D>(tree);
    T restored;
    std::string text = tree.to_string(T::TraversalOrder::RNL);
    bool success = restored.from_string(text);
    assert(success);
    assert_copy(tree,restored);
    assert(restored.to_string(T::TraversalOrder::RNL) == text);
}

// MATRIX
void test_matrix_student_traverse_lrn() {
    using D = StudentData;
    using T = D::Tree;
    T tree;
    fill_data<D>(tree);
    using K = decltype(D::key(1));
    using V = decltype(D::value(1));
    std::vector<K> keys;
    std::vector<V> values;
    tree.traverse(T::TraversalOrder::LRN,TypedCollect<K,V>{keys,values});
    const int expected[] = {1,3,2};
    assert(keys.size() == 3 && values.size() == 3);
    for (int j=0;j<3;++j) assert(keys[j] == D::key(expected[j]) && values[j] == D::value(expected[j]));
}

// LOAD-016
void test_load_roundtrip_types_student_lrn() {
    using D = StudentData;
    using T = D::Tree;
    T tree;
    fill_data<D>(tree);
    T restored;
    std::string text = tree.to_string(T::TraversalOrder::LRN);
    bool success = restored.from_string(text);
    assert(success);
    assert_copy(tree,restored);
    assert(restored.to_string(T::TraversalOrder::LRN) == text);
}

// MATRIX
void test_matrix_student_traverse_rln() {
    using D = StudentData;
    using T = D::Tree;
    T tree;
    fill_data<D>(tree);
    using K = decltype(D::key(1));
    using V = decltype(D::value(1));
    std::vector<K> keys;
    std::vector<V> values;
    tree.traverse(T::TraversalOrder::RLN,TypedCollect<K,V>{keys,values});
    const int expected[] = {3,1,2};
    assert(keys.size() == 3 && values.size() == 3);
    for (int j=0;j<3;++j) assert(keys[j] == D::key(expected[j]) && values[j] == D::value(expected[j]));
}

// LOAD-016
void test_load_roundtrip_types_student_rln() {
    using D = StudentData;
    using T = D::Tree;
    T tree;
    fill_data<D>(tree);
    T restored;
    std::string text = tree.to_string(T::TraversalOrder::RLN);
    bool success = restored.from_string(text);
    assert(success);
    assert_copy(tree,restored);
    assert(restored.to_string(T::TraversalOrder::RLN) == text);
}

// MATRIX
void test_matrix_teacher_empty() {
    using D = TeacherData;
    using T = D::Tree;
    T tree;
    assert(tree.empty() && check_tree(tree));
}

// MATRIX
void test_matrix_teacher_single() {
    using D = TeacherData;
    using T = D::Tree;
    T tree;
    tree.insert(D::key(1),D::value(1));
    assert(tree.find(D::key(1))->value == D::value(1) && node_count(tree) == 1 && check_tree(tree));
}

// MATRIX
void test_matrix_teacher_find() {
    using D = TeacherData;
    using T = D::Tree;
    T tree;
    fill_data<D>(tree);
    assert_data<D>(tree);
}

// MATRIX
void test_matrix_teacher_missing() {
    using D = TeacherData;
    using T = D::Tree;
    T tree;
    fill_data<D>(tree);
    assert(!tree.find(D::key(99)));
}

// MATRIX
void test_matrix_teacher_duplicate() {
    using D = TeacherData;
    using T = D::Tree;
    T tree;
    fill_data<D>(tree);
    tree.insert(D::key(1),D::transform(D::value(1)));
    assert_data<D>(tree);
}

// MATRIX
void test_matrix_teacher_erase() {
    using D = TeacherData;
    using T = D::Tree;
    T tree;
    fill_data<D>(tree);
    bool erased = tree.erase(D::key(2));
    assert(erased && !tree.find(D::key(2)) && node_count(tree) == 2 && check_tree(tree));
    assert(tree.find(D::key(1))->value == D::value(1) && tree.find(D::key(3))->value == D::value(3));
}

// MATRIX
void test_matrix_teacher_copy() {
    using D = TeacherData;
    using T = D::Tree;
    T tree;
    fill_data<D>(tree);
    T copy(tree);
    assert_copy(tree,copy);
}

// MATRIX
void test_matrix_teacher_copy_assign() {
    using D = TeacherData;
    using T = D::Tree;
    T tree;
    fill_data<D>(tree);
    T copy;
    copy.insert(D::key(99),D::value(1));
    copy = tree;
    assert_copy(tree,copy);
}

// MATRIX
void test_matrix_teacher_move() {
    using D = TeacherData;
    using T = D::Tree;
    T tree;
    fill_data<D>(tree);
    auto root = tree.get_root();
    T moved(std::move(tree));
    assert(tree.empty() && moved.get_root() == root);
    assert_data<D>(moved);
    tree.insert(D::key(1),D::value(1));
    assert(check_tree(tree));
}

// MATRIX
void test_matrix_teacher_move_assign() {
    using D = TeacherData;
    using T = D::Tree;
    T tree;
    fill_data<D>(tree);
    T moved;
    moved.insert(D::key(99),D::value(1));
    moved = std::move(tree);
    assert(tree.empty());
    assert_data<D>(moved);
}

// MATRIX
void test_matrix_teacher_copy_independent() {
    using D = TeacherData;
    using T = D::Tree;
    T tree;
    fill_data<D>(tree);
    T copy(tree);
    copy.find(D::key(1))->value = D::transform(D::value(1));
    bool erased = copy.erase(D::key(2));
    assert(erased);
    assert_data<D>(tree);
}

// MATRIX
void test_matrix_teacher_map() {
    using D = TeacherData;
    using T = D::Tree;
    T tree;
    fill_data<D>(tree);
    T result = tree.map(D::transform);
    for (int i=1;i<=3;++i) assert(result.find(D::key(i))->value == D::transform(D::value(i)) && result.find(D::key(i)) != tree.find(D::key(i)));
    assert(check_tree(result));
    assert_data<D>(tree);
}

// MATRIX
void test_matrix_teacher_where() {
    using D = TeacherData;
    using T = D::Tree;
    T tree;
    fill_data<D>(tree);
    T result = tree.where(D::keep);
    assert(node_count(result) == 2 && !result.find(D::key(1)) && check_tree(result));
    assert(result.find(D::key(2))->value == D::value(2) && result.find(D::key(3))->value == D::value(3));
    assert_data<D>(tree);
}

// MATRIX
void test_matrix_teacher_reduce() {
    using D = TeacherData;
    using T = D::Tree;
    T tree;
    fill_data<D>(tree);
    D::Result result = tree.reduce(D::Result(),D::fold);
    assert_result(result,D::expected());
    assert_data<D>(tree);
}

// MATRIX
void test_matrix_teacher_merge() {
    using D = TeacherData;
    using T = D::Tree;
    T first,second;
    fill_data<D>(first);
    second.insert(D::key(2),D::transform(D::value(2)));
    second.insert(D::key(4),D::value(4));
    T result = first.merge(second);
    assert(node_count(result) == 4 && check_tree(result));
    assert(result.find(D::key(2))->value == D::value(2) && result.find(D::key(4))->value == D::value(4));
    assert_data<D>(first);
    assert(second.find(D::key(2))->value == D::transform(D::value(2)));
}

// MATRIX
void test_matrix_teacher_extract_root() {
    using D = TeacherData;
    using T = D::Tree;
    T tree;
    fill_data<D>(tree);
    T result = tree.extract_subtree(D::key(2));
    assert_copy(tree,result);
}

// MATRIX
void test_matrix_teacher_extract_child() {
    using D = TeacherData;
    using T = D::Tree;
    T tree;
    fill_data<D>(tree);
    T result = tree.extract_subtree(D::key(1));
    assert(node_count(result) == 1 && result.get_root()->color && tree.contains_subtree(result));
    assert(result.find(D::key(1)) != tree.find(D::key(1)));
    assert_data<D>(tree);
}

// MATRIX
void test_matrix_teacher_contains_mismatch() {
    using D = TeacherData;
    using T = D::Tree;
    T tree;
    fill_data<D>(tree);
    T result = tree.extract_subtree(D::key(1));
    result.get_root()->value = D::transform(D::value(1));
    assert(!tree.contains_subtree(result));
}

// MATRIX
void test_matrix_teacher_absolute_path() {
    using D = TeacherData;
    using T = D::Tree;
    T tree;
    fill_data<D>(tree);
    assert(tree.find_by_path("L") == tree.find(D::key(1)));
    assert(tree.find_by_path("R") == tree.find(D::key(3)));
    assert(tree.find_by_path("LL") == nullptr);
}

// MATRIX
void test_matrix_teacher_relative_path() {
    using D = TeacherData;
    using T = D::Tree;
    T tree;
    fill_data<D>(tree);
    assert(tree.find_by_path(D::key(1),"UR") == tree.find(D::key(3)));
    assert(tree.find_by_path(D::key(1),"UU") == nullptr);
}

// MATRIX
void test_matrix_teacher_traverse_nlr() {
    using D = TeacherData;
    using T = D::Tree;
    T tree;
    fill_data<D>(tree);
    using K = decltype(D::key(1));
    using V = decltype(D::value(1));
    std::vector<K> keys;
    std::vector<V> values;
    tree.traverse(T::TraversalOrder::NLR,TypedCollect<K,V>{keys,values});
    const int expected[] = {2,1,3};
    assert(keys.size() == 3 && values.size() == 3);
    for (int j=0;j<3;++j) assert(keys[j] == D::key(expected[j]) && values[j] == D::value(expected[j]));
}

// LOAD-016
void test_load_roundtrip_types_teacher_nlr() {
    using D = TeacherData;
    using T = D::Tree;
    T tree;
    fill_data<D>(tree);
    T restored;
    std::string text = tree.to_string(T::TraversalOrder::NLR);
    bool success = restored.from_string(text);
    assert(success);
    assert_copy(tree,restored);
    assert(restored.to_string(T::TraversalOrder::NLR) == text);
}

// MATRIX
void test_matrix_teacher_traverse_nrl() {
    using D = TeacherData;
    using T = D::Tree;
    T tree;
    fill_data<D>(tree);
    using K = decltype(D::key(1));
    using V = decltype(D::value(1));
    std::vector<K> keys;
    std::vector<V> values;
    tree.traverse(T::TraversalOrder::NRL,TypedCollect<K,V>{keys,values});
    const int expected[] = {2,3,1};
    assert(keys.size() == 3 && values.size() == 3);
    for (int j=0;j<3;++j) assert(keys[j] == D::key(expected[j]) && values[j] == D::value(expected[j]));
}

// LOAD-016
void test_load_roundtrip_types_teacher_nrl() {
    using D = TeacherData;
    using T = D::Tree;
    T tree;
    fill_data<D>(tree);
    T restored;
    std::string text = tree.to_string(T::TraversalOrder::NRL);
    bool success = restored.from_string(text);
    assert(success);
    assert_copy(tree,restored);
    assert(restored.to_string(T::TraversalOrder::NRL) == text);
}

// MATRIX
void test_matrix_teacher_traverse_lnr() {
    using D = TeacherData;
    using T = D::Tree;
    T tree;
    fill_data<D>(tree);
    using K = decltype(D::key(1));
    using V = decltype(D::value(1));
    std::vector<K> keys;
    std::vector<V> values;
    tree.traverse(T::TraversalOrder::LNR,TypedCollect<K,V>{keys,values});
    const int expected[] = {1,2,3};
    assert(keys.size() == 3 && values.size() == 3);
    for (int j=0;j<3;++j) assert(keys[j] == D::key(expected[j]) && values[j] == D::value(expected[j]));
}

// LOAD-016
void test_load_roundtrip_types_teacher_lnr() {
    using D = TeacherData;
    using T = D::Tree;
    T tree;
    fill_data<D>(tree);
    T restored;
    std::string text = tree.to_string(T::TraversalOrder::LNR);
    bool success = restored.from_string(text);
    assert(success);
    assert_copy(tree,restored);
    assert(restored.to_string(T::TraversalOrder::LNR) == text);
}

// MATRIX
void test_matrix_teacher_traverse_rnl() {
    using D = TeacherData;
    using T = D::Tree;
    T tree;
    fill_data<D>(tree);
    using K = decltype(D::key(1));
    using V = decltype(D::value(1));
    std::vector<K> keys;
    std::vector<V> values;
    tree.traverse(T::TraversalOrder::RNL,TypedCollect<K,V>{keys,values});
    const int expected[] = {3,2,1};
    assert(keys.size() == 3 && values.size() == 3);
    for (int j=0;j<3;++j) assert(keys[j] == D::key(expected[j]) && values[j] == D::value(expected[j]));
}

// LOAD-016
void test_load_roundtrip_types_teacher_rnl() {
    using D = TeacherData;
    using T = D::Tree;
    T tree;
    fill_data<D>(tree);
    T restored;
    std::string text = tree.to_string(T::TraversalOrder::RNL);
    bool success = restored.from_string(text);
    assert(success);
    assert_copy(tree,restored);
    assert(restored.to_string(T::TraversalOrder::RNL) == text);
}

// MATRIX
void test_matrix_teacher_traverse_lrn() {
    using D = TeacherData;
    using T = D::Tree;
    T tree;
    fill_data<D>(tree);
    using K = decltype(D::key(1));
    using V = decltype(D::value(1));
    std::vector<K> keys;
    std::vector<V> values;
    tree.traverse(T::TraversalOrder::LRN,TypedCollect<K,V>{keys,values});
    const int expected[] = {1,3,2};
    assert(keys.size() == 3 && values.size() == 3);
    for (int j=0;j<3;++j) assert(keys[j] == D::key(expected[j]) && values[j] == D::value(expected[j]));
}

// LOAD-016
void test_load_roundtrip_types_teacher_lrn() {
    using D = TeacherData;
    using T = D::Tree;
    T tree;
    fill_data<D>(tree);
    T restored;
    std::string text = tree.to_string(T::TraversalOrder::LRN);
    bool success = restored.from_string(text);
    assert(success);
    assert_copy(tree,restored);
    assert(restored.to_string(T::TraversalOrder::LRN) == text);
}

// MATRIX
void test_matrix_teacher_traverse_rln() {
    using D = TeacherData;
    using T = D::Tree;
    T tree;
    fill_data<D>(tree);
    using K = decltype(D::key(1));
    using V = decltype(D::value(1));
    std::vector<K> keys;
    std::vector<V> values;
    tree.traverse(T::TraversalOrder::RLN,TypedCollect<K,V>{keys,values});
    const int expected[] = {3,1,2};
    assert(keys.size() == 3 && values.size() == 3);
    for (int j=0;j<3;++j) assert(keys[j] == D::key(expected[j]) && values[j] == D::value(expected[j]));
}

// LOAD-016
void test_load_roundtrip_types_teacher_rln() {
    using D = TeacherData;
    using T = D::Tree;
    T tree;
    fill_data<D>(tree);
    T restored;
    std::string text = tree.to_string(T::TraversalOrder::RLN);
    bool success = restored.from_string(text);
    assert(success);
    assert_copy(tree,restored);
    assert(restored.to_string(T::TraversalOrder::RLN) == text);
}

