#version 460

out vec4 frag_color;

in vec3 normal;
in vec3 vert_pos;

uniform vec3 cube_color;
uniform vec3 light_color;
uniform vec3 light_pos;
uniform float ambient_strength;
uniform vec3 camera_pos;
float specular_strength;

void main () {
    vec3 ambient_color = ambient_strength * light_color;

    vec3 norm = normalize(normal);
    vec3 light_dir = normalize(light_pos-vert_pos);
    float diffuse_strength = max(dot(norm, light_dir), 0.0);
    vec3 diffuse_color = diffuse_strength * light_color;

    vec3 view_dir = normalize(camera_pos - vert_pos);
    vec3 reflect_dir = reflect(-light_dir, norm);
    float spec_level = pow(max(dot(view_dir, reflect_dir), 0.0), 32);
    vec3 specular_color = specular_strength * spec_level * light_color;

    frag_color = vec4((ambient_color+diffuse_color+specular_color) * cube_color, 1.0);
};