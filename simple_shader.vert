#version 150

uniform mat4 projection_matrix;
uniform mat4 modelview_matrix;
in vec3 vertex_position;
in vec3 vertex_normal;
//in vec3 gl_vertex;
out vec4 my_Color;
out vec3 fragment_normal;
void main (void)
{
gl_Position = projection_matrix * modelview_matrix * vec4 (vertex_position, 1.0);
//fragment_normal = (modelview_matrix *vec4(vertex_normal, 0.0)).xyz;
my_Color = vec4(1.0,0.0,0.0,0.0);
}


