#include "test_helpers.h"

struct SerializedRecord {
    std::string path;
    bool color;
    std::string key;
    std::string value;
};
std::string read_serialized_field(std::istream& input) {
    std::size_t length = 0;
    input >> length;
    assert(input);
    int colon = input.get();
    assert(colon == ':');
    std::string text(length,'\0');
    if (length) input.read(&text[0], length);
    assert(input);
    return text;
}
SerializedRecord read_serialized_record(std::istream& input) {
    SerializedRecord record;
    input >> record.path >> record.color;
    int first_space = input.get();
    assert(first_space == ' ');
    record.key = read_serialized_field(input);
    int second_space = input.get();
    assert(second_space == ' ');
    record.value = read_serialized_field(input);
    int newline = input.get();
    assert(newline == '\n');
    return record;
}
struct Printable {
    int value;
    Printable(int v = 0) : value(v) {}
};
inline std::ostream& operator<<(std::ostream& stream, const Printable& value) {
    return stream << "item:" << value.value << "\nend";
}

// SAVE-001
void test_save_empty_nlr() {
    Tree tree;
    std::string text = tree.to_string(Order::NLR);
    assert(text == "NLR\n");
}

// SAVE-003
void test_save_nlr() {
    Tree tree;
    fill_b(tree);
    std::string text = tree.to_string(Order::NLR);
    assert(text == fixture_text(Order::NLR));
}

// LOAD-001
void test_load_empty_nlr() {
    Tree tree;
    tree.insert(99,990);
    bool success = tree.from_string("NLR\n");
    assert(success && tree.empty() && check_tree(tree));
}

// LOAD-003
void test_load_nlr() {
    Tree tree;
    bool success = tree.from_string(fixture_text(Order::NLR));
    Tree expected;
    fill_b(expected);
    assert(success);
    assert_copy(expected,tree);
}

// SAVE-001
void test_save_empty_nrl() {
    Tree tree;
    std::string text = tree.to_string(Order::NRL);
    assert(text == "NRL\n");
}

// SAVE-004
void test_save_nrl() {
    Tree tree;
    fill_b(tree);
    std::string text = tree.to_string(Order::NRL);
    assert(text == fixture_text(Order::NRL));
}

// LOAD-001
void test_load_empty_nrl() {
    Tree tree;
    tree.insert(99,990);
    bool success = tree.from_string("NRL\n");
    assert(success && tree.empty() && check_tree(tree));
}

// LOAD-004
void test_load_nrl() {
    Tree tree;
    bool success = tree.from_string(fixture_text(Order::NRL));
    Tree expected;
    fill_b(expected);
    assert(success);
    assert_copy(expected,tree);
}

// SAVE-001
void test_save_empty_lnr() {
    Tree tree;
    std::string text = tree.to_string(Order::LNR);
    assert(text == "LNR\n");
}

// SAVE-005
void test_save_lnr() {
    Tree tree;
    fill_b(tree);
    std::string text = tree.to_string(Order::LNR);
    assert(text == fixture_text(Order::LNR));
}

// LOAD-001
void test_load_empty_lnr() {
    Tree tree;
    tree.insert(99,990);
    bool success = tree.from_string("LNR\n");
    assert(success && tree.empty() && check_tree(tree));
}

// LOAD-005
void test_load_lnr() {
    Tree tree;
    bool success = tree.from_string(fixture_text(Order::LNR));
    Tree expected;
    fill_b(expected);
    assert(success);
    assert_copy(expected,tree);
}

// SAVE-001
void test_save_empty_rnl() {
    Tree tree;
    std::string text = tree.to_string(Order::RNL);
    assert(text == "RNL\n");
}

// SAVE-006
void test_save_rnl() {
    Tree tree;
    fill_b(tree);
    std::string text = tree.to_string(Order::RNL);
    assert(text == fixture_text(Order::RNL));
}

// LOAD-001
void test_load_empty_rnl() {
    Tree tree;
    tree.insert(99,990);
    bool success = tree.from_string("RNL\n");
    assert(success && tree.empty() && check_tree(tree));
}

// LOAD-006
void test_load_rnl() {
    Tree tree;
    bool success = tree.from_string(fixture_text(Order::RNL));
    Tree expected;
    fill_b(expected);
    assert(success);
    assert_copy(expected,tree);
}

