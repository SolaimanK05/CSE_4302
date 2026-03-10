#include "Date.h"
#include <sstream>
#include <iomanip>
#include <ctime>

using namespace std;

int Date::toJulian() const {
    int a = (14 - month) / 12;
    int y = year + 4800 - a;
    int m = month + 12 * a - 3;
    return day + (153 * m + 2) / 5 + 365 * y
               + y / 4 - y / 100 + y / 400 - 32045;
}

Date::Date() : day(1), month(1), year(2026) {}

Date::Date(int d, int m, int y) : day(d), month(m), year(y) {}

Date Date::today() {
    time_t t = time(nullptr);
    tm* lt = localtime(&t);
    return Date(lt->tm_mday, lt->tm_mon + 1, lt->tm_year + 1900);
}

bool Date::isValid() const {
    if (month < 1 || month > 12 || year < 2024 || year > 2035)
        return false;
    int dim[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    bool leap = (year % 4 == 0) && (year % 100 != 0 || year % 400 == 0);
    if (leap) dim[1] = 29;
    return day >= 1 && day <= dim[month - 1];
}

int Date::daysBetween(const Date& other) const {
    return other.toJulian() - toJulian();
}

bool Date::operator<(const Date& other) const {
    return toJulian() < other.toJulian();
}
bool Date::operator<=(const Date& other) const {
    return toJulian() <= other.toJulian();
}
bool Date::operator>=(const Date& other) const {
    return toJulian() >= other.toJulian();
}
bool Date::operator==(const Date& other) const {
    return toJulian() == other.toJulian();
}

string Date::toString() const {
    ostringstream ss;
    ss << setfill('0') << setw(2) << day << "/"
       << setw(2) << month << "/" << year;
    return ss.str();
}

int Date::getDay() const { 
    return day;  
}
int Date::getMonth() const { 
    return month; 
}
int Date::getYear()  const { 
    return year;  
}
