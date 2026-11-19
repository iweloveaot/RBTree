#ifndef RED_BLACK_TREE_H
#define RED_BLACK_TREE_H

#include <iostream>
#include <functional>
#include <string>
#include <sstream>
#include <limits>
#include <type_traits>
#include <vector>
#include <stdexcept>

#include "traversals.h"
#include "ienumerable.h"

template <typename Key, typename Value>
class RedBlackTree {
public:

    struct Node {
        Key key;
        Value value;
        Node* left;
        Node* right;
        Node* parent;
        bool color; // false - красный (0), true - черный (1)

        Node(const Key& k, const Value& v, Node* nil_node)
            : key(k), value(v), left(nil_node), right(nil_node), parent(nil_node), color(false) {}
    };


    // Дерево должно существовать и не изменяться во время обхода.
    template <typename Strategy = traversal::LNR>
    class TreeEnumerator : public IEnumerator<Node> {
    private:
        struct Step {
            const Node* node;
            bool visit;
        };

        struct AddStep {
            std::vector<Step>& steps;
            const Node* node;
            const Node* nil;
            bool visit;

            void operator()() const {
                if (node != nil) steps.push_back(Step{node, visit});
            }
        };

        const Node* nil;
        const Node* current;
        Strategy strategy;
        std::vector<Step> initial_steps;
        std::vector<Step> pending;
        std::vector<Step> next_steps;

    public:
        TreeEnumerator(const RedBlackTree& tree, Strategy order = Strategy(),
                       const Node* first = nullptr)
            : nil(tree.nil), current(nullptr), strategy(order) {
            const Node* start = first;
            if (start == nullptr) start = tree.root;
            if (start != nil) initial_steps.push_back(Step{start, false});
            next_steps.reserve(3);
            Reset();
        }

        TreeEnumerator(const RedBlackTree& tree, const std::vector<std::string>& paths,
                       Strategy order = Strategy())
            : nil(tree.nil), current(nullptr), strategy(order) {
            // Стек выдаёт последний добавленный шаг первым.
            for (std::size_t i = paths.size(); i > 0; --i) {
                for (char direction : paths[i - 1]) {
                    if (direction != 'L' && direction != 'R')
                        throw std::invalid_argument("Invalid enumeration path: " + paths[i - 1]);
                }
                const Node* node = tree.find_by_path(paths[i - 1].c_str());
                if (node == nullptr)
                    throw std::invalid_argument("Invalid enumeration path: " + paths[i - 1]);
                initial_steps.push_back(Step{node, true});
            }
            Reset();
        }

        bool MoveNext() override {
            current = nullptr;
            while (!pending.empty()) {
                Step step = pending.back();
                pending.pop_back();
                if (step.visit) {
                    current = step.node;
                    return true;
                }

                next_steps.clear();
                AddStep visit{next_steps, step.node, nil, true};
                AddStep left{next_steps, step.node->left, nil, false};
                AddStep right{next_steps, step.node->right, nil, false};
                strategy(visit, left, right);
                for (std::size_t i = next_steps.size(); i > 0; --i)
                    pending.push_back(next_steps[i - 1]);
            }
            return false;
        }

        const Node& GetCurrent() const override {
            if (current == nullptr)
                throw std::out_of_range("Enumerator not positioned");
            return *current;
        }

        void Reset() override {
            current = nullptr;
            pending = initial_steps;
            next_steps.clear();
        }
    };

   
    IEnumerator<Node>* GetEnumerator() const {
        return new TreeEnumerator<>(*this);
    }

    IEnumerator<Node>* GetEnumerator(const std::vector<std::string>& paths) const {
        return new TreeEnumerator<>(*this, paths);
    }

    template <typename Strategy>
    IEnumerator<Node>* GetEnumerator(Strategy strategy) const {
        return new TreeEnumerator<Strategy>(*this, strategy);
    }

