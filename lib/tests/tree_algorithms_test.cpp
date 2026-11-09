#include "test_helpers.h"

struct MapOrder {
    std::vector<int>& keys;
    int operator()(const int& value) const { keys.push_back(value/10); return value; }
};
struct PredicateOrder {
    std::vector<int>& values;
    bool operator()(const int& value) const { values.push_back(value); return true; }
};
struct ReduceCalls {
    int& count;
    int operator()(const int& result, const int& value) const { ++count; return result+value; }
};

inline bool over_sixty(const int& value) { return value > 60; }

// TRV-001
void test_trv_nlr() {
    Tree tree;
    fill_b(tree);
    std::vector<int> keys, values;
    tree.traverse(Order::NLR, Collect{keys,values});
    const std::vector<int> expected = {4,2,1,3,6,5,7};
    assert(keys == expected && values.size() == 7);
    for (int j=0;j<7;++j) assert(values[j] == expected[j]*10);
}

// TRV-007
void test_trv_empty_nlr() {
    Tree tree;
    int count = 0;
    tree.traverse(Order::NLR, Calls{count});
    assert(count == 0);
}

// TRV-008
void test_trv_single_nlr() {
    Tree tree;
    tree.insert(4,40);
    std::vector<int> keys, values;
    tree.traverse(Order::NLR, Collect{keys,values});
    assert(keys == std::vector<int>{4} && values == std::vector<int>{40});
}

// TRV-009
void test_trv_asymmetric_nlr() {
    Tree tree;
    fill_a(tree);
    std::vector<int> keys, values;
    tree.traverse(Order::NLR, Collect{keys,values});
    const std::vector<int> expected = {4,2,1,3,6};
    assert(keys == expected);
}

// TRV-002
void test_trv_nrl() {
    Tree tree;
    fill_b(tree);
    std::vector<int> keys, values;
    tree.traverse(Order::NRL, Collect{keys,values});
    const std::vector<int> expected = {4,6,7,5,2,3,1};
    assert(keys == expected && values.size() == 7);
    for (int j=0;j<7;++j) assert(values[j] == expected[j]*10);
}

// TRV-007
void test_trv_empty_nrl() {
    Tree tree;
    int count = 0;
    tree.traverse(Order::NRL, Calls{count});
    assert(count == 0);
}

// TRV-008
void test_trv_single_nrl() {
    Tree tree;
    tree.insert(4,40);
    std::vector<int> keys, values;
    tree.traverse(Order::NRL, Collect{keys,values});
    assert(keys == std::vector<int>{4} && values == std::vector<int>{40});
}

// TRV-009
void test_trv_asymmetric_nrl() {
    Tree tree;
    fill_a(tree);
    std::vector<int> keys, values;
    tree.traverse(Order::NRL, Collect{keys,values});
    const std::vector<int> expected = {4,6,2,3,1};
    assert(keys == expected);
}

// TRV-003
void test_trv_lnr() {
    Tree tree;
    fill_b(tree);
    std::vector<int> keys, values;
    tree.traverse(Order::LNR, Collect{keys,values});
    const std::vector<int> expected = {1,2,3,4,5,6,7};
    assert(keys == expected && values.size() == 7);
    for (int j=0;j<7;++j) assert(values[j] == expected[j]*10);
}

// TRV-007
void test_trv_empty_lnr() {
    Tree tree;
    int count = 0;
    tree.traverse(Order::LNR, Calls{count});
    assert(count == 0);
}

// TRV-008
void test_trv_single_lnr() {
    Tree tree;
    tree.insert(4,40);
    std::vector<int> keys, values;
    tree.traverse(Order::LNR, Collect{keys,values});
    assert(keys == std::vector<int>{4} && values == std::vector<int>{40});
}

// TRV-009
void test_trv_asymmetric_lnr() {
    Tree tree;
    fill_a(tree);
    std::vector<int> keys, values;
    tree.traverse(Order::LNR, Collect{keys,values});
    const std::vector<int> expected = {1,2,3,4,6};
    assert(keys == expected);
}

// TRV-004
void test_trv_rnl() {
    Tree tree;
    fill_b(tree);
    std::vector<int> keys, values;
    tree.traverse(Order::RNL, Collect{keys,values});
    const std::vector<int> expected = {7,6,5,4,3,2,1};
    assert(keys == expected && values.size() == 7);
    for (int j=0;j<7;++j) assert(values[j] == expected[j]*10);
}

