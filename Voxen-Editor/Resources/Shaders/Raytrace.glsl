#version 460 core

layout(local_size_x = 16, local_size_y = 16) in;

layout(rgba32f, binding = 0) uniform image2D u_OutputImage;

layout(location = 0) uniform vec2 u_ScreenSize;   // Screen size (width, height)
layout(location = 1) uniform mat4 u_CameraMatrix; // View-projection matrix

// Ray structure
struct Ray {
    vec3 origin;
    vec3 direction;
};

// Create a ray from the camera through the pixel in NDC space
Ray getRay(vec2 uv) {
    // Construct a ray in normalized device coordinates
    vec4 rayStart = vec4(uv, -1.0, 1.0);  // Near plane
    vec4 rayEnd = vec4(uv, 0.0, 1.0);     // Far plane (optional, use for perspective if needed)

    // Transform rays from NDC space to world space using the inverse camera matrix
    vec3 origin = (inverse(u_CameraMatrix) * rayStart).xyz;
    vec3 target = (inverse(u_CameraMatrix) * rayEnd).xyz;

    vec3 direction = normalize(target - origin);
    return Ray(origin, direction);
}

// Sphere intersection function
bool hit_sphere(vec3 center, float radius, Ray ray) {
    vec3 oc = ray.origin - center;
    float a = dot(ray.direction, ray.direction);
    float b = 2.0 * dot(oc, ray.direction);
    float c = dot(oc, oc) - radius * radius;
    float discriminant = b * b - 4.0 * a * c;
    return (discriminant >= 0.0);
}

void main() {
    // Get pixel coordinates
    ivec2 pixelCoord = ivec2(gl_GlobalInvocationID.xy);

    // Check if within screen bounds
    if (pixelCoord.x >= int(u_ScreenSize.x) || pixelCoord.y >= int(u_ScreenSize.y)) return;

    // Map pixel to normalized device coordinates (NDC) in [-1, 1]
    vec2 uv = (vec2(pixelCoord) / u_ScreenSize) * 2.0 - 1.0;
    uv.y = -uv.y;  // Invert y-axis for screen space

    // Get the ray from the camera through this pixel
    Ray ray = getRay(uv);

    // Hardcoded sphere in the center of the screen at (0, 0, -1) with radius 0.5
    vec3 sphere_center = vec3(0.0, 0.0, -1.0);
    float sphere_radius = 0.5;

    // Default color is black
    vec3 color = vec3(0.0);

    // Check if the ray hits the sphere
    if (hit_sphere(sphere_center, sphere_radius, ray)) {
        color = vec3(1.0);  // White if hit
    }

    // Output the final color to the image
    imageStore(u_OutputImage, pixelCoord, vec4(color, 1.0));
}
