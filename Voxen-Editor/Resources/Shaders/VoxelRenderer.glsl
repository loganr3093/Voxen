#version 460 core

#define EPSILON 1e-4
#define EMPTY_VOXEL 255

// Structs
struct Material {
    uint rgb;  // Packed RGB values (8 bits per channel)
    uint rrme; // Packed reflectivity, roughness, metallic, emissive (8 bits per channel)
};

struct AABB {
    vec4 min;
    vec4 max;
};

struct VoxelShape {
    mat4 transform;
    AABB bounds;
    Material materials[256];
    uint materialMapOffset;
    uint materialMapSize;
    uint padding[2];
};

struct HitInfo {
    bool didHit;
    float dist;
    vec3 hitPoint;
    vec3 normal;
    Material material;
};

struct Ray {
    vec3 origin;
    vec3 direction;
};

// Input
layout(local_size_x = 16, local_size_y = 16) in;

layout(rgba32f, binding = 0) uniform image2D u_OutputImage;

layout(location = 0) uniform vec2 u_ScreenSize;
layout(location = 1) uniform mat4 u_ViewProjectionMatrix;
layout(location = 2) uniform vec3 u_CameraPosition;

layout(location = 3) uniform int u_NumShapes;

layout(std430, binding = 0) buffer ShapeBuffer {
    VoxelShape shapes[];
};

layout(std430, binding = 1) buffer VoxelBuffer {
    uint voxels[];
};

// Utility functions
vec3 UnpackRGB(uint rgb) {
    return vec3(float((rgb >> 16) & 0xFF), float((rgb >> 8) & 0xFF), float((rgb >> 0) & 0xFF)) / 255.0;
}

uvec4 UnpackIndices(uint indices) {
    return uvec4((indices >> 24) & 0xFF, (indices >> 16) & 0xFF, (indices >> 8) & 0xFF, (indices >> 0) & 0xFF);
}

HitInfo InitializeHitInfo() {
    return HitInfo(false, 1e8, vec3(0.0), vec3(0.0), Material(0, 0));
}

// Perform intersection test between Ray and AABB
bool IntersectAABBWithRay(AABB aabb, Ray ray, out float tNear, out float tFar) {
    vec3 invDir = 1.0 / ray.direction;
    vec3 tMin = (aabb.min.rgb - ray.origin) * invDir;
    vec3 tMax = (aabb.max.rgb - ray.origin) * invDir;

    tNear = max(max(min(tMin, tMax).x, min(tMin, tMax).y), min(tMin, tMax).z);
    tFar = min(min(max(tMin, tMax).x, max(tMin, tMax).y), max(tMin, tMax).z);

    return (tNear <= tFar && tFar >= 0.0);
}

