#include "dataTypes.h"

#include <cmath>
#include <functional>
#include <iostream>
#include <limits>
#include <sstream>
#include <stdexcept>
#include <string>
#include <utility>

#ifdef _WIN32
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#endif

bool read_line(const char* prompt, std::string& text) {
    std::cout << prompt;
    return bool(std::getline(std::cin, text));
}

template <typename T>
bool valid_number(const T&) { return true; }

bool valid_number(const double& value) {
    return std::isfinite(value);
}

bool valid_number(const Complex& value) {
    return std::isfinite(value.real()) && std::isfinite(value.imag());
}

template <typename T>
bool read_value(const char* prompt, T& value) {
    std::string text;
    while (read_line(prompt, text)) {
        std::istringstream stream(text);
        T candidate;
        if (stream >> candidate) {
            stream >> std::ws;
            if (stream.eof() && valid_number(candidate)) {
                value = candidate;
                return true;
            }
        }
        std::cout << "Некорректное значение. Повторите ввод.\n";
    }
    return false;
}

bool read_value(const char* prompt, std::string& value) {
    return read_line(prompt, value);
}

bool read_choice(const char* prompt, int minimum, int maximum, int& value) {
    while (read_value(prompt, value)) {
        if (value >= minimum && value <= maximum) return true;
        std::cout << "Введите число от " << minimum << " до " << maximum << ".\n";
    }
    return false;
}

bool read_required_text(const char* prompt, std::string& text) {
    while (read_line(prompt, text)) {
        if (text.find_first_not_of(" \t\r\n") != std::string::npos) return true;
        std::cout << "Поле не должно быть пустым.\n";
    }
    return false;
}

bool read_value(const char* prompt, FunctionValue& value) {
    std::cout << "Функции: 0 — не выбрана; 1 — identity(x)=x;\n"
                 "2 — is_even(x), 1 для чётного числа; 3 — sign(x).\n";
    int id;
    if (!read_choice(prompt, 0, 3, id)) return false;
    value = FunctionValue(id);
    return true;
}

bool read_date(Date& date) {
    int day, month, year;
    while (true) {
        if (!read_choice("День рождения: ", 1, 31, day)) return false;
        if (!read_choice("Месяц рождения: ", 1, 12, month)) return false;
        if (!read_choice("Год рождения: ", 1, std::numeric_limits<int>::max(), year)) return false;
        if (Date::is_valid(day, month, year)) {
            date = Date(day, month, year);
            return true;
        }
        std::cout << "Такой даты не существует. Введите дату заново.\n";
    }
}

bool read_person(Person& person) {
    PersonID id;
    std::string first, middle, last;
    Date birth;
    if (!read_required_text("Идентификатор: ", id)) return false;
    if (!read_required_text("Имя: ", first)) return false;
    if (!read_line("Отчество (можно оставить пустым): ", middle)) return false;
    if (!read_required_text("Фамилия: ", last)) return false;
    if (!read_date(birth)) return false;
    person = Person(id, first, middle, last, birth);
    return true;
}

template <typename Key, typename Value>
bool read_entry(Key& key, Value& value) {
    return read_value("Ключ: ", key) && read_value("Значение: ", value);
}

bool read_entry(PersonID& key, Student& value) {
    Person person;
    std::string group;
    int course;
    if (!read_person(person)) return false;
    if (!read_required_text("Группа: ", group)) return false;
    if (!read_choice("Курс (0 — не указан): ", 0, std::numeric_limits<int>::max(), course)) return false;
    key = person.get_id();
    value = Student(person, group, course);
    return true;
}

bool read_entry(PersonID& key, Teacher& value) {
    Person person;
    std::string department, position;
    if (!read_person(person)) return false;
    if (!read_required_text("Кафедра: ", department)) return false;
    if (!read_required_text("Должность: ", position)) return false;
    key = person.get_id();
    value = Teacher(person, department, position);
    return true;
}

void print_text(const std::string& text) {
    std::cout << '"';
    for (char symbol : text) {
        if (symbol == '\n') std::cout << "\\n";
        else if (symbol == '\r') std::cout << "\\r";
        else if (symbol == '\t') std::cout << "\\t";
        else if (symbol == '\0') std::cout << "\\0";
        else if (symbol == '"' || symbol == '\\') std::cout << '\\' << symbol;
        else if (static_cast<unsigned char>(symbol) < 32 ||
                 static_cast<unsigned char>(symbol) == 127) std::cout << '?';
        else std::cout << symbol;
    }
    std::cout << '"';
}

template <typename T>
void print_value(const T& value) { std::cout << value; }

void print_value(const std::string& value) { print_text(value); }

