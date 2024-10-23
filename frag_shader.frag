#version 460

out vec4 frag_color;

in vec4 vertex_color_out;

void main () {
    frag_color = vertex_color_out;
};