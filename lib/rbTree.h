#ifndef RED_BLACK_TREE_H
#define RED_BLACK_TREE_H

#include <iostream>
#include <functional>
#include <string>
#include <sstream>
#include <limits>

template <typename Key, typename Value, typename Compare = std::less<Key>>
class RedBlackTree {
public:
    enum class TraversalOrder {
        NLR, // корень, левое, правое
        NRL, // корень, правое, левое
        LNR, // левое, корень, правое
        RNL, // правое, корень, левое
        LRN, // левое, правое, корень
        RLN  // правое, левое, корень
    };

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


    RedBlackTree() : comp(Compare()) {
        nil = new Node(Key(), Value(), nullptr);
        nil->color = true;
        nil->left = nil;
        nil->right = nil;
        nil->parent = nil;
        root = nil;
    }

    explicit RedBlackTree(const Compare& comparator) : comp(comparator) {
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
        Node* new_nil = nil;
        root = other.root;
        nil = other.nil;
        other.root = new_nil;
        other.nil = new_nil;
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

        comp = other.comp;
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


    void traverse(TraversalOrder order, const std::function<void(const Key&, const Value&)>& action) const {
        traverse_subtree(root, order, action);
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

    template <typename Result, typename Action>
    Result reduce(const Result& initial, const Action& action,
                  TraversalOrder order = TraversalOrder::LNR) const {
        Result result = initial;
        reduce_subtree(root, order, result, action);
        return result;
    }

    RedBlackTree merge(const RedBlackTree& other) const {
        return merge(other, comp);
    }

    RedBlackTree merge(const RedBlackTree& other, const Compare& comparator) const {
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

    std::string to_string(TraversalOrder order = TraversalOrder::LNR) const {
        std::ostringstream stream;

        switch (order) {
            case TraversalOrder::NLR: stream << "NLR\n"; break;
            case TraversalOrder::NRL: stream << "NRL\n"; break;
            case TraversalOrder::LNR: stream << "LNR\n"; break;
            case TraversalOrder::RNL: stream << "RNL\n"; break;
            case TraversalOrder::LRN: stream << "LRN\n"; break;
            case TraversalOrder::RLN: stream << "RLN\n"; break;
            default: return "";
        }

        to_string_subtree(root, "", order, stream);
        return stream.str();
    }

    bool from_string(const std::string& text) {
        if (text.size() < 4 || text[3] != '\n') return false;

        TraversalOrder order;
        std::string name = text.substr(0, 3);
        if (name == "NLR") order = TraversalOrder::NLR;
        else if (name == "NRL") order = TraversalOrder::NRL;
        else if (name == "LNR") order = TraversalOrder::LNR;
        else if (name == "RNL") order = TraversalOrder::RNL;
        else if (name == "LRN") order = TraversalOrder::LRN;
        else if (name == "RLN") order = TraversalOrder::RLN;
        else return false;

        RedBlackTree result(comp);
        std::size_t position = 4;
        StringRecord record;
        bool has_record = position < text.size();
        bool valid = true;

        if (has_record && !result.read_string_record(text, position, record)) return false;
        result.root = result.read_string_subtree(text, position, record, has_record, valid, "", order);

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
    const Compare& get_compare() const { return comp; }      

private:
    Node* root;
    Node* nil;
    Compare comp;

    void traverse_subtree(const Node* node, TraversalOrder order,
                          const std::function<void(const Key&, const Value&)>& action) const {
        if (node == nil) return;

        switch (order) {
            case TraversalOrder::NLR:
                action(node->key, node->value);
                traverse_subtree(node->left, order, action);
                traverse_subtree(node->right, order, action);
                break;
            case TraversalOrder::NRL:
                action(node->key, node->value);
                traverse_subtree(node->right, order, action);
                traverse_subtree(node->left, order, action);
                break;
            case TraversalOrder::LNR:
                traverse_subtree(node->left, order, action);
                action(node->key, node->value);
                traverse_subtree(node->right, order, action);
                break;
            case TraversalOrder::RNL:
                traverse_subtree(node->right, order, action);
                action(node->key, node->value);
                traverse_subtree(node->left, order, action);
                break;
            case TraversalOrder::LRN:
                traverse_subtree(node->left, order, action);
                traverse_subtree(node->right, order, action);
                action(node->key, node->value);
                break;
            case TraversalOrder::RLN:
                traverse_subtree(node->right, order, action);
                traverse_subtree(node->left, order, action);
                action(node->key, node->value);
                break;
        }
    }

    void map_subtree(const Node* node, RedBlackTree& result,
                     const std::function<Value(const Value&)>& action) const {
        if (node == nil) return;

        map_subtree(node->left, result, action);

        Value new_value = action(node->value);
        result.insert(node->key, new_value);

        map_subtree(node->right, result, action);
    }

    void where_subtree(const Node* node, RedBlackTree& result,
                       const std::function<bool(const Value&)>& predicate) const {
        if (node == nil) return;

        where_subtree(node->left, result, predicate);

        if (predicate(node->value)) result.insert(node->key, node->value);

        where_subtree(node->right, result, predicate);
    }

    template <typename Result, typename Action>
    void reduce_subtree(const Node* node, TraversalOrder order,
                        Result& result, const Action& action) const {
        if (node == nil) return;

        switch (order) {
            case TraversalOrder::NLR:
                result = action(result, node->value);
                reduce_subtree(node->left, order, result, action);
                reduce_subtree(node->right, order, result, action);
                break;
            case TraversalOrder::NRL:
                result = action(result, node->value);
                reduce_subtree(node->right, order, result, action);
                reduce_subtree(node->left, order, result, action);
                break;
            case TraversalOrder::LNR:
                reduce_subtree(node->left, order, result, action);
                result = action(result, node->value);
                reduce_subtree(node->right, order, result, action);
                break;
            case TraversalOrder::RNL:
                reduce_subtree(node->right, order, result, action);
                result = action(result, node->value);
                reduce_subtree(node->left, order, result, action);
                break;
            case TraversalOrder::LRN:
                reduce_subtree(node->left, order, result, action);
                reduce_subtree(node->right, order, result, action);
                result = action(result, node->value);
                break;
            case TraversalOrder::RLN:
                reduce_subtree(node->right, order, result, action);
                reduce_subtree(node->left, order, result, action);
                result = action(result, node->value);
                break;
        }
    }

    void merge_subtree(const Node* node, RedBlackTree& result) const {
        if (node == nil) return;

        merge_subtree(node->left, result);
        result.insert(node->key, node->value);
        merge_subtree(node->right, result);
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
                           TraversalOrder order, std::ostringstream& stream) const {
        if (node == nil) return;

        switch (order) {
            case TraversalOrder::NLR:
                write_node(node, path, stream);
                to_string_subtree(node->left, path + "L", order, stream);
                to_string_subtree(node->right, path + "R", order, stream);
                break;
            case TraversalOrder::NRL:
                write_node(node, path, stream);
                to_string_subtree(node->right, path + "R", order, stream);
                to_string_subtree(node->left, path + "L", order, stream);
                break;
            case TraversalOrder::LNR:
                to_string_subtree(node->left, path + "L", order, stream);
                write_node(node, path, stream);
                to_string_subtree(node->right, path + "R", order, stream);
                break;
            case TraversalOrder::RNL:
                to_string_subtree(node->right, path + "R", order, stream);
                write_node(node, path, stream);
                to_string_subtree(node->left, path + "L", order, stream);
                break;
            case TraversalOrder::LRN:
                to_string_subtree(node->left, path + "L", order, stream);
                to_string_subtree(node->right, path + "R", order, stream);
                write_node(node, path, stream);
                break;
            case TraversalOrder::RLN:
                to_string_subtree(node->right, path + "R", order, stream);
                to_string_subtree(node->left, path + "L", order, stream);
                write_node(node, path, stream);
                break;
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
                              bool& has_record, bool& valid, const std::string& path,
                              TraversalOrder order) {
        if (!valid || !has_record || record.path.compare(0, path.size(), path) != 0) return nil;

        Node* node = nil;
        Node* left = nil;
        Node* right = nil;

        try {
            switch (order) {
                case TraversalOrder::NLR:
                    node = read_string_node(text, position, record, has_record, valid, path);
                    left = read_string_subtree(text, position, record, has_record, valid, path + "L", order);
                    right = read_string_subtree(text, position, record, has_record, valid, path + "R", order);
                    break;
                case TraversalOrder::NRL:
                    node = read_string_node(text, position, record, has_record, valid, path);
                    right = read_string_subtree(text, position, record, has_record, valid, path + "R", order);
                    left = read_string_subtree(text, position, record, has_record, valid, path + "L", order);
                    break;
                case TraversalOrder::LNR:
                    left = read_string_subtree(text, position, record, has_record, valid, path + "L", order);
                    node = read_string_node(text, position, record, has_record, valid, path);
                    right = read_string_subtree(text, position, record, has_record, valid, path + "R", order);
                    break;
                case TraversalOrder::RNL:
                    right = read_string_subtree(text, position, record, has_record, valid, path + "R", order);
                    node = read_string_node(text, position, record, has_record, valid, path);
                    left = read_string_subtree(text, position, record, has_record, valid, path + "L", order);
                    break;
                case TraversalOrder::LRN:
                    left = read_string_subtree(text, position, record, has_record, valid, path + "L", order);
                    right = read_string_subtree(text, position, record, has_record, valid, path + "R", order);
                    node = read_string_node(text, position, record, has_record, valid, path);
                    break;
                case TraversalOrder::RLN:
                    right = read_string_subtree(text, position, record, has_record, valid, path + "R", order);
                    left = read_string_subtree(text, position, record, has_record, valid, path + "L", order);
                    node = read_string_node(text, position, record, has_record, valid, path);
                    break;
            }

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