#ifndef TEACHER_H
#define TEACHER_H

#include "person.h"

class Teacher : public Person {
public:
    Teacher() : Person(), department(""), position("") {}

    Teacher(const Person& person, const std::string& teacher_department,
            const std::string& teacher_position)
        : Person(person), department(teacher_department), position(teacher_position) {}

    const std::string& get_department() const { return department; }
    const std::string& get_position() const { return position; }

    bool operator==(const Teacher& other) const {
        return Person::operator==(other) && department == other.department && position == other.position;
    }

private:
    std::string department;
    std::string position;
};

inline std::ostream& operator<<(std::ostream& stream, const Teacher& teacher) {
    const Person& person = teacher;
    return stream << person << ' ' << std::quoted(teacher.get_department()) << ' '
                  << std::quoted(teacher.get_position());
}

inline std::istream& operator>>(std::istream& stream, Teacher& teacher) {
    Person person;
    std::string department;
    std::string position;
    if (!(stream >> person >> std::quoted(department) >> std::quoted(position))) return stream;

    teacher = Teacher(person, department, position);
    return stream;
}

#endif
