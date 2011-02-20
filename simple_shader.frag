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


uniform vec4 base_color;
out vec4 fragment_Color;
uniform sampler2D my_texture;
in vec3 fragment_normal;
in vec3 light_direction;
uniform material_params material;
uniform light_params light0;

in vec4 fragment_diffuse;
in vec2 fragment_texcoord;
in vec4 ambient;
in vec3 halfway_vector0;

void main (void)
{
    vec3 n;
    float ndotl;
    vec4 color;
    
    n = normalize (fragment_normal);
    
    ndotl = max (dot(n, light_direction), 0.0);
    //better for testing normals
//   fragment_Color = fragment_diffuse * ndotl ;

    color = ambient;
 //if specular is possible, compute
    if (ndotl > 0.0){
        color += fragment_diffuse * ndotl;
        float ndothv = max(dot(fragment_normal, halfway_vector0),0.0);
        color += material.specular * light0.specular * pow(ndothv,
        1.0);
    }

    vec3 tex_col = texture2D(my_texture, fragment_texcoord).rgb;

    fragment_Color = vec4(color.rgb * tex_col.rgb, material.diffuse.a); 
}

