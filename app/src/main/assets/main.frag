#version 300 es
precision mediump float;

in vec2 textureCoords;
uniform sampler2D uTexture;

out vec4 outColor;

uniform vec4 colorMultiplier;

uniform sampler2D image;

void main() {
    outColor = texture(uTexture, textureCoords) * colorMultiplier;
}