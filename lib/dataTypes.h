#ifndef DATA_TYPES_H
#define DATA_TYPES_H

#include <complex>
#include "rbTree.h"
#include "types/student.h"
#include "types/teacher.h"
#include "types/functionValue.h"

using Complex = std::complex<double>;

struct ComplexCompare {
    bool operator()(const Complex& a, const Complex& b) const {
        if (a.real() < b.real()) return true;
        if (b.real() < a.real()) return false;
        return a.imag() < b.imag();
    }
};

using IntegerTree = RedBlackTree<int, int>;
using RealTree = RedBlackTree<double, double>;
using StringTree = RedBlackTree<std::string, std::string>;
using FunctionTree = RedBlackTree<int, FunctionValue>;
using StudentTree = RedBlackTree<PersonID, Student>;
using TeacherTree = RedBlackTree<PersonID, Teacher>;


class ComplexTree : public RedBlackTree<Complex, Complex> {
public:
    ComplexTree() : RedBlackTree<Complex, Complex>(ComplexCompare{}) {}
    using RedBlackTree<Complex, Complex>::RedBlackTree; 
    
    ComplexTree(RedBlackTree<Complex, Complex>&& base) 
        : RedBlackTree<Complex, Complex>(std::move(base)) {}
    
    ComplexTree& operator=(RedBlackTree<Complex, Complex>&& base) {
        static_cast<RedBlackTree<Complex, Complex>&>(*this) = std::move(base);
        return *this;
    }
};

#endif