#version 460

layout (location = 0) in vec3 world_coords;
layout (location = 1) in vec3 normal_in;

uniform mat4 look_at; // a uniform is a global variable
uniform mat4 projection;

out vec3 normal;
out vec3 vert_pos;

void main() {
    gl_Position = projection*look_at*vec4(world_coords, 1.0f);
    vert_pos = world_coords;
    normal = normal_in;
};