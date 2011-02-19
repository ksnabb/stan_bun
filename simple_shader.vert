#version 150

uniform mat4 projection_matrix;
uniform mat4 modelview_matrix;
uniform mat4 normal_matrix;
in vec3 vertex_position;
in vec3 vertex_normal;
in vec2 vertex_texcoord;
uniform vec3 light_location;
uniform vec4 light_color;
uniform vec4 material_diffuse;
uniform vec4 base_color;
//in vec3 gl_vertex;
out vec3 fragment_normal;
out vec2 fragment_texcoord;
out vec3 light_direction;
out vec4 fragment_diffuse;


void main (void)
{

    vec3 light;
    light_direction = normalize((modelview_matrix * vec4 (light_location, 0.0)).xyz);
        
    fragment_texcoord = vertex_texcoord;
    fragment_normal = (modelview_matrix * vec4(vertex_normal, 0.0)).xyz;
    fragment_diffuse = material_diffuse * light_color;
    gl_Position = projection_matrix * modelview_matrix * vec4 (vertex_position, 1.0);
}


