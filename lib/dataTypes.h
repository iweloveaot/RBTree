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
using ComplexTree = RedBlackTree<Complex, Complex, ComplexCompare>;
using StringTree = RedBlackTree<std::string, std::string>;
using FunctionTree = RedBlackTree<int, FunctionValue>;
using StudentTree = RedBlackTree<PersonID, Student>;
using TeacherTree = RedBlackTree<PersonID, Teacher>;

#endif
