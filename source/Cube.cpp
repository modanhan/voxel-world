//^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
/*
*	Author:	Camilo Talero
*
*
*	Version: 0.0.1
*
*	Header for the definition of a generic cube object
*/
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/*
*	Includes and macros
*/
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
#include <glm/gtc/matrix_transform.hpp>

#include "Cube.hpp"
#include "Rendering.hpp"
#include "cout-definitions.hpp"

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

//========================================================================================
/*
*	Class implementation:
*/
//========================================================================================
//TODO: comment this section
Cube::Cube(string wavefront_file, string t, vec3 p)
{
    texture = new Texture();
    mesh = new Geometry();
    position = p;

   createTexture(*texture, (t.c_str()), GL_TEXTURE_2D);
   createGeometry(*mesh);

   load_obj(wavefront_file, (vector<float>*) &mesh->vertices, 
     (vector<float>*) &mesh->normals, (vector<float>*) &mesh->uvs);
}

Cube::Cube() : Cube("Assets/Objs/cube.obj", "Assets/Textures/Cube-map.png", vec3(0))
{
    
}

Cube::~Cube()
{
    delete(texture);
    delete(mesh);
}

//TODO: correct this
//Warning this is innefficient, all cubes should be rendered in 1 call
void Cube::render_cube()
{
    loadModelMatrix(programs[0], glm::translate(mat4(1), position));
    loadGeometryArrays(programs[0], *mesh);
    loadTexture(programs[0], *texture);

    render(programs[0], *mesh, GL_TRIANGLES);
}
//########################################################################################