// SAVE-001
void test_save_empty_lrn() {
    Tree tree;
    std::string text = tree.to_string(Order::LRN);
    assert(text == "LRN\n");
}

// SAVE-007
void test_save_lrn() {
    Tree tree;
    fill_b(tree);
    std::string text = tree.to_string(Order::LRN);
    assert(text == fixture_text(Order::LRN));
}

// LOAD-001
void test_load_empty_lrn() {
    Tree tree;
    tree.insert(99,990);
    bool success = tree.from_string("LRN\n");
    assert(success && tree.empty() && check_tree(tree));
}

// LOAD-007
void test_load_lrn() {
    Tree tree;
    bool success = tree.from_string(fixture_text(Order::LRN));
    Tree expected;
    fill_b(expected);
    assert(success);
    assert_copy(expected,tree);
}

// SAVE-001
void test_save_empty_rln() {
    Tree tree;
    std::string text = tree.to_string(Order::RLN);
    assert(text == "RLN\n");
}

// SAVE-008
void test_save_rln() {
    Tree tree;
    fill_b(tree);
    std::string text = tree.to_string(Order::RLN);
    assert(text == fixture_text(Order::RLN));
}

// LOAD-001
void test_load_empty_rln() {
    Tree tree;
    tree.insert(99,990);
    bool success = tree.from_string("RLN\n");
    assert(success && tree.empty() && check_tree(tree));
}

// LOAD-008
void test_load_rln() {
    Tree tree;
    bool success = tree.from_string(fixture_text(Order::RLN));
    Tree expected;
    fill_b(expected);
    assert(success);
    assert_copy(expected,tree);
}

// SAVE-002
void test_save_single() {
    Tree tree;
    tree.insert(4,40);
    assert(tree.to_string() == "LNR\n. 1 1:4 2:40\n");
}

// SAVE-009
void test_save_default_order() {
    Tree tree;
    fill_b(tree);
    assert(tree.to_string() == fixture_text(Order::LNR));
}

// SAVE-010
void test_save_paths_colors() {
    Tree tree;
    fill_a(tree);
    std::string expected = "LNR\nLL 0 1:1 2:10\nL 1 1:2 2:20\nLR 0 1:3 2:30\n. 1 1:4 2:40\nR 1 1:6 2:60\n";
    assert(tree.to_string() == expected);
}

// SAVE-011
void test_save_empty_string_fields() {
    StringTree tree;
    tree.insert("","");
    assert(tree.to_string() == "LNR\n. 1 0: 0:\n");
}

// SAVE-012
void test_save_special_bytes_space() {
    StringTree tree;
    std::string value = " a b ";
    tree.insert(value,value);
    std::string expected = "LNR\n" + record_text(".",1,value,value);
    assert(tree.to_string() == expected);
}

// LOAD-013
void test_load_string_bytes_space() {
    StringTree tree;
    std::string value = " a b ";
    bool success = tree.from_string("LNR\n" + record_text(".",1,value,value));
    assert(success && tree.find(value)->value == value && check_tree(tree));
}

// SAVE-012
void test_save_special_bytes_colon() {
    StringTree tree;
    std::string value = "a:b";
    tree.insert(value,value);
    std::string expected = "LNR\n" + record_text(".",1,value,value);
    assert(tree.to_string() == expected);
}

// LOAD-013
void test_load_string_bytes_colon() {
    StringTree tree;
    std::string value = "a:b";
    bool success = tree.from_string("LNR\n" + record_text(".",1,value,value));
    assert(success && tree.find(value)->value == value && check_tree(tree));
}

// SAVE-012
void test_save_special_bytes_newline() {
    StringTree tree;
    std::string value = "a\nb";
    tree.insert(value,value);
    std::string expected = "LNR\n" + record_text(".",1,value,value);
    assert(tree.to_string() == expected);
}

// LOAD-013
void test_load_string_bytes_newline() {
    StringTree tree;
    std::string value = "a\nb";
    bool success = tree.from_string("LNR\n" + record_text(".",1,value,value));
    assert(success && tree.find(value)->value == value && check_tree(tree));
}

// SAVE-012
void test_save_special_bytes_zero() {
    StringTree tree;
    std::string value = std::string("a\0b",3);
    tree.insert(value,value);
    std::string expected = "LNR\n" + record_text(".",1,value,value);
    assert(tree.to_string() == expected);
}

