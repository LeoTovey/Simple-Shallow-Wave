#version 330 core
out vec4 FragColor;

in vec2 UV;

uniform sampler2D height_map;

void main()
{
    vec3 col = texture(height_map, UV).rgb;
    FragColor = vec4(col, 1.0);
}