void print_person(const Person& person) {
    std::cout << "ID=";
    print_text(person.get_id());
    std::cout << ", ФИО=";
    print_text(person.get_full_name());
    const Date& date = person.get_birth_date();
    std::cout << ", дата рождения=" << date.get_day() << '.'
              << date.get_month() << '.' << date.get_year();
}

void print_value(const Student& value) {
    print_person(value);
    std::cout << ", группа=";
    print_text(value.get_group());
    std::cout << ", курс=" << value.get_course();
}

void print_value(const Teacher& value) {
    print_person(value);
    std::cout << ", кафедра=";
    print_text(value.get_department());
    std::cout << ", должность=";
    print_text(value.get_position());
}

void print_value(const FunctionValue& value) {
    std::cout << value.get_id() << " (" << value.get_name() << ')';
}

template <typename Key, typename Value>
void print_entry(const Key& key, const Value& value) {
    print_value(key);
    std::cout << " -> ";
    print_value(value);
    std::cout << '\n';
}

template <typename Tree>
void print_node(const typename Tree::Node* node) {
    print_entry(node->key, node->value);
    std::cout << "Цвет: " << (node->color ? "чёрный" : "красный") << ".\n";
}

bool choose_action(const char* first, const char* second, int& choice) {
    std::cout << "1. " << first << "\n2. " << second << "\n0. Назад\n";
    return read_choice("Выберите действие: ", 0, 2, choice);
}

template <typename Tree>
bool choose_order(typename Tree::TraversalOrder& order) {
    std::cout << "1. NLR — корень, левое, правое\n"
                 "2. NRL — корень, правое, левое\n"
                 "3. LNR — левое, корень, правое\n"
                 "4. RNL — правое, корень, левое\n"
                 "5. LRN — левое, правое, корень\n"
                 "6. RLN — правое, левое, корень\n";
    int choice;
    if (!read_choice("Обход: ", 1, 6, choice)) return false;
    order = static_cast<typename Tree::TraversalOrder>(choice - 1);
    return true;
}

long long checked_sum(long long first, long long second) {
    if ((second > 0 && first > std::numeric_limits<long long>::max() - second) ||
        (second < 0 && first < std::numeric_limits<long long>::min() - second)) {
        throw std::overflow_error("Результат не помещается в long long.");
    }
    return first + second;
}

int checked_integer(long long value) {
    if (value < std::numeric_limits<int>::min() || value > std::numeric_limits<int>::max()) {
        throw std::overflow_error("Результат не помещается в int.");
    }
    return static_cast<int>(value);
}

double checked_real(double value) {
    if (!valid_number(value)) throw std::overflow_error("Результат не является конечным числом.");
    return value;
}

Complex checked_complex(const Complex& value) {
    if (!valid_number(value)) throw std::overflow_error("Результат не является конечным комплексным числом.");
    return value;
}

int double_integer(const int& value) { return checked_integer(static_cast<long long>(value) * 2); }
int increment_integer(const int& value) { return checked_integer(static_cast<long long>(value) + 1); }
double double_real(const double& value) { return checked_real(value * 2); }
double increment_real(const double& value) { return checked_real(value + 1); }
Complex conjugate_value(const Complex& value) { return std::conj(value); }
Complex double_complex(const Complex& value) { return checked_complex(value * 2.0); }

struct StringMap {
    int choice;
    std::string text;
    std::string operator()(const std::string& value) const {
        if (choice == 1) return value + text;
        return text + value;
    }
};

FunctionValue next_function(const FunctionValue& value) {
    if (value.empty()) return value;
    return FunctionValue(value.get_id() % 3 + 1);
}

FunctionValue clear_function(const FunctionValue&) { return FunctionValue(); }

Student next_course(const Student& value) {
    return Student(value, value.get_group(), increment_integer(value.get_course()));
}

Student reset_course(const Student& value) {
    return Student(value, value.get_group(), 0);
}

struct TeacherMap {
    int choice;
    std::string text;
    Teacher operator()(const Teacher& value) const {
        if (choice == 1) return Teacher(value, value.get_department(), text);
        return Teacher(value, text, value.get_position());
    }
};

bool choose_map(std::function<int(const int&)>& action) {
    int choice;
    if (!choose_action("Удвоить значения", "Прибавить единицу", choice)) return false;
    if (choice == 1) action = double_integer;
    else if (choice == 2) action = increment_integer;
    return true;
}

bool choose_map(std::function<double(const double&)>& action) {
    int choice;
    if (!choose_action("Удвоить значения", "Прибавить единицу", choice)) return false;
    if (choice == 1) action = double_real;
    else if (choice == 2) action = increment_real;
    return true;
}

