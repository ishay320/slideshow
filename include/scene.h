#ifndef SCENE_H
#define SCENE_H

#include <vector>

#include "camera.h"
#include "mesh.h"
#include "object.h"
#include "shader.h"
#include "texture.h"

class Scene
{
public:
    Scene();
    Scene(Camera& camera);
    Scene(Camera& camera, Object& object);

    ~Scene();

    void addCamera(Camera& camera);

    bool chooseCamera(size_t number);

    bool removeCamera(size_t number);

    Camera* getCamera(size_t number);

    void addObject(Object& object);

    bool removeObject(size_t number);

    Object* getObject(size_t number);

    void render(float screen_width, float screen_height);

private:
    size_t _current_camera = 0;
    std::vector<Object*> _objects;
    std::vector<Camera*> _cameras;
};

#endif // SCENE_H