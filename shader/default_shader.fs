#version 330 core

struct Material {
    float shininess;
    float opacity;
    float density;
    float illum;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct Light {
    vec3 position;
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

in vec3 frag_pos;
in vec3 normal;
in vec2 tex_coords;

out vec4 FragColor;

uniform bool use_material;
uniform Material material;
uniform Light light;
uniform vec3 view_pos;
uniform sampler2D texture_diffuse1;

void main() {
    if (use_material) {
        // ambient
        vec3 ambient = light.ambient * material.ambient;

        // diffuse
        vec3 norm = normalize(normal);
        // vec3 light_dir = normalize(light.position - frag_pos);
        vec3 light_dir = normalize(-light.direction);
        float diff = max(dot(norm, light_dir), 0.0);
        vec3 diffuse = light.diffuse * diff * material.diffuse;

        // specular
        vec3 view_dir = normalize(view_pos - frag_pos);
        vec3 reflect_dir = reflect(-light_dir, norm);
        float spec = pow(max(dot(view_dir, reflect_dir), 0.0), material.shininess);
        vec3 specular = light.specular * spec * material.specular;

        // vec3 result = ambient + diffuse + specular;
        vec3 result = material.diffuse;
        FragColor = vec4(result, material.opacity);
    } else {
        FragColor = texture(texture_diffuse1, tex_coords);
    }
}