bool choose_map(std::function<Complex(const Complex&)>& action) {
    int choice;
    if (!choose_action("Комплексное сопряжение", "Удвоить значения", choice)) return false;
    if (choice == 1) action = conjugate_value;
    else if (choice == 2) action = double_complex;
    return true;
}

bool choose_map(std::function<std::string(const std::string&)>& action) {
    int choice;
    if (!choose_action("Добавить суффикс", "Добавить префикс", choice)) return false;
    if (choice == 0) return true;
    std::string text;
    if (!read_line("Добавляемый текст: ", text)) return false;
    action = StringMap{choice, text};
    return true;
}

bool choose_map(std::function<FunctionValue(const FunctionValue&)>& action) {
    int choice;
    if (!choose_action("Следующая функция каталога (ID 0 сохраняется)", "Установить ID 0", choice)) return false;
    if (choice == 1) action = next_function;
    else if (choice == 2) action = clear_function;
    return true;
}

bool choose_map(std::function<Student(const Student&)>& action) {
    int choice;
    if (!choose_action("Увеличить курс на 1", "Установить курс 0 (не указан)", choice)) return false;
    if (choice == 1) action = next_course;
    else if (choice == 2) action = reset_course;
    return true;
}

bool choose_map(std::function<Teacher(const Teacher&)>& action) {
    int choice;
    if (!choose_action("Изменить должность", "Изменить кафедру", choice)) return false;
    if (choice == 0) return true;
    std::string text;
    if (!read_required_text("Новое значение поля: ", text)) return false;
    action = TeacherMap{choice, text};
    return true;
}

bool positive_integer(const int& value) { return value > 0; }
bool even_integer(const int& value) { return value % 2 == 0; }
bool positive_real(const double& value) { return value > 0; }
bool nonzero_real(const double& value) { return value != 0; }
bool positive_real_part(const Complex& value) { return value.real() > 0; }
bool nonzero_imaginary_part(const Complex& value) { return value.imag() != 0; }
bool nonempty_string(const std::string& value) { return !value.empty(); }
bool nonempty_function(const FunctionValue& value) { return !value.empty(); }

struct StringPrefix {
    std::string prefix;
    bool operator()(const std::string& value) const {
        return value.compare(0, prefix.size(), prefix) == 0;
    }
};

struct FunctionMatch {
    int id;
    bool operator()(const FunctionValue& value) const { return value.get_id() == id; }
};

struct StudentPredicate {
    int choice;
    int course;
    std::string group;
    bool operator()(const Student& value) const {
        if (choice == 1) return value.get_course() >= course;
        return value.get_group() == group;
    }
};

struct TeacherPredicate {
    int choice;
    std::string text;
    bool operator()(const Teacher& value) const {
        if (choice == 1) return value.get_department() == text;
        return value.get_position() == text;
    }
};

bool choose_predicate(std::function<bool(const int&)>& predicate) {
    int choice;
    if (!choose_action("Положительные значения", "Чётные значения", choice)) return false;
    if (choice == 1) predicate = positive_integer;
    else if (choice == 2) predicate = even_integer;
    return true;
}

bool choose_predicate(std::function<bool(const double&)>& predicate) {
    int choice;
    if (!choose_action("Положительные значения", "Ненулевые значения", choice)) return false;
    if (choice == 1) predicate = positive_real;
    else if (choice == 2) predicate = nonzero_real;
    return true;
}

bool choose_predicate(std::function<bool(const Complex&)>& predicate) {
    int choice;
    if (!choose_action("Действительная часть больше 0", "Мнимая часть не равна 0", choice)) return false;
    if (choice == 1) predicate = positive_real_part;
    else if (choice == 2) predicate = nonzero_imaginary_part;
    return true;
}

bool choose_predicate(std::function<bool(const std::string&)>& predicate) {
    int choice;
    if (!choose_action("Непустые строки", "Строки с заданным префиксом", choice)) return false;
    if (choice == 1) predicate = nonempty_string;
    else if (choice == 2) {
        std::string prefix;
        if (!read_line("Префикс: ", prefix)) return false;
        predicate = StringPrefix{prefix};
    }
    return true;
}

bool choose_predicate(std::function<bool(const FunctionValue&)>& predicate) {
    int choice;
    if (!choose_action("Непустые функции", "Функции с заданным ID", choice)) return false;
    if (choice == 1) predicate = nonempty_function;
    else if (choice == 2) {
        FunctionValue value;
        if (!read_value("ID функции: ", value)) return false;
        predicate = FunctionMatch{value.get_id()};
    }
    return true;
}

