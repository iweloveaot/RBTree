#ifndef FUNCTION_VALUE_H
#define FUNCTION_VALUE_H

#include <iostream>
#include <stdexcept>

using IntFunction = int (*)(int);

class FunctionValue {
public:
    FunctionValue() : id(0), function(nullptr) {}

    explicit FunctionValue(int number) : id(number), function(nullptr) {
        switch (id) {
            case 0: break;
            case 1: function = &FunctionValue::identity; break;
            case 2: function = &FunctionValue::is_even; break;
            case 3: function = &FunctionValue::sign; break;
            default: throw std::invalid_argument("Unknown function ID");
        }
    }

    int get_id() const { return id; }
    IntFunction get_function() const { return function; }
    bool empty() const { return function == nullptr; }

    const char* get_name() const {
        switch (id) {
            case 1: return "identity";
            case 2: return "is_even";
            case 3: return "sign";
            default: return "none";
        }
    }

    int call(int value) const {
        if (function == nullptr) throw std::logic_error("Function is not selected");
        return function(value);
    }

    bool operator==(const FunctionValue& other) const {
        return id == other.id;
    }

private:
    int id;
    IntFunction function;

    static int identity(int value) {
        return value;
    }

    static int is_even(int value) {
        return value % 2 == 0 ? 1 : 0;
    }

    static int sign(int value) {
        if (value < 0) return -1;
        if (value > 0) return 1;
        return 0;
    }
};

inline std::ostream& operator<<(std::ostream& stream, const FunctionValue& value) {
    return stream << value.get_id();
}

inline std::istream& operator>>(std::istream& stream, FunctionValue& value) {
    int id;
    if (!(stream >> id)) return stream;

    if (id < 0 || id > 3) {
        stream.setstate(std::ios::failbit);
        return stream;
    }

    value = FunctionValue(id);
    return stream;
}

#endif
