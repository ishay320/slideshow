#version 330 core
in vec3 addedColor;
in vec2 TexCoord;

out vec4 FragColor;

uniform sampler2D Texture;

void main()
{
    FragColor = texture(Texture, TexCoord);
}