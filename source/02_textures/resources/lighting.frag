#version 330 core

in vec2 TexCoord;

out vec4 color;
out vec4 color1;

uniform vec3 objectColor;
uniform vec3 lightColor;

//uniform sampler2D ourTexture1;

void main() {
    //color1 = texture(ourTexture1, TexCoord);
    //color = vec4(1.0f, 0.0f, 0.1f, TexCoord);
    color = vec4(lightColor * objectColor, TexCoord);
}