bool choose_predicate(std::function<bool(const Student&)>& predicate) {
    int choice;
    if (!choose_action("Курс не меньше заданного", "Студенты заданной группы", choice)) return false;
    if (choice == 0) return true;
    StudentPredicate condition{choice, 0, ""};
    if (choice == 1) {
        if (!read_choice("Минимальный курс: ", 0, std::numeric_limits<int>::max(), condition.course)) return false;
    } else if (!read_line("Группа: ", condition.group)) return false;
    predicate = condition;
    return true;
}

bool choose_predicate(std::function<bool(const Teacher&)>& predicate) {
    int choice;
    if (!choose_action("Преподаватели заданной кафедры", "Преподаватели заданной должности", choice)) return false;
    if (choice == 0) return true;
    std::string text;
    if (!read_line("Значение для сравнения: ", text)) return false;
    predicate = TeacherPredicate{choice, text};
    return true;
}

long long sum_integer(const long long& total, const int& value) { return checked_sum(total, value); }
double sum_real(const double& total, const double& value) { return checked_real(total + value); }
Complex sum_complex(const Complex& total, const Complex& value) { return checked_complex(total + value); }
std::string join_strings(const std::string& total, const std::string& value) { return total + value; }
long long sum_courses(const long long& total, const Student& value) { return checked_sum(total, value.get_course()); }

template <typename Value>
long long count_elements(const long long& total, const Value&) { return checked_sum(total, 1); }

struct FunctionSum {
    int argument;
    long long operator()(const long long& total, const FunctionValue& value) const {
        if (value.empty()) throw std::logic_error("В дереве есть невыбранная функция с ID 0.");
        return checked_sum(total, value.call(argument));
    }
};

struct CountDepartment {
    std::string department;
    long long operator()(const long long& total, const Teacher& value) const {
        return checked_sum(total, value.get_department() == department ? 1 : 0);
    }
};

const char* reduce_title(const int&) { return "Сумма значений (long long)"; }
const char* reduce_title(const double&) { return "Сумма значений"; }
const char* reduce_title(const Complex&) { return "Сумма комплексных значений"; }
const char* reduce_title(const std::string&) { return "Соединить строки"; }
const char* reduce_title(const FunctionValue&) { return "Сумма результатов вызова функций"; }
const char* reduce_title(const Student&) { return "Сумма курсов"; }
const char* reduce_title(const Teacher&) { return "Количество преподавателей заданной кафедры"; }

template <typename Key, typename Compare>
bool reduce_special(const RedBlackTree<Key, int, Compare>& tree,
                    typename RedBlackTree<Key, int, Compare>::TraversalOrder order) {
    long long initial;
    if (!read_value("Начальное значение (long long): ", initial)) return false;
    auto result = tree.reduce(initial, sum_integer, order);
    std::cout << "Результат: " << result << '\n';
    return true;
}

template <typename Key, typename Compare>
bool reduce_special(const RedBlackTree<Key, double, Compare>& tree,
                    typename RedBlackTree<Key, double, Compare>::TraversalOrder order) {
    double initial;
    if (!read_value("Начальное значение: ", initial)) return false;
    auto result = tree.reduce(initial, sum_real, order);
    std::cout << "Результат: " << result << '\n';
    return true;
}

template <typename Key, typename Compare>
bool reduce_special(const RedBlackTree<Key, Complex, Compare>& tree,
                    typename RedBlackTree<Key, Complex, Compare>::TraversalOrder order) {
    Complex initial;
    if (!read_value("Начальное значение (re,im): ", initial)) return false;
    auto result = tree.reduce(initial, sum_complex, order);
    std::cout << "Результат: " << result << '\n';
    return true;
}

template <typename Key, typename Compare>
bool reduce_special(const RedBlackTree<Key, std::string, Compare>& tree,
                    typename RedBlackTree<Key, std::string, Compare>::TraversalOrder order) {
    std::string initial;
    if (!read_line("Начальная строка (можно пустую): ", initial)) return false;
    std::string result = tree.reduce(initial, join_strings, order);
    std::cout << "Результат: ";
    print_text(result);
    std::cout << '\n';
    return true;
}

template <typename Key, typename Compare>
bool reduce_special(const RedBlackTree<Key, FunctionValue, Compare>& tree,
                    typename RedBlackTree<Key, FunctionValue, Compare>::TraversalOrder order) {
    int argument;
    long long initial;
    if (!read_value("Аргумент функций: ", argument)) return false;
    if (!read_value("Начальное значение (long long): ", initial)) return false;
    auto result = tree.reduce(initial, FunctionSum{argument}, order);
    std::cout << "Результат: " << result << '\n';
    return true;
}

