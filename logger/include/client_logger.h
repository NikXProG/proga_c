#ifndef MATH_PRACTICE_AND_OPERATING_SYSTEMS_CLIENT_LOGGER_H
#define MATH_PRACTICE_AND_OPERATING_SYSTEMS_CLIENT_LOGGER_H

#include "logger.h"
#include "client_logger_builder.h"

#include <iostream>
#include <sstream>
#include <map>
#include <list>
#include <string>
#include <iomanip>
#include <fstream>
#include <set>
#include <mutex>

class client_logger final :
    public logger
{

private:

    static std::map<std::string, std::pair<std::ofstream*, size_t>> _files_streams_all;

    std::map<std::string, std::pair<std::ofstream*, std::set<logger::severity>>> _files_streams_local;
   
    std::set<logger::severity> _console_streams_local;

    std::string _log_format_mask;

    std::mutex _mutex;
   
private:

    std::string string_format(std::string output_message, logger::severity severity, std::string msg) const;

public:

    client_logger(
        std::map<std::string, std::set<logger::severity>> _files_streams_local,
        std::set<logger::severity> _console_streams_local,
        std::string _log_format_mask
    );

    client_logger();

    client_logger(
        client_logger const &other);

    client_logger &operator=(
        client_logger const &other);

    client_logger(
        client_logger &&other) noexcept;   

    client_logger &operator=(
        client_logger &&other) noexcept; 

    ~client_logger() noexcept final;

    void clear_streams_all();

    void copy_from_other(client_logger const& other);

    void move_from_other(client_logger&& other);

public:

    [[nodiscard]] logger const *log(
        const std::string &message,
        logger::severity severity) const noexcept override;

};

#endif //MATH_PRACTICE_AND_OPERATING_SYSTEMS_CLIENT_LOGGER_H