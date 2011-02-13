#version 150

uniform mat4 projection_matrix;
uniform mat4 modelview_matrix;
in vec3 vertex_Position;
//in vec3 gl_vertex;
out vec4 my_Color;
void main (void)
{
gl_Position = projection_matrix * modelview_matrix * vec4 (vertex_Position, 1.0);
my_Color = vec4(1.0,0.0,0.0,0.0);
//gl_Position = ftransform();
}


