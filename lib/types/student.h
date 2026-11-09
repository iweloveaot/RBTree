#ifndef STUDENT_H
#define STUDENT_H

#include "person.h"

class Student : public Person {
public:
    Student() : Person(), group(""), course(0) {}

    Student(const Person& person, const std::string& student_group, int student_course)
        : Person(person), group(student_group), course(student_course) {
        if (course < 0) throw std::invalid_argument("Invalid course");
    }

    const std::string& get_group() const { return group; }
    int get_course() const { return course; }

    bool operator==(const Student& other) const {
        return Person::operator==(other) && group == other.group && course == other.course;
    }

private:
    std::string group;
    int course;
};

inline std::ostream& operator<<(std::ostream& stream, const Student& student) {
    const Person& person = student;
    return stream << person << ' ' << std::quoted(student.get_group()) << ' ' << student.get_course();
}

inline std::istream& operator>>(std::istream& stream, Student& student) {
    Person person;
    std::string group;
    int course;
    if (!(stream >> person >> std::quoted(group) >> course)) return stream;

    if (course < 0) {
        stream.setstate(std::ios::failbit);
        return stream;
    }

    student = Student(person, group, course);
    return stream;
}

#endif
