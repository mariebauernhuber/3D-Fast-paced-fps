#pragma once
#include <vector>
#include "geometry.hpp"

bool IsTriangleInView(const triangle &tri);
void UpdateFrustumPlanes();
std::vector<triangle> ClipTriangleToFrustumOptimized(const triangle &tri);
