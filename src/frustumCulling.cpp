#include "../include/frustumCulling.hpp"
#include "../include/geometry.hpp"
#include <vector>
#include <cmath>

struct Plane {
    vec3d point;
    vec3d normal;
};

std::vector<Plane> gFrustumPlanes;

extern float fNear, fFar;
extern float fFov;
extern float fAspectRatio;

bool IsTriangleInView(const triangle &tri) {
    // Check if any vertex is inside NDC cube
    for(int i=0; i<3; i++){
        if(tri.p[i].x >= -1.0f && tri.p[i].x <= 1.0f &&
           tri.p[i].y >= -1.0f && tri.p[i].y <= 1.0f &&
           tri.p[i].z >= 0.0f  && tri.p[i].z <= 1.0f) {
            return true; // At least one vertex is inside view
        }
    }
    return false; // Fully outside
}

void UpdateFrustumPlanes() {
    gFrustumPlanes.clear();

    float nearHeight = 2.0f * tanf(fFov * 0.5f * 3.14159265f / 180.0f) * fNear;
    float nearWidth  = nearHeight / fAspectRatio;

    float farHeight  = 2.0f * tanf(fFov * 0.5f * 3.14159265f / 180.0f) * fFar;
    float farWidth   = farHeight / fAspectRatio;

    // Near plane
    gFrustumPlanes.push_back({ {0,0,fNear}, {0,0,1} });
    // Far plane
    gFrustumPlanes.push_back({ {0,0,fFar}, {0,0,-1} });

    // Right plane
    gFrustumPlanes.push_back({ {0,0,0}, Vector_Normalise({fNear/2,0,fNear}) });
    // Left plane
    gFrustumPlanes.push_back({ {0,0,0}, Vector_Normalise({-fNear/2,0,fNear}) });
    // Top plane
    gFrustumPlanes.push_back({ {0,0,0}, Vector_Normalise({0,nearHeight/2,fNear}) });
    // Bottom plane
    gFrustumPlanes.push_back({ {0,0,0}, Vector_Normalise({0,-nearHeight/2,fNear}) });
}

std::vector<triangle> ClipTriangleToFrustumOptimized(const triangle &tri) {
    std::vector<triangle> clippedTris;
    clippedTris.push_back(tri);

    for(const auto &plane : gFrustumPlanes) {
        std::vector<triangle> newTris;
        for(auto &t : clippedTris) {
            triangle t1, t2;
            int n = Triangle_ClipAgainstPlane(plane.point, plane.normal, t, t1, t2);
            if(n == 1) newTris.push_back(t1);
            if(n == 2){ newTris.push_back(t1); newTris.push_back(t2); }
        }
        clippedTris = newTris;
        if(clippedTris.empty()) break; // Fully outside
    }

    return clippedTris;
}