// TRV-007
void test_trv_empty_rnl() {
    Tree tree;
    int count = 0;
    tree.traverse(Order::RNL, Calls{count});
    assert(count == 0);
}

// TRV-008
void test_trv_single_rnl() {
    Tree tree;
    tree.insert(4,40);
    std::vector<int> keys, values;
    tree.traverse(Order::RNL, Collect{keys,values});
    assert(keys == std::vector<int>{4} && values == std::vector<int>{40});
}

// TRV-009
void test_trv_asymmetric_rnl() {
    Tree tree;
    fill_a(tree);
    std::vector<int> keys, values;
    tree.traverse(Order::RNL, Collect{keys,values});
    const std::vector<int> expected = {6,4,3,2,1};
    assert(keys == expected);
}

// TRV-005
void test_trv_lrn() {
    Tree tree;
    fill_b(tree);
    std::vector<int> keys, values;
    tree.traverse(Order::LRN, Collect{keys,values});
    const std::vector<int> expected = {1,3,2,5,7,6,4};
    assert(keys == expected && values.size() == 7);
    for (int j=0;j<7;++j) assert(values[j] == expected[j]*10);
}

// TRV-007
void test_trv_empty_lrn() {
    Tree tree;
    int count = 0;
    tree.traverse(Order::LRN, Calls{count});
    assert(count == 0);
}

// TRV-008
void test_trv_single_lrn() {
    Tree tree;
    tree.insert(4,40);
    std::vector<int> keys, values;
    tree.traverse(Order::LRN, Collect{keys,values});
    assert(keys == std::vector<int>{4} && values == std::vector<int>{40});
}

// TRV-009
void test_trv_asymmetric_lrn() {
    Tree tree;
    fill_a(tree);
    std::vector<int> keys, values;
    tree.traverse(Order::LRN, Collect{keys,values});
    const std::vector<int> expected = {1,3,2,6,4};
    assert(keys == expected);
}

// TRV-006
void test_trv_rln() {
    Tree tree;
    fill_b(tree);
    std::vector<int> keys, values;
    tree.traverse(Order::RLN, Collect{keys,values});
    const std::vector<int> expected = {7,5,6,3,1,2,4};
    assert(keys == expected && values.size() == 7);
    for (int j=0;j<7;++j) assert(values[j] == expected[j]*10);
}

// TRV-007
void test_trv_empty_rln() {
    Tree tree;
    int count = 0;
    tree.traverse(Order::RLN, Calls{count});
    assert(count == 0);
}

// TRV-008
void test_trv_single_rln() {
    Tree tree;
    tree.insert(4,40);
    std::vector<int> keys, values;
    tree.traverse(Order::RLN, Collect{keys,values});
    assert(keys == std::vector<int>{4} && values == std::vector<int>{40});
}

// TRV-009
void test_trv_asymmetric_rln() {
    Tree tree;
    fill_a(tree);
    std::vector<int> keys, values;
    tree.traverse(Order::RLN, Collect{keys,values});
    const std::vector<int> expected = {6,3,1,2,4};
    assert(keys == expected);
}

// TRV-010
void test_trv_const_source() {
    Tree source;
    fill_b(source);
    const Tree& tree = source;
    int count = 0;
    tree.traverse(Order::LNR, Calls{count});
    assert(count == 7 && check_tree(tree));
}

// TRV-011
void test_trv_empty_action() {
    Tree tree;
    tree.insert(4,40);
    std::function<void(const int&,const int&)> action;
    bool threw = false;
    try { tree.traverse(Order::LNR, action); }
    catch (const std::bad_function_call&) { threw = true; }
    assert(threw && tree.find(4)->value == 40);
}

// TRV-012
void test_trv_throwing_action() {
    Tree tree;
    fill_b(tree);
    Tree before(tree);
    int count = 0;
    bool threw = false;
    try { tree.traverse(Order::LNR, ThrowVisit{count}); }
    catch (const std::runtime_error&) { threw = true; }
    assert(threw && count == 3);
    assert_copy(tree, before);
}

// TRV-013
void test_trv_empty_action_empty_tree() {
    Tree tree;
    std::function<void(const int&,const int&)> action;
    tree.traverse(Order::LNR, action);
    assert(tree.empty());
}

