#version 150
in vec4 my_Color;
varying out vec4 fragment_Color;
void main (void)
{
fragment_Color = my_Color;
}

