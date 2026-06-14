#version 410 core

uniform vec3 camera_pos;
uniform int useTextures;

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

uniform sampler2D diffuseMap;
uniform sampler2D normalMap;
uniform sampler2D roughnessMap;

in vec3 interpolated_pos;

out vec4 fragment_color;

const float uv_scale = 0.7;

vec4 clamp4(vec4 v) {
    return clamp(v, vec4(0.0), vec4(1.0));
}

void main() {
    vec3 pos = interpolated_pos;

    // Compute flat geometric normal using screen-space derivatives
    vec3 dx = dFdx(pos);
    vec3 dy = dFdy(pos);
    vec3 geom_normal = normalize(cross(dx, dy));

    vec3 albedo;
    float roughness;

    if (useTextures == 1) {
        // --- BOX MAPPING ---
        vec3 abs_normal = abs(geom_normal);
        vec2 uv;

        if (abs_normal.x >= abs_normal.y && abs_normal.x >= abs_normal.z) {
            uv = pos.zy;
        } else if (abs_normal.y >= abs_normal.x && abs_normal.y >= abs_normal.z) {
            uv = pos.xz;
        } else {
            uv = pos.xy;
        }

        uv *= uv_scale;

        albedo = texture(diffuseMap, uv).rgb;
        roughness = texture(roughnessMap, uv).r;
    } else {
        // --- SOLID COLOR MODE ---
        albedo = vec3(1.0);
        roughness = 0.1;
    }

    // --- PHONG SHADING ---

    // Ambient
    vec3 ambient = material.ambient * light.ambient_val * albedo;

    // Diffuse
    vec3 light_dir = normalize(light.direct_pos - pos);
    float diff = max(dot(geom_normal, light_dir), 0.0);
    vec3 diffuse = material.diffuse * diff * light.direct_val * albedo;

    // Specular
    vec3 view_dir = normalize(camera_pos - pos);
    vec3 reflect_dir = reflect(-light_dir, geom_normal);
    float current_shininess = mix(1.0, material.shininess, 1.0 - roughness);
    float spec = pow(max(dot(view_dir, reflect_dir), 0.0), current_shininess);
    vec3 specular = material.specular * spec * light.direct_val * (1.0 - roughness);

    vec3 result = ambient + diffuse + specular;

    // Gamma correction
    result = pow(result, vec3(1.0 / 2.2));

    fragment_color = clamp4(vec4(result, 1.0));
}
