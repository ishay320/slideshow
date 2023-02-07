#version 330 core

layout(location = 0) in vec2 position;
layout(location = 1) in vec4 color;
layout(location = 2) in vec2 uv;

uniform vec2 resolution;
uniform float time;
uniform float camera_scale;
uniform vec2 camera_pos;
uniform mat4 transform;

out vec4 out_color;
out vec2 out_uv;

vec2 camera_project(vec2 point)
{
    return 2.0 * (point - camera_pos) * camera_scale / resolution;
}

void main()
{
    vec2 transform_pos = vec2(transform * vec4(position, 0, 1)); 
    gl_Position        =  vec4(camera_project(transform_pos), 1.0f, 1.0f );
    out_color          = color;
    out_uv             = uv;
}