#version 150

struct light_params {
    vec4 position;
    vec4 ambient;
    vec4 diffuse;
    vec4 specular;
    vec3 spot_dir;
    float spot_exponent;
    float constant_att;
    float linear_att;
    float quadratic_att;
    int type;
    };

struct material_params{
    vec4 emission;
    vec4 ambient;
    vec4 diffuse;
    vec4 specular;
    float shininess; 
    };


uniform light_params light0;
uniform int num_lights;

uniform material_params material;

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
//out vec3 halfway_vector;
out vec3 fragment_normal;
out vec2 fragment_texcoord;
out vec3 light_direction;
out vec4 fragment_diffuse;
out vec4 fragment_color;
out vec4 ambient;
out vec3 halfway_vector0;

void main (void)
{
    light_direction = normalize((modelview_matrix * vec4 (light_location, 0.0)).xyz);

//    float foo = 0.333;
//    fragment_texcoord = foo*vertex_texcoord; //goes to interpolation
    fragment_texcoord = vertex_texcoord; //goes to interpolation
    fragment_normal = (modelview_matrix * vec4(vertex_normal, 0.0)).xyz;

    //light 0 is directional

    fragment_diffuse = material_diffuse * light0.diffuse;
    //for directional light location = direction more or less
    light_direction = normalize(vec3(light0.position));
    vec4 ambient_0 = material.ambient + light0.ambient;
    vec4 g_ambient = base_color * material.ambient;

    //light 1 not implemented yet

    ambient =  g_ambient + ambient_0;

    gl_Position = projection_matrix * modelview_matrix * vec4 (vertex_position, 1.0);
    //halfway vector is the "average" of position vector and light vector in
    //eye space. position *should* be the inverse of eye vector after
    //transform
    halfway_vector0 = normalize((light0.position-gl_Position)/2).xyz;

}


