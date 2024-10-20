#ifndef QUADMESH_H
#define QUADMESH_H

#include "VECTOR3D.h"
#include <utility>  // Necessary for std::pair

// Structure representing a mesh vertex
struct MeshVertex {
    VECTOR3D position;
    VECTOR3D normal;
};

// Structure representing a quad (four vertices)
struct MeshQuad {
    MeshVertex* vertices[4];  // pointers to vertices of each quad
};

class QuadMesh {
private:
    int maxMeshSize;
    int minMeshSize;
    float meshDim;

    int numVertices;
    MeshVertex* vertices;

    int numQuads;
    MeshQuad* quads;

    int numFacesDrawn;

    // Material properties
    GLfloat mat_ambient[4];
    GLfloat mat_specular[4];
    GLfloat mat_diffuse[4];
    GLfloat mat_shininess[1];

private:
    bool CreateMemory();  // Allocates memory for the mesh
    void FreeMemory();    // Frees memory used by the mesh

public:
    typedef std::pair<int, int> MaxMeshDim;  // Corrected the type definition for mesh dimensions

    QuadMesh(int maxMeshSize = 40, float meshDim = 1.0f);

    ~QuadMesh() {
        FreeMemory();
    }

    MaxMeshDim GetMaxMeshDimensions() const;  // Corrected the function signature

    bool InitMesh(int meshSize, VECTOR3D origin, double meshLength, double meshWidth, VECTOR3D dir1, VECTOR3D dir2);
    void DrawMesh(int meshSize);
    void SetMaterial(VECTOR3D ambient, VECTOR3D diffuse, VECTOR3D specular, double shininess);
    void ComputeNormals();
};

#endif  // QUADMESH_H
