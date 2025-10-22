#version 330 core
out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D tex;

void main()
{
    float intensity = texture(tex, TexCoord).r;
    FragColor = vec4(vec3(intensity), 1.0);
}