#version 410 core

layout(location = 0) in vec3 pos;

uniform mat4 model;
uniform mat4 vp;

void main() {
    gl_Position = vp * model * vec4(pos, 1.0);
}
