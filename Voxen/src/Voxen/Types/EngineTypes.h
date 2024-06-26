#pragma once

#include "Voxen/Types/MathTypes.h"
#include "Voxen/Types/FixedWidthTypes.h"

struct Color
{
	uint8 r;
	uint8 g;
	uint8 b;
};

struct Color4
{
	uint8 r;
	uint8 g;
	uint8 b;
	uint8 a;
};

struct UV
{
    union
    {
        Vector2 vec;
        struct
        {
            float u, v;
        };
        struct
        {
            float x, y;
        };
    };

    UV() : vec(0.0f, 0.0f) {}  // Default constructor initializing to zero
    UV(float _u, float _v) : vec(_u, _v) {}  // Constructor with parameters
};

typedef UV TexCoord;

typedef uint32 ID;