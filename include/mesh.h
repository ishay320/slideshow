#ifndef MESH_H
#define MESH_H
#include <cstdint>

class Mesh
{
public:
    Mesh(float* vertices, int vertices_number, uint8_t vertex_block_size, unsigned int* indices, int sizeof_indices);
    Mesh(const Mesh& other);
    ~Mesh();

    Mesh& operator=(Mesh&& other);

    void draw();
    void deallocate();

    void bind();

private:
    float* _vertices;
    uint8_t _vertex_block_size;
    unsigned int* _indices;
    int _sizeof_indices;

    unsigned int _VBO, _VAO, _EBO;
};

#endif // MESH_H