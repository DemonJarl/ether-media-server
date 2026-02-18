#include "chrono"
#include "format"
#define DATE_TIME_FORMAT "{:%Y-%m-%d %X}"

inline std::string getCurrentDateTime()
{
    return std::format(DATE_TIME_FORMAT, std::chrono::current_zone()->to_local(std::chrono::system_clock::now()));
}

inline std::chrono::year_month_day stringToDate(const std::string& date, const std::string& format)
{
    std::istringstream in{date};
    // in.exceptions(ios::failbit);
    std::chrono::local_seconds t;
    in >> parse(format, t);
    auto td = floor<std::chrono::days>(t);
    return std::chrono::year_month_day {td}; 
}

inline int getYearFromDate(const std::string& date)
{
    if (date.empty())
        return 0;
    auto properDate = stringToDate(date, "%Y-%m-%d");
    return int(properDate.year());
}   