template <typename Key, typename Compare>
bool reduce_special(const RedBlackTree<Key, Student, Compare>& tree,
                    typename RedBlackTree<Key, Student, Compare>::TraversalOrder order) {
    long long initial;
    if (!read_value("Начальное значение (long long): ", initial)) return false;
    auto result = tree.reduce(initial, sum_courses, order);
    std::cout << "Результат: " << result << '\n';
    return true;
}

template <typename Key, typename Compare>
bool reduce_special(const RedBlackTree<Key, Teacher, Compare>& tree,
                    typename RedBlackTree<Key, Teacher, Compare>::TraversalOrder order) {
    std::string department;
    long long initial;
    if (!read_line("Кафедра: ", department)) return false;
    if (!read_value("Начальное значение (long long): ", initial)) return false;
    auto result = tree.reduce(initial, CountDepartment{department}, order);
    std::cout << "Результат: " << result << '\n';
    return true;
}

template <typename Key, typename Value>
bool valid_entry(const Key& key, const Value& value) {
    return valid_number(key) && valid_number(value);
}

bool valid_entry(const PersonID& key, const Student& value) {
    return key == value.get_id();
}

bool valid_entry(const PersonID& key, const Teacher& value) {
    return key == value.get_id();
}

template <typename Tree>
bool valid_entries(const Tree& tree, const typename Tree::Node* node) {
    if (node == tree.get_nil()) return true;
    return valid_entry(node->key, node->value) &&
           valid_entries(tree, node->left) && valid_entries(tree, node->right);
}

template <typename Key, typename Value, typename Compare>
int check_subtree(const RedBlackTree<Key, Value, Compare>& tree,
                  const typename RedBlackTree<Key, Value, Compare>::Node* node,
                  const typename RedBlackTree<Key, Value, Compare>::Node* parent,
                  const Key* lower, const Key* upper) {
    if (node == tree.get_nil()) return 1;
    if (node == nullptr || node->left == nullptr || node->right == nullptr) return 0;
    if (node->parent != parent) return 0;
    if (lower != nullptr && !tree.get_compare()(*lower, node->key)) return 0;
    if (upper != nullptr && !tree.get_compare()(node->key, *upper)) return 0;
    if (!node->color && (!node->left->color || !node->right->color)) return 0;

    int left = check_subtree(tree, node->left, node, lower, &node->key);
    if (left == 0) return 0;
    int right = check_subtree(tree, node->right, node, &node->key, upper);
    if (left != right) return 0;
    return left + (node->color ? 1 : 0);
}

template <typename Key, typename Value, typename Compare>
bool valid_tree(const RedBlackTree<Key, Value, Compare>& tree) {
    if (!tree.get_root()->color || !tree.get_nil()->color) return false;
    if (tree.get_nil()->left != tree.get_nil() || tree.get_nil()->right != tree.get_nil()) return false;
    const Key* lower = nullptr;
    const Key* upper = nullptr;
    return check_subtree(tree, tree.get_root(), tree.get_nil(), lower, upper) > 0;
}

template <typename Tree>
std::size_t count_nodes(const Tree& tree, const typename Tree::Node* node) {
    if (node == tree.get_nil()) return 0;
    return 1 + count_nodes(tree, node->left) + count_nodes(tree, node->right);
}

template <typename Tree>
int tree_height(const Tree& tree, const typename Tree::Node* node) {
    if (node == tree.get_nil()) return 0;
    int left = tree_height(tree, node->left);
    int right = tree_height(tree, node->right);
    return 1 + (left > right ? left : right);
}

template <typename Tree>
void print_structure(const Tree& tree, const typename Tree::Node* node,
                     int depth, char branch, std::size_t& shown, bool& truncated) {
    if (node == tree.get_nil()) return;
    if (shown == 50) {
        truncated = true;
        return;
    }
    for (int i = 0; i < depth; ++i) std::cout << "  ";
    if (depth != 0) std::cout << branch << " - ";
    print_value(node->key);
    std::cout << (node->color ? " [Ч]\n" : " [К]\n");
    ++shown;
    print_structure(tree, node->left, depth + 1, 'L', shown, truncated);
    print_structure(tree, node->right, depth + 1, 'R', shown, truncated);
}

template <typename Key, typename Value>
struct PrintEntries {
    std::size_t& count;
    void operator()(const Key& key, const Value& value) const {
        if (count < 50) print_entry(key, value);
        ++count;
    }
};

