#include <cstring>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>

namespace utils
{

/**
 * @brief read all the file to one big string
 *
 * @param path to the file
 * @return std::string
 */
inline std::string readFile(const std::string& path)
{
    std::ifstream file;
    file.open(path, std::ios_base::in);

    if (!file.is_open())
    {
        std::cerr << "ERROR: could not open file \'" + path + "\': " << strerror(errno) << std::endl;
        return "";
    }

    std::stringstream stream;
    stream << file.rdbuf();

    file.close();
    return stream.str();
}

/**
 * @brief print the matrix for debugging
 * use:   utils::printMat<float, 4, 4>(&(projection[0][0]));
 *
 * @tparam T
 * @tparam x of the matrix
 * @tparam y of the matrix
 * @param mat
 */
template <typename T, unsigned int x, unsigned int y>
inline void printMat(T* mat)
{
    const char separator = ' ';
    const int numWidth   = 8;
    const int mat_size   = x * y;
    std::cout << '|';
    for (size_t i = 0; i < mat_size; i++)
    {
        std::cout << std::left << std::setw(numWidth) << mat[i];
        if (!((i + 1) % x))
        {
            std::cout << '|';
            std::cout << '\n';
            if (!(i + 1 == mat_size))
            {
                std::cout << '|';
            }
        }
        else
        {
            std::cout << "," << std::setfill(separator);
        }
    }
    std::cout << '\n';
}

} // namespace utils