    RedBlackTree() : comp(std::less<Key>()) {
        static_assert(std::is_invocable_r_v<bool, std::less<Key>, const Key&, const Key&>, "Key must be comparable with std::less<Key>");
        validate_comparator();
        nil = new Node(Key(), Value(), nullptr);
        nil->color = true;
        nil->left = nil;
        nil->right = nil;
        nil->parent = nil;
        root = nil;
    }

    template <typename Compare>
    explicit RedBlackTree(const Compare& comparator) : comp(comparator) {
        static_assert(std::is_invocable_r_v<bool, Compare, const Key&, const Key&>,
                      "Compare must be callable as bool(const Key&, const Key&)");
        validate_comparator();
        nil = new Node(Key(), Value(), nullptr);
        nil->color = true;
        nil->left = nil;
        nil->right = nil;
        nil->parent = nil;
        root = nil;
    }

    ~RedBlackTree() {
        destroy_subtree(root);
        delete nil;
    }


    RedBlackTree(const RedBlackTree& other) : RedBlackTree(other.comp) {
        root = copy_subtree(other.root, other.nil, nil);
        
    }

    RedBlackTree(RedBlackTree&& other) : RedBlackTree(other.comp) {
        Node* temp_nil = nil;

        root = other.root;
        nil = other.nil;

        other.root = temp_nil;
        other.nil = temp_nil;
    }

    RedBlackTree& operator=(const RedBlackTree& other) {
        if (this == &other) return *this;

        RedBlackTree temp(other);
        comp = other.comp;

        Node* old_root = root;
        Node* old_nil = nil;
        root = temp.root;
        nil = temp.nil;
        temp.root = old_root;
        temp.nil = old_nil;
        return *this;
    }

    RedBlackTree& operator=(RedBlackTree&& other) {
    if (this == &other) return *this;

    comp = std::move(other.comp);
    destroy_subtree(root);

    Node* old_nil = nil;
    root = other.root;
    nil = other.nil;
    other.root = old_nil;
    other.nil = old_nil;
    other.nil->parent = other.nil;
    return *this;
}



    void insert(const Key& key, const Value& value) {       
        Node* z = new Node(key, value, nil);
        Node* y = nil;
        Node* x = root;

        try {
            while (x != nil) {
                y = x;
                if (comp(z->key, x->key)) x = x->left;
                else if (comp(x->key, z->key)) x = x->right;           
                else {
                    delete z;
                    return;
                }
            }

            z->parent = y;
            if (y == nil) root = z;
            else if (comp(z->key, y->key)) y->left = z;        
            else y->right = z;
        } catch (...) {
            delete z;
            throw;
        }

        z->left = nil;
        z->right = nil;
        z->color = false;
        insert_fixup(z);
    }

    bool erase(const Key& key) {       
        Node* z = find_node(key);
        if (z == nil) return false;

        Node* y = z;
        Node* x;
        bool y_orig_color = y->color;

        if (z->left == nil) {
            x = z->right;
            transplant(z, z->right);
        } else if (z->right == nil) {
            x = z->left;
            transplant(z, z->left);
        } else {
            y = maximum(z->left);
            y_orig_color = y->color;
            x = y->left;
            if (y->parent == z) {
                x->parent = y;
            } else {
                transplant(y, y->left);
                y->left = z->left;
                y->left->parent = y;
            }
            transplant(z, y);
            y->right = z->right;
            y->right->parent = y;
            y->color = z->color;
        }

        delete z;

        if (y_orig_color == true) {
            delete_fixup(x);
        }
        return true;
    }

    Node* find(const Key& key) const {     
        Node* res = find_node(key);
        return (res == nil) ? nullptr : res;
    }
  
    bool empty() const {      
        return root == nil;
    }

    template <typename Strategy, typename Action>
    void traverse(Strategy strategy, const Action& action) const {
        TreeEnumerator<Strategy> enumerator(*this, strategy);
        while (enumerator.MoveNext()) {
            const Node& node = enumerator.GetCurrent();
            action(node.key, node.value);
        }
    }
    