template <typename Key, typename Value, typename Compare>
bool show_tree(const RedBlackTree<Key, Value, Compare>& tree) {
    if (tree.empty()) {
        std::cout << "Дерево пусто.\n";
        return true;
    }
    int choice;
    if (!choose_action("Обход с выводом ключей и значений", "Структура и цвета", choice)) return false;
    if (choice == 0) return true;

    std::size_t shown = 0;
    if (choice == 1) {
        using Tree = RedBlackTree<Key, Value, Compare>;
        typename Tree::TraversalOrder order;
        if (!choose_order<Tree>(order)) return false;
        tree.traverse(order, PrintEntries<Key, Value>{shown});
        if (shown > 50) std::cout << "Показаны первые 50 из " << shown << " элементов.\n";
    } else {
        bool truncated = false;
        std::cout << "Ключ [цвет]: Ч — чёрный, К — красный.\n";
        print_structure(tree, tree.get_root(), 0, ' ', shown, truncated);
        if (truncated) std::cout << "Вывод ограничен первыми 50 узлами.\n";
    }
    return true;
}

template <typename Key, typename Value, typename Compare>
bool insert_element(RedBlackTree<Key, Value, Compare>& tree) {
    Key key;
    Value value;
    if (!read_entry(key, value)) return false;
    if (tree.find(key) != nullptr) {
        std::cout << "Такой ключ уже существует. Прежнее значение сохранено.\n";
        return true;
    }
    tree.insert(key, value);
    std::cout << "Элемент добавлен.\n";
    return true;
}

template <typename Key, typename Value, typename Compare>
bool erase_element(RedBlackTree<Key, Value, Compare>& tree) {
    Key key;
    if (!read_value("Удаляемый ключ: ", key)) return false;
    bool erased = tree.erase(key);
    std::cout << (erased ? "Элемент удалён.\n" : "Ключ не найден.\n");
    return true;
}

template <typename Key, typename Value, typename Compare>
bool find_element(const RedBlackTree<Key, Value, Compare>& tree) {
    Key key;
    if (!read_value("Искомый ключ: ", key)) return false;
    auto node = tree.find(key);
    if (node == nullptr) std::cout << "Ключ не найден.\n";
    else print_node<RedBlackTree<Key, Value, Compare>>(node);
    return true;
}

template <typename Key, typename Value, typename Compare>
bool map_tree(const RedBlackTree<Key, Value, Compare>& tree,
              RedBlackTree<Key, Value, Compare>& result) {
    std::function<Value(const Value&)> action;
    if (!choose_map(action)) return false;
    if (!action) return true;
    result = tree.map(action);
    std::cout << "Результат map записан в дерево R.\n";
    return true;
}

template <typename Key, typename Value, typename Compare>
bool filter_tree(const RedBlackTree<Key, Value, Compare>& tree,
                 RedBlackTree<Key, Value, Compare>& result) {
    std::function<bool(const Value&)> predicate;
    if (!choose_predicate(predicate)) return false;
    if (!predicate) return true;
    result = tree.where(predicate);
    std::cout << "Результат where записан в дерево R.\n";
    return true;
}

template <typename Key, typename Value, typename Compare>
bool reduce_tree(const RedBlackTree<Key, Value, Compare>& tree) {
    int choice;
    if (!choose_action(reduce_title(tree.get_nil()->value), "Количество элементов", choice)) return false;
    if (choice == 0) return true;
    using Tree = RedBlackTree<Key, Value, Compare>;
    typename Tree::TraversalOrder order;
    if (!choose_order<Tree>(order)) return false;
    if (choice == 1) return reduce_special(tree, order);

    long long initial;
    if (!read_value("Начальное значение (long long): ", initial)) return false;
    long long result = tree.reduce(initial, count_elements<Value>, order);
    std::cout << "Результат: " << result << '\n';
    return true;
}

template <typename Key, typename Value, typename Compare>
bool extract_tree(const RedBlackTree<Key, Value, Compare>& tree,
                  RedBlackTree<Key, Value, Compare>& result) {
    Key key;
    if (!read_value("Ключ корня поддерева: ", key)) return false;
    bool found = tree.find(key) != nullptr;
    result = tree.extract_subtree(key);
    if (found) std::cout << "Поддерево записано в дерево R.\n";
    else std::cout << "Ключ не найден. В дерево R записано пустое дерево.\n";
    return true;
}

