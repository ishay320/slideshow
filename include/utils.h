#pragma once
#include <filesystem>

#include "commandline.h"

struct vec2i {
    int x;
    int y;
};

inline std::vector<std::string> saparateByDelim(const std::string& s,
                                                const std::string& delim)
{
    std::vector<std::string> out;
    size_t pos  = 0;
    size_t last = 0;
    while ((pos = s.find(delim, last)) != std::string::npos) {
        out.push_back(s.substr(last, pos - last));
        last = pos + 1;
    }
    return out;
}

inline std::vector<std::filesystem::path> getFilesByTypes(
    const std::filesystem::path& path, const std::vector<std::string>& types)
{
    std::vector<std::string> search_prompt;
    if (types.empty()) {
        throw std::runtime_error{"no types passed"};
    }

    search_prompt.push_back(path);
    search_prompt.push_back("-iname");
    search_prompt.push_back("*\\." + types[0]);
    for (size_t i = 1; i < types.size(); i++) {
        search_prompt.push_back("-o");
        search_prompt.push_back("-iname");
        search_prompt.push_back("*\\." + types[i]);
    }

    std::vector<std::filesystem::path> out;
    std::string c_out = run_command_with_out("find", search_prompt);
    for (auto&& s : saparateByDelim(c_out, "\n")) {
        out.push_back(s);
    }
    return out;
}