    RedBlackTree map(const std::function<Value(const Value&)>& action) const {
        RedBlackTree result(comp);
        map_subtree(root, result, action);
        return result;
    }

    RedBlackTree where(const std::function<bool(const Value&)>& predicate) const {
        RedBlackTree result(comp);
        where_subtree(root, result, predicate);
        return result;
    }

    template <typename Strategy, typename Result, typename Action>
    Result reduce(Strategy strategy, const Result& initial, const Action& action) const {
        Result result = initial;
        TreeEnumerator<Strategy> enumerator(*this, strategy);
        while (enumerator.MoveNext())
            result = action(result, enumerator.GetCurrent().value);
        return result;
    }

    RedBlackTree merge(const RedBlackTree& other) const {
        return merge(other, comp);
    }

    template <typename Compare>
    RedBlackTree merge(const RedBlackTree& other, const Compare& comparator) const {
        static_assert(std::is_invocable_r_v<bool, Compare, const Key&, const Key&>,
                      "Compare must be callable as bool(const Key&, const Key&)");
        RedBlackTree result(comparator);
        merge_subtree(root, result);
        other.merge_subtree(other.root, result);
        return result;
    }

    RedBlackTree extract_subtree(const Key& key) const {
        RedBlackTree result(comp);
        Node* node = find_node(key);
        if (node == nil) return result;

        result.root = result.copy_subtree(node, nil, result.nil);
        result.root->color = true;
        return result;
    }

    bool contains_subtree(const RedBlackTree& other) const {
        if (other.empty()) return true;

        Node* node = find_node(other.root->key);
        return equal_subtrees(node, other.root, other.nil);
    }

    Node* find_by_path(const char* path) const {
        return follow_path(root, path, false);
    }

    Node* find_by_path(const Key& start_key, const char* path) const {
        return follow_path(find_node(start_key), path, true);
    }

    std::string to_string() const {
        std::ostringstream stream;
        stream << "RBT\n";
        to_string_subtree(root, "", stream);
        return stream.str();
    }

    bool from_string(const std::string& text) {
        if (text.compare(0, 4, "RBT\n") != 0) return false;

        RedBlackTree result(comp);
        std::size_t position = 4;
        StringRecord record;
        bool has_record = position < text.size();
        bool valid = true;

        if (has_record && !result.read_string_record(text, position, record)) return false;
        result.root = result.read_string_subtree(text, position, record, has_record, valid, "");

        if (!valid || has_record) return false;
        if (result.root != result.nil && !result.root->color) return false;
        if (result.check_string_subtree(result.root, nullptr, nullptr) == 0) return false;

        Node* old_root = root;
        Node* old_nil = nil;
        root = result.root;
        nil = result.nil;
        result.root = old_root;
        result.nil = old_nil;
        return true;
    }

    Node* get_root() const { return root; }     
    Node* get_nil() const { return nil; }      
    const std::function<bool(const Key&, const Key&)>& get_compare() const { return comp; }   

private:
    Node* root;
    Node* nil;
    std::function<bool(const Key&, const Key&)> comp;

    void validate_comparator() const {
        if (!comp) {
            throw std::invalid_argument("Comparator is empty");
        }
        Key probe = Key();
        if (comp(probe, probe)) {
            throw std::invalid_argument(
                "Comparator is not irreflexive: comp(a, a) must be false "
                "(strict weak ordering required, use < instead of <=)");
        }
    }


    void map_subtree(const Node* node, RedBlackTree& result,
                     const std::function<Value(const Value&)>& action) const {
        TreeEnumerator<traversal::LNR> enumerator(*this, traversal::LNR{}, node);
        while (enumerator.MoveNext()) {
            const Node& current = enumerator.GetCurrent();
            result.insert(current.key, action(current.value));
        }
    }

