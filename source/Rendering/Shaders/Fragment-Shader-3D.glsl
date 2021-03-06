//^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
/**
 *	@file		Fragment-Shader-3D.glsl
 *	@author		Camilo Talero
 *
 *
 *	Version: 0.0.3
 *
 *	@brief Basic fragment shader
 */
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#version 450

struct Light
{
  vec4 position;
  vec4 color;
  double intensity;
};

in vec3 normal;//Normal to the vertex
in vec3 vertexPos;//True position of the vertex (i.e it's location in space)
in vec2 texture_coord;

out vec4 outColor;//Final color of the pixel

uniform sampler2D text;
uniform samplerCube depth_map;

uniform vec4 color = vec4(1);//Default color
//TODO: make this an array
uniform vec3 lum = vec3(80,70,10);//A unique light position
uniform vec3 cameraPos = vec3(0);//The position of the camera in the world
uniform vec3 cameraDir = vec3(0);

void main()
{
	vec3 l = vec3(lum-vertexPos);
	if(length(l)>0)
		l = normalize(l);
	vec3 c = vec3(texture(text,abs(texture_coord)));
	vec3 n = normalize(normal);
	vec3 e = cameraPos-vertexPos;
	e = normalize(e);
	vec3 h = normalize(e+l);

	outColor = vec4(c*(vec3(0.5)+0.5*max(0,dot(n,l))) + 
		vec3(0.1)*max(0,pow(dot(h,n), 100)), 1);

	vec3 temp = vertexPos-lum; 

	float test = texture(depth_map, temp).r;
	double d = length(temp);

	if(d>test*256 + 0.5)
		outColor = vec4(test);
}