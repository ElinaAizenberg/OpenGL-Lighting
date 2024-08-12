#include "../include/loader.h"
#include <iostream>
#include <complex>


void ObjectLoader::loadObjFileData(const std::string &filepath,
                                  std::vector<float> &object_vertices,
                                  std::vector<float> &object_normals,
                                  std::vector<unsigned int> &indices_)
/**  Loads vertices and vector of shapes where each shape contains indices using open-source library tiny-obj-loader.
Calculates and loads normals per every vertex.*/
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

    std::vector<std::vector<unsigned int>> object_shapes;
    for (auto const& shape : shapes)
    {
        std::vector<unsigned int> shape_indices;
        for (auto const& index : shape.mesh.indices)
        {
            indices_.push_back(index.vertex_index);
            shape_indices.push_back(index.vertex_index);
        }
        object_shapes.push_back(shape_indices);
    }
    loadObjNormals(object_normals, object_vertices, object_shapes);
}

void ObjectLoader::loadObjNormals(std::vector<float>& object_normals,
        std::vector<float>& object_vertices,
                                   std::vector<std::vector<unsigned int>>& object_shapes)
/**Calculates normals for each vertex in an object using the vertices and their indices.
It first computes the surface normal for each triangle and then averages them to find the vertex normals. */
{
    // Iterate through each shape in the object
    for (auto const& shape_indices : object_shapes)
    {
        // Map to store surface normals associated with each vertex
        std::map<unsigned int, std::vector<Vertex>> surfaces_normals;

        // Iterate through indices in steps of 3 (each triplet represents a triangle)
        for (uint32_t i = 0; i < shape_indices.size(); i += 3)
        {
            // Retrieve indices of the triangle's vertices
            unsigned int a_index = shape_indices[i];
            unsigned int b_index = shape_indices[i + 1];
            unsigned int c_index = shape_indices[i + 2];

            // Retrieve the vertices using the indices
            Vertex a = {object_vertices[a_index * 3 + 0],
                        object_vertices[a_index * 3 + 1],
                        object_vertices[a_index * 3 + 2]};
            Vertex b = {object_vertices[b_index * 3 + 0],
                        object_vertices[b_index * 3 + 1],
                        object_vertices[b_index * 3 + 2]};
            Vertex c = {object_vertices[c_index * 3 + 0],
                        object_vertices[c_index * 3 + 1],
                        object_vertices[c_index * 3 + 2]};

            // Calculate the surface normal for the triangle
            Vertex surface_normal = calculateSurfaceNormal(a, b, c);

            // Associate the calculated surface normal with each vertex of the triangle
            for (auto vertex_ind : {a_index, b_index, c_index})
            {
                auto it = surfaces_normals.find(vertex_ind);

                // If the vertex already has associated normals, append the new one
                if (it != surfaces_normals.end())
                {
                    it->second.push_back(surface_normal);
                }
                    // If the vertex has no associated normals, create a new entry
                else
                {
                    std::vector<Vertex> to_add = {surface_normal};
                    surfaces_normals.insert(std::make_pair(vertex_ind, to_add));
                }
            }
        }

        // Calculate the average normal for each vertex and store it in object_normals
        for (uint32_t i = 0; i < object_vertices.size() / 3; i++)
        {
            auto it = surfaces_normals.find(i);
            if (it != surfaces_normals.end())
            {
                Vertex vertex_normal = calculateAverageSurfaceNormal(it->second);
                object_normals.push_back(vertex_normal.x);
                object_normals.push_back(vertex_normal.y);
                object_normals.push_back(vertex_normal.z);
            }
        }
    }
}

Vertex ObjectLoader::calculateSurfaceNormal(Vertex& v1, Vertex& v2, Vertex& v3)
/** This function computes the normal vector for a surface defined by three vertices.
It uses the cross product of two edges of the triangle to find the surface normal. */
{
    // Calculate the vectors representing two edges of the triangle
    Vertex edge1 = {v2.x - v1.x, v2.y - v1.y, v2.z - v1.z};
    Vertex edge2 = {v3.x - v1.x, v3.y - v1.y, v3.z - v1.z};

    // Compute the cross product of the two edges to get the normal vector
    Vertex normal = {edge1.y * edge2.z - edge1.z * edge2.y,
                     edge1.z * edge2.x - edge1.x * edge2.z,
                     edge1.x * edge2.y - edge1.y * edge2.x};

    // Normalize the normal vector (make it a unit vector)
    float length = std::sqrt(normal.x * normal.x + normal.y * normal.y + normal.z * normal.z);

    if (length > 0)
    {
        normal.x /= length;
        normal.y /= length;
        normal.z /= length;
    }

    return normal;
}

Vertex ObjectLoader::calculateAverageSurfaceNormal(std::vector<Vertex> const& normals)
/** Calculates the average normal vector for a vertex by averaging the normals of all surfaces sharing the vertex. */
{
    Vertex sum = {0.0, 0.0, 0.0};

    for (auto& normal : normals)
    {
        sum.x += normal.x;
        sum.y += normal.y;
        sum.z += normal.z;
    }

    float length = std::sqrt(sum.x * sum.x + sum.y * sum.y + sum.z * sum.z);

    if (length > 0)
    {
        sum.x /= length;
        sum.y /= length;
        sum.z /= length;
    }
    return sum;
}