    void where_subtree(const Node* node, RedBlackTree& result,
                       const std::function<bool(const Value&)>& predicate) const {
        TreeEnumerator<traversal::LNR> enumerator(*this, traversal::LNR{}, node);
        while (enumerator.MoveNext()) {
            const Node& current = enumerator.GetCurrent();
            if (predicate(current.value)) result.insert(current.key, current.value);
        }
    }

    void merge_subtree(const Node* node, RedBlackTree& result) const {
        TreeEnumerator<traversal::LNR> enumerator(*this, traversal::LNR{}, node);
        while (enumerator.MoveNext()) {
            const Node& current = enumerator.GetCurrent();
            result.insert(current.key, current.value);
        }
    }

    bool equal_subtrees(const Node* node, const Node* other, const Node* other_nil) const {
        if (node == nil || other == other_nil) {
            return node == nil && other == other_nil;
        }

        if (comp(node->key, other->key) || comp(other->key, node->key)) return false;
        if (!(node->value == other->value)) return false;

        return equal_subtrees(node->left, other->left, other_nil) &&
               equal_subtrees(node->right, other->right, other_nil);
    }

    Node* follow_path(Node* node, const char* path, bool allow_parent) const {
        if (node == nil || path == nullptr) return nullptr;

        while (*path != '\0') {
            if (*path == 'L') node = node->left;
            else if (*path == 'R') node = node->right;
            else if (*path == 'U' && allow_parent) node = node->parent;
            else return nullptr;

            if (node == nil) return nullptr;
            ++path;
        }
        return node;
    }

    void write_node(const Node* node, const std::string& path, std::ostringstream& stream) const {
        std::ostringstream key_stream;
        std::ostringstream value_stream;
        key_stream.precision(std::numeric_limits<long double>::max_digits10);
        value_stream.precision(std::numeric_limits<long double>::max_digits10);
        key_stream << node->key;
        value_stream << node->value;

        std::string key_text = key_stream.str();
        std::string value_text = value_stream.str();

        if (path.empty()) stream << '.';
        else stream << path;

        stream << ' ' << node->color << ' '
               << key_text.size() << ':' << key_text << ' '
               << value_text.size() << ':' << value_text << '\n';
    }

    void to_string_subtree(const Node* node, const std::string& path,
                           std::ostringstream& stream) const {
        TreeEnumerator<traversal::NLR> enumerator(*this, traversal::NLR{}, node);
        while (enumerator.MoveNext()) {
            const Node* current = &enumerator.GetCurrent();
            const Node* parent = current;
            std::string reversed;
            while (parent != node) {
                reversed += parent == parent->parent->left ? 'L' : 'R';
                parent = parent->parent;
            }
            std::string node_path = path;
            for (std::size_t i = reversed.size(); i > 0; --i)
                node_path += reversed[i - 1];
            write_node(current, node_path, stream);
        }
    }

    struct StringRecord {
        std::string path;
        bool color;
        std::string key_text;
        std::string value_text;
    };

    bool read_string_field(const std::string& text, std::size_t& position, std::string& field) const {
        if (position == text.size() || text[position] < '0' || text[position] > '9') return false;

        std::size_t length = 0;
        while (position < text.size() && text[position] >= '0' && text[position] <= '9') {
            std::size_t digit = text[position] - '0';
            if (length > (std::numeric_limits<std::size_t>::max() - digit) / 10) return false;
            length = length * 10 + digit;
            ++position;
        }

        if (position == text.size() || text[position] != ':') return false;
        ++position;
        if (length > text.size() - position) return false;

        field = text.substr(position, length);
        position += length;
        return true;
    }

