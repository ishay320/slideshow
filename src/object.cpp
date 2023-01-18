#include "object.h"

#include <glm/gtc/matrix_transform.hpp>

Object::Object(const Shader& shader, const Mesh& mesh, const Texture& texture) : _shader(shader), _mesh(mesh), _texture(texture) { resetTransform(); }

Object::Object(const Object& other) : _shader(other._shader), _mesh(other._mesh), _texture(other._texture), _transform(other._transform) {}

Object& Object::operator=(Object&& other)
{
    // Guard self assignment
    if (this == &other)
    {
        return *this;
    }

    _shader    = other._shader;
    _texture   = other._texture;
    _mesh      = std::move(other._mesh);
    _transform = other._transform;
    return *this;
}

void Object::applyTransform() { _shader.setMat4("transform", _transform); }

void Object::resetTransform()
{
    _transform = glm::mat4(1.0f); // initialize matrix to identity matrix
    applyTransform();
}

void Object::rotate(const float angle, const glm::vec3& rotation_vector)
{
    _transform = glm::rotate(_transform, angle, rotation_vector);
    applyTransform();
}

void Object::translate(const glm::vec3& translation_vector)
{
    _transform = glm::translate(_transform, translation_vector);
    applyTransform();
}

void Object::scale(const glm::vec3& scale_vector)
{
    _transform = glm::scale(_transform, scale_vector);
    applyTransform();
}

void Object::draw()
{
    _shader.use();
    _texture.bind();
    _mesh.draw();
}
void Object::deallocate() { _mesh.deallocate(); }
