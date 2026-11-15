#ifndef TRAVERSALS_H
#define TRAVERSALS_H

namespace traversal {

struct LNR {
    template <typename V, typename L, typename R>
    void operator()(const V& v, const L& l, const R& r) const {
        l(); v(); r();
    }
};

struct NLR {
    template <typename V, typename L, typename R>
    void operator()(const V& v, const L& l, const R& r) const {
        v(); l(); r();
    }
};

struct NRL {
    template <typename V, typename L, typename R>
    void operator()(const V& v, const L& l, const R& r) const {
        v(); r(); l();
    }
};

struct RNL {
    template <typename V, typename L, typename R>
    void operator()(const V& v, const L& l, const R& r) const {
        r(); v(); l();
    }
};

struct LRN {
    template <typename V, typename L, typename R>
    void operator()(const V& v, const L& l, const R& r) const {
        l(); r(); v();
    }
};

struct RLN {
    template <typename V, typename L, typename R>
    void operator()(const V& v, const L& l, const R& r) const {
        r(); l(); v();
    }
};

} // namespace traversal

#endif