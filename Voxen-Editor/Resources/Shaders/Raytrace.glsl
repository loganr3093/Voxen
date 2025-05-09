#version 460 core

layout(local_size_x = 16, local_size_y = 16) in;

layout(rgba32f, binding = 0) uniform image2D u_OutputImage;

layout(location = 0) uniform vec2 u_ScreenSize;
layout(location = 1) uniform mat4 u_ViewProjectionMatrix;
layout(location = 2) uniform vec3 u_CameraPosition;

layout(std430, binding = 3) buffer VoxelShapeBuffer
{
    uint voxelShapeData[];
};

layout(std430, binding = 4) buffer MaterialBuffer
{
    uint materialData[];
};

struct Material
{
    vec4 color;
    float reflectivity;
    float roughness;
    float metallic;
    float emissive;
};

struct Ray
{
    vec3 origin;
    vec3 direction;
};

struct HitInfo
{
    bool didHit;
    float dist;
    vec3 hitPoint;
    vec3 normal;
    Material material;
};

HitInfo hitInfoInit()
{
    HitInfo hitInfo;
    hitInfo.didHit = false;
    hitInfo.dist = 0;
    hitInfo.hitPoint = vec3(0.0f);
    hitInfo.normal = vec3(0.0f);
    hitInfo.material = Material(vec4(0.0), 0.0, 0.0, 0.0, 0.0);
    return hitInfo;
}

vec3 CalculateNormal(vec3 hitPointLocal, vec3 boxMin, vec3 boxMax)
{
    vec3 normal = vec3(0.0);
    float epsilon = 0.001;

    if (abs(hitPointLocal.x - boxMin.x) < epsilon)
    {
        normal = vec3(-1.0, 0.0, 0.0);
    }
    else if (abs(hitPointLocal.x - boxMax.x) < epsilon)
    {
        normal = vec3(1.0, 0.0, 0.0);
    }
    else if (abs(hitPointLocal.y - boxMin.y) < epsilon)
    {
        normal = vec3(0.0, -1.0, 0.0);
    }
    else if (abs(hitPointLocal.y - boxMax.y) < epsilon)
    {
        normal = vec3(0.0, 1.0, 0.0);
    }
    else if (abs(hitPointLocal.z - boxMin.z) < epsilon)
    {
        normal = vec3(0.0, 0.0, -1.0);
    }
    else if (abs(hitPointLocal.z - boxMax.z) < epsilon)
    {
        normal = vec3(0.0, 0.0, 1.0);
    }

    return normal;
}

HitInfo RayVoxelShape(Ray ray, mat4 shapeTransform)
{
    HitInfo hitInfo = hitInfoInit();

    vec3 rayOriginLocal = (inverse(shapeTransform) * vec4(ray.origin, 1)).xyz;
    vec3 rayDirectionLocal = normalize(inverse(mat3(shapeTransform)) * ray.direction);

    vec3 boxMin = vec3(-0.5);
    vec3 boxMax = vec3(0.5);

    vec3 tMin = (boxMin - rayOriginLocal) / rayDirectionLocal;
    vec3 tMax = (boxMax - rayOriginLocal) / rayDirectionLocal;

    vec3 t1 = min(tMin, tMax);
    vec3 t2 = max(tMin, tMax);

    float tNear = max(max(t1.x, t1.y), t1.z);
    float tFar = min(min(t2.x, t2.y), t2.z);

    if (tNear <= tFar && tFar > 0)
    {
        vec3 hitPointLocal = rayOriginLocal + rayDirectionLocal * tNear;
        vec3 normalLocal = CalculateNormal(hitPointLocal, boxMin, boxMax);

        hitInfo.hitPoint = (shapeTransform * vec4(hitPointLocal, 1)).xyz;
        hitInfo.normal = normalize((transpose(inverse(mat3(shapeTransform))) * normalLocal));
        hitInfo.didHit = true;
        hitInfo.dist = tNear;
    }

    return hitInfo;
}

HitInfo CalculateRayCollisions(Ray ray)
{
    HitInfo closestHit = hitInfoInit();
    closestHit.dist = 100000000.0;

    for (int i = 0; i < voxelShapeData.length() / 2; ++i)
    {
        uint baseIndex = i * 2;

        uint x = (voxelShapeData[baseIndex] & 0xFFFF);
        uint y = ((voxelShapeData[baseIndex] >> 16) & 0xFFFF);
        uint z = (voxelShapeData[baseIndex + 1] & 0xFFFF);
        uint k = ((voxelShapeData[baseIndex + 1] >> 16) & 0xFFFF);

        mat4 shapeTransform = mat4(1.0);
        shapeTransform[3] = vec4(float(x), float(y), float(z), 1.0);

        HitInfo hitInfo = RayVoxelShape(ray, shapeTransform);

        if (hitInfo.didHit && hitInfo.dist < closestHit.dist)
        {
            closestHit = hitInfo;
            closestHit.material.color = vec4(
                ((materialData[k * 2] >> 24) & 0xFF) / 255.0,
                ((materialData[k * 2] >> 16) & 0xFF) / 255.0,
                ((materialData[k * 2] >> 8) & 0xFF) / 255.0,
                1.0);
            closestHit.material.reflectivity = ((materialData[k * 2 + 1] >> 24) & 0xFF) / 255.0;
            closestHit.material.roughness = ((materialData[k * 2 + 1] >> 16) & 0xFF) / 255.0;
            closestHit.material.metallic = ((materialData[k * 2 + 1] >> 8) & 0xFF) / 255.0;
            closestHit.material.emissive = (materialData[k * 2 + 1] & 0xFF) / 255.0;
        }
    }

    return closestHit;
}

void main()
{
    vec2 textureCoords = vec2(gl_GlobalInvocationID.xy) / u_ScreenSize;
    vec4 clipSpaceRayOrigin = vec4(textureCoords * 2.0 - 1.0, -1.0, 1.0);

    vec4 viewSpaceRayOrigin = inverse(u_ViewProjectionMatrix) * clipSpaceRayOrigin;
    vec3 rayOrigin = viewSpaceRayOrigin.xyz / viewSpaceRayOrigin.w;
    vec3 rayDirection = normalize(rayOrigin - u_CameraPosition);

    Ray ray;
    ray.origin = u_CameraPosition;
    ray.direction = rayDirection;

    vec3 pixelColor = vec3(0.0);
    HitInfo hit = CalculateRayCollisions(ray);
    if (hit.didHit)
    {
        pixelColor = hit.material.color.rgb;
    }

    ivec2 pixelCoord = ivec2(gl_GlobalInvocationID.xy);
    imageStore(u_OutputImage, pixelCoord, vec4(pixelColor, 1.0));
}
