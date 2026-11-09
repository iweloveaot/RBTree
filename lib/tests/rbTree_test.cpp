#include "test_helpers.h"


// LIFE-001
void test_life_default_empty() {
    Tree tree;
    assert(tree.empty());
    assert(tree.get_root() == tree.get_nil());
}

// LIFE-002
void test_life_default_sentinel() {
    Tree tree;
    Tree::Node* nil = tree.get_nil();
    assert(nil->color && nil->left == nil && nil->right == nil && nil->parent == nil);
}

// LIFE-003
void test_life_custom_comparator() {
    CustomTree tree(Compare(true));
    tree.insert(1, 10);
    tree.insert(2, 20);
    assert(tree.get_compare().reverse);
    assert(tree.get_root()->left->key == 2);
    assert(check_tree(tree));
}

// LIFE-004
void test_life_node_constructor() {
    Tree tree;
    Tree::Node node(4, 40, tree.get_nil());
    assert(node.key == 4 && node.value == 40 && !node.color);
    assert(node.left == tree.get_nil() && node.right == tree.get_nil() && node.parent == tree.get_nil());
}

// LIFE-005
void test_life_copy_empty() {
    Tree tree;
    Tree copy(tree);
    assert_copy(tree, copy);
    assert(copy.empty());
}

// LIFE-006
void test_life_copy_structure() {
    Tree tree;
    fill_b(tree);
    Tree copy(tree);
    assert_copy(tree, copy);
}

// LIFE-007
void test_life_copy_ownership() {
    Tree tree;
    fill_b(tree);
    Tree copy(tree);
    assert_copy(tree, copy);
    assert(copy.get_root()->parent == copy.get_nil());
}

// LIFE-008
void test_life_copy_independent_value() {
    Tree tree;
    fill_b(tree);
    Tree copy(tree);
    copy.find(2)->value = 999;
    assert(tree.find(2)->value == 20);
}

// LIFE-009
void test_life_copy_independent_erase() {
    Tree tree;
    fill_b(tree);
    Tree copy(tree);
    bool erased = copy.erase(2);
    assert(erased && tree.find(2) && !copy.find(2));
    assert(check_tree(tree) && check_tree(copy));
}

// LIFE-010
void test_life_copy_comparator() {
    CustomTree tree(Compare(true));
    fill_b(tree);
    CustomTree copy(tree);
    assert(copy.get_compare().reverse);
    assert_copy(tree, copy);
}

// LIFE-011
void test_life_copy_assign_to_empty() {
    Tree tree;
    fill_b(tree);
    Tree copy;
    copy = tree;
    assert_copy(tree, copy);
}

// LIFE-012
void test_life_copy_assign_to_nonempty() {
    Tree tree;
    fill_b(tree);
    Tree copy;
    copy.insert(99,990);
    copy = tree;
    assert(!copy.find(99));
    assert_copy(tree, copy);
}

// LIFE-013
void test_life_copy_assign_empty() {
    Tree tree;
    fill_b(tree);
    Tree empty;
    tree = empty;
    assert(tree.empty());
    tree.insert(9,90);
    assert(tree.find(9));
}

// LIFE-014
void test_life_copy_assign_self() {
    Tree tree;
    fill_b(tree);
    Tree::Node* root = tree.get_root();
    tree = tree;
    assert(tree.get_root() == root);
    assert(node_count(tree) == 7 && check_tree(tree));
}

// LIFE-015
void test_life_copy_assign_return() {
    Tree tree;
    fill_b(tree);
    Tree copy;
    Tree* result = &(copy = tree);
    assert(result == &copy);
}

// LIFE-016
void test_life_move_empty() {
    Tree tree;
    Tree moved(std::move(tree));
    assert(tree.empty() && moved.empty());
    assert(tree.get_nil() != moved.get_nil());
}

// LIFE-017
void test_life_move_ownership() {
    Tree tree;
    fill_b(tree);
    Tree::Node* root = tree.get_root();
    Tree::Node* nil = tree.get_nil();
    Tree moved(std::move(tree));
    assert(tree.empty() && moved.get_root() == root && moved.get_nil() == nil);
    assert(check_tree(tree) && check_tree(moved));
}

