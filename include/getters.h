#pragma once
#include <string>
#include <thread>
#include <vector>

#include "utils.h"
#include "wrappers.h"

class FileGetter {
   public:
    FileGetter(const std::string& path, const std::vector<std::string>& types)
    {
        _paths = getFilesByTypes(path, types);
        srand(time(NULL));
    }
    ~FileGetter() = default;

    std::filesystem::path getNext()
    {
        int pos = rand() % _paths.size();
        return _paths[pos];
    }

   private:
    std::vector<std::filesystem::path> _paths;
};

class ImageGetter {
   public:
    ImageGetter(FileGetter file_getter) : _file_getter(file_getter)
    {
        _buffer = _file_getter.getNext();
    }
    ImageGetter(ImageGetter&&)            = default;
    ImageGetter& operator=(ImageGetter&&) = default;
    ~ImageGetter() { join(); }

    ImageWrapper getNext()
    {
        join();
        ImageWrapper image = std::move(_buffer);
        _buffer_thread =
            std::thread{[&]() { _buffer = _file_getter.getNext(); }};
        return image;
    }

   private:
    void join()
    {
        if (_buffer_thread.joinable()) {
            _buffer_thread.join();
        }
    }
    FileGetter _file_getter;
    ImageWrapper _buffer;
    std::thread _buffer_thread;
};
