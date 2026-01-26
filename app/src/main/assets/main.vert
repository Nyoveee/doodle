#version 300 es
precision mediump float;

// A VBO-less square render.
const vec3 vertexPos[4] = vec3[4](
    vec3(-0.5, -0.5,  0),	// bottom left
    vec3( 0.5, -0.5,  0),	// bottom right
    vec3( 0.5,  0.5,  0),	// top right
    vec3(-0.5,  0.5,  0) 	// top left
);

// THE Y-COMPONENT OF TEXTURE COORDINATES ARE FLIPPED
// because loaded images are flipped.
const vec2 textureCoordinates[4] = vec2[4](
    vec2(0, 1),
    vec2(1, 1),
    vec2(1, 0),
    vec2(0, 0)
);

const int indices[6] = int[6](0, 2, 1, 2, 0, 3);

uniform mat4 model;
uniform mat4 viewProjection;
out vec2 textureCoords;

void main() {
    int index = indices[gl_VertexID];

    gl_Position = viewProjection * model * vec4(vertexPos[index], 1);
    textureCoords = textureCoordinates[index];
}