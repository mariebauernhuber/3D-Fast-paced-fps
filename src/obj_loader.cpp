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

#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <charconv>
#include <cctype>

bool mesh::LoadFromObjectFileNew(const std::string& filename) {
    std::ifstream f(filename);
    if (!f.is_open()) {
        return false;
    }

    // Temporary storage for OBJ data
    std::vector<vec3d> tempVerts;
    std::vector<vec3d> tempNormals;
    std::vector<vec2d> tempTexCoords;  // assuming vec2d exists for UVs

    std::string line;
    while (std::getline(f, line)) {
        // Skip empty lines and comments
        if (line.empty() || line[0] == '#') {
            continue;
        }

        std::istringstream s(line);
        std::string type;
        s >> type;

        if (type == "v") {
            vec3d v{};
            s >> v.x >> v.y >> v.z;
            tempVerts.push_back(v);
        }
        else if (type == "vt") {
            vec2d uv{};
            s >> uv.x >> uv.y;
            // Optionally flip V if needed (OpenGL convention)
            // uv.y = 1.0 - uv.y;
            tempTexCoords.push_back(uv);
        }
        else if (type == "vn") {
            vec3d n{};
            s >> n.x >> n.y >> n.z;
            tempNormals.push_back(n);
        }
        else if (type == "f") {
            // Parse face: v1[/vt1[/vn1]] v2[/vt2[/vn2]] v3[/vt3[/vn3]]
            std::vector<std::string> faceTokens;
            std::string token;
            while (s >> token) {
                faceTokens.push_back(token);
            }

            // We need at least 3 vertices for a triangle
            if (faceTokens.size() < 3) {
                continue;
            }

            // Parse each vertex index triple (v/vt/vn) in the face
            for (size_t i = 0; i < 3; ++i) {
                const std::string& vertStr = faceTokens[i];
                int vIdx = -1, vtIdx = -1, vnIdx = -1;

                // Split by '/' and parse indices
                size_t start = 0;
                size_t pos = 0;
                int field = 0;
                while ((pos = vertStr.find('/', start)) != std::string::npos) {
                    if (start < pos) {
                        std::string_view fieldView(vertStr.data() + start, pos - start);
                        if (field == 0) {
                            // Vertex index
                            std::from_chars(fieldView.data(), fieldView.data() + fieldView.size(), vIdx);
                        }
                        else if (field == 1) {
                            // Texture coord index
                            std::from_chars(fieldView.data(), fieldView.data() + fieldView.size(), vtIdx);
                        }
                    }
                    start = pos + 1;
                    ++field;
                }
                // Last part (after last '/')
                if (start < vertStr.size()) {
                    std::string_view lastField(vertStr.data() + start, vertStr.size() - start);
                    if (field == 0) {
                        std::from_chars(lastField.data(), lastField.data() + lastField.size(), vIdx);
                    }
                    else if (field == 1) {
                        std::from_chars(lastField.data(), lastField.data() + lastField.size(), vtIdx);
                    }
                    else if (field == 2) {
                        std::from_chars(lastField.data(), lastField.data() + lastField.size(), vnIdx);
                    }
                }

                // OBJ indices are 1-based; convert to 0-based
                if (vIdx > 0 && vIdx <= static_cast<int>(tempVerts.size())) {
                    vIdx -= 1;
                } else {
                    // Invalid vertex index; skip this face
                    goto next_face;
                }

                // For now, just use vertex position (ignore vt/vn for triangle mesh)
                // In a real engine, you’d build a vertex array with pos/uv/normal
                if (i == 0) {
                    // First vertex of triangle
                    triangle tri;
                    tri.p[0] = tempVerts[vIdx];
                    // tri.uv[0] = (vtIdx > 0 && vtIdx <= tempTexCoords.size()) ? tempTexCoords[vtIdx-1] : vec2d{0,0};
                    // tri.n[0] = (vnIdx > 0 && vnIdx <= tempNormals.size())  ? tempNormals[vnIdx-1]  : vec3d{0,0,1};
                    tri.p[1] = tempVerts[vIdx]; // will be overwritten below
                    tri.p[2] = tempVerts[vIdx]; // will be overwritten below
                    tris.push_back(tri);
                    ++nTrisPushedBack;
                } else if (i == 1) {
                    tris.back().p[1] = tempVerts[vIdx];
                } else if (i == 2) {
                    tris.back().p[2] = tempVerts[vIdx];
                }
            }
            next_face:;
        }
    }

    return true;
}

#include <vector>
#include <string>

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