template <typename Key, typename Value, typename Compare>
bool find_path(const RedBlackTree<Key, Value, Compare>& tree) {
    int choice;
    if (!choose_action("Абсолютный путь от корня", "Относительный путь от узла", choice)) return false;
    if (choice == 0) return true;
    std::cout << "L — влево, R — вправо";
    if (choice == 2) std::cout << ", U — к родителю";
    std::cout << ". Пустой путь возвращает начальный узел.\n";

    Key key = Key();
    if (choice == 2 && !read_value("Ключ начального узла: ", key)) return false;
    std::string path;
    if (!read_line("Путь: ", path)) return false;
    for (char step : path) {
        if (step != 'L' && step != 'R' && !(choice == 2 && step == 'U')) {
            std::cout << "Недопустимый символ пути.\n";
            return true;
        }
    }
    auto node = choice == 1 ? tree.find_by_path(path.c_str()) : tree.find_by_path(key, path.c_str());
    if (node == nullptr) std::cout << "Узел по заданному пути не найден.\n";
    else print_node<RedBlackTree<Key, Value, Compare>>(node);
    return true;
}

template <typename Tree>
bool save_tree(const Tree& tree) {
    typename Tree::TraversalOrder order;
    if (!choose_order<Tree>(order)) return false;
    std::string text = tree.to_string(order);
    std::cout << "Сохранённая строка:\n" << text;
    std::size_t lines = 0;
    for (char symbol : text) if (symbol == '\n') ++lines;
    std::cout << "Количество строк для чтения: " << lines << "\nКонец сохранённой строки.\n";
    return true;
}

template <typename Tree>
bool load_tree(Tree& tree) {
    int lines;
    if (!read_choice("Количество вводимых строк (включая заголовок): ",
                     1, std::numeric_limits<int>::max(), lines)) return false;
    std::cout << "Введите сохранённую строку. Первая строка — имя обхода.\n";
    std::string text, line;
    for (int i = 0; i < lines; ++i) {
        if (!read_line("", line)) return false;
        text += line;
        text += '\n';
    }

    Tree loaded(tree.get_compare());
    if (!loaded.from_string(text)) {
        std::cout << "Ошибка формата или свойств дерева. Прежнее содержимое сохранено.\n";
    } else if (!valid_entries(loaded, loaded.get_root())) {
        std::cout << "Некорректные данные: проверьте конечность чисел и совпадение ключей с ID. Прежнее содержимое сохранено.\n";
    } else {
        tree = std::move(loaded);
        std::cout << "Дерево прочитано.\n";
    }
    return true;
}

template <typename Tree>
bool call_function(const Tree&) {
    std::cout << "Эта операция доступна только для дерева функций.\n";
    return true;
}

bool call_function(const FunctionTree& tree) {
    int key, argument;
    if (!read_value("Ключ функции: ", key)) return false;
    auto node = tree.find(key);
    if (node == nullptr) {
        std::cout << "Ключ не найден.\n";
        return true;
    }
    if (node->value.empty()) {
        std::cout << "Для этого элемента функция не выбрана (ID 0).\n";
        return true;
    }
    if (!read_value("Аргумент: ", argument)) return false;
    std::cout << "Результат вызова: " << node->value.call(argument) << '\n';
    return true;
}

bool choose_tree(const char* prompt, int& index) {
    std::cout << "1. A\n2. B\n3. R\n";
    int choice;
    if (!read_choice(prompt, 1, 3, choice)) return false;
    index = choice - 1;
    return true;
}

void show_operations(bool functions) {
    std::cout << " 1. Выбрать дерево A / B / R\n"
                 " 2. Добавить элемент\n"
                 " 3. Удалить элемент по ключу\n"
                 " 4. Найти элемент по ключу\n"
                 " 5. Показать дерево\n"
                 " 6. Выполнить map\n"
                 " 7. Выполнить where\n"
                 " 8. Выполнить reduce\n"
                 " 9. Слить два дерева\n"
                 "10. Извлечь поддерево\n"
                 "11. Проверить вхождение поддерева\n"
                 "12. Найти узел по пути\n"
                 "13. Сохранить в строку\n"
                 "14. Прочитать из строки\n"
                 "15. Проверить свойства дерева\n"
                 "16. Показать количество узлов и высоту\n";
    if (functions) std::cout << "17. Вызвать функцию по ключу\n";
    std::cout << " 0. Вернуться к выбору типа\n";
}