// LOAD-013
void test_load_string_bytes_zero() {
    StringTree tree;
    std::string value = std::string("a\0b",3);
    bool success = tree.from_string("LNR\n" + record_text(".",1,value,value));
    assert(success && tree.find(value)->value == value && check_tree(tree));
}

// LOAD-013
void test_load_string_bytes_empty() {
    StringTree tree;
    bool success = tree.from_string("LNR\n. 1 0: 0:\n");
    assert(success && tree.find("")->value.empty());
}

// LOAD-013
void test_load_string_bytes_quote() {
    StringTree tree;
    std::string value = "a\"b";
    bool success = tree.from_string("LNR\n" + record_text(".",1,value,value));
    assert(success && tree.find(value)->value == value);
}

// SAVE-013
void test_save_utf8_lengths() {
    StringTree tree;
    std::string value = "\xD0\x98";
    tree.insert(value,value);
    std::string expected = "LNR\n. 1 2:" + value + " 2:" + value + "\n";
    assert(tree.to_string() == expected);
}

// SAVE-014
void test_save_floating_precision() {
    RealTree tree;
    double value = 1.2345678901234567;
    tree.insert(value, std::numeric_limits<double>::max());
    std::istringstream input(tree.to_string());
    std::string header;
    std::getline(input, header);
    SerializedRecord record = read_serialized_record(input);
    double key = 0, restored = 0;
    std::istringstream(record.key) >> key;
    std::istringstream(record.value) >> restored;
    assert(key == value && restored == std::numeric_limits<double>::max());
}

// SAVE-015
void test_save_custom_output() {
    RedBlackTree<int,Printable> tree;
    tree.insert(1,Printable(42));
    std::string expected = "LNR\n" + record_text(".",1,"1","item:42\nend");
    assert(tree.to_string() == expected);
}

// SAVE-016
void test_save_invalid_order() {
    Tree tree;
    fill_b(tree);
    Tree before(tree);
    std::string text = tree.to_string(static_cast<Order>(99));
    assert(text.empty());
    assert_copy(tree,before);
}

// LOAD-002
void test_load_single() {
    Tree tree;
    bool success = tree.from_string("LNR\n. 1 1:4 2:40\n");
    assert(success && tree.find(4)->value == 40 && tree.get_root()->color && check_tree(tree));
}

// LOAD-009
void test_load_replace_nonempty() {
    Tree tree;
    tree.insert(99,990);
    bool success = tree.from_string("LNR\n. 1 1:4 2:40\n");
    assert(success && !tree.find(99) && tree.find(4)->value == 40);
}

// LOAD-010
void test_load_own_sentinel() {
    Tree source;
    fill_b(source);
    Tree restored;
    bool success = restored.from_string(fixture_text(Order::NLR));
    assert(success);
    assert_copy(source,restored);
}

// LOAD-011
void test_load_read_then_edit() {
    Tree tree;
    bool success = tree.from_string(fixture_text(Order::LNR));
    tree.insert(9,90);
    bool erased = tree.erase(2);
    assert(success && erased && tree.find(9)->value == 90 && check_tree(tree));
    assert(tree.find_by_path("") == tree.get_root());
}

// LOAD-012
void test_load_reverse_comparator() {
    CustomTree source(Compare(true));
    fill_b(source);
    CustomTree tree(Compare(true));
    bool success = tree.from_string(source.to_string());
    assert(success && tree.get_compare().reverse);
    assert_copy(source,tree);
}

// LOAD-014
void test_load_char_value_space() {
    RedBlackTree<int,char> tree;
    char value = ' ';
    bool success = tree.from_string("LNR\n" + record_text(".",1,"1",std::string(1,value)));
    assert(success && tree.find(1)->value == value);
}

// LOAD-014
void test_load_char_value_newline() {
    RedBlackTree<int,char> tree;
    char value = '\n';
    bool success = tree.from_string("LNR\n" + record_text(".",1,"1",std::string(1,value)));
    assert(success && tree.find(1)->value == value);
}

// LOAD-014
void test_load_char_value_zero() {
    RedBlackTree<int,char> tree;
    char value = '\0';
    bool success = tree.from_string("LNR\n" + record_text(".",1,"1",std::string(1,value)));
    assert(success && tree.find(1)->value == value);
}

