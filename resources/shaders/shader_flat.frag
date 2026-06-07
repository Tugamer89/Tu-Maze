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

uniform sampler2D diffuseMap;
uniform sampler2D normalMap;
uniform sampler2D roughnessMap;

in vec3 interpolated_pos;

out vec4 fragment_color;

vec4 clamp4(vec4 v) {
    return clamp(v, vec4(0.0), vec4(1.0));
}

void main() {
    vec3 pos = interpolated_pos;

    // Compute flat geometric normal using screen-space derivatives
    vec3 dx = dFdx(pos);
    vec3 dy = dFdy(pos);
    vec3 geom_normal = normalize(cross(dx, dy));

    // --- TRIPLANAR MAPPING ---
    float uv_scale = 0.25;
    vec3 blend_weights = abs(geom_normal);
    blend_weights = pow(blend_weights, vec3(4.0));
    blend_weights /= (blend_weights.x + blend_weights.y + blend_weights.z);

    // Diffuse
    vec3 diffX = texture(diffuseMap, pos.yz * uv_scale).rgb;
    vec3 diffY = texture(diffuseMap, pos.xz * uv_scale).rgb;
    vec3 diffZ = texture(diffuseMap, pos.xy * uv_scale).rgb;
    vec3 albedo = diffX * blend_weights.x + diffY * blend_weights.y + diffZ * blend_weights.z;

    // Roughness (Aggiunto per consistenza con lo specular di Phong)
    float roughX = texture(roughnessMap, pos.yz * uv_scale).r;
    float roughY = texture(roughnessMap, pos.xz * uv_scale).r;
    float roughZ = texture(roughnessMap, pos.xy * uv_scale).r;
    float roughness = roughX * blend_weights.x + roughY * blend_weights.y + roughZ * blend_weights.z;

    // Nota: Ignoriamo la normalMap perché stiamo calcolando una geometria flat rigorosa.

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
