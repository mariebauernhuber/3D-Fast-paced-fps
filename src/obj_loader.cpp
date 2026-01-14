#include "../include/mesh.hpp"
#include <fstream>
#include <sstream>
#include <iostream>

unsigned long long nTrisPushedBack;

bool mesh::LoadFromObjectFile(const std::string& filename) {
    std::ifstream f(filename);
    if (!f.is_open())
        return false;

    std::vector<vec3d> verts;
    std::string line;

    while (std::getline(f, line)) {
        if (line.empty())
            continue;

        std::istringstream s(line);
        char type;

        s >> type;
        if (type == 'v') {
            vec3d v{};
            s >> v.x >> v.y >> v.z;
            verts.push_back(v);
        } else if (type == 'f') {
            int i0, i1, i2;
            s >> i0 >> i1 >> i2;
            // OBJ indices are 1-based
            triangle tri{
                verts[i0 - 1],
                verts[i1 - 1],
                verts[i2 - 1]
            };
            tris.push_back(tri);
	    nTrisPushedBack++;
        }
    }
    return true;
}

#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

bool loadObjPositions(const std::string& path, std::vector<float>& outVertices){
    std::ifstream file(path);
    if (!file.is_open())
        return false;

    outVertices.clear();

    std::string line;
    while (std::getline(file, line))
    {
        // Skip empty lines and comments
        if (line.empty() || line[0] == '#')
            continue;

        std::istringstream iss(line);
        std::string type;
        iss >> type;

        // Only handle vertex position lines: "v x y z"
        if (type == "v")
        {
            float x, y, z;
            if (!(iss >> x >> y >> z))
                continue; // malformed line, skip

            outVertices.push_back(x);
            outVertices.push_back(y);
            outVertices.push_back(z);
        }
    }

    return !outVertices.empty();
}