// MAP-001
void test_map_empty() {
    Tree tree;
    int calls = 0;
    Tree result = tree.map(MapCalls{calls});
    assert(calls == 0 && result.empty() && result.get_nil() != tree.get_nil());
}

// MAP-002
void test_map_single() {
    Tree tree;
    tree.insert(4,40);
    Tree result = tree.map(twice);
    assert(result.find(4)->value == 80 && tree.find(4)->value == 40);
    assert(check_tree(result));
}

// MAP-003
void test_map_all_nodes() {
    Tree tree;
    fill_b(tree);
    int calls = 0;
    Tree result = tree.map(MapCalls{calls});
    assert(calls == 7 && node_count(result) == 7 && check_tree(result));
    for (int key=1;key<=7;++key) assert(result.find(key)->value == key*10+1 && tree.find(key)->value == key*10);
}

// MAP-004
void test_map_equal_results() {
    Tree tree;
    fill_b(tree);
    Tree result = tree.map(five);
    assert(node_count(result) == 7);
    for (int key=1;key<=7;++key) assert(result.find(key)->value == 5);
}

// MAP-005
void test_map_independence() {
    Tree tree;
    fill_b(tree);
    Tree result = tree.map(plus_one);
    assert(result.get_nil() != tree.get_nil());
    result.find(2)->value = 999;
    bool erased = result.erase(3);
    assert(erased && tree.find(2)->value == 20 && tree.find(3));
    for (int key=1;key<=7;++key) if (result.find(key)) assert(result.find(key) != tree.find(key));
}

// MAP-006
void test_map_comparator() {
    CustomTree tree(Compare(true));
    fill_b(tree);
    CustomTree result = tree.map(twice);
    assert(result.get_compare().reverse && check_tree(result));
}

// MAP-007
void test_map_order() {
    Tree tree;
    fill_b(tree);
    std::vector<int> keys;
    Tree result = tree.map(MapOrder{keys});
    const std::vector<int> expected = {1,2,3,4,5,6,7};
    assert(keys == expected && node_count(result) == 7);
}

// MAP-008
void test_map_empty_action() {
    Tree tree;
    tree.insert(4,40);
    std::function<int(const int&)> action;
    bool threw = false;
    try { tree.map(action); }
    catch (const std::bad_function_call&) { threw = true; }
    assert(threw && tree.find(4)->value == 40);
}

// MAP-009
void test_map_throwing_action() {
    Tree tree;
    fill_b(tree);
    Tree before(tree);
    int calls = 0;
    bool threw = false;
    try { tree.map(ThrowMap{calls}); }
    catch (const std::runtime_error&) { threw = true; }
    assert(threw && calls == 3);
    assert_copy(tree, before);
}

// MAP-010
void test_map_empty_action_empty_tree() {
    Tree tree;
    std::function<int(const int&)> action;
    Tree result = tree.map(action);
    assert(result.empty());
}

// WHR-001
void test_whr_empty() {
    Tree tree;
    int calls = 0;
    Tree result = tree.where(PredicateCalls{calls});
    assert(result.empty() && calls == 0 && result.get_nil() != tree.get_nil());
}

// WHR-002
void test_whr_single_match() {
    Tree tree;
    tree.insert(4,40);
    Tree result = tree.where(positive);
    assert(node_count(result) == 1 && check_tree(result));
    assert(result.find(4)->value == 40 && result.find(4) != tree.find(4));
}

// WHR-003
void test_whr_single_reject() {
    Tree tree;
    tree.insert(4,40);
    Tree result = tree.where(negative);
    assert(node_count(result) == 0 && check_tree(result));
}

// WHR-004
void test_whr_all() {
    Tree tree;
    fill_b(tree);
    Tree result = tree.where(accept_all);
    assert(node_count(result) == 7 && check_tree(result));
    for (int key=1;key<=7;++key) assert(result.find(key)->value == key*10);
}

// WHR-005
void test_whr_none() {
    Tree tree;
    fill_b(tree);
    Tree result = tree.where(accept_none);
    assert(result.empty() && check_tree(result));
}

