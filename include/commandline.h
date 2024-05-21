#pragma once
#include <stdio.h>

#include <array>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

inline std::string cat_command(const std::string command,
                               const std::vector<std::string> &args)
{
    std::stringstream ss;
    ss << command;
    for (auto &&arg : args) {
        ss << ' ' << arg;
    }
    return ss.str();
}

inline std::string run_command_with_out(const std::string command,
                                        const std::vector<std::string> &args)
{
    const std::string command_with_args = cat_command(command, args);

    // see:
    // https://stackoverflow.com/questions/76867698/what-does-ignoring-attributes-on-template-argument-mean-in-this-context
    std::unique_ptr<FILE, decltype(&pclose)> pipe{
        popen(command_with_args.c_str(), "r"), pclose};

    std::stringstream ss;
    std::array<char, 256> buffer;
    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
        ss << buffer.data();
    }

    return ss.str();
}

inline int run_command(const std::string command,
                       const std::vector<std::string> &args)
{
    const std::string command_with_args = cat_command(command, args);

    const auto ret = std::system(command_with_args.c_str());
    return ret;
}
