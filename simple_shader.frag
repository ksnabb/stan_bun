#version 150
uniform vec4 base_color;
out vec4 fragment_Color;
uniform sampler2D texture0;
in vec3 fragment_normal;
in vec3 light_direction;

in vec4 fragment_diffuse;
in vec2 fragment_texcoord;

void main (void)
{
    vec3 n;
    float ndotl;
    
    n = normalize (fragment_normal);
    
    ndotl = max (dot(n, light_direction), 0.0);
   fragment_Color = fragment_diffuse * ndotl + base_color;

//    fragment_Color = fragment_diffuse * ndotl + texture2D(texture0,
//    fragment_texcoord) + base_color;
}

