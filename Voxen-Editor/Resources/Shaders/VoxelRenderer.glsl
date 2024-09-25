#version 460 core

#define EPSILON 1e-4
#define EMPTY_VOXEL 255

// Structs
struct Material
{
	uint rgb;  // Packed RGB values (8 bits per channel)
	uint rrme; // Packed reflectivity, roughness, metallic, emissive (8 bits per channel)
};

struct AABB
{
	vec4 min;
	vec4 max;
};

struct VoxelShape
{
	mat4 transform;
	AABB bounds;
	Material materials[256];
	uint materialMapOffset;
	uint materialMapSize;
};

struct HitInfo
{
	bool didHit;
	float dist;
	vec3 hitPoint;
	vec3 normal;
	Material material;
};

struct Ray
{
	vec3 origin;
	vec3 direction;
};

// Input
layout(local_size_x = 16, local_size_y = 16) in;

layout(rgba32f, binding = 0) uniform image2D u_OutputImage;

layout(location = 0) uniform vec2 u_ScreenSize;
layout(location = 1) uniform mat4 u_ViewProjectionMatrix;
layout(location = 2) uniform vec3 u_CameraPosition;

layout(std430, binding = 0) buffer ShapeBuffer
{
	VoxelShape shapes[];
};

layout(std430, binding = 1) buffer VoxelBuffer
{
	uint voxels[];
};

// Utility functions
vec3 UnpackRGB(uint rgb)
{
	float r = float((rgb >> 16) & 0xFF) / 255.0;
	float g = float((rgb >> 8) & 0xFF) / 255.0;
	float b = float((rgb >> 0) & 0xFF) / 255.0;

	return vec3(r, g, b);
}

vec4 UnpackRRME(uint rrme)
{
	float reflectivity = float((rrme >> 24) & 0xFF) / 255.0;
	float roughness    = float((rrme >> 16) & 0xFF) / 255.0;
	float metallic     = float((rrme >>  8) & 0xFF) / 255.0;
	float emissive     = float((rrme >>  0) & 0xFF) / 255.0;

	return vec4(reflectivity, roughness, metallic, emissive);
}

uvec4 UnpackIndices(uint indices)
{
	uint index0 = (indices >> 24) & 0xFF;
	uint index1 = (indices >> 16) & 0xFF;
	uint index2 = (indices >>  8) & 0xFF;
	uint index3 = (indices >>  0) & 0xFF;

	return uvec4(index0, index1, index2, index3);
}

HitInfo InitializeHitInfo()
{
	return HitInfo(false, 0.0, vec3(0.0), vec3(0.0), Material(0, 0));
}

// Perform intersection test between Ray and AABB
bool IntersectAABBWithRay(AABB aabb, Ray ray, out float tNear, out float tFar)
{
	vec3 invDir = 1.0 / ray.direction;
	vec3 tMin = (aabb.min.rgb - ray.origin) * invDir;
	vec3 tMax = (aabb.max.rgb - ray.origin) * invDir;

	vec3 t1 = min(tMin, tMax);
	vec3 t2 = max(tMin, tMax);

	tNear = max(max(t1.x, t1.y), t1.z);
	tFar = min(min(t2.x, t2.y), t2.z);

	return (tNear <= tFar && tFar >= 0.0);
}

// Perform Amantides and Woo ray marching inside voxel grid
HitInfo RayMarch(Ray ray, VoxelShape shape, vec3 entryPoint)
{
	// Setup hit
	HitInfo closestHit = InitializeHitInfo();
	closestHit.dist = 1e8;

	// Find the voxel we entered on and the direction of the next voxel
	ivec3 voxelPos = ivec3(floor(entryPoint));
	ivec3 step = ivec3(sign(ray.direction));
	ray.origin += ray.direction * EPSILON;

	vec3 voxelMin = vec3(voxelPos);
	vec3 invDir = 1.0 / ray.direction;
	vec3 tMax = ((voxelMin + step * 0.5 + 0.5 - ray.origin) * invDir);
	vec3 tDelta = abs(invDir);

	// While we have not exited the shape
	while (voxelPos.x >= -1 && voxelPos.y >= -1 && voxelPos.z >= -1 &&
		   voxelPos.x <= shape.bounds.max.x && voxelPos.y <= shape.bounds.max.y && voxelPos.z <= shape.bounds.max.z)
	{
		// If our voxel is inside the shape
		if (voxelPos.x >= 0 && voxelPos.y >= 0 && voxelPos.z >= 0 &&
			voxelPos.x < shape.bounds.max.x && voxelPos.y < shape.bounds.max.y && voxelPos.z < shape.bounds.max.z)
		{
			// Find the "3D" index into 1D array
			int index = voxelPos.z * int(shape.bounds.max.x) * int(shape.bounds.max.y) + voxelPos.y * int(shape.bounds.max.x) + voxelPos.x;
			int packedIndex = index / 4;
			int subIndex = index % 4;

			uvec4 indices = UnpackIndices(voxels[shape.materialMapOffset + packedIndex]);
			uint materialIndex = indices[subIndex];

			// Check if we hit a real voxel
			if (materialIndex < EMPTY_VOXEL)
			{
				closestHit.didHit = true;
				closestHit.dist = length(ray.origin - (vec3(voxelPos) + 0.5));
				closestHit.hitPoint = ray.origin + ray.direction * closestHit.dist;
				closestHit.material = shape.materials[materialIndex];
				closestHit.normal = vec3(0.0); // TODO: Calculate normal
				break;
			}
		}

		if (tMax.x < tMax.y && tMax.x < tMax.z)
		{
			voxelPos.x += step.x;
			tMax.x += tDelta.x;
		}
		else if (tMax.y < tMax.z)
		{
			voxelPos.y += step.y;
			tMax.y += tDelta.y;
		}
		else
		{
			voxelPos.z += step.z;
			tMax.z += tDelta.z;
		}
	}

	return closestHit;
}

// Main calculation for ray-voxel collisions
HitInfo FindRayCollisions(Ray ray, VoxelShape shape)
{
	// Setup hit & ray
	HitInfo closestHit = InitializeHitInfo();
	ray.direction = normalize(ray.direction);

	// Setup ray in local space
	Ray localRay;
	mat4 invTransform = inverse(shape.transform);
	localRay.origin = (invTransform * vec4(ray.origin, 1.0)).xyz;
	localRay.direction = normalize((invTransform * vec4(ray.direction, 0.0)).xyz);

	// Check if local ray intersects with this shape's AABB
	float tNear, tFar;
	if (!IntersectAABBWithRay(shape.bounds, localRay, tNear, tFar))
	{
		// Local ray did not hit AABB
		return closestHit;
	}

	// Find entry point of the ray into the AABB
	vec3 entryPoint = localRay.origin + localRay.direction * max(tNear, 0.0);
	// Ray march through the shape's voxels
	return RayMarch(localRay, shape, entryPoint);
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

	HitInfo hit = FindRayCollisions(ray, shapes[0]);

	vec3 pixelColor = vec3(0.0);

	if (hit.didHit)
	{
		pixelColor = UnpackRGB(hit.material.rgb);
	}
	else
	{
		pixelColor = vec3(abs(rayDirection));
	}

	ivec2 texelCoord = ivec2(gl_GlobalInvocationID.xy);
	imageStore(u_OutputImage, texelCoord, vec4(pixelColor, 1.0));
}