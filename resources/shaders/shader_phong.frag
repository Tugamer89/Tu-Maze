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

    // 1. Calculate blending weights based on the FLAT geometric normal
    vec3 blend_weights = abs(flat_normal);
    blend_weights = pow(blend_weights, vec3(4.0));  // Sharpen transitions
    blend_weights /= (blend_weights.x + blend_weights.y + blend_weights.z);

    // (U, V) mapping based on planes
    vec2 uvX = pos.zy * uv_scale;
    vec2 uvY = pos.xz * uv_scale;
    vec2 uvZ = pos.xy * uv_scale;

    // 2. Triplanar Diffuse (Albedo)
    vec3 diffX = texture(diffuseMap, uvX).rgb;
    vec3 diffY = texture(diffuseMap, uvY).rgb;
    vec3 diffZ = texture(diffuseMap, uvZ).rgb;
    vec3 albedo = diffX * blend_weights.x + diffY * blend_weights.y + diffZ * blend_weights.z;

    // 3. Triplanar Roughness
    float roughX = texture(roughnessMap, uvX).r;
    float roughY = texture(roughnessMap, uvY).r;
    float roughZ = texture(roughnessMap, uvZ).r;
    float roughness =
        roughX * blend_weights.x + roughY * blend_weights.y + roughZ * blend_weights.z;

    // 4. Triplanar Normal
    vec3 tnormX = texture(normalMap, uvX).rgb * 2.0 - 1.0;
    vec3 tnormY = texture(normalMap, uvY).rgb * 2.0 - 1.0;
    vec3 tnormZ = texture(normalMap, uvZ).rgb * 2.0 - 1.0;

    // To respect the direction of the face by ALWAYS using the flat normal,
    // to avoid flipping the normal halfway through a corner
    vec3 axis_sign = sign(flat_normal);
    tnormX.z *= axis_sign.x;
    tnormY.z *= axis_sign.y;
    tnormZ.z *= axis_sign.z;

    // Swizzle to align sampled normals to real-world space
    vec3 nX = vec3(tnormX.z, tnormX.y, -tnormX.x);
    vec3 nY = vec3(tnormY.x, tnormY.z, -tnormY.y);
    vec3 nZ = vec3(tnormZ.x, tnormZ.y, tnormZ.z);

    // Final blending
    vec3 final_normal =
        normalize(nX * blend_weights.x + nY * blend_weights.y + nZ * blend_weights.z);

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
