#version 150

uniform mat4 projection_matrix;
uniform mat4 modelview_matrix;
in vec3 vertex_position;
in vec3 vertex_normal;
uniform vec3 light_location;
uniform vec4 light_color;
uniform vec4 material_diffuse;
uniform vec4 base_color;
//in vec3 gl_vertex;
out vec4 my_Color;
out vec3 fragment_normal;
out vec3 light_direction;
out vec4 fragment_diffuse;


void main (void)
{
//    vec3 light_loc = vec3(1.0,3.0,2.0);
//    my_Color = vec4(0.0,0.0,0.5,0.0); //a little blue as basis

//    vec4 light_col = vec4(1.0,1.0,1.0, 0);
 //   vec4 mat_dif = vec4(1.5,1.5,1.5,1.0);
    vec3 light;
//    light_location.x = light_location.x + +.01;        
//    light_color.x = light_color.x + +.01;        
//    material_diffuse.x = material_diffuse.x + +.01;        
    light_direction = normalize((modelview_matrix * vec4 (light_location, 0.0)).xyz);
    

    fragment_normal = vertex_normal; //(modelview_matrix * vec4(vertex_normal, 0.0)).xyz;
    fragment_diffuse = material_diffuse * light_color;
    gl_Position = projection_matrix * modelview_matrix * vec4 (vertex_position, 1.0);
}


