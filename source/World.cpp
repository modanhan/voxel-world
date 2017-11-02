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

#include "World.hpp"
#include "cout-definitions.hpp"

#define MESH Cube::meshes[0]
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/*
*	Global Values
*/
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
vector<GLuint> cube_VBO_types = {GL_ARRAY_BUFFER, GL_ARRAY_BUFFER, GL_ARRAY_BUFFER, 
    GL_SHADER_STORAGE_BUFFER, GL_ELEMENT_ARRAY_BUFFER};

	
Texture *texture = NULL;
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

//========================================================================================
/*
*	Chunk Class implementation:
*/
//========================================================================================
//TODO: comment this section

Chunk::Chunk() : Chunk(vec3(0)){}

Chunk::Chunk(vec3 offset)
{    
    for(int i=0; i<CHUNK_DIMENSIONS*CHUNK_DIMENSIONS*CHUNK_DIMENSIONS; i++)
    {
        chunk_cubes[i] = 
            new Cube(vec3(i/(CHUNK_DIMENSIONS*CHUNK_DIMENSIONS) + offset[0], 
            (i/CHUNK_DIMENSIONS) % CHUNK_DIMENSIONS + offset[1], 
            -i%CHUNK_DIMENSIONS - offset[2]));
    }

    cube_rendering_VBOs = vector<GLuint>(5);
    glGenVertexArrays(1, &world_cubes_VAO);
    glGenBuffers(5,cube_rendering_VBOs.data());

    update_visible_faces();
    update_render_info();

    if(texture == NULL)
    {
        texture = new Texture();
        createTexture(*texture, "Assets/Textures/Face_Orientation.png", GL_TEXTURE_2D);
            
        loadTexture(Rendering_Handler->current_program, *(texture));
    }
    
}

Chunk::~Chunk()
{
    for(int i=0; i<CHUNK_DIMENSIONS*CHUNK_DIMENSIONS*CHUNK_DIMENSIONS; i++)
    {
       delete(chunk_cubes[i]);
    }
}

Cube* Chunk::operator()(uint x, uint y, uint z)
{
    if(x<0 || x>15)
        return NULL;
    if(y<0 || y>15)
        return NULL;
    if(z<0 || z>15)
        return NULL;
    
    return chunk_cubes[x*CHUNK_DIMENSIONS*CHUNK_DIMENSIONS+y*CHUNK_DIMENSIONS+z];
}

void Chunk::render_chunk()
{
    Rendering_Handler->multi_render(world_cubes_VAO, &cube_rendering_VBOs, 
        &cube_VBO_types, 4, MESH->indices->size(),faces_info.size());
}
#define CHECK_NEIGHBOUR(c,n,f) if(n==NULL || n->transparent)\
faces_info.push_back(vec4(c->position,f));

void Chunk::update_visible_faces()
{
    for(uint i=0; i<CHUNK_DIMENSIONS; i++)
    {
        for(uint j=0; j<CHUNK_DIMENSIONS; j++)
        {
            for(uint k=0; k<CHUNK_DIMENSIONS; k++)
            {
                Cube* current = (*this)(i,j,k);
                Cube* neighbour = (*this)(i-1,j,k);
                CHECK_NEIGHBOUR(current, neighbour, Left)
                neighbour = (*this)(i+1,j,k);
                CHECK_NEIGHBOUR(current, neighbour, Right)
                neighbour = (*this)(i,j+1,k);
                CHECK_NEIGHBOUR(current, neighbour, Front)
                neighbour = (*this)(i,j-1,k);
                CHECK_NEIGHBOUR(current, neighbour, Back)
                neighbour = (*this)(i,j,k+1);
                CHECK_NEIGHBOUR(current, neighbour, Top)
                neighbour = (*this)(i,j,k-1);
                CHECK_NEIGHBOUR(current, neighbour, Bottom)
            }
        }
    }
}

//TODO: don't update all buffers, only the shader storage buffers need to be updated
void Chunk::update_render_info()
{
    glBindVertexArray(world_cubes_VAO);

    glBindBuffer(GL_ARRAY_BUFFER, cube_rendering_VBOs[0]);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vec3), (void*)0);
    glBufferData(GL_ARRAY_BUFFER, MESH->vertices->size()*sizeof(vec3), 
    MESH->vertices->data(), GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, cube_rendering_VBOs[1]);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_TRUE, sizeof(vec3), (void*)0);
    glBufferData(GL_ARRAY_BUFFER, MESH->normals->size()*sizeof(vec3),
    MESH->normals->data(), GL_DYNAMIC_DRAW);
    
    glBindBuffer(GL_ARRAY_BUFFER, cube_rendering_VBOs[2]);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(vec2), (void*)0);
    glBufferData(GL_ARRAY_BUFFER, MESH->uvs->size()*sizeof(vec2),
        MESH->uvs->data(), GL_DYNAMIC_DRAW);

    //TODO: figure out if other buffers are faster
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, cube_rendering_VBOs[3]);
    glBufferData(GL_SHADER_STORAGE_BUFFER, faces_info.size()*sizeof(vec4), faces_info.data(), GL_DYNAMIC_COPY);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, cube_rendering_VBOs[3]);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cube_rendering_VBOs[4]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, MESH->indices->size()*sizeof(uint),
        MESH->indices->data(), GL_DYNAMIC_DRAW);
}
//########################################################################################

//========================================================================================
/*
*	Mega_Chunk Class implementation:
*/
//========================================================================================
//TODO: comment this section

World::World()
{    
    loaded_chunks = (Chunk****)malloc(load_distance*sizeof(Chunk***));

    for(int i=0; i<load_distance; i++)
    {
        loaded_chunks[i] = (Chunk***)malloc(load_distance*sizeof(Chunk**));
        for(int j=0; j<load_distance; j++)
        {
            loaded_chunks[i][j] = (Chunk**)malloc(load_distance*sizeof(Chunk*));
        }
    }

    for(uint i=0; i<load_distance; i++)
    {
        for(uint j=0; j<load_distance; j++)
        {
            for(uint k=0; k<load_distance; k++)
            {
                loaded_chunks[i][j][k] = new Chunk(vec3(i*CHUNK_DIMENSIONS,j*CHUNK_DIMENSIONS,k*CHUNK_DIMENSIONS));
                //c->render_chunk();
            }
        }
    }
}

void World::render_world()
{
   for(uint i=0; i<load_distance; i++)
   {
       for(uint j=0; j<load_distance; j++)
       {
           for(uint k=0; k<load_distance; k++)
           {
               Chunk* c = loaded_chunks[i][j][k];
               c->render_chunk();
           }
       }
   }
}
//########################################################################################