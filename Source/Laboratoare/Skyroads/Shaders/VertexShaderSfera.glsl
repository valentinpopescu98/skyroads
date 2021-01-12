#version 330

layout(location = 0) in vec3 v_position;
layout(location = 1) in vec3 v_normal;
layout(location = 2) in vec2 v_texture_coord;
layout(location = 3) in vec3 v_color;

// Uniform properties
uniform mat4 Model;
uniform mat4 View;
uniform mat4 Projection;

uniform int deformare;

// Output value to fragment shader
out vec3 frag_normal;
out vec3 frag_color;
out vec2 tex_coord;

float random (vec2 st)
{
	return fract(sin(dot(st.xy, vec2(12.9898, 78.233))) * 43758.5453123);
}

void main()
{
	frag_normal = v_normal;
	frag_color = v_color;
	tex_coord = v_texture_coord;

	if(deformare == 0)
		gl_Position = Projection * View * Model * vec4(v_position, 1.0);
	else
		gl_Position = Projection * View * Model * vec4(v_position - v_normal * random(tex_coord), 1.0);
}
