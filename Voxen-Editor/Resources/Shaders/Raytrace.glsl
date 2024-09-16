#version 460 core

layout(local_size_x = 16, local_size_y = 16) in;

layout(rgba32f, binding = 0) uniform image2D u_OutputImage;

layout(location = 0) uniform vec2 u_ScreenSize;  // Screen size for mapping UV coordinates

// Function to compute the Mandelbrot value at a point
float Mandelbrot(vec2 c)
{
    vec2 z = vec2(0.0);
    float iter = 0.0;
    const int maxIter = 100;

    // Iterate to check how fast the point escapes
    for (int i = 0; i < maxIter; i++)
    {
        if (length(z) > 2.0) break;  // If the length exceeds 2, it escapes
        z = vec2(z.x * z.x - z.y * z.y, 2.0 * z.x * z.y) + c;
        iter++;
    }

    return iter / float(maxIter);  // Return the normalized iteration count
}

void main()
{
    // Calculate pixel coordinates and UV mapping
    ivec2 pixelCoord = ivec2(gl_GlobalInvocationID.xy);
    vec2 uv = vec2(pixelCoord) / u_ScreenSize * 2.0 - 1.0; // Map to [-1, 1] coordinates

    // Default center and zoom values for the Mandelbrot set
    vec2 center = vec2(-0.5, 0.0);  // Center on the typical Mandelbrot set view
    float zoom = 1;               // Default zoom level

    // Scale and translate UV based on default zoom and center
    vec2 c = center + uv / zoom;

    // Compute the Mandelbrot set for the current pixel
    float mandelbrotValue = Mandelbrot(c);

    // Use the result to determine color (grayscale)
    vec3 finalColor = vec3(mandelbrotValue);

    // Output the final pixel color
    imageStore(u_OutputImage, pixelCoord, vec4(finalColor, 1.0));
}