// LOAD-015
void test_load_numeric_whitespace() {
    Tree tree;
    bool success = tree.from_string("LNR\n" + record_text(".",1," 4 "," 40 "));
    assert(success && tree.find(4)->value == 40);
}

// BAD-001
void test_bad_empty_input() {
    reject_tree_text("");
}

// BAD-002
void test_bad_short_header_1() {
    reject_tree_text("L");
}

// BAD-002
void test_bad_short_header_2() {
    reject_tree_text("LN");
}

// BAD-002
void test_bad_short_header_3() {
    reject_tree_text("LNR");
}

// BAD-003
void test_bad_header_separator() {
    reject_tree_text("LNR ");
}

// BAD-004
void test_bad_unknown_header() {
    reject_tree_text("XYZ\n");
}

// BAD-005
void test_bad_blank_record() {
    reject_tree_text("LNR\n\n");
}

// BAD-006
void test_bad_missing_path_separator() {
    reject_tree_text("LNR\n.");
}

// BAD-007
void test_bad_empty_path() {
    reject_tree_text("LNR\n 1 1:4 2:40\n");
}

// BAD-008
void test_bad_invalid_path() {
    reject_tree_text("LNR\nLX 1 1:4 2:40\n");
}

// BAD-009
void test_bad_dot_inside_path() {
    reject_tree_text("LNR\n.L 1 1:4 2:40\n");
}

// BAD-011
void test_bad_missing_color() {
    reject_tree_text("LNR\n. ");
}

// BAD-012
void test_bad_invalid_color() {
    reject_tree_text("LNR\n. 2 1:4 2:40\n");
}

// BAD-013
void test_bad_missing_color_separator() {
    reject_tree_text("LNR\n. 1");
}

// BAD-014
void test_bad_wrong_color_separator() {
    reject_tree_text("LNR\n. 1:1:4 2:40\n");
}

// BAD-015
void test_bad_missing_length_key() {
    reject_tree_text("LNR\n. 1 ");
}

// BAD-015
void test_bad_missing_length_value() {
    reject_tree_text("LNR\n. 1 1:4 ");
}

// BAD-016
void test_bad_nondigit_length_key() {
    reject_tree_text("LNR\n. 1 x:4 2:40\n");
}

// BAD-016
void test_bad_nondigit_length_value() {
    reject_tree_text("LNR\n. 1 1:4 x:40\n");
}

// BAD-017
void test_bad_negative_length_key() {
    reject_tree_text("LNR\n. 1 -1:4 2:40\n");
}

// BAD-017
void test_bad_negative_length_value() {
    reject_tree_text("LNR\n. 1 1:4 -2:40\n");
}

// BAD-018
void test_bad_length_overflow_key() {
    reject_tree_text("LNR\n. 1 999999999999999999999999999999:4 2:40\n");
}

// BAD-018
void test_bad_length_overflow_value() {
    reject_tree_text("LNR\n. 1 1:4 99999999999999999999999999999:40\n");
}

// BAD-019
void test_bad_missing_colon_key() {
    reject_tree_text("LNR\n. 1 1");
}

// BAD-019
void test_bad_missing_colon_value() {
    reject_tree_text("LNR\n. 1 1:4 2");
}

// BAD-020
void test_bad_wrong_colon_key() {
    reject_tree_text("LNR\n. 1 1.4 2:40\n");
}

// BAD-020
void test_bad_wrong_colon_value() {
    reject_tree_text("LNR\n. 1 1:4 2.40\n");
}

// BAD-021
void test_bad_truncated_field_key() {
    reject_tree_text("LNR\n. 1 999:4 2:40\n");
}

// BAD-021
void test_bad_truncated_field_value() {
    reject_tree_text("LNR\n. 1 1:4 999:40\n");
}

// BAD-022
void test_bad_missing_key_separator() {
    reject_tree_text("LNR\n. 1 1:4");
}

// BAD-023
void test_bad_wrong_key_separator() {
    reject_tree_text("LNR\n. 1 1:4:2:40\n");
}

// BAD-024
void test_bad_missing_record_end() {
    reject_tree_text("LNR\n. 1 1:4 2:40");
}

// BAD-025
void test_bad_wrong_record_end() {
    reject_tree_text("LNR\n. 1 1:4 2:40!");
}

// BAD-026
void test_bad_invalid_key() {
    reject_tree_text("LNR\n. 1 3:abc 2:40\n");
}