// Perform Amantides and Woo ray marching inside voxel grid
HitInfo RayMarch(Ray ray, VoxelShape shape, vec3 entryPoint) {
    HitInfo closestHit = InitializeHitInfo();

    ivec3 voxelPos = ivec3(floor(entryPoint));
    ivec3 step = ivec3(sign(ray.direction));
    ray.origin += ray.direction * EPSILON;

    vec3 invDir = 1.0 / ray.direction;
    vec3 tMax = ((vec3(voxelPos) + step * 0.5 + 0.5 - ray.origin) * invDir);
    vec3 tDelta = abs(invDir);

    while (voxelPos.x >= -1 && voxelPos.y >= -1 && voxelPos.z >= -1 &&
        voxelPos.x <= shape.bounds.max.x && voxelPos.y <= shape.bounds.max.y && voxelPos.z <= shape.bounds.max.z) {

        if (voxelPos.x >= 0 && voxelPos.y >= 0 && voxelPos.z >= 0 &&
            voxelPos.x < shape.bounds.max.x && voxelPos.y < shape.bounds.max.y && voxelPos.z < shape.bounds.max.z) {

            int index = voxelPos.z * int(shape.bounds.max.x) * int(shape.bounds.max.y) + voxelPos.y * int(shape.bounds.max.x) + voxelPos.x;
            int packedIndex = index >> 2;  // Optimized division by 4 using bitshift
            int subIndex = index & 3;      // Optimized modulus by 4 using bitmask

            uvec4 indices = UnpackIndices(voxels[shape.materialMapOffset + packedIndex]);
            uint materialIndex = indices[subIndex];

            if (materialIndex < EMPTY_VOXEL) {
                closestHit.didHit = true;
                closestHit.dist = length(ray.origin - (vec3(voxelPos) + 0.5));
                closestHit.hitPoint = ray.origin + ray.direction * closestHit.dist;
                closestHit.material = shape.materials[materialIndex];
                closestHit.normal = vec3(0.0); // TODO: Calculate normal
                break;
            }
        }

        if (tMax.x < tMax.y && tMax.x < tMax.z) {
            voxelPos.x += step.x;
            tMax.x += tDelta.x;
        }
        else if (tMax.y < tMax.z) {
            voxelPos.y += step.y;
            tMax.y += tDelta.y;
        }
        else {
            voxelPos.z += step.z;
            tMax.z += tDelta.z;
        }
    }

    return closestHit;
}

// Transform ray to local space, considering voxel shape transform
Ray TransformRayToLocal(Ray ray, mat4 transform) {
    mat4 invTransform = inverse(transform);
    vec4 localOrigin = invTransform * vec4(ray.origin, 1.0);
    vec4 localDir = invTransform * vec4(ray.direction, 0.0);
    return Ray(localOrigin.xyz, normalize(localDir.xyz));
}

// Transform hit data from local to world space
HitInfo TransformHitToWorld(HitInfo localHit, mat4 transform) {
    mat4 invTransT = transpose(inverse(transform));
    return HitInfo(localHit.didHit, localHit.dist, (transform * vec4(localHit.hitPoint, 1.0)).xyz,
        normalize((invTransT * vec4(localHit.normal, 0.0)).xyz), localHit.material);
}

// Check ray collision with all shapes and return the closest hit
HitInfo FindRayCollisions(Ray ray) {
    HitInfo closestHit = InitializeHitInfo();

    for (int i = 0; i < u_NumShapes; i++) {
        VoxelShape shape = shapes[i];
        Ray localRay = TransformRayToLocal(ray, shape.transform);

        float tNear, tFar;
        if (IntersectAABBWithRay(shape.bounds, localRay, tNear, tFar)) {
            vec3 entryPoint = localRay.origin + localRay.direction * max(tNear, 0.0);

            HitInfo localHit = RayMarch(localRay, shape, entryPoint);
            if (localHit.didHit && localHit.dist < closestHit.dist) {
                closestHit = TransformHitToWorld(localHit, shape.transform);
            }
        }
    }

    return closestHit;
}

void main() {
    vec2 textureCoords = vec2(gl_GlobalInvocationID.xy) / u_ScreenSize;
    vec4 clipSpaceRayOrigin = vec4(textureCoords * 2.0 - 1.0, -1.0, 1.0);
    vec4 viewSpaceRayOrigin = inverse(u_ViewProjectionMatrix) * clipSpaceRayOrigin;
    vec3 rayOrigin = viewSpaceRayOrigin.xyz / viewSpaceRayOrigin.w;
    vec3 rayDirection = normalize(rayOrigin - u_CameraPosition);

    Ray ray = Ray(u_CameraPosition, rayDirection);
    HitInfo hit = FindRayCollisions(ray);

    vec3 pixelColor = hit.didHit ? UnpackRGB(hit.material.rgb) : vec3(0.0);
    ivec2 texelCoord = ivec2(gl_GlobalInvocationID.xy);
    imageStore(u_OutputImage, texelCoord, vec4(pixelColor, 1.0));
}
