#pragma once
#include <string>
#include <thread>
#include <vector>

#include "wrappers.h"

class FileGetter {
   public:
    FileGetter(const std::string& path, const std::vector<std::string>& types);
    ~FileGetter() = default;

    std::filesystem::path getNext();

   private:
    std::vector<std::filesystem::path> _paths;
};

class ImageGetter {
   public:
    ImageGetter(FileGetter file_getter);
    ImageGetter(ImageGetter&&)            = default;
    ImageGetter& operator=(ImageGetter&&) = default;
    ~ImageGetter();

    ImageWrapper getNext();

   private:
    void join();
    FileGetter _file_getter;
    ImageWrapper _buffer;
    std::thread _buffer_thread;
};
