#ifndef FILE_GETTER_H
#define FILE_GETTER_H
#include "image.h"

#include <queue>
#include <string>
#include <thread>

namespace FileGetter
{
class FileGetter
{
public:
    FileGetter()                   = default;
    ~FileGetter()                  = default;
    virtual bool refreshDatabase() = 0;
    virtual Image getNext()        = 0;
};

class LocalFileGetter : public FileGetter
{
public:
    LocalFileGetter(const char* base_folder, const char* types[], size_t types_len);
    ~LocalFileGetter();

    virtual bool refreshDatabase();

    virtual Image getNext();

private:
    std::string getNextPath();

    const char* _base_folder;
    const char** _types;
    size_t _types_len;

    size_t _pos = 0;
    std::vector<std::string> _files_path;
};

class ImageBuffer
{
public:
    ImageBuffer(FileGetter& file_getter);
    ~ImageBuffer();

    Image getNext();

    size_t sizeOfBuffer();
    bool empty();

    size_t pushNextImageToBuffer();
    static void runInBackground(ImageBuffer* image_buffer);

private:
    FileGetter& _file_getter;

    std::queue<Image> _buffer;

    bool _run_in_background = true;
    std::thread _background_thread;
};

} // namespace FileGetter
#endif // FILE_GETTER_H