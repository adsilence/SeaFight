#version 450

layout(location = 0) in vec2 fragTexCoord;
layout(binding = 1) uniform sampler2D texSampler;

layout (location = 0) out vec4 outColor;

layout(push_constant) uniform Push {
	mat4 transform;
	vec3 color;
} push;

void main() {
  //outColor = vec4(push.color, 1.0);
  outColor = texture(texSampler, fragTexCoord);
}