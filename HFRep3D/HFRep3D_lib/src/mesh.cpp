#include "include/mesh.h"

namespace hfrep3D {

mesh::mesh()
{
    VAO = 0;
    VBO = 0;
    IBO = 0;
    indexCount = 0;
}

mesh::~mesh()
{
    ClearMesh();
}

void mesh::CreateMesh(GLfloat *vertices, unsigned int *indices, unsigned int numOfVertices, unsigned int numOfIndices)
{
    indexCount = numOfIndices;

    glGenVertexArrays( 1, &VAO );// creating vertex array on the GPU, reseving some space for it.
    glBindVertexArray( VAO );

    glGenBuffers ( 1, &IBO );
    glBindBuffer ( GL_ELEMENT_ARRAY_BUFFER, IBO ); //buffer for storing indeces only
    glBufferData ( GL_ELEMENT_ARRAY_BUFFER, sizeof( indices[0])*numOfIndices, indices, GL_STATIC_DRAW );

    glGenBuffers( 1, &VBO ); // generating buffers on the GPU
    glBindBuffer( GL_ARRAY_BUFFER, VBO );
    glBufferData( GL_ARRAY_BUFFER, sizeof( vertices[0] )*numOfVertices, vertices, GL_STATIC_DRAW ); // last parameter can be either GL_STATIC_DRAW/GL_DYNAMIC_DRAW

    // 1 param - layout; 2 param - 3 axes; 3 param - normalize; 4 param - vertices array can handle coords+colors, how many
    // values will be skipped; 5 param - offset, where the data starts
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vertices[0]) * 8, 0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(vertices[0]) * 8, (void*)(sizeof(vertices[0]) * 3));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(vertices[0]) * 8, (void*)(sizeof(vertices[0]) * 5));
    glEnableVertexAttribArray(2);

    //unbinding above bindings
    glBindBuffer( GL_ARRAY_BUFFER, 0 );
    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );
    glBindVertexArray( 0 );
}

void mesh::RenderMesh()
{
    glBindVertexArray( VAO );
    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, IBO );
    glDrawElements( GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0 );
    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );
    glBindVertexArray( 0 );
}

void mesh::ClearMesh()
{
    if( IBO != 0 )
    {
        glDeleteBuffers( 1, &IBO );
        IBO = 0;
    }

    if( VBO != 0 )
    {
        glDeleteBuffers( 1, &VBO );
        VBO = 0;
    }

    if( VAO != 0 )
    {
        glDeleteVertexArrays( 1, &VAO );
        VAO = 0;
    }

    indexCount = 0;
}

}//namespace hfrep3D
