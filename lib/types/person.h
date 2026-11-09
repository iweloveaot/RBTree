#ifndef PERSON_H
#define PERSON_H

#include <iomanip>
#include <string>

#include "date.h"

using PersonID = std::string;

class Person {
public:
    Person() : id(""), first_name(""), middle_name(""), last_name(""), birth_date() {}

    Person(const PersonID& person_id, const std::string& first, const std::string& middle,
           const std::string& last, const Date& birth)
        : id(person_id), first_name(first), middle_name(middle), last_name(last), birth_date(birth) {}

    const PersonID& get_id() const { return id; }
    const std::string& get_first_name() const { return first_name; }
    const std::string& get_middle_name() const { return middle_name; }
    const std::string& get_last_name() const { return last_name; }
    const Date& get_birth_date() const { return birth_date; }

    std::string get_full_name() const {
        std::string name = first_name;
        if (!middle_name.empty()) {
            if (!name.empty()) name += ' ';
            name += middle_name;
        }
        if (!last_name.empty()) {
            if (!name.empty()) name += ' ';
            name += last_name;
        }
        return name;
    }

    bool operator==(const Person& other) const {
        return id == other.id && first_name == other.first_name &&
               middle_name == other.middle_name && last_name == other.last_name &&
               birth_date == other.birth_date;
    }

private:
    PersonID id;
    std::string first_name;
    std::string middle_name;
    std::string last_name;
    Date birth_date;
};

inline std::ostream& operator<<(std::ostream& stream, const Person& person) {
    return stream << std::quoted(person.get_id()) << ' '
                  << std::quoted(person.get_first_name()) << ' '
                  << std::quoted(person.get_middle_name()) << ' '
                  << std::quoted(person.get_last_name()) << ' '
                  << person.get_birth_date();
}

inline std::istream& operator>>(std::istream& stream, Person& person) {
    PersonID id;
    std::string first;
    std::string middle;
    std::string last;
    Date birth;
    if (!(stream >> std::quoted(id) >> std::quoted(first) >> std::quoted(middle)
                 >> std::quoted(last) >> birth)) return stream;

    person = Person(id, first, middle, last, birth);
    return stream;
}

#endif
