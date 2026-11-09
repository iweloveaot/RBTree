#ifndef TYPE_FIXTURES_H
#define TYPE_FIXTURES_H
#include "test_helpers.h"

struct IntegerData {
    using Tree = IntegerTree;
    using Result = int;
    static int key(int i) { return i; }
    static int value(int i) { return i*10; }
    static int transform(const int& value) { return value*2; }
    static bool keep(const int& value) { return value >= 20; }
    static int fold(const int& result,const int& value) { return result+value; }
    static int expected() { return 60; }
};
struct RealData {
    using Tree = RealTree;
    using Result = double;
    static double key(int i) { return i+0.5; }
    static double value(int i) { return i+0.25; }
    static double transform(const double& value) { return value+0.5; }
    static bool keep(const double& value) { return value >= 2; }
    static double fold(const double& result,const double& value) { return result+value; }
    static double expected() { return 6.75; }
};
struct ComplexData {
    using Tree = ComplexTree;
    using Result = Complex;
    static Complex key(int i) { return Complex(1,i); }
    static Complex value(int i) { return Complex(i,-i/2.0); }
    static Complex transform(const Complex& value) { return std::conj(value); }
    static bool keep(const Complex& value) { return value.real() >= 2; }
    static Complex fold(const Complex& result,const Complex& value) { return result+value; }
    static Complex expected() { return Complex(6,-3); }
};
struct StringData {
    using Tree = StringTree;
    using Result = std::string;
    static std::string key(int i) { return "key"+std::to_string(i); }
    static std::string value(int i) { return "v"+std::to_string(i); }
    static std::string transform(const std::string& value) { return value+"!"; }
    static bool keep(const std::string& value) { return value != "v1"; }
    static std::string fold(const std::string& result,const std::string& value) { return result+value; }
    static std::string expected() { return "v1v2v3"; }
};
struct FunctionData {
    using Tree = FunctionTree;
    using Result = int;
    static int key(int i) { return i; }
    static FunctionValue value(int i) { return FunctionValue((i-1)%3+1); }
    static FunctionValue transform(const FunctionValue& value) {
        if (value.empty()) return FunctionValue();
        return FunctionValue(value.get_id()%3+1);
    }
    static bool keep(const FunctionValue& value) { return value.get_id() >= 2; }
    static int fold(const int& result,const FunctionValue& value) { return result+(value.empty()?0:1); }
    static int expected() { return 3; }
};
struct StudentData {
    using Tree = StudentTree;
    using Result = int;
    static PersonID key(int i) { return "id"+std::to_string(i); }
    static Student value(int i) {
        return Student(Person(key(i),"First","","Last",Date(1,1,2000+i)),"A",i);
    }
    static Student transform(const Student& value) {
        return Student(value,value.get_group(),value.get_course()+1);
    }
    static bool keep(const Student& value) { return value.get_course() >= 2; }
    static int fold(const int& result,const Student& value) { return result+value.get_course(); }
    static int expected() { return 6; }
};
struct TeacherData {
    using Tree = TeacherTree;
    using Result = int;
    static PersonID key(int i) { return "id"+std::to_string(i); }
    static Teacher value(int i) {
        return Teacher(Person(key(i),"First","","Last",Date()),i==1?"Physics":"Math","Lecturer");
    }
    static Teacher transform(const Teacher& value) {
        return Teacher(value,value.get_department(),"Professor");
    }
    static bool keep(const Teacher& value) { return value.get_department() == "Math"; }
    static int fold(const int& result,const Teacher&) { return result+1; }
    static int expected() { return 3; }
};

template <typename D>
void fill_data(typename D::Tree& tree) {
    for (int i=1;i<=3;++i) tree.insert(D::key(i),D::value(i));
}
template <typename D>
void assert_data(const typename D::Tree& tree) {
    assert(node_count(tree) == 3 && check_tree(tree));
    for (int i=1;i<=3;++i) {
        auto node = tree.find(D::key(i));
        assert(node && node->value == D::value(i));
    }
}
template <typename K,typename V>
struct TypedCollect {
    std::vector<K>& keys;
    std::vector<V>& values;
    void operator()(const K& key,const V& value) const {
        keys.push_back(key);
        values.push_back(value);
    }
};
inline void assert_result(int value,int expected) { assert(value == expected); }
inline void assert_result(double value,double expected) { assert(std::abs(value-expected) <= 1e-12); }
inline void assert_result(const Complex& value,const Complex& expected) { assert(std::abs(value-expected) <= 1e-12); }
inline void assert_result(const std::string& value,const std::string& expected) { assert(value == expected); }
#endif