// WHR-006
void test_whr_partial() {
    Tree tree;
    tree.insert(1,12);
    tree.insert(2,15);
    tree.insert(3,18);
    Tree result = tree.where(even);
    assert(node_count(result) == 2 && result.find(1)->value == 12 && result.find(3)->value == 18 && !result.find(2));
}

// WHR-007
void test_whr_independence() {
    Tree tree;
    fill_b(tree);
    Tree result = tree.where(accept_all);
    result.find(2)->value = 999;
    bool erased = result.erase(3);
    assert(erased && tree.find(2)->value == 20 && tree.find(3));
    assert(result.find(2) != tree.find(2) && result.get_nil() != tree.get_nil());
}

// WHR-008
void test_whr_comparator() {
    CustomTree tree(Compare(true));
    fill_b(tree);
    CustomTree result = tree.where(accept_all);
    assert(result.get_compare().reverse && check_tree(result));
}

// WHR-009
void test_whr_one_call_per_node() {
    Tree tree;
    fill_b(tree);
    std::vector<int> values;
    Tree result = tree.where(PredicateOrder{values});
    const std::vector<int> expected = {10,20,30,40,50,60,70};
    assert(values == expected && node_count(result) == 7);
}

// WHR-010
void test_whr_empty_predicate() {
    Tree tree;
    tree.insert(4,40);
    std::function<bool(const int&)> action;
    bool threw = false;
    try { tree.where(action); }
    catch (const std::bad_function_call&) { threw = true; }
    assert(threw && tree.find(4)->value == 40);
}

// WHR-011
void test_whr_throwing_predicate() {
    Tree tree;
    fill_b(tree);
    Tree before(tree);
    int count = 0;
    bool threw = false;
    try { tree.where(ThrowPredicate{count}); }
    catch (const std::runtime_error&) { threw = true; }
    assert(threw && count == 3);
    assert_copy(tree, before);
}

// WHR-012
void test_whr_empty_predicate_empty_tree() {
    Tree tree;
    std::function<bool(const int&)> action;
    Tree result = tree.where(action);
    assert(result.empty());
}

// RED-001
void test_red_empty() {
    Tree tree;
    int count = 0;
    int result = tree.reduce(9, ThrowReduce{count});
    assert(result == 9 && count == 0);
}

// RED-002
void test_red_single() {
    Tree tree;
    tree.insert(4,40);
    int calls = 0;
    int result = tree.reduce(2, ReduceCalls{calls});
    assert(result == 42 && calls == 1);
}

// RED-003
void test_red_initial_used() {
    Tree tree;
    fill_b(tree);
    int result = tree.reduce(100, add);
    assert(result == 380);
}

// RED-004
void test_red_default_order() {
    Tree tree;
    fill_b(tree);
    int result = tree.reduce(0, digits);
    assert(result == 1234567);
}

// RED-005
void test_red_nlr() {
    Tree tree;
    fill_b(tree);
    int result = tree.reduce(9, digits, Order::NLR);
    assert(result == 94213657);
}

// RED-006
void test_red_nrl() {
    Tree tree;
    fill_b(tree);
    int result = tree.reduce(9, digits, Order::NRL);
    assert(result == 94675231);
}

// RED-007
void test_red_lnr() {
    Tree tree;
    fill_b(tree);
    int result = tree.reduce(9, digits, Order::LNR);
    assert(result == 91234567);
}

// RED-008
void test_red_rnl() {
    Tree tree;
    fill_b(tree);
    int result = tree.reduce(9, digits, Order::RNL);
    assert(result == 97654321);
}

// RED-009
void test_red_lrn() {
    Tree tree;
    fill_b(tree);
    int result = tree.reduce(9, digits, Order::LRN);
    assert(result == 91325764);
}

// RED-010
void test_red_rln() {
    Tree tree;
    fill_b(tree);
    int result = tree.reduce(9, digits, Order::RLN);
    assert(result == 97563124);
}

// RED-011
void test_red_different_accumulator() {
    StringTree tree;
    tree.insert("a","hello");
    tree.insert("b","abc");
    int result = tree.reduce(0, length_sum);
    assert(result == 8);
}

// RED-012
void test_red_nondefault_accumulator() {
    Tree tree;
    fill_b(tree);
    Accumulator result = tree.reduce(Accumulator(1), accumulate);
    assert(result.total == 281);
}

