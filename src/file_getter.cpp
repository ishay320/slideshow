#include "file_getter.h"

#include <cstring>
#include <filesystem>
#include <iostream>
#include <unistd.h>

namespace FileGetter
{

LocalFileGetter::LocalFileGetter(const char* base_folder, const char* types[], size_t types_len)
    : _base_folder(base_folder), _types(types), _types_len(types_len)
{
}

LocalFileGetter::~LocalFileGetter() {}

bool LocalFileGetter::refreshDatabase()
{
    if (!std::filesystem::exists(_base_folder))
    {
        return false;
    }

    for (const auto& dir_entry : std::filesystem::recursive_directory_iterator{_base_folder})
    {
        for (size_t i = 0; i < _types_len; i++)
        {
            if (!strcmp(dir_entry.path().extension().c_str(), _types[i]))
            {
                _files_path.push_back(dir_entry.path());
            }
        }
    }
    return true;
}

Image LocalFileGetter::getNext()
{
    return Image{getNextPath().c_str()};
}

std::string LocalFileGetter::getNextPath()
{
    if (_files_path.size() == 0)
    {
        std::cerr << "ERROR: paths not loaded or empty\n";
        return "";
    }
    return _files_path.at(_pos++ % _files_path.size());
}

ImageBuffer::ImageBuffer(FileGetter& file_getter) : _file_getter(file_getter)
{
    // TODO: async
    _file_getter.refreshDatabase();
    _background_thread = std::thread{runInBackground, this};
}
ImageBuffer::~ImageBuffer()
{
    _run_in_background = false;
    _background_thread.join();
}

Image ImageBuffer::getNext()
{
    if (sizeOfBuffer() == 0)
    {
        return Image();
    }
    Image out = std::move(_buffer.front());
    _buffer.pop();
    return out;
}

size_t ImageBuffer::sizeOfBuffer()
{
    return _buffer.size();
}

size_t ImageBuffer::pushNextImageToBuffer()
{
    _buffer.push(_file_getter.getNext());
    return sizeOfBuffer();
}

void ImageBuffer::runInBackground(ImageBuffer* image_buffer)
{
    while (image_buffer->_run_in_background)
    {
        if (image_buffer->sizeOfBuffer() > 2)
        {
            sleep(1);
        }
        else
        {
            image_buffer->pushNextImageToBuffer();
        }
    }
}

} // namespace FileGetter