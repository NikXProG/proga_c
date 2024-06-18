#include "../include/logger.h"

#include <ctime>
#include <sstream>
#include <iomanip>
#include <chrono>


logger const *logger::trace(
    std::string const &message) const noexcept
{
    return log(message, logger::severity::trace);
}

logger const *logger::debug(
    std::string const &message) const noexcept
{
    return log(message, logger::severity::debug);
}

logger const *logger::information(
    std::string const &message) const noexcept
{
    return log(message, logger::severity::information);
}

logger const *logger::warning(
    std::string const &message) const noexcept
{
    return log(message, logger::severity::warning);
}

logger const *logger::error(
    std::string const &message) const noexcept
{
    return log(message, logger::severity::error);
}

logger const *logger::critical(
    std::string const &message) const noexcept
{
    return log(message, logger::severity::critical);
}

std::string logger::severity_to_string(
    logger::severity severity)
{
    switch (severity)
    {
        case logger::severity::trace:
            return "TRACE";
        case logger::severity::debug:
            return "DEBUG";
        case logger::severity::information:
            return "INFORMATION";
        case logger::severity::warning:
            return "WARNING";
        case logger::severity::error:
            return "ERROR";
        case logger::severity::critical:
            return "CRITICAL";
    }

    throw std::out_of_range("Invalid severity value");
}


std::string logger::current_datetime_to_string() noexcept {
    auto now = std::chrono::system_clock::now();
    std::time_t time = std::chrono::system_clock::to_time_t(now);
    
    std::tm tm;
    localtime_s(&tm, &time);
    
    std::ostringstream result_stream;
    result_stream << std::put_time(&tm, "%d.%m.%Y %H:%M:%S");
    return result_stream.str();
}