#version 330

in vec3 frag_pos;
in vec3 frag_color;

uniform float roundnessX;
uniform float roundnessY;

layout(location = 0) out vec4 out_color;

float lengthOnElipse(float radiusX, float radiusY, vec2 direction)
{
    vec2 pointOnElipse = (radiusX * radiusY) * 
        direction / length(vec2(radiusY, radiusX) * direction);
    return length(pointOnElipse);
}

void main()
{
    float radiusX = 0.5f * roundnessX;
    float radiusY = 0.5f * roundnessY;
    vec2 distFromElipseCenter = abs(frag_pos.xy) - vec2(0.5f) + vec2(radiusX, radiusY);
    vec2 distInUpperRightQuadrant = max(distFromElipseCenter, vec2(0.0f));
    float clampedDistToRoundEdge = length(distInUpperRightQuadrant) - 
        lengthOnElipse(radiusX, radiusY, distInUpperRightQuadrant);
    float smoothedDist = smoothstep(0.0f, 0.001f, clampedDistToRoundEdge);
    float colorWeight = 1.0f - smoothedDist;
    out_color = vec4(0.0f) * smoothedDist + vec4(frag_color, 1.0f) * colorWeight;
}