// BAD-027
void test_bad_invalid_value() {
    reject_tree_text("LNR\n. 1 1:4 3:abc\n");
}

// BAD-028
void test_bad_key_overflow() {
    reject_tree_text("LNR\n. 1 30:999999999999999999999999999999 2:40\n");
}

// BAD-029
void test_bad_value_overflow() {
    reject_tree_text("LNR\n. 1 1:4 30:999999999999999999999999999999\n");
}

// BAD-030
void test_bad_key_tail() {
    reject_tree_text("LNR\n. 1 5:12abc 2:40\n");
}

// BAD-031
void test_bad_value_tail() {
    reject_tree_text("LNR\n. 1 1:4 5:12abc\n");
}

// BAD-032
void test_bad_empty_numeric_field_key() {
    reject_tree_text("LNR\n. 1 0: 2:40\n");
}

// BAD-032
void test_bad_empty_numeric_field_value() {
    reject_tree_text("LNR\n. 1 1:4 0:\n");
}

// BAD-035
void test_bad_missing_root() {
    reject_tree_text("LNR\nL 0 1:2 2:20\n");
}

// BAD-036
void test_bad_missing_parent() {
    reject_tree_text("LNR\nLL 0 1:1 2:10\n. 1 1:4 2:40\n");
}

// BAD-037
void test_bad_duplicate_root() {
    reject_tree_text("LNR\n. 1 1:4 2:40\n. 1 1:4 2:40\n");
}

// BAD-038
void test_bad_duplicate_path() {
    reject_tree_text("LNR\nL 0 1:2 2:20\nL 0 1:2 2:20\n. 1 1:4 2:40\n");
}

// BAD-040
void test_bad_unconsumed_record() {
    reject_tree_text("LNR\n. 1 1:4 2:40\nL 0 1:2 2:20\n");
}

// BAD-042
void test_bad_red_root() {
    reject_tree_text("LNR\n. 0 1:4 2:40\n");
}

// BAD-043
void test_bad_red_left_child() {
    reject_tree_text("LNR\nLL 0 1:1 2:10\nL 0 1:2 2:20\n. 1 1:4 2:40\n");
}

// BAD-044
void test_bad_red_right_child() {
    reject_tree_text("LNR\n. 1 1:4 2:40\nR 0 1:6 2:60\nRR 0 1:7 2:70\n");
}

// BAD-045
void test_bad_black_height_left() {
    reject_tree_text("LNR\nL 1 1:2 2:20\n. 1 1:4 2:40\n");
}

// BAD-046
void test_bad_black_height_right() {
    reject_tree_text("LNR\n. 1 1:4 2:40\nR 1 1:6 2:60\n");
}

// BAD-047
void test_bad_left_key_order() {
    reject_tree_text("LNR\nL 0 1:6 2:60\n. 1 1:4 2:40\n");
}

// BAD-048
void test_bad_right_key_order() {
    reject_tree_text("LNR\n. 1 1:4 2:40\nR 0 1:2 2:20\n");
}

// BAD-050
void test_bad_duplicate_key() {
    reject_tree_text("LNR\nL 0 1:4 2:40\n. 1 1:4 2:40\n");
}

// BAD-010
void test_bad_excessive_depth() {
    reject_tree_text("LRN\n" + std::string(10000, 'L') + " 0 1:1 2:10\n");
}

// BAD-039
void test_bad_wrong_record_order_nlr() {
    std::string text = fixture_text(Order::NLR);
    text.replace(0,3,"NRL");
    reject_tree_text(text);
}

// BAD-039
void test_bad_wrong_record_order_nrl() {
    std::string text = fixture_text(Order::NRL);
    text.replace(0,3,"LNR");
    reject_tree_text(text);
}

// BAD-039
void test_bad_wrong_record_order_lnr() {
    std::string text = fixture_text(Order::LNR);
    text.replace(0,3,"RNL");
    reject_tree_text(text);
}

// BAD-039
void test_bad_wrong_record_order_rnl() {
    std::string text = fixture_text(Order::RNL);
    text.replace(0,3,"LRN");
    reject_tree_text(text);
}

// BAD-039
void test_bad_wrong_record_order_lrn() {
    std::string text = fixture_text(Order::LRN);
    text.replace(0,3,"RLN");
    reject_tree_text(text);
}

// BAD-039
void test_bad_wrong_record_order_rln() {
    std::string text = fixture_text(Order::RLN);
    text.replace(0,3,"NLR");
    reject_tree_text(text);
}

