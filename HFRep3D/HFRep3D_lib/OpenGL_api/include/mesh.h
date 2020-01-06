#ifndef H_MESH_CLASS
#define H_MESH_CLASS

#include <GL/glew.h>

namespace hfrep3D {

class mesh
{
public:
    mesh();
    ~mesh();

    void CreateMesh( GLfloat *vertices, unsigned int *indices,
                     unsigned int numOfVertices, unsigned int numOfIndices );
    void RenderMesh();
    void ClearMesh();

private:
    GLuint VAO, VBO, IBO;
    GLsizei indexCount;
};

}//namespace hfrep3D
#endif
