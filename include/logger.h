#pragma once
#include <iostream>

#define LOG_ERROR(...)                                            \
    std::cout << "\033[31m" << __FILE__ << ':' << __LINE__ << " " \
              << "[ERROR] \033[0m" << __VA_ARGS__ << "\n"

#define LOG_INFO(...)                                             \
    std::cout << "\033[32m" << __FILE__ << ':' << __LINE__ << " " \
              << "[INFO] \033[0m" << __VA_ARGS__ << "\n"

#define LOG_WARNING(...)                                          \
    std::cout << "\033[33m" << __FILE__ << ':' << __LINE__ << " " \
              << "[WARNING] \033[0m" << __VA_ARGS__ << "\n"

#define LOG_DEBUG(...)                                            \
    std::cout << "\033[34m" << __FILE__ << ':' << __LINE__ << " " \
              << __FUNCTION__ << " [DEBUG] \033[0m" << __VA_ARGS__ << "\n"
