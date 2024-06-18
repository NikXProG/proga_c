#include "../include/client_logger_builder.h"
#include <fstream>
//#include <json/json.h> // Uncomment this if you are using a JSON library

client_logger_builder *client_logger_builder::add_file_stream(
    const std::string &stream_file_path,
    logger::severity severity) {
    _files_streams_local[stream_file_path].insert(severity);
    return this;
}

client_logger_builder *client_logger_builder::add_console_stream(
    logger::severity severity) {
    _console_streams_local.insert(severity);
    return this;
}

client_logger_builder* client_logger_builder::transform_with_configuration(
    const std::string &configuration_file_path,
    const std::string &configuration_path) {
    std::ifstream config_file(configuration_file_path);
    if (!config_file.is_open()) {
        throw std::runtime_error("Could not open configuration file");
    }

    // Uncomment and adjust the following lines according to your JSON library
    // Json::Value config;
    // config_file >> config;
    // config_file.close();

    // auto config_node = config[configuration_path];
    // if (config_node.isNull()) {
    //     throw std::runtime_error("Invalid configuration path");
    // }

    // _log_format_mask = config_node.get("log_format_mask", "[%s][%d %t] %m").asString();

    // for (const auto &file_stream : config_node["file_streams"]) {
    //     std::string path = file_stream["path"].asString();
    //     for (const auto &severity_str : file_stream["severities"]) {
    //         _files_streams_local[path].insert(string_to_severity(severity_str.asString()));
    //     }
    // }

    // for (const auto &severity_str : config_node["console_severities"]) {
    //     _console_streams_local.insert(string_to_severity(severity_str.asString()));
    // }

    return this;
}

client_logger_builder *client_logger_builder::clear() {
    _files_streams_local.clear();
    _console_streams_local.clear();
    _log_format_mask = "[%s][%d %t] %m";
    return this;
}

logger *client_logger_builder::build() const {
    return new client_logger(_files_streams_local, _console_streams_local, _log_format_mask);
}