// RED-013
void test_red_const_source() {
    Tree tree;
    fill_b(tree);
    const Tree& source = tree;
    int result = source.reduce(0, add);
    assert(result == 280 && check_tree(tree));
}

// RED-014
void test_red_throwing_action() {
    Tree tree;
    fill_b(tree);
    Tree before(tree);
    int count = 0;
    bool threw = false;
    try { tree.reduce(0, ThrowReduce{count}); }
    catch (const std::runtime_error&) { threw = true; }
    assert(threw && count == 3);
    assert_copy(tree, before);
}

// RED-015
void test_red_empty_action() {
    Tree tree;
    tree.insert(4,40);
    std::function<int(const int&,const int&)> action;
    bool threw = false;
    try { tree.reduce(0, action); }
    catch (const std::bad_function_call&) { threw = true; }
    assert(threw && tree.find(4)->value == 40);
}

// MRG-001
void test_mrg_both_empty() {
    Tree a,b;
    Tree result = a.merge(b);
    assert(result.empty() && result.get_nil() != a.get_nil() && result.get_nil() != b.get_nil());
}

// MRG-002
void test_mrg_first_empty() {
    CustomTree a(Compare(true)), b;
    b.insert(4,40);
    CustomTree result = a.merge(b);
    assert(result.get_compare().reverse && result.find(4)->value == 40 && result.find(4) != b.find(4));
}

// MRG-003
void test_mrg_second_empty() {
    Tree a,b;
    a.insert(4,40);
    Tree result = a.merge(b);
    assert_copy(a, result);
}

// MRG-004
void test_mrg_disjoint() {
    Tree a,b;
    a.insert(1,10);
    a.insert(3,30);
    b.insert(2,20);
    b.insert(4,40);
    Tree result = a.merge(b);
    assert(node_count(result) == 4 && check_tree(result));
    for (int i=1;i<=4;++i) assert(result.find(i)->value == i*10);
}

// MRG-005
void test_mrg_duplicate_key() {
    Tree a,b;
    a.insert(2,20);
    b.insert(2,200);
    Tree result = a.merge(b);
    assert(node_count(result) == 1 && result.find(2)->value == 20);
}

// MRG-006
void test_mrg_all_duplicates() {
    Tree a,b;
    fill_b(a);
    for (int i=1;i<=7;++i) b.insert(i, i*100);
    Tree result = a.merge(b);
    assert(node_count(result) == 7);
    for (int i=1;i<=7;++i) assert(result.find(i)->value == i*10);
}

// MRG-007
void test_mrg_self() {
    Tree tree;
    fill_b(tree);
    Tree result = tree.merge(tree);
    assert(node_count(result) == 7 && result.get_nil() != tree.get_nil());
    for (int i=1;i<=7;++i) assert(result.find(i)->value == i*10 && result.find(i) != tree.find(i));
}

// MRG-008
void test_mrg_default_comparator() {
    CustomTree a(Compare(true)),b;
    fill_b(a);
    fill_b(b);
    CustomTree result = a.merge(b);
    assert(result.get_compare().reverse && check_tree(result));
}

// MRG-009
void test_mrg_explicit_comparator() {
    CustomTree a,b;
    fill_b(a);
    fill_b(b);
    CustomTree result = a.merge(b, Compare(true));
    assert(result.get_compare().reverse && check_tree(result));
}

// MRG-010
void test_mrg_new_equivalence() {
    CustomTree a,b;
    a.insert(11,110);
    a.insert(12,120);
    b.insert(13,130);
    b.insert(21,210);
    CustomTree result = a.merge(b, Compare(false,10));
    assert(node_count(result) == 2 && result.find(12)->key == 11 && result.find(12)->value == 110);
    assert(result.find(21)->value == 210 && check_tree(result));
}

// MRG-011
void test_mrg_independence() {
    Tree a,b;
    fill_b(a);
    b.insert(9,90);
    Tree result = a.merge(b);
    result.find(4)->value = 999;
    bool erased = result.erase(9);
    assert(erased && a.find(4)->value == 40 && b.find(9)->value == 90);
    assert(result.get_nil() != a.get_nil() && result.get_nil() != b.get_nil());
}

// MRG-012
void test_mrg_const_sources() {
    Tree a,b;
    fill_b(a);
    b.insert(9,90);
    const Tree& x = a;
    const Tree& y = b;
    Tree result = x.merge(y);
    assert(node_count(result) == 8 && node_count(a) == 7 && node_count(b) == 1);
}

