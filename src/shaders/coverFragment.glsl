#version 420 core

out vec4 color;

in vec3 pos;
in vec2 texCoord;

layout(binding=0) uniform sampler2D colorTexture;

void main() {
    color = vec4(
      texture(colorTexture, texCoord).xyz * 0.2,
      0.2);
}
