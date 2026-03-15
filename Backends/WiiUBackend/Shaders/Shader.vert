#version 450

layout(location = 0) in vec4 aPosition;

layout(binding = 1, std140) uniform CameraUBO
{
	mat4 view;
	mat4 projection;
	vec4 position;
};

layout(binding = 2, std140) uniform ModelUBO
{
	mat4 model;
};

void main()
{
	gl_Position = projection * view * model * vec4(aPosition.xyz, 1.0);
}