// EXT-001
void test_ext_empty() {
    Tree tree;
    Tree result = tree.extract_subtree(4);
    assert(result.empty() && result.get_nil() != tree.get_nil());
}

// EXT-002
void test_ext_missing() {
    Tree tree;
    fill_b(tree);
    Tree result = tree.extract_subtree(99);
    assert(result.empty() && node_count(tree) == 7);
}

// EXT-003
void test_ext_whole_tree() {
    Tree tree;
    fill_b(tree);
    Tree result = tree.extract_subtree(4);
    assert_copy(tree, result);
}

// EXT-004
void test_ext_internal() {
    Tree tree;
    fill_b(tree);
    Tree result = tree.extract_subtree(2);
    assert(result.get_root()->key == 2 && node_count(result) == 3);
    assert_same(tree, tree.find(2), result, result.get_root(), true);
    assert(check_tree(result));
}

// EXT-005
void test_ext_red_leaf() {
    Tree tree;
    fill_b(tree);
    assert(!tree.find(1)->color);
    Tree result = tree.extract_subtree(1);
    assert(result.get_root()->key == 1 && result.get_root()->color && !tree.find(1)->color);
    assert(node_count(result) == 1 && check_tree(result));
}

// EXT-006
void test_ext_red_internal() {
    Tree tree;
    fill_c(tree);
    assert(!tree.find(4)->color);
    Tree result = tree.extract_subtree(4);
    assert(result.get_root()->color && !tree.find(4)->color);
    assert_same(tree, tree.find(4)->left, result, result.get_root()->left, true);
    assert_same(tree, tree.find(4)->right, result, result.get_root()->right, true);
    assert(check_tree(result));
}

// EXT-007
void test_ext_parent_links() {
    Tree tree;
    fill_b(tree);
    Tree result = tree.extract_subtree(2);
    assert(result.get_root()->parent == result.get_nil());
    assert(result.get_nil() != tree.get_nil() && check_tree(result));
    assert(result.find(1)->parent == result.get_root());
}

// EXT-008
void test_ext_independence() {
    Tree tree;
    fill_b(tree);
    Tree result = tree.extract_subtree(2);
    result.find(1)->value = 999;
    bool erased = result.erase(3);
    assert(erased && tree.find(1)->value == 10 && tree.find(3));
}

// EXT-009
void test_ext_comparator() {
    CustomTree tree(Compare(true));
    fill_b(tree);
    CustomTree result = tree.extract_subtree(2);
    assert(result.get_compare().reverse && check_tree(result));
}

// SUB-001
void test_sub_both_empty() {
    Tree a,b;
    assert(a.contains_subtree(b));
}

// SUB-002
void test_sub_empty_candidate() {
    Tree a,b;
    fill_b(a);
    assert(a.contains_subtree(b));
}

// SUB-003
void test_sub_empty_source() {
    Tree a,b;
    b.insert(4,40);
    assert(!a.contains_subtree(b));
}

// SUB-004
void test_sub_self() {
    Tree tree;
    fill_b(tree);
    assert(tree.contains_subtree(tree));
}

// SUB-005
void test_sub_whole_copy() {
    Tree tree;
    fill_b(tree);
    Tree copy(tree);
    assert(tree.contains_subtree(copy));
}

// SUB-006
void test_sub_extracted() {
    Tree tree;
    fill_b(tree);
    Tree copy = tree.extract_subtree(2);
    assert(tree.contains_subtree(copy));
}

// SUB-007
void test_sub_root_missing() {
    Tree a,b;
    fill_b(a);
    b.insert(99,990);
    assert(!a.contains_subtree(b));
}

// SUB-008
void test_sub_value_differs() {
    Tree tree;
    fill_b(tree);
    Tree copy = tree.extract_subtree(2);
    copy.find(1)->value = -1;
    assert(!tree.contains_subtree(copy));
}

// SUB-009
void test_sub_child_missing() {
    Tree tree;
    fill_b(tree);
    Tree copy = tree.extract_subtree(2);
    bool erased = copy.erase(1);
    assert(erased && copy.get_root()->key == 2);
    assert(!tree.contains_subtree(copy));
}

