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
uniform sampler2D texture_1;
uniform sampler2D texture_2;
in vec3 fragment_normal;
in vec3 light_direction;
uniform material_params material;
uniform light_params light0;
uniform int toon;

in vec4 fragment_diffuse;
in vec2 fragment_texcoord;
in vec4 ambient;
in vec3 halfway_vector0;
in float texture_select;

void main (void)
{
    vec3 n;
    float ndotl;
    vec4 color;
    vec4 specular = vec4(0.0,0.0,0.0,0.0); 
    vec4 diffuse = vec4(0.0,0.0,0.0,0.0); 
    n = normalize (fragment_normal);
    vec3 h = normalize(halfway_vector0);
    vec3 ld = normalize(light_direction);

    vec4 tex_col;
    if(texture_select < 1.8){
        tex_col = texture2D(texture_1, fragment_texcoord).bgra;
    }else{
        tex_col = texture2D(texture_2, fragment_texcoord).bgra;
    }

    vec4 ambient_t = ambient * tex_col;
    vec4 diffuse_t = fragment_diffuse * tex_col;
    vec4 specular_t = material.specular * tex_col;

    
    ndotl = max (dot(n, ld), 0.0);

    float ndothv = max(dot(n, h),0.0);
//    if (ndothv > 1.0){
//        ndothv = 1.0;
//    }
    color = ambient_t;
 //if specular is possible, compute
    if (ndotl > 0.0){
        diffuse = diffuse_t * ndotl;
        specular = material.specular * light0.specular * pow(ndothv,
        material.shininess);
    }

    color = color + diffuse + specular;


    fragment_Color = color;
//    fragment_Color = 
//    ambient + 
//   fragment_diffuse * ndotl+
//    material.specular*light0.specular*pow(ndothv, 12.0);
    if (toon >0){
        float intensity = (ndotl + pow(ndothv, material.shininess))/2;

        if (intensity < 0) {
            fragment_Color = vec4(0.0,0.0,0.0,1.0);
        }else if (intensity < 0.25){
            fragment_Color = vec4(0.3,0.3,0.3,1.0)*vec4(tex_col.rgb,1.0);
        }else if (intensity < 0.5){
            fragment_Color = vec4(0.6,0.6,0.6,1.0)*vec4(tex_col.rgb,1.0);
        }else if (intensity < 0.95){
            fragment_Color = vec4(1.0,1.0,1.0,1.0)*vec4(tex_col.rgb,1.0);
        }
    }

}

