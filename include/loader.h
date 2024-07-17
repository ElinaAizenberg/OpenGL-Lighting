#ifndef PROJECT_3_LOADER_H
#define PROJECT_3_LOADER_H

#include <string>
#include <vector>
#include "tiny_obj_loader.h"
struct Vertex
{
    float x, y, z;
};
class ObjectLoader
{
public:

    static void loadObjFileData(const std::string &filepath,
                               std::vector<float> &object_vertices,
                               std::vector<float> &object_normals,
                               std::vector<unsigned int> &indices_);
private:
    static Vertex calculateAverageSurfaceNormal(std::vector<Vertex> const& normals);
    static Vertex calculateSurfaceNormal(Vertex& v1, Vertex& v2, Vertex& v3);
    static void loadObjNormals(std::vector<float>& object_normals, std::vector<float>& object_vertices,
                                                    std::vector<std::vector<unsigned int>>& object_shapes);


};
#endif //PROJECT_3_LOADER_H