// LIFE-018
void test_life_move_reuse_source() {
    Tree tree;
    fill_b(tree);
    Tree moved(std::move(tree));
    tree.insert(9,90);
    bool erased = tree.erase(9);
    assert(erased && tree.empty() && node_count(moved) == 7);
}

// LIFE-019
void test_life_move_comparator() {
    CustomTree tree(Compare(true));
    fill_b(tree);
    CustomTree moved(std::move(tree));
    tree.insert(8,80);
    assert(moved.get_compare().reverse);
    assert(check_tree(tree) && check_tree(moved));
}

// LIFE-020
void test_life_move_assign_to_nonempty() {
    Tree tree;
    fill_b(tree);
    Tree target;
    target.insert(99,990);
    Tree::Node* root = tree.get_root();
    target = std::move(tree);
    assert(target.get_root() == root && !target.find(99));
    assert(tree.empty() && check_tree(target));
}

// LIFE-021
void test_life_move_assign_empty() {
    Tree tree;
    fill_b(tree);
    Tree empty;
    tree = std::move(empty);
    assert(tree.empty() && empty.empty());
    assert(check_tree(tree) && check_tree(empty));
}

// LIFE-022
void test_life_move_assign_self() {
    Tree tree;
    fill_b(tree);
    Tree::Node* root = tree.get_root();
    tree = std::move(tree);
    assert(tree.get_root() == root && node_count(tree) == 7 && check_tree(tree));
}

// LIFE-023
void test_life_move_assign_return() {
    Tree tree;
    fill_b(tree);
    Tree target;
    Tree* result = &(target = std::move(tree));
    assert(result == &target && tree.empty());
}

// LIFE-024
void test_life_destroy_empty() {
    int before = Track::alive;
    { TrackTree tree; }
    assert(Track::alive == before);
}

// LIFE-025
void test_life_destroy_nonempty() {
    int before = Track::alive;
    { TrackTree tree;
    for (int i=0;i<7;++i) tree.insert(i, Track(i));
    }
    assert(Track::alive == before);
}

// LIFE-026
void test_life_destroy_copy_after_source() {
    Tree copy;
    { Tree source;
    fill_b(source);
    copy = source;
    }
    assert(copy.find(3)->value == 30 && check_tree(copy));
}

// INS-001
void test_ins_first_node() {
    Tree tree;
    tree.insert(4,40);
    assert(tree.get_root()->key == 4 && tree.get_root()->color);
    assert(check_tree(tree));
}

// INS-002
void test_ins_left_child() {
    Tree tree;
    tree.insert(4,40);
    tree.insert(2,20);
    assert(tree.get_root()->left->key == 2 && check_tree(tree));
}

// INS-003
void test_ins_right_child() {
    Tree tree;
    tree.insert(4,40);
    tree.insert(6,60);
    assert(tree.get_root()->right->key == 6 && check_tree(tree));
}

// INS-004
void test_ins_duplicate_same_value() {
    Tree tree;
    tree.insert(4,40);
    Tree::Node* original = tree.find(4);
    tree.insert(4,40);
    assert(node_count(tree) == 1 && tree.find(4) == original && original->value == 40);
}

// INS-005
void test_ins_duplicate_other_value() {
    Tree tree;
    tree.insert(4,40);
    Tree::Node* original = tree.find(4);
    tree.insert(4,400);
    assert(node_count(tree) == 1 && tree.find(4) == original && original->value == 40);
}

// INS-006
void test_ins_rotate_left_left() {
    Tree tree;
    const int keys[] = {3,2,1};
    fill_tree(tree, keys, 3);
    assert(tree.get_root()->key == 2);
    assert(tree.get_root()->left->key == 1 && tree.get_root()->right->key == 3);
    assert(check_tree(tree));
}

