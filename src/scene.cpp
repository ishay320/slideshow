#include "scene.h"

Scene::Scene() {}

Scene::Scene(Camera& camera, Object& object)
{
    addCamera(camera);
    addObject(object);
}

Scene::Scene(Camera& camera) { addCamera(camera); }

Scene::~Scene()
{
    // TODO: empty vectors
}

void Scene::addCamera(Camera& camera) { _cameras.push_back(&camera); }

bool Scene::chooseCamera(size_t number)
{
    if (_cameras.size() < number)
    {
        return false;
    }

    _current_camera = number;
    return true;
}

bool Scene::removeCamera(size_t number)
{
    size_t cameras_size = _cameras.size();
    if (_cameras.size() < number)
    {
        return false;
    }

    _cameras.erase(_cameras.begin() + number);

    if (cameras_size == _cameras.size())
    {
        return false;
    }
    return true;
}

void Scene::addObject(Object& object) { _objects.push_back(&object); }

bool Scene::removeObject(size_t number)
{
    size_t objects_size = _objects.size();
    if (_objects.size() < number)
    {
        return false;
    }

    _objects.erase(_objects.begin() + number);

    if (objects_size == _objects.size())
    {
        return false;
    }
    return true;
}

Camera* Scene::getCamera(size_t number)
{
    if (_cameras.size() < number)
    {
        return NULL;
    }

    return _cameras.at(number);
}

Object* Scene::getObject(size_t number)
{
    if (_objects.size() < number)
    {
        return NULL;
    }

    return _objects.at(number);
}

void Scene::render(float screen_width, float screen_height)
{
    for (auto&& object : _objects)
    {
        // Apply camera
        glm::mat4 projection = _cameras.at(_current_camera)->getProjectionMatrix(screen_width, screen_height);
        object->_shader.setMat4("projection", projection);
        glm::mat4 view = _cameras.at(_current_camera)->getViewMatrix();
        object->_shader.setMat4("view", view);

        // Apply transformation
        object->applyTransform();

        // Render
        object->draw();
    }
}