// BAD-041
void test_bad_trailing_garbage() {
    reject_tree_text(fixture_text(Order::LNR) + "garbage");
}

// BAD-049
void test_bad_ancestor_bound() {
    std::string text = fixture_text(Order::LNR);
    std::string old = "LR 0 1:3";
    std::size_t position = text.find(old);
    assert(position != std::string::npos);
    text.replace(position,old.size(),"LR 0 1:5");
    reject_tree_text(text);
}

// BAD-033
void test_bad_char_empty() {
    RedBlackTree<int,char> tree;
    tree.insert(99,'x');
    bool success = tree.from_string("LNR\n" + record_text(".",1,"4",""));
    assert(!success && tree.find(99)->value == 'x');
}

// BAD-034
void test_bad_char_multiple_bytes() {
    RedBlackTree<int,char> tree;
    tree.insert(99,'x');
    bool success = tree.from_string("LNR\n" + record_text(".",1,"4","ab"));
    assert(!success && tree.find(99)->value == 'x');
}

// BAD-051
void test_bad_comparator_equivalence() {
    CustomTree tree(Compare(false,10));
    tree.insert(99,990);
    bool success = tree.from_string("LNR\nL 0 2:11 1:1\n. 1 2:12 1:2\n");
    assert(!success && tree.find(99)->value == 990);
}

// BAD-052
void test_bad_wrong_comparator() {
    CustomTree source(Compare(true));
    fill_b(source);
    CustomTree tree;
    tree.insert(99,990);
    bool success = tree.from_string(source.to_string());
    assert(!success && tree.find(99)->value == 990);
}

// BAD-053
void test_bad_invalid_student() {
    StudentTree tree;
    Student value;
    tree.insert("old",value);
    std::string payload = output_text(Person()) + " \"A\" -1";
    bool success = tree.from_string("LNR\n" + record_text(".",1,"new",payload));
    assert(!success && tree.find("old")->value == value);
}

// BAD-054
void test_bad_invalid_teacher_date() {
    TeacherTree tree;
    Teacher value;
    tree.insert("old",value);
    std::string payload = "\"1\" \"A\" \"\" \"B\" 31 2 2024 \"Math\" \"Lecturer\"";
    bool success = tree.from_string("LNR\n" + record_text(".",1,"new",payload));
    assert(!success && tree.find("old")->value == value);
}

// BAD-055
void test_bad_unknown_function() {
    FunctionTree tree;
    tree.insert(99,FunctionValue(1));
    bool success = tree.from_string("LNR\n" + record_text(".",1,"4","99"));
    assert(!success && tree.find(99)->value.get_id() == 1);
}

// BAD-056
void test_bad_damaged_quoted_name() {
    StudentTree tree;
    Student value;
    tree.insert("old",value);
    bool success = tree.from_string("LNR\n" + record_text(".",1,"new","\"unfinished"));
    assert(!success && tree.find("old")->value == value);
}

// EXC-001
void test_exc_copy_constructor() {
    TrackTree source;
    fill_b(source);
    int before = Track::alive;
    Track::copies_left = 3;
    bool threw = false;
    try { TrackTree copy(source); }
    catch (const std::runtime_error&) { threw = true; }
    Track::copies_left = -1;
    assert(threw && Track::alive == before);
    assert(source.find(4)->value.value == 40);
}

// EXC-002
void test_exc_copy_assignment() {
    TrackTree source;
    fill_b(source);
    TrackTree target;
    target.insert(99,Track(990));
    int before = Track::alive;
    Track::copies_left = 3;
    bool threw = false;
    try { target = source; }
    catch (const std::runtime_error&) { threw = true; }
    Track::copies_left = -1;
    assert(threw && Track::alive == before);
    assert(target.find(99)->value.value == 990 && node_count(target) == 1);
}

// EXC-003
void test_exc_move_constructor_comparator() {
    CompareTrackTree source;
    fill_b(source);
    auto root = source.get_root();
    int before = Track::alive;
    TrackCompare::copies_left = 0;
    bool threw = false;
    try { CompareTrackTree moved(std::move(source)); }
    catch (const std::runtime_error&) { threw = true; }
    TrackCompare::copies_left = -1;
    assert(threw && Track::alive == before && source.get_root() == root);
}

