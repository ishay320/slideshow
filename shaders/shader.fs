#version 330 core

uniform sampler2D image;

in float out_opacity;
in vec4 out_color;
in vec2 out_uv;

void main()
{
    gl_FragColor = texture(image, out_uv) * vec4(1.0, 1.0, 1.0, out_opacity);
}