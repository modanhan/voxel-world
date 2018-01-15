//^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
/*
*	Author:	Camilo Talero
*
*
*	Version: 0.0.2
*
*	Basic vertex shader
*/
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#version 450

#define PI 3.1415926535897932384626433832795

struct light
{
  vec3 position;
  vec4 color;
  double intensity;
};

layout(location = 0) in vec3 position; //(x,y,z) coordinates of a vertex
layout(location = 1) in vec3 norm; //a 3D vertex representing the normal to teh vertex 
layout(location = 2) in vec2 texture_coordinate; // texture coordinates

layout(std430, binding = 3) buffer face_buffer
{
    vec4 face_info[];//first 3 values are position of face, final value is face type 
};

layout(std430, binding = 4) buffer light_buffer
{
    light w_lights[];
};

layout(std430, binding = 5) buffer index_buffer
{
   ivec2 solids_meta_data[];
};

layout(std430, binding = 6) buffer object_buffer
{
   vec3 w_solids[];
};

out float visible;
out vec3 normal; 
out vec3 vertexPos; //projected vertex
out vec2 texture_coord;

out light lights[];
out vec3 solids[];

uniform mat4 view = mat4(1); //Camera orientation and position
uniform mat4 proj = mat4(1); //the projection parameters (FOV, viewport dimensions)
//uniform int face_types[2048];

//Taken from: https://gist.github.com/neilmendoza/4512992
/*
* Calculate an arbitrary rotation in 3D
*/
mat4 rotationMatrix(vec3 axis, float angle)
{
    axis = normalize(axis);
    float s = sin(angle);
    float c = cos(angle);
    float oc = 1.0 - c;
    
    return mat4(oc * axis.x * axis.x + c,           oc * axis.x * axis.y - axis.z * s,  oc * axis.z * axis.x + axis.y * s,  0.0,
                oc * axis.x * axis.y + axis.z * s,  oc * axis.y * axis.y + c,           oc * axis.y * axis.z - axis.x * s,  0.0,
                oc * axis.z * axis.x - axis.y * s,  oc * axis.y * axis.z + axis.x * s,  oc * axis.z * axis.z + c,           0.0,
                0.0,                                0.0,                                0.0,                                1.0);
}

void main()
{
    mat4 rotation = mat4(1);
    //Select the appropriate rotation to render a front, bottom, top... face
    switch(int(face_info[gl_InstanceID][3]))
    {
        case 0:
            rotation = mat4(1);
            break;
        case 1:
            rotation = rotationMatrix(vec3(0,0,1), -PI/2.f);
            break;
        case 2:
            rotation = rotationMatrix(vec3(0,0,1), PI/2.f);
            break;
        case 3:
            rotation = rotationMatrix(vec3(0,0,1), PI);
            break;
        case 4:
            rotation = rotationMatrix(vec3(1,0,0), -PI/2.f);
            break;
        case 5:
            rotation = rotationMatrix(vec3(1,0,0), PI/2.f);
            break;

        case 6:
            rotation = mat4(1);
            break;
        case 7:
            rotation = rotationMatrix(vec3(0,0,1), -PI/2.f);
            break;
        case 8:
            rotation = rotationMatrix(vec3(0,0,1), PI/2.f);
            break;
        case 9:
            rotation = rotationMatrix(vec3(0,0,1), PI);
            break;
        case 10:
            rotation = rotationMatrix(vec3(1,0,0), -PI/2.f);
            break;
        case 11:
            rotation = rotationMatrix(vec3(1,0,0), PI/2.f);
            break;
        
        default:
            rotation = mat4(1);
            break;
    }
    visible = 1.f;
    if(int(face_info[gl_InstanceID][3])>=6)
        visible = 0.f;
       
    texture_coord = texture_coordinate + vec2((face_info[gl_InstanceID][3])*(1/6.f),0);
    gl_Position = proj*view*(rotation*vec4(position, 1.0) + vec4(vec3(face_info[gl_InstanceID]),0));
    normal = vec3(rotation*vec4(norm,1.0));
    vertexPos = vec3(vec4(position+vec3(face_info[gl_InstanceID]), 1.0)); //calculate the transformed pos

    lights = w_lights;   
    solids = w_solids;
}