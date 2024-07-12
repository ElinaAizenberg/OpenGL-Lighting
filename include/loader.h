#ifndef PROJECT_3_LOADER_H
#define PROJECT_3_LOADER_H

#include <string>
#include <vector>
#include "tiny_obj_loader.h"

class ObjectLoader
{
public:
    static void loadObFileData(const std::string &filepath,
                               std::vector<float> &object_vertices,
                               std::vector<unsigned int> &indices_);

};
#endif //PROJECT_3_LOADER_H