// SUB-010
void test_sub_child_extra() {
    Tree a,b;
    a.insert(4,40);
    b.insert(4,40);
    b.insert(2,20);
    assert(!a.contains_subtree(b));
}

// SUB-011
void test_sub_same_set_other_shape() {
    Tree a,b;
    const int first[] = {2,1,3,4};
    const int second[] = {3,1,4,2};
    fill_tree(a,first,4);
    fill_tree(b,second,4);
    assert(a.get_root()->key == 2 && b.get_root()->key == 3);
    assert(!a.contains_subtree(b));
}

// SUB-012
void test_sub_root_color_differs() {
    Tree tree;
    fill_b(tree);
    Tree result = tree.extract_subtree(1);
    assert(!tree.find(1)->color && result.get_root()->color);
    assert(tree.contains_subtree(result));
}

// SUB-013
void test_sub_comparator_equivalence() {
    CustomTree a(Compare(false,10)),b(Compare(false,10));
    a.insert(11,100);
    b.insert(12,100);
    assert(a.contains_subtree(b));
}

// PATH-001
void test_path_absolute_empty_tree() {
    Tree tree;
    Tree::Node* found = tree.find_by_path("");
    assert(found == nullptr);
}

// PATH-002
void test_path_absolute_root() {
    Tree tree;
    fill_b(tree);
    Tree::Node* found = tree.find_by_path("");
    assert(found == tree.find(4));
}

// PATH-003
void test_path_absolute_left() {
    Tree tree;
    fill_b(tree);
    Tree::Node* found = tree.find_by_path("L");
    assert(found == tree.find(2));
}

// PATH-004
void test_path_absolute_right() {
    Tree tree;
    fill_b(tree);
    Tree::Node* found = tree.find_by_path("R");
    assert(found == tree.find(6));
}

// PATH-005
void test_path_absolute_deep() {
    Tree tree;
    fill_b(tree);
    Tree::Node* found = tree.find_by_path("LR");
    assert(found == tree.find(3));
}

// PATH-006
void test_path_absolute_missing() {
    Tree tree;
    fill_b(tree);
    Tree::Node* found = tree.find_by_path("LLL");
    assert(found == nullptr);
}

// PATH-007
void test_path_absolute_null() {
    Tree tree;
    fill_b(tree);
    Tree::Node* found = tree.find_by_path(nullptr);
    assert(found == nullptr);
}

// PATH-008
void test_path_absolute_up_forbidden() {
    Tree tree;
    fill_b(tree);
    Tree::Node* found = tree.find_by_path("LU");
    assert(found == nullptr);
}

// PATH-009
void test_path_absolute_bad_symbol() {
    Tree tree;
    fill_b(tree);
    Tree::Node* found = tree.find_by_path("LX");
    assert(found == nullptr);
}

// PATH-010
void test_path_absolute_lowercase() {
    Tree tree;
    fill_b(tree);
    Tree::Node* found = tree.find_by_path("l");
    assert(found == nullptr);
}

// PATH-011
void test_path_absolute_space() {
    Tree tree;
    fill_b(tree);
    Tree::Node* found = tree.find_by_path("L R");
    assert(found == nullptr);
}

// PATH-012
void test_path_relative_empty_path() {
    Tree tree;
    fill_b(tree);
    Tree::Node* found = tree.find_by_path(2, "");
    assert(found == tree.find(2));
}

// PATH-013
void test_path_relative_start_missing() {
    Tree tree;
    fill_b(tree);
    Tree::Node* found = tree.find_by_path(99, "");
    assert(found == nullptr);
}

// PATH-014
void test_path_relative_null() {
    Tree tree;
    fill_b(tree);
    Tree::Node* found = tree.find_by_path(2, nullptr);
    assert(found == nullptr);
}

// PATH-015
void test_path_relative_children() {
    Tree tree;
    fill_b(tree);
    Tree::Node* found = tree.find_by_path(2, "R");
    assert(found == tree.find(3));
}

// PATH-016
void test_path_relative_parent() {
    Tree tree;
    fill_b(tree);
    Tree::Node* found = tree.find_by_path(1, "U");
    assert(found == tree.find(2));
}

// PATH-017
void test_path_relative_sibling() {
    Tree tree;
    fill_b(tree);
    Tree::Node* found = tree.find_by_path(1, "UR");
    assert(found == tree.find(3));
}