// EXC-004
void test_exc_insert_key_copy() {
    RedBlackTree<Track,int,TrackKeyCompare> tree;
    Track key(4);
    tree.insert(key,40);
    int before = Track::alive;
    Track::copies_left = 0;
    bool threw = false;
    try { tree.insert(key,400); }
    catch (const std::runtime_error&) { threw = true; }
    Track::copies_left = -1;
    assert(threw && Track::alive == before && node_count(tree) == 1);
}

// EXC-005
void test_exc_insert_value_copy() {
    TrackTree tree;
    tree.insert(4,Track(40));
    Track value(50);
    int before = Track::alive;
    Track::copies_left = 0;
    bool threw = false;
    try { tree.insert(5,value); }
    catch (const std::runtime_error&) { threw = true; }
    Track::copies_left = -1;
    assert(threw && Track::alive == before && !tree.find(5));
}

// EXC-006
void test_exc_insert_compare() {
    CompareTrackTree tree;
    tree.insert(4,Track(40));
    int before = Track::alive;
    TrackCompare::calls_left = 0;
    bool threw = false;
    try { tree.insert(5,Track(50)); }
    catch (const std::runtime_error&) { threw = true; }
    TrackCompare::calls_left = -1;
    assert(threw && !tree.find(5) && tree.find(4)->value.value == 40);
    assert(Track::alive == before);
}

// EXC-007
void test_exc_merge_first() {
    TrackTree first,second;
    for (int i=0;i<3;++i) first.insert(i,Track(i));
    for (int i=3;i<6;++i) second.insert(i,Track(i));
    int before = Track::alive;
    Track::copies_left = 2;
    bool threw = false;
    try { first.merge(second); }
    catch (const std::runtime_error&) { threw = true; }
    Track::copies_left = -1;
    assert(threw && Track::alive == before);
    assert(node_count(first) == 3 && node_count(second) == 3);
}

// EXC-008
void test_exc_merge_second() {
    TrackTree first,second;
    for (int i=0;i<3;++i) first.insert(i,Track(i));
    for (int i=3;i<6;++i) second.insert(i,Track(i));
    int before = Track::alive;
    Track::copies_left = 5;
    bool threw = false;
    try { first.merge(second); }
    catch (const std::runtime_error&) { threw = true; }
    Track::copies_left = -1;
    assert(threw && Track::alive == before);
    assert(node_count(first) == 3 && node_count(second) == 3);
}

// EXC-009
void test_exc_extract_copy() {
    TrackTree tree;
    fill_b(tree);
    int before = Track::alive;
    Track::copies_left = 3;
    bool threw = false;
    try { tree.extract_subtree(4); }
    catch (const std::runtime_error&) { threw = true; }
    Track::copies_left = -1;
    assert(threw && Track::alive == before && node_count(tree) == 7);
}

// EXC-010
void test_exc_subtree_equality() {
    TrackTree tree;
    fill_b(tree);
    TrackTree copy(tree);
    int before = Track::alive;
    Track::throw_equality = true;
    bool threw = false;
    try { tree.contains_subtree(copy); }
    catch (const std::runtime_error&) { threw = true; }
    Track::throw_equality = false;
    assert(threw && Track::alive == before && node_count(tree) == 7 && node_count(copy) == 7);
}

// EXC-011
void test_exc_save_output() {
    TrackTree tree;
    fill_b(tree);
    int before = Track::alive;
    Track::throw_output = true;
    bool threw = false;
    try { tree.to_string(); }
    catch (const std::runtime_error&) { threw = true; }
    Track::throw_output = false;
    assert(threw && Track::alive == before && tree.find(4)->value.value == 40);
}

// EXC-012
void test_exc_read_conversion() {
    TrackTree tree;
    tree.insert(99,Track(990));
    int before = Track::alive;
    Track::reads_left = 2;
    bool threw = false;
    try { tree.from_string(fixture_text(Order::LNR)); }
    catch (const std::runtime_error&) { threw = true; }
    Track::reads_left = -1;
    assert(threw && Track::alive == before && tree.find(99)->value.value == 990);
}

// EXC-013
void test_exc_read_node_copy_nlr() {
    TrackTree tree;
    tree.insert(99,Track(990));
    std::string text = fixture_text(Order::NLR);
    int before = Track::alive;
    Track::copies_left = 3;
    bool threw = false;
    try { tree.from_string(text); }
    catch (const std::runtime_error&) { threw = true; }
    Track::copies_left = -1;
    assert(threw && Track::alive == before && tree.find(99)->value.value == 990);
}