// INS-007
void test_ins_rotate_right_right() {
    Tree tree;
    const int keys[] = {1,2,3};
    fill_tree(tree, keys, 3);
    assert(tree.get_root()->key == 2);
    assert(tree.get_root()->left->key == 1 && tree.get_root()->right->key == 3);
    assert(check_tree(tree));
}

// INS-008
void test_ins_rotate_left_right() {
    Tree tree;
    const int keys[] = {3,1,2};
    fill_tree(tree, keys, 3);
    assert(tree.get_root()->key == 2);
    assert(tree.get_root()->left->key == 1 && tree.get_root()->right->key == 3);
    assert(check_tree(tree));
}

// INS-009
void test_ins_rotate_right_left() {
    Tree tree;
    const int keys[] = {1,3,2};
    fill_tree(tree, keys, 3);
    assert(tree.get_root()->key == 2);
    assert(tree.get_root()->left->key == 1 && tree.get_root()->right->key == 3);
    assert(check_tree(tree));
}

// INS-010
void test_ins_recolor_left() {
    Tree tree;
    const int keys[] = {4,2,6,1};
    fill_tree(tree, keys, 4);
    assert(tree.get_root()->color && tree.find(2)->color && tree.find(6)->color);
    assert(check_tree(tree));
}

// INS-011
void test_ins_recolor_right() {
    Tree tree;
    const int keys[] = {4,2,6,7};
    fill_tree(tree, keys, 4);
    assert(tree.get_root()->color && tree.find(2)->color && tree.find(6)->color);
    assert(check_tree(tree));
}

// INS-012
void test_ins_ascending() {
    Tree tree;
    for (int i=1;i<=15;++i) tree.insert(i, i);
    assert(node_count(tree) == 15 && check_tree(tree));
    assert(tree_height(tree, tree.get_root()) <= 8);
    for (int i=1;i<=15;++i) assert(tree.find(i));
}

// INS-013
void test_ins_descending() {
    Tree tree;
    for (int i=1;i<=15;++i) tree.insert(16-i, i);
    assert(node_count(tree) == 15 && check_tree(tree));
    assert(tree_height(tree, tree.get_root()) <= 8);
    for (int i=1;i<=15;++i) assert(tree.find(i));
}

// INS-014
void test_ins_reverse_compare() {
    CustomTree tree(Compare(true));
    const int keys[] = {1,3,2};
    fill_tree(tree, keys, 3);
    assert(tree.get_root()->left->key == 3 && tree.get_root()->right->key == 1);
    assert(check_tree(tree));
}

// INS-015
void test_ins_equivalent_keys() {
    CustomTree tree(Compare(false,10));
    tree.insert(11,100);
    tree.insert(12,200);
    assert(node_count(tree) == 1 && tree.find(12)->key == 11 && tree.find(12)->value == 100);
}

// DEL-001
void test_del_empty() {
    Tree tree;
    bool erased = tree.erase(4);
    assert(!erased && tree.empty() && check_tree(tree));
}

// DEL-002
void test_del_missing() {
    Tree tree;
    fill_b(tree);
    Tree copy(tree);
    bool erased = tree.erase(99);
    assert(!erased);
    assert_copy(tree, copy);
}

// DEL-003
void test_del_only_node() {
    Tree tree;
    const int keys[] = {4};
    fill_tree(tree, keys, 1);
    assert(check_tree(tree));
    bool erased = tree.erase(4);
    assert(erased && !tree.find(4));
    assert(node_count(tree) == 0 && check_tree(tree));
}

// DEL-004
void test_del_red_leaf() {
    Tree tree;
    const int keys[] = {4,2,6,1,3,5,7};
    fill_tree(tree, keys, 7);
    assert(check_tree(tree));
    assert(!tree.find(1)->color);
    bool erased = tree.erase(1);
    assert(erased && !tree.find(1));
    assert(node_count(tree) == 6 && check_tree(tree));
    assert(tree.find(4)->value == 40);
    assert(tree.find(2)->value == 20);
    assert(tree.find(6)->value == 60);
    assert(tree.find(3)->value == 30);
    assert(tree.find(5)->value == 50);
    assert(tree.find(7)->value == 70);
}