// PATH-018
void test_path_relative_mixed() {
    Tree tree;
    fill_b(tree);
    Tree::Node* found = tree.find_by_path(1, "UUR");
    assert(found == tree.find(6));
}

// PATH-019
void test_path_relative_above_root() {
    Tree tree;
    fill_b(tree);
    Tree::Node* found = tree.find_by_path(1, "UUU");
    assert(found == nullptr);
}

// PATH-020
void test_path_relative_missing_child() {
    Tree tree;
    fill_b(tree);
    Tree::Node* found = tree.find_by_path(1, "L");
    assert(found == nullptr);
}

// PATH-022
void test_path_relative_bad_symbol() {
    Tree tree;
    fill_b(tree);
    Tree::Node* found = tree.find_by_path(2, "UX");
    assert(found == nullptr);
}

// PATH-021
void test_path_relative_no_recovery() {
    Tree tree;
    fill_b(tree);
    bool erased = tree.erase(1);
    assert(erased);
    Tree::Node* found = tree.find_by_path(3, "LU");
    assert(found == nullptr);
}

// PATH-023
void test_path_after_rotation() {
    Tree tree;
    const int keys[] = {3,2,1};
    fill_tree(tree, keys, 3);
    assert(tree.find_by_path("")->key == 2);
    assert(tree.find_by_path("L")->key == 1 && tree.find_by_path("R")->key == 3);
}

// PATH-024
void test_path_after_extraction() {
    Tree tree;
    fill_b(tree);
    Tree copy = tree.extract_subtree(2);
    assert(copy.find_by_path(2,"U") == nullptr);
    assert(copy.find_by_path(1,"U")->key == 2);
}

// SEQ-001
void test_seq_reuse_after_operations() {
    Tree tree;
    tree.insert(4,40);
    assert(tree.find(4)->value == 40 && check_tree(tree));
    bool erased = tree.erase(4);
    assert(erased && check_tree(tree));
    tree.insert(2,20);
    assert(tree.find(2)->value == 20 && check_tree(tree));
}

// SEQ-002
void test_seq_copy_move_extract() {
    Tree tree;
    fill_b(tree);
    Tree copy(tree);
    Tree moved(std::move(copy));
    Tree extracted = moved.extract_subtree(2);
    assert(copy.empty() && tree.contains_subtree(extracted) && moved.contains_subtree(extracted));
    assert(extracted.find(1) != tree.find(1) && extracted.find(1) != moved.find(1));
}

// SEQ-003
void test_seq_map_filter_reduce() {
    Tree tree;
    fill_b(tree);
    Tree mapped = tree.map(twice);
    Tree filtered = mapped.where(over_sixty);
    int result = filtered.reduce(0,add);
    assert(result == 440 && tree.find(4)->value == 40 && check_tree(filtered));
}

// SEQ-004
void test_seq_merge_save_load() {
    Tree a,b;
    a.insert(1,10);
    a.insert(2,20);
    b.insert(2,200);
    b.insert(3,30);
    Tree merged = a.merge(b);
    std::string text = merged.to_string();
    Tree restored;
    bool success = restored.from_string(text);
    assert(success && restored.find(2)->value == 20);
    assert_copy(merged,restored);
}

// SEQ-005
void test_seq_failed_then_valid_read() {
    Tree tree;
    tree.insert(99,990);
    bool invalid = tree.from_string("bad");
    assert(!invalid && tree.find(99));
    bool valid = tree.from_string(fixture_text(Order::LNR));
    assert(valid && !tree.find(99) && node_count(tree) == 7 && check_tree(tree));
}

// SEQ-006
void test_seq_reference_operations() {
    Tree tree;
    std::map<int,int> reference;
    std::mt19937 random(20261109);
    for (int i=0;i<200;++i) {
    int key = int(random()%20);
    int operation = int(random()%3);
    if (operation == 0) { tree.insert(key,key*10); reference.emplace(key,key*10); }
    else if (operation == 1) { bool erased = tree.erase(key); bool expected = reference.erase(key) != 0; assert(erased == expected); }
    else assert((tree.find(key) != nullptr) == (reference.count(key) != 0));
    assert(node_count(tree) == int(reference.size()) && check_tree(tree));
    for (const auto& pair : reference) assert(tree.find(pair.first)->value == pair.second);
    }
}
