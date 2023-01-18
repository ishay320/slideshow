#ifndef OBJECT_H
#define OBJECT_H
#include <glm/glm.hpp>

#include "mesh.h"
#include "shader.h"
#include "texture.h"

class Object
{
public:
    Object(const Shader& shader, const Mesh& mesh, const Texture& texture);
    Object(const Object& other);

    Object& operator=(Object&&);

    /**
     * @brief reset the current transformation
     *
     */
    void resetTransform();

    /**
     * @brief rotate the vertices by the specify angle in x, y, z axis
     * WARNING: additive transform - the transformation have memory (resetTransform)
     *
     * @param angle
     * @param rotation_vector
     */
    void rotate(float angle, const glm::vec3& rotation_vector);

    /**
     * @brief translate the vertices in x, y, z axis
     * WARNING: additive transform - the transformation have memory (resetTransform)
     *
     * @param translation_vector
     */
    void translate(const glm::vec3& translation_vector);

    /**
     * @brief scale the vertices in x, y, z axis
     * WARNING: additive transform - the transformation have memory (resetTransform)
     *
     * @param scale_vector
     */
    void scale(const glm::vec3& scale_vector);

    void draw();

    /**
     * @brief apply the transformation to the shader
     *
     */
    void applyTransform();

    void deallocate();

    Shader _shader;
    Mesh _mesh;
    Texture _texture;

    glm::mat4 _transform;
};

#endif // OBJECT_H