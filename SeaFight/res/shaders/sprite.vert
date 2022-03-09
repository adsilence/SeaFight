#version 450

layout(location = 0) in vec2 pos;
layout (location = 1) in vec3 color;

layout(set = 0, binding = 0) uniform UBO {
	mat4 proj;
	mat4 view;
} ubo;

layout(push_constant) uniform Push {
	mat4 transform;
	vec3 color;
} push;

void main() {
	gl_Position = ubo.proj * ubo.view * push.transform * vec4(pos, 0.0, 1.0);
}