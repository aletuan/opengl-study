#version 330 core

layout (location = 0) in vec3 position;     // position variable has attribute position 0
layout (location = 2) in vec2 texCoord;     // texture coordination

out vec2 TexCoord;

// Declare transform matrix for coordination system
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
    gl_Position = projection * view * model * vec4(position, 1.0f);
    TexCoord    = vec2(texCoord.x, 1.0 - texCoord.y);
}
