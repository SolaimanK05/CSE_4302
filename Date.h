#pragma once
#include <string>
#include <sstream>
#include <iomanip>
#include <ctime>
#include <algorithm>

using namespace std;

class Date {
private:
    int day, month, year;

    int toJulian() const {
        //boost c++ julian day formula
        int a = (14 - month) / 12;
        int y = year + 4800 - a;
        int m = month + 12 * a - 3;
        return day + (153*m+2)/5 + 365*y + y/4 - y/100 + y/400 - 32045;
    }

public:
    Date() : day(1), month(1), year(2026) {}
    Date(int d, int m, int y) : day(d), month(m), year(y) {}

    static Date today() {
        time_t t = time(nullptr);
        tm* lt = localtime(&t);
        return Date(lt->tm_mday, lt->tm_mon + 1, lt->tm_year + 1900);
    }

    bool isValid() const {
        if (month < 1 || month > 12 || year < 2024 || year > 2035) return false;
        int dim[] = {31,28,31,30,31,30,31,31,30,31,30,31};
        if (year % 4 == 0 && (year % 100 != 0 || year % 400 == 0)) dim[1] = 29;
        return day >= 1 && day <= dim[month - 1];
    }

    int daysBetween(const Date& o) const { 
        return o.toJulian() - toJulian(); 
    }

    bool operator<(const Date& o)  const { 
        return toJulian() <  o.toJulian(); 
    }
    bool operator<=(const Date& o) const { 
        return toJulian() <= o.toJulian(); 
    }
    bool operator>=(const Date& o) const { 
        return toJulian() >= o.toJulian(); 
    }
    bool operator==(const Date& o) const { 
        return toJulian() == o.toJulian(); 
    }

    string toString() const {
        ostringstream ss;
        ss << setfill('0') << setw(2) << day << "/"
           << setw(2) << month << "/" << year;
        return ss.str();
    }

    int getDay()   const { 
        return day;   
    }
    int getMonth() const { 
        return month; 
    }
    int getYear()  const { 
        return year;  
    }
};
