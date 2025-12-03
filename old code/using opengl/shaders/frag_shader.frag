#version 460

out vec4 frag_color;

in vec4 vertex_color_out;
in vec2 tex_coords_out;

uniform sampler2D input_texture;
uniform sampler2D input_texture_2;

// uniform sampler2D texture_diffuse1;
// uniform sampler2D texture_diffuse2;
// uniform sampler2D texture_diffuse3;
// uniform sampler2D texture_specular1;
// uniform sampler2D texture_specular2;

void main () {
    frag_color = mix(texture(input_texture, tex_coords_out), texture(input_texture_2, tex_coords_out), 0.5)*vertex_color_out;
};