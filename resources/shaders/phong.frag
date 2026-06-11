#version 410 core

uniform vec3 camera_pos;

struct Light {
    vec3 direct_pos;
    vec3 direct_val;
    vec3 ambient_val;
};
uniform Light light;

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};
uniform Material material;

// Texture Samplers
uniform sampler2D diffuseMap;
uniform sampler2D normalMap;
uniform sampler2D roughnessMap;

in vec3 interpolated_pos;
in vec3 interpolated_normal;

out vec4 fragment_color;

const float uv_scale = 0.7;

void main() {
    vec3 pos = interpolated_pos;

    vec3 dx = dFdx(pos);
    vec3 dy = dFdy(pos);
    vec3 flat_normal = normalize(cross(dx, dy));

    // --- BOX MAPPING ---
    vec3 abs_normal = abs(flat_normal);
    vec2 uv;
    int axis = 0;  // 0 = X, 1 = Y, 2 = Z

    if (abs_normal.x >= abs_normal.y && abs_normal.x >= abs_normal.z) {
        uv = pos.zy;
        axis = 0;
    } else if (abs_normal.y >= abs_normal.x && abs_normal.y >= abs_normal.z) {
        uv = pos.xz;
        axis = 1;
    } else {
        uv = pos.xy;
        axis = 2;
    }

    uv *= uv_scale;

    // Single sample per texture
    vec3 albedo = texture(diffuseMap, uv).rgb;
    float roughness = texture(roughnessMap, uv).r;
    vec3 tnorm = texture(normalMap, uv).rgb * 2.0 - 1.0;

    vec3 axis_sign = sign(flat_normal);
    vec3 final_normal;

    if (axis == 0) {
        tnorm.z *= axis_sign.x;
        final_normal = vec3(tnorm.z, tnorm.y, -tnorm.x);
    } else if (axis == 1) {
        tnorm.z *= axis_sign.y;
        final_normal = vec3(tnorm.x, tnorm.z, -tnorm.y);
    } else {
        tnorm.z *= axis_sign.z;
        final_normal = vec3(tnorm.x, tnorm.y, tnorm.z);
    }

    final_normal = normalize(final_normal);

    // --- PHONG SHADING ---

    // Ambient
    vec3 ambient = material.ambient * light.ambient_val * albedo;

    // Diffuse
    vec3 light_dir = normalize(light.direct_pos - pos);
    float diff = max(dot(final_normal, light_dir), 0.0);
    vec3 diffuse = material.diffuse * diff * light.direct_val * albedo;

    // Specular
    vec3 view_dir = normalize(camera_pos - pos);
    vec3 reflect_dir = reflect(-light_dir, final_normal);

    // Convert roughness to a Phong shininess exponent
    float current_shininess = mix(1.0, material.shininess, 1.0 - roughness);
    float spec = pow(max(dot(view_dir, reflect_dir), 0.0), current_shininess);
    vec3 specular = material.specular * spec * light.direct_val * (1.0 - roughness);

    vec3 result = ambient + diffuse + specular;

    // Gamma correction
    result = pow(result, vec3(1.0 / 2.2));

    fragment_color = vec4(result, 1.0);
}
