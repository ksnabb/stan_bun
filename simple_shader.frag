#version 150
in vec4 my_Color;
out vec4 fragment_Color;
in vec3 fragment_normal;
in vec3 light_direction;
in vec4 fragment_diffuse;

void main (void)
{
    vec3 n;
    float ndotl;
    
    n = normalize (fragment_normal);
    
    ndotl = max (dot(n, light_direction), 0.0);

    fragment_Color = fragment_diffuse * ndotl + my_Color;
}

