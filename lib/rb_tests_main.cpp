#include "dataTypes.h"
#include "rb_tree_tests.h"

int main() {
    run_rb_tree_tests();
    return rb_tests_failed == 0 ? 0 : 1;
}
