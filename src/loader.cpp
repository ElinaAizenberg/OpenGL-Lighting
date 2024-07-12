#include "../include/loader.h"
#include <iostream>


void ObjectLoader::loadObFileData(const std::string &filepath,
                                  std::vector<float> &object_vertices,
                                  std::vector<unsigned int> &indices_)
{
    tinyobj::ObjReaderConfig reader_config;
    tinyobj::ObjReader reader;

    if (!reader.ParseFromFile(filepath, reader_config))
    {
        if (!reader.Error().empty())
        {
            std::cerr << "TinyObjReader: " << reader.Error();
            throw reader.Error();
        }
        exit(1);
    }

    if (!reader.Warning().empty() && reader.Warning().find("Material") == std::string::npos)
    {
        std::cout << "TinyObjReader: " << reader.Warning();
        throw reader.Warning();
    }

    tinyobj::attrib_t attrib    = reader.GetAttrib();
    std::vector<tinyobj::shape_t> shapes    = reader.GetShapes();

    for (auto const& vertex : attrib.vertices)
    {
        object_vertices.push_back(vertex);
    }

    for (auto const& shape : shapes)
    {
        for (auto const& index : shape.mesh.indices)
        {
            indices_.push_back(index.vertex_index);
        }
    }
}