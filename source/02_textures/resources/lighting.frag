#version 330 core

in vec2 TexCoord;

out vec4 color;
out vec4 color1;

uniform vec3 objectColor;
uniform vec3 lightColor;

void main() {
    color = vec4(lightColor * objectColor, TexCoord);
}