// DEL-005
void test_del_one_left_child() {
    Tree tree;
    const int keys[] = {4,2};
    fill_tree(tree, keys, 2);
    assert(check_tree(tree));
    bool erased = tree.erase(4);
    assert(erased && !tree.find(4));
    assert(node_count(tree) == 1 && check_tree(tree));
    assert(tree.find(2)->value == 20);
}

// DEL-006
void test_del_one_right_child() {
    Tree tree;
    const int keys[] = {4,6};
    fill_tree(tree, keys, 2);
    assert(check_tree(tree));
    bool erased = tree.erase(4);
    assert(erased && !tree.find(4));
    assert(node_count(tree) == 1 && check_tree(tree));
    assert(tree.find(6)->value == 60);
}

// DEL-007
void test_del_two_children_direct_predecessor() {
    Tree tree;
    const int keys[] = {4,2,6};
    fill_tree(tree, keys, 3);
    assert(check_tree(tree));
    bool erased = tree.erase(4);
    assert(erased && !tree.find(4));
    assert(node_count(tree) == 2 && check_tree(tree));
    assert(tree.find(2)->value == 20);
    assert(tree.find(6)->value == 60);
    assert(tree.get_root()->key == 2);
}

// DEL-008
void test_del_two_children_deep_predecessor() {
    Tree tree;
    const int keys[] = {4,2,6,1,3,5,7};
    fill_tree(tree, keys, 7);
    assert(check_tree(tree));
    bool erased = tree.erase(4);
    assert(erased && !tree.find(4));
    assert(node_count(tree) == 6 && check_tree(tree));
    assert(tree.find(2)->value == 20);
    assert(tree.find(6)->value == 60);
    assert(tree.find(1)->value == 10);
    assert(tree.find(3)->value == 30);
    assert(tree.find(5)->value == 50);
    assert(tree.find(7)->value == 70);
    assert(tree.get_root()->key == 3);
}

// DEL-009
void test_del_predecessor_with_left_child() {
    Tree tree;
    const int keys[] = {8,4,12,2,6,10,14,5};
    fill_tree(tree, keys, 8);
    assert(check_tree(tree));
    assert(tree.find(6)->left->key == 5);
    bool erased = tree.erase(8);
    assert(erased && !tree.find(8));
    assert(node_count(tree) == 7 && check_tree(tree));
    assert(tree.find(4)->value == 40);
    assert(tree.find(12)->value == 120);
    assert(tree.find(2)->value == 20);
    assert(tree.find(6)->value == 60);
    assert(tree.find(10)->value == 100);
    assert(tree.find(14)->value == 140);
    assert(tree.find(5)->value == 50);
}

// DEL-012
void test_del_fleft_red_sibling() {
    Tree tree;
    const int keys[] = {2,3,1,6,4,5};
    fill_tree(tree, keys, 6);
    assert(check_tree(tree));
    assert(tree.find(1)->parent->right->color == false);
    bool erased = tree.erase(1);
    assert(erased && !tree.find(1));
    assert(node_count(tree) == 5 && check_tree(tree));
    assert(tree.find(2)->value == 20);
    assert(tree.find(3)->value == 30);
    assert(tree.find(6)->value == 60);
    assert(tree.find(4)->value == 40);
    assert(tree.find(5)->value == 50);
}

// DEL-013
void test_del_fright_red_sibling() {
    Tree tree;
    const int keys[] = {5,6,2,3,4,1};
    fill_tree(tree, keys, 6);
    assert(check_tree(tree));
    assert(tree.find(6)->parent->left->color == false);
    bool erased = tree.erase(6);
    assert(erased && !tree.find(6));
    assert(node_count(tree) == 5 && check_tree(tree));
    assert(tree.find(5)->value == 50);
    assert(tree.find(2)->value == 20);
    assert(tree.find(3)->value == 30);
    assert(tree.find(4)->value == 40);
    assert(tree.find(1)->value == 10);
}

// DEL-014
void test_del_fleft_black_children() {
    Tree tree;
    const int keys[] = {4,2,6,1};
    fill_tree(tree, keys, 4);
    bool prepared = tree.erase(1);
    assert(prepared);
    assert(check_tree(tree));
    assert(tree.find(2)->parent->right->color);
    bool erased = tree.erase(2);
    assert(erased && !tree.find(2));
    assert(node_count(tree) == 2 && check_tree(tree));
    assert(tree.find(4)->value == 40);
    assert(tree.find(6)->value == 60);
}

// DEL-015
void test_del_fright_black_children() {
    Tree tree;
    const int keys[] = {4,2,6,7};
    fill_tree(tree, keys, 4);
    bool prepared = tree.erase(7);
    assert(prepared);
    assert(check_tree(tree));
    assert(tree.find(6)->parent->left->color);
    bool erased = tree.erase(6);
    assert(erased && !tree.find(6));
    assert(node_count(tree) == 2 && check_tree(tree));
    assert(tree.find(4)->value == 40);
    assert(tree.find(2)->value == 20);
}

// DEL-016
void test_del_fleft_near_red() {
    Tree tree;
    const int keys[] = {1,2,4,3};
    fill_tree(tree, keys, 4);
    assert(check_tree(tree));
    assert(!tree.find(4)->left->color && tree.find(4)->right == tree.get_nil());
    bool erased = tree.erase(1);
    assert(erased && !tree.find(1));
    assert(node_count(tree) == 3 && check_tree(tree));
    assert(tree.find(2)->value == 20);
    assert(tree.find(4)->value == 40);
    assert(tree.find(3)->value == 30);
}

// DEL-017
void test_del_fright_near_red() {
    Tree tree;
    const int keys[] = {4,3,1,2};
    fill_tree(tree, keys, 4);
    assert(check_tree(tree));
    assert(!tree.find(1)->right->color && tree.find(1)->left == tree.get_nil());
    bool erased = tree.erase(4);
    assert(erased && !tree.find(4));
    assert(node_count(tree) == 3 && check_tree(tree));
    assert(tree.find(3)->value == 30);
    assert(tree.find(1)->value == 10);
    assert(tree.find(2)->value == 20);
}

// DEL-018
void test_del_fleft_far_red() {
    Tree tree;
    const int keys[] = {3,2,1,4};
    fill_tree(tree, keys, 4);
    assert(check_tree(tree));
    assert(!tree.find(3)->right->color);
    bool erased = tree.erase(1);
    assert(erased && !tree.find(1));
    assert(node_count(tree) == 3 && check_tree(tree));
    assert(tree.find(3)->value == 30);
    assert(tree.find(2)->value == 20);
    assert(tree.find(4)->value == 40);
}

// DEL-019
void test_del_fright_far_red() {
    Tree tree;
    const int keys[] = {3,2,4,1};
    fill_tree(tree, keys, 4);
    assert(check_tree(tree));
    assert(!tree.find(2)->left->color);
    bool erased = tree.erase(4);
    assert(erased && !tree.find(4));
    assert(node_count(tree) == 3 && check_tree(tree));
    assert(tree.find(3)->value == 30);
    assert(tree.find(2)->value == 20);
    assert(tree.find(1)->value == 10);
}

// DEL-010
void test_del_repeat() {
    Tree tree;
    fill_b(tree);
    bool first = tree.erase(1);
    Tree before(tree);
    bool second = tree.erase(1);
    assert(first && !second);
    assert_copy(tree, before);
}

// DEL-011
void test_del_delete_all() {
    Tree tree;
    fill_b(tree);
    const int keys[] = {4,2,6,1,3,5,7};
    for (int key : keys) {
    bool erased = tree.erase(key);
    assert(erased && check_tree(tree));
    }
    assert(tree.empty());
}

// DEL-020
void test_del_reuse_after_emptying() {
    Tree tree;
    tree.insert(4,40);
    bool first = tree.erase(4);
    tree.insert(9,90);
    assert(tree.find(9)->value == 90);
    bool second = tree.erase(9);
    assert(first && second && tree.empty() && check_tree(tree));
}

// FIND-001
void test_find_empty() {
    Tree tree;
    assert(tree.find(4) == nullptr);
}

// FIND-002
void test_find_root() {
    Tree tree;
    fill_b(tree);
    assert(tree.find(4) == tree.get_root() && tree.find(4)->value == 40);
}

// FIND-003
void test_find_left() {
    Tree tree;
    fill_b(tree);
    assert(tree.find(1)->value == 10);
}

// FIND-004
void test_find_right() {
    Tree tree;
    fill_b(tree);
    assert(tree.find(7)->value == 70);
}

// FIND-005
void test_find_missing_low() {
    Tree tree;
    fill_b(tree);
    assert(tree.find(0) == nullptr);
}

// FIND-006
void test_find_missing_high() {
    Tree tree;
    fill_b(tree);
    assert(tree.find(99) == nullptr);
}

// FIND-007
void test_find_const_tree() {
    Tree source;
    fill_b(source);
    const Tree& tree = source;
    assert(!tree.empty() && tree.find(3)->value == 30);
    assert(tree.get_root()->key == 4 && tree.get_nil()->color);
    assert(tree.get_compare()(1,2));
}

// FIND-008
void test_find_empty_after_insert() {
    Tree tree;
    tree.insert(4,40);
    assert(!tree.empty());
}

// FIND-009
void test_find_empty_after_erase() {
    Tree tree;
    tree.insert(4,40);
    bool erased = tree.erase(4);
    assert(erased && tree.empty());
}

// FIND-010
void test_find_root_accessor() {
    Tree tree;
    tree.insert(4,40);
    assert(tree.get_root() == tree.find(4));
}

// FIND-011
void test_find_nil_accessor() {
    Tree a;
    Tree b;
    assert(a.get_nil() && b.get_nil() && a.get_nil() != b.get_nil());
}

// FIND-012
void test_find_comparator_accessor() {
    CustomTree tree(Compare(true));
    assert(&tree.get_compare() == &tree.get_compare());
    assert(tree.get_compare().reverse);
}

// CHK-001
void test_chk_valid() {
    Tree empty;
    Tree single;
    single.insert(4,40);
    Tree tree;
    fill_b(tree);
    CustomTree reversed(Compare(true));
    fill_b(reversed);
    assert(check_tree(empty) && check_tree(single) && check_tree(tree) && check_tree(reversed));
}

// CHK-002
void test_chk_root_red() {
    Tree tree;
    tree.insert(4,40);
    tree.get_root()->color = false;
    assert(!check_tree(tree));
}

// CHK-003
void test_chk_nil_red() {
    Tree tree;
    tree.get_nil()->color = false;
    assert(!check_tree(tree));
    tree.get_nil()->color = true;
}

// CHK-004
void test_chk_parent_link() {
    Tree tree;
    fill_b(tree);
    tree.find(1)->parent = tree.get_nil();
    assert(!check_tree(tree));
}

// CHK-005
void test_chk_ancestor_key() {
    Tree tree;
    fill_b(tree);
    tree.find(3)->key = 5;
    assert(!check_tree(tree));
}

// CHK-006
void test_chk_red_pair() {
    Tree tree;
    fill_b(tree);
    tree.find(2)->color = false;
    assert(!check_tree(tree));
}

// CHK-007
void test_chk_black_height() {
    Tree tree;
    tree.insert(4,40);
    tree.insert(2,20);
    tree.find(2)->color = true;
    assert(!check_tree(tree));
}

// CHK-008
void test_chk_repeated_pointer() {
    Tree tree;
    fill_b(tree);
    Tree::Node* old = tree.get_root()->right;
    tree.get_root()->right = tree.get_root()->left;
    bool valid = check_tree(tree);
    tree.get_root()->right = old;
    assert(!valid);
}
