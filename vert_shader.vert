#version 460

layout (location = 0) in vec3 world_coords;
layout (location = 1) in vec4 vertex_color;

uniform mat4 look_at;
uniform mat4 projection;

out vec4 vertex_color_out;

void main() {
    gl_Position = projection*look_at*vec4(world_coords, 1.0f);
    vertex_color_out = vertex_color;
};