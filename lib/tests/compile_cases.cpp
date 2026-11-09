#include "../dataTypes.h"
#include <cassert>
#ifdef CHECK_ASSERT
#ifdef NDEBUG
#error assert disabled
#endif
#endif
struct Minimal {
    int value;
    Minimal(int n = 0) : value(n) {}
};
int main() {
#if defined(MERGE_KEY)
    IntegerTree a;
    RedBlackTree<long,int> b;
    a.merge(b);
#elif defined(MERGE_VALUE)
    IntegerTree a;
    RedBlackTree<int,long> b;
    a.merge(b);
#elif defined(MERGE_COMPARE)
    IntegerTree a;
    RedBlackTree<int,int,std::greater<int>> b;
    a.merge(b);
#elif defined(MINIMAL_VALUE)
    RedBlackTree<int,Minimal> tree;
    tree.insert(1,Minimal(2));
    auto node = tree.find(1);
    bool erased = tree.erase(1);
    assert(node && erased);
#elif defined(MISSING_OUTPUT)
    RedBlackTree<int,Minimal> tree;
    tree.to_string();
#elif defined(MISSING_INPUT)
    RedBlackTree<int,Minimal> tree;
    tree.from_string("LNR\n");
#elif defined(MISSING_EQUALITY)
    RedBlackTree<int,Minimal> first,second;
    first.contains_subtree(second);
#endif
}

