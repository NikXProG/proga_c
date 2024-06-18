#ifndef MATH_PRACTICE_AND_OPERATING_SYSTEMS_CLIENT_LOGGER_BUILDER_H
#define MATH_PRACTICE_AND_OPERATING_SYSTEMS_CLIENT_LOGGER_BUILDER_H

#include "logger_builder.h"
#include "client_logger.h"
#include <iostream>
#include <map>
#include <set>

class client_logger_builder : public logger_builder {
private:
    std::map<std::string, std::set<logger::severity>> _files_streams_local;
    std::set<logger::severity> _console_streams_local;
    std::string _log_format_mask = "[%s][%d %t] %m";

public:
    ~client_logger_builder() noexcept override = default;

    client_logger_builder *add_file_stream(
        const std::string &stream_file_path,
        logger::severity severity) override;

    client_logger_builder *add_console_stream(
        logger::severity severity) override;

    client_logger_builder* transform_with_configuration(
        const std::string &configuration_file_path,
        const std::string &configuration_path) override;

    client_logger_builder *clear() override;

    logger *build() const override;
};

#endif //MATH_PRACTICE_AND_OPERATING_SYSTEMS_CLIENT_LOGGER_BUILDER_H