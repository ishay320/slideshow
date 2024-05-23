#include "getters.h"

#include "utils.h"

FileGetter::FileGetter(const std::string& path,
                       const std::vector<std::string>& types)
{
    _paths = getFilesByTypes(path, types);
    srand(time(NULL));
}

std::filesystem::path FileGetter::getNext()
{
    int pos = rand() % _paths.size();
    return _paths[pos];
}

ImageGetter::ImageGetter(FileGetter file_getter) : _file_getter(file_getter)
{
    _buffer = _file_getter.getNext();
}
ImageGetter::~ImageGetter() { join(); }

ImageWrapper ImageGetter::getNext()
{
    join();
    ImageWrapper image = std::move(_buffer);
    _buffer_thread = std::thread{[&]() { _buffer = _file_getter.getNext(); }};
    return image;
}

void ImageGetter::join()
{
    if (_buffer_thread.joinable()) {
        _buffer_thread.join();
    }
}