    bool read_string_record(const std::string& text, std::size_t& position, StringRecord& record) const {
        std::size_t end = text.find(' ', position);
        if (end == std::string::npos) return false;

        record.path = text.substr(position, end - position);
        if (record.path == ".") record.path = "";
        else {
            if (record.path.empty()) return false;
            for (std::size_t i = 0; i < record.path.size(); ++i) {
                if (record.path[i] != 'L' && record.path[i] != 'R') return false;
            }
        }

        // Число символов строки даёт верхнюю оценку числа узлов.
        std::size_t max_depth = 0;
        for (std::size_t length = text.size(); length > 0; length /= 2) max_depth += 2;
        if (record.path.size() > max_depth) return false;

        position = end + 1;
        if (position == text.size() || (text[position] != '0' && text[position] != '1')) return false;
        record.color = text[position] == '1';
        ++position;
        if (position == text.size() || text[position] != ' ') return false;
        ++position;

        if (!read_string_field(text, position, record.key_text)) return false;
        if (position == text.size() || text[position] != ' ') return false;
        ++position;
        if (!read_string_field(text, position, record.value_text)) return false;
        if (position == text.size() || text[position] != '\n') return false;
        ++position;
        return true;
    }

    template <typename T>
    bool read_string_value(const std::string& text, T& value) const {
        std::istringstream stream(text);
        if (!(stream >> value)) return false;
        stream >> std::ws;
        return stream.eof();
    }

    bool read_string_value(const std::string& text, std::string& value) const {
        value = text;
        return true;
    }

    bool read_string_value(const std::string& text, char& value) const {
        if (text.size() != 1) return false;
        value = text[0];
        return true;
    }

    Node* read_string_node(const std::string& text, std::size_t& position, StringRecord& record,
                           bool& has_record, bool& valid, const std::string& path) {
        if (!valid || !has_record) {
            valid = false;
            return nil;
        }
        if (record.path != path) {
            valid = false;
            return nil;
        }

        Key key;
        Value value;
        if (!read_string_value(record.key_text, key) || !read_string_value(record.value_text, value)) {
            valid = false;
            return nil;
        }

        bool color = record.color;
        has_record = position < text.size();
        if (has_record && !read_string_record(text, position, record)) {
            valid = false;
            return nil;
        }

        Node* node = new Node(key, value, nil);
        node->color = color;
        return node;
    }

    Node* read_string_subtree(const std::string& text, std::size_t& position, StringRecord& record,
                              bool& has_record, bool& valid, const std::string& path) {
        if (!valid || !has_record || record.path.compare(0, path.size(), path) != 0) return nil;

        Node* node = nil;
        Node* left = nil;
        Node* right = nil;

        try {
            node = read_string_node(text, position, record, has_record, valid, path);
            left = read_string_subtree(text, position, record, has_record, valid, path + "L");
            right = read_string_subtree(text, position, record, has_record, valid, path + "R");

            if (!valid) {
                destroy_subtree(left);
                destroy_subtree(right);
                destroy_subtree(node);
                return nil;
            }

            node->left = left;
            node->right = right;
            if (left != nil) left->parent = node;
            if (right != nil) right->parent = node;
            return node;
        } catch (...) {
            destroy_subtree(left);
            destroy_subtree(right);
            destroy_subtree(node);
            throw;
        }
    }

    int check_string_subtree(const Node* node, const Key* lower, const Key* upper) const {
        if (node == nil) return 1;

        if (lower != nullptr && !comp(*lower, node->key)) return 0;
        if (upper != nullptr && !comp(node->key, *upper)) return 0;
        if (!node->color && (!node->left->color || !node->right->color)) return 0;

        int left = check_string_subtree(node->left, lower, &node->key);
        if (left == 0) return 0;
        int right = check_string_subtree(node->right, &node->key, upper);
        if (left != right) return 0;
        return left + (node->color ? 1 : 0);
    }

    Node* copy_subtree(const Node* node, const Node* other_nil, Node* parent) {
        if (node == other_nil) return nil;

        Node* copy = new Node(node->key, node->value, nil);
        copy->parent = parent;
        copy->color = node->color;

        try {
            copy->left = copy_subtree(node->left, other_nil, copy);
            copy->right = copy_subtree(node->right, other_nil, copy);
        } catch (...) {
            destroy_subtree(copy);
            throw;
        }
        return copy;
    }

