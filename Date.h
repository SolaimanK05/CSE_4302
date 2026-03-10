#pragma once
#include <string>

// Represents a calendar date (day/month/year).
// Valid range: 2024-01-01 to 2035-12-31.
class Date {
private:
    int day;
    int month;
    int year;

    // Converts date to a Julian day number for arithmetic.
    int toJulian() const;

public:
    Date();
    Date(int d, int m, int y);

    // Returns today's date using the system clock.
    static Date today();

    bool isValid() const;

    // Returns the number of days from this date to other (can be negative).
    int daysBetween(const Date& other) const;

    bool operator<(const Date& other)  const;
    bool operator<=(const Date& other) const;
    bool operator>=(const Date& other) const;
    bool operator==(const Date& other) const;

    // Returns "DD/MM/YYYY" string.
    std::string toString() const;

    int getDay()   const;
    int getMonth() const;
    int getYear()  const;
};