// EXC-013
void test_exc_read_node_copy_lnr() {
    TrackTree tree;
    tree.insert(99,Track(990));
    std::string text = fixture_text(Order::LNR);
    int before = Track::alive;
    Track::copies_left = 3;
    bool threw = false;
    try { tree.from_string(text); }
    catch (const std::runtime_error&) { threw = true; }
    Track::copies_left = -1;
    assert(threw && Track::alive == before && tree.find(99)->value.value == 990);
}

// EXC-013
void test_exc_read_node_copy_lrn() {
    TrackTree tree;
    tree.insert(99,Track(990));
    std::string text = fixture_text(Order::LRN);
    int before = Track::alive;
    Track::copies_left = 3;
    bool threw = false;
    try { tree.from_string(text); }
    catch (const std::runtime_error&) { threw = true; }
    Track::copies_left = -1;
    assert(threw && Track::alive == before && tree.find(99)->value.value == 990);
}

// EXC-014
void test_exc_read_validation_compare() {
    CompareTrackTree tree;
    tree.insert(99,Track(990));
    std::string text = fixture_text(Order::LNR);
    int before = Track::alive;
    TrackCompare::calls_left = 0;
    bool threw = false;
    try { tree.from_string(text); }
    catch (const std::runtime_error&) { threw = true; }
    TrackCompare::calls_left = -1;
    assert(threw && Track::alive == before && tree.find(99)->value.value == 990);
}

// EXC-015
void test_exc_read_invalid_after_children() {
    TrackTree tree;
    tree.insert(99,Track(990));
    std::string text = "LRN\nL 0 1:2 2:20\nR 0 1:6 2:60\n. 1 1:4 3:bad\n";
    int before = Track::alive;
    bool success = tree.from_string(text);
    assert(!success && Track::alive == before && tree.find(99)->value.value == 990);
}

// EXC-016
void test_exc_read_invalid_black_height() {
    TrackTree tree;
    tree.insert(99,Track(990));
    int before = Track::alive;
    bool success = tree.from_string("LNR\nL 1 1:2 2:20\n. 1 1:4 2:40\n");
    assert(!success && Track::alive == before && tree.find(99)->value.value == 990);
}

// EXC-017
void test_exc_allocation_failure_create() {
    bool threw = false;
    fail_next_allocation();
    try { Tree tree; }
    catch (const std::bad_alloc&) { threw = true; }
    allow_allocations();
    assert(threw);
}

// EXC-017
void test_exc_allocation_failure_copy() {
    Tree source;
    fill_b(source);
    bool threw = false;
    fail_next_allocation();
    try { Tree copy(source); }
    catch (const std::bad_alloc&) { threw = true; }
    allow_allocations();
    assert(threw);
    assert(node_count(source) == 7 && check_tree(source));
}

// EXC-017
void test_exc_allocation_failure_read() {
    Tree source;
    source.insert(99,990);
    std::string text = fixture_text(Order::LNR);
    bool threw = false;
    fail_next_allocation();
    try { source.from_string(text); }
    catch (const std::bad_alloc&) { threw = true; }
    allow_allocations();
    assert(threw);
    assert(source.find(99)->value == 990 && check_tree(source));
}

// EXC-006
void test_exc_insert_compare_second() {
    CompareTrackTree tree;
    tree.insert(4,Track(40));
    int before = Track::alive;
    TrackCompare::calls_left = 1;
    bool threw = false;
    try { tree.insert(5,Track(50)); }
    catch (const std::runtime_error&) { threw = true; }
    TrackCompare::calls_left = -1;
    assert(threw && !tree.find(5) && tree.find(4)->value.value == 40);
    assert(Track::alive == before);
}

// EXC-006
void test_exc_insert_compare_link() {
    CompareTrackTree tree;
    tree.insert(4,Track(40));
    int before = Track::alive;
    TrackCompare::calls_left = 2;
    bool threw = false;
    try { tree.insert(5,Track(50)); }
    catch (const std::runtime_error&) { threw = true; }
    TrackCompare::calls_left = -1;
    assert(threw && !tree.find(5) && tree.find(4)->value.value == 40);
    assert(Track::alive == before);
}