    void destroy_subtree(Node* node) {        
        if (node != nil) {
            destroy_subtree(node->left);
            destroy_subtree(node->right);
            delete node;
        }
    }

    void left_rotate(Node* x) {     
        Node* y = x->right;
        x->right = y->left;
        if (y->left != nil) y->left->parent = x;

        y->parent = x->parent;
        if (x->parent == nil) root = y;
        else if (x == x->parent->left) x->parent->left = y;
        else x->parent->right = y;

        y->left = x;
        x->parent = y;
    }

    void right_rotate(Node* x) {    
        Node* y = x->left;
        x->left = y->right;
        if (y->right != nil) y->right->parent = x;

        y->parent = x->parent;
        if (x->parent == nil) root = y;
        else if (x == x->parent->right) x->parent->right = y;
        else x->parent->left = y;

        y->right = x;
        x->parent = y;
    }

    void insert_fixup(Node* z) {      
        while (z->parent->color == false) {
            if (z->parent == z->parent->parent->left) {  
                Node* y = z->parent->parent->right;  
                if (y->color == false) {
                    z->parent->color = true;
                    y->color = true;
                    z->parent->parent->color = false;
                    z = z->parent->parent;
                } else {
                    if (z == z->parent->right) {
                        z = z->parent;
                        left_rotate(z);
                    }
                    z->parent->color = true;
                    z->parent->parent->color = false;
                    right_rotate(z->parent->parent);
                }
            } else {
                Node* y = z->parent->parent->left;
                if (y->color == false) {
                    z->parent->color = true;
                    y->color = true;
                    z->parent->parent->color = false;
                    z = z->parent->parent;
                } else {
                    if (z == z->parent->left) {
                        z = z->parent;
                        right_rotate(z);
                    }
                    z->parent->color = true;
                    z->parent->parent->color = false;
                    left_rotate(z->parent->parent);
                }
            }
        }
        root->color = true;
    }

    void transplant(Node* u, Node* v) {     
        if (u->parent == nil) root = v;
        else if (u == u->parent->left) u->parent->left = v;
        else u->parent->right = v;
        v->parent = u->parent;
    }

    Node* maximum(Node* x) const {     
        while (x->right != nil) x = x->right;
        return x;
    }

    void delete_fixup(Node* x) {                                 
        while (x != root && x->color == true) {
            if (x == x->parent->left) {
                Node* w = x->parent->right;
                if (w->color == false) {
                    w->color = true;
                    x->parent->color = false;
                    left_rotate(x->parent);
                    w = x->parent->right;                                                                                                                                                                                                                                   
                }
                if (w->left->color == true && w->right->color == true) {
                    w->color = false;
                    x = x->parent;
                } else {
                    if (w->right->color == true) {
                        w->color = false;
                        w->left->color = true;
                        right_rotate(w);
                        w = x->parent->right;
                    }
                    w->color = x->parent->color;
                    x->parent->color = true;
                    w->right->color = true;
                    left_rotate(x->parent);
                    x = root;
                }
            } else {
                Node* w = x->parent->left;
                if (w->color == false) {
                    w->color = true;
                    x->parent->color = false;
                    right_rotate(x->parent);
                    w = x->parent->left;
                }
                if (w->right->color == true && w->left->color == true) {
                    w->color = false;
                    x = x->parent;
                } else {
                    if (w->left->color == true) {
                        w->color = false;
                        w->right->color = true;
                        left_rotate(w);
                        w = x->parent->left;
                    }
                    w->color = x->parent->color;
                    x->parent->color = true;
                    w->left->color = true;
                    right_rotate(x->parent);
                    x = root;
                }
            }
        }
        x->color = true;
    }

    Node* find_node(const Key& key) const {                            
        Node* ptr = root;
        while (ptr != nil) {
            if (comp(key, ptr->key)) ptr = ptr->left;
            else if (comp(ptr->key, key)) ptr = ptr->right;
            else return ptr;
        }
        return nil;
    }
};

#endif 
