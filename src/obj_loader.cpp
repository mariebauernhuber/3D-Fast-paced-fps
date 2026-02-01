#include "../include/mesh.hpp"
#include <SDL3/SDL_oldnames.h>
#include <SDL3/SDL_surface.h>
#include <SDL3/SDL_pixels.h>
#include <SDL3_image/SDL_image.h>
#include <fstream>
#include <sstream>
#include <iostream>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

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

bool mesh::LoadFromAssimp(const std::string& filename) {
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(filename,
        aiProcess_Triangulate |   // quads/polys → triangles
        aiProcess_FlipUVs         // Blender OBJ compatibility
    );

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        return false;
    }

    tris.clear();
    nTrisPushedBack = 0;

    // OBJ = typically 1 root mesh (matches your manual loader)
    if (scene->mNumMeshes > 0) {
        aiMesh* ai_mesh = scene->mMeshes[0];
        tris.reserve(ai_mesh->mNumFaces);

        for (unsigned i = 0; i < ai_mesh->mNumFaces; ++i) {
            const aiFace& face = ai_mesh->mFaces[i];
            if (face.mNumIndices != 3) continue;  // safety post-triangulate

            triangle tri;

            // p[0] ← face indices[0] (exactly like verts[i0-1] in your OBJ loader)
            tri.p[0] = {
                ai_mesh->mVertices[face.mIndices[0]].x,
                ai_mesh->mVertices[face.mIndices[0]].y,
                ai_mesh->mVertices[face.mIndices[0]].z
            };

            // p[1]
            tri.p[1] = {
                ai_mesh->mVertices[face.mIndices[1]].x,
                ai_mesh->mVertices[face.mIndices[1]].y,
                ai_mesh->mVertices[face.mIndices[1]].z
            };

            // p[2]
            tri.p[2] = {
                ai_mesh->mVertices[face.mIndices[2]].x,
                ai_mesh->mVertices[face.mIndices[2]].y,
                ai_mesh->mVertices[face.mIndices[2]].z
            };

if (ai_mesh->HasTextureCoords(0)) {
                tri.t[0] = {
                    ai_mesh->mTextureCoords[0][face.mIndices[0]].x,
                    ai_mesh->mTextureCoords[0][face.mIndices[0]].y
                };
                tri.t[1] = {
                    ai_mesh->mTextureCoords[0][face.mIndices[1]].x,
                    ai_mesh->mTextureCoords[0][face.mIndices[1]].y
                };
                tri.t[2] = {
                    ai_mesh->mTextureCoords[0][face.mIndices[2]].x,
                    ai_mesh->mTextureCoords[0][face.mIndices[2]].y
                };
            } else {
                // Fallback: white (1,1) or zeros
                tri.t[0] = tri.t[1] = tri.t[2] = {0.5f, 0.5f};
            }

            tris.push_back(tri);
            nTrisPushedBack++;
        }
    }

    return !tris.empty();
}

GLuint LoadTexture(SDL_Surface* surface) {
    GLuint tex;
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);

    // FIXED: SDL3 - format is direct member, no -> needed
    GLenum format;
    if (SDL_BYTESPERPIXEL(surface->format) == 4) {
        format = GL_RGBA;
    } else {
        format = GL_RGB;
    }

    glTexImage2D(GL_TEXTURE_2D, 0, format, surface->w, surface->h, 0, 
                 format, GL_UNSIGNED_BYTE, surface->pixels);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    SDL_DestroySurface(surface);
    return tex;
}

#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <charconv>

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
