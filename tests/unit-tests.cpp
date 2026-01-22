#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>
#include "../include/geometry.hpp"

TEST_CASE("Vec3 operations") {
    vec3d v1{1.0f, 2.0f, 3.0f};
    vec3d v2{4.0f, 5.0f, 6.0f};
    
    SECTION("Dot product") {
	float result = Vector_DotProduct(v1, v2);
        REQUIRE(result == 32.0f);
    }
}
