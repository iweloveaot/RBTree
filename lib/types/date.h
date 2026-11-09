#ifndef DATE_H
#define DATE_H

#include <iostream>
#include <stdexcept>

class Date {
public:
    Date() : day(1), month(1), year(2000) {}

    Date(int d, int m, int y) : day(d), month(m), year(y) {
        if (!is_valid(d, m, y)) throw std::invalid_argument("Invalid date");
    }

    int get_day() const { return day; }
    int get_month() const { return month; }
    int get_year() const { return year; }

    static bool is_valid(int d, int m, int y) {
        if (y < 1 || m < 1 || m > 12 || d < 1) return false;

        int days[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
        if (y % 400 == 0 || (y % 4 == 0 && y % 100 != 0)) days[1] = 29;
        return d <= days[m - 1];
    }

    bool operator==(const Date& other) const {
        return day == other.day && month == other.month && year == other.year;
    }

private:
    int day;
    int month;
    int year;
};

inline std::ostream& operator<<(std::ostream& stream, const Date& date) {
    return stream << date.get_day() << ' ' << date.get_month() << ' ' << date.get_year();
}

inline std::istream& operator>>(std::istream& stream, Date& date) {
    int day;
    int month;
    int year;
    if (!(stream >> day >> month >> year)) return stream;

    if (!Date::is_valid(day, month, year)) {
        stream.setstate(std::ios::failbit);
        return stream;
    }

    date = Date(day, month, year);
    return stream;
}

#endif