template <typename Tree>
bool run_tree_menu(const char* type_name, bool functions = false) {
    Tree trees[3];
    const char* names[] = {"A", "B", "R"};
    int active = 0;

    while (true) {
        std::cout << "\nТип данных: " << type_name << "\nАктивное дерево: " << names[active] << '\n';
        show_operations(functions);
        int command;
        if (!read_choice("Выберите действие: ", 0, functions ? 17 : 16, command)) return false;

        Tree& tree = trees[active];
        try {
            switch (command) {
                case 0: {
                    if (trees[0].empty() && trees[1].empty() && trees[2].empty()) return true;
                    int answer;
                    if (!read_choice("Данные A, B и R будут сброшены. 1 — продолжить, 0 — остаться: ",
                                     0, 1, answer)) return false;
                    if (answer == 1) return true;
                    std::cout << "Смена типа отменена.\n";
                    break;
                }
                case 1:
                    if (!choose_tree("Выберите активное дерево: ", active)) return false;
                    break;
                case 2:
                    if (!insert_element(tree)) return false;
                    break;
                case 3:
                    if (!erase_element(tree)) return false;
                    break;
                case 4:
                    if (!find_element(tree)) return false;
                    break;
                case 5:
                    if (!show_tree(tree)) return false;
                    break;
                case 6:
                    if (!map_tree(tree, trees[2])) return false;
                    break;
                case 7:
                    if (!filter_tree(tree, trees[2])) return false;
                    break;
                case 8:
                    if (!reduce_tree(tree)) return false;
                    break;
                case 9: {
                    int first, second;
                    if (!choose_tree("Первое дерево (приоритет при совпадении ключей): ", first)) return false;
                    if (!choose_tree("Второе дерево: ", second)) return false;
                    trees[2] = trees[first].merge(trees[second]);
                    std::cout << "Слияние записано в дерево R. Использован компаратор первого дерева.\n";
                    break;
                }
                case 10:
                    if (!extract_tree(tree, trees[2])) return false;
                    break;
                case 11: {
                    int first, second;
                    if (!choose_tree("В каком дереве искать: ", first)) return false;
                    if (!choose_tree("Какое дерево проверить на вхождение: ", second)) return false;
                    bool found = trees[first].contains_subtree(trees[second]);
                    std::cout << (found ? "Поддерево входит.\n" : "Поддерево не входит.\n");
                    break;
                }
                case 12:
                    if (!find_path(tree)) return false;
                    break;
                case 13:
                    if (!save_tree(tree)) return false;
                    break;
                case 14:
                    if (!load_tree(tree)) return false;
                    break;
                case 15:
                    if (valid_tree(tree) && valid_entries(tree, tree.get_root())) {
                        std::cout << "Свойства и данные дерева корректны.\n";
                    } else {
                        std::cout << "Обнаружено нарушение свойств или данных дерева.\n";
                    }
                    break;
                case 16:
                    std::cout << "Количество узлов: " << count_nodes(tree, tree.get_root())
                              << "\nВысота: " << tree_height(tree, tree.get_root()) << '\n';
                    break;
                case 17:
                    if (!call_function(tree)) return false;
                    break;
            }
        } catch (const std::bad_alloc&) {
            std::cout << "Недостаточно памяти для операции.\n";
        } catch (const std::exception& error) {
            std::cout << "Ошибка: " << error.what() << '\n';
        }
    }
}

int main() {
#ifdef _WIN32
    unsigned int input_page = GetConsoleCP();
    unsigned int output_page = GetConsoleOutputCP();
    SetConsoleCP(CP_UTF8);
    SetConsoleOutputCP(CP_UTF8);
#endif
    std::cout.precision(std::numeric_limits<double>::max_digits10);
    bool running = true;
    while (running) {
        std::cout << "\nКРАСНО-ЧЁРНОЕ ДЕРЕВО\n"
                     "1. Целые числа\n"
                     "2. Вещественные числа\n"
                     "3. Комплексные числа\n"
                     "4. Строки\n"
                     "5. Функции\n"
                     "6. Студенты\n"
                     "7. Преподаватели\n"
                     "0. Выход\n";
        int choice;
        if (!read_choice("Выберите тип: ", 0, 7, choice) || choice == 0) break;
        if (choice == 2 || choice == 3) std::cout << "Дробная часть записывается через точку.\n";
        if (choice == 3) std::cout << "Комплексное число вводится в формате (re,im), например (1.5,-2).\n";
        try {
            switch (choice) {
                case 1: running = run_tree_menu<IntegerTree>("целые числа"); break;
                case 2: running = run_tree_menu<RealTree>("вещественные числа"); break;
                case 3: running = run_tree_menu<ComplexTree>("комплексные числа"); break;
                case 4: running = run_tree_menu<StringTree>("строки"); break;
                case 5: running = run_tree_menu<FunctionTree>("функции", true); break;
                case 6: running = run_tree_menu<StudentTree>("студенты"); break;
                case 7: running = run_tree_menu<TeacherTree>("преподаватели"); break;
            }
        } catch (const std::exception& error) {
            std::cout << "Ошибка: " << error.what() << '\n';
        }
    }
    std::cout << "\nРабота завершена.\n";
#ifdef _WIN32
    if (input_page != 0) SetConsoleCP(input_page);
    if (output_page != 0) SetConsoleOutputCP(output_page);
#endif
    return 0;
}

