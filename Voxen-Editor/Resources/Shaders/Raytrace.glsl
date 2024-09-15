// Raytracer
#version 430 core

layout(local_size_x = 32, local_size_y = 18, local_size_z = 1) in;

layout(rgba32f, binding = 0) uniform image2D u_OutputImage;

layout(location = 0) uniform vec2 u_ScreenSize;
layout(location = 1) uniform vec3 u_ViewParameters;
layout(location = 2) uniform mat4 u_CameraMatrix;

struct Ray
{
    vec3 origin;
    vec3 direction;
};

struct HitInfo
{
    bool hit;
    float distance;
    vec3 hitPoint;
    vec3 normal;
};

HitInfo InitHitInfo()
{
    HitInfo info;
    info.hit = false;
    info.distance = 0.0;
    info.hitPoint = vec3(0.0);
    info.normal = vec3(0.0);
    return info;
}

// Simple ray-sphere intersection function
HitInfo RayIntersectSphere(Ray ray, vec3 sphereCenter, float sphereRadius)
{
    HitInfo info = InitHitInfo();

    vec3 oc = ray.origin - sphereCenter;
    float a = dot(ray.direction, ray.direction);
    float b = 2.0 * dot(oc, ray.direction);
    float c = dot(oc, oc) - sphereRadius * sphereRadius;
    float discriminant = b * b - 4.0 * a * c;

    if (discriminant > 0.0)
    {
        float t = (-b - sqrt(discriminant)) / (2.0 * a);
        if (t > 0.0)
        {
            info.hit = true;
            info.distance = t;
            info.hitPoint = ray.origin + t * ray.direction;
            info.normal = normalize(info.hitPoint - sphereCenter);
        }
    }

    return info;
}

void main()
{
    vec3 pixelColor = vec3(0.0);

    ivec2 texelCoord = ivec2(gl_GlobalInvocationID.xy);
    imageStore(u_OutputImage, texelCoord, vec4(pixelColor, 1.0));
}
