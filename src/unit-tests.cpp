#include <cmath>
#define CATCH_CONFIG_RUNNER
#include <catch2/catch.hpp>
#include <catch2/catch_reporter_automake.hpp>
#include "../include/unit-tests.hpp"
#include "../include/geometry.hpp"

//TODO TESTS:
//Vector_IntersectPlane(vec3d &plane_p, vec3d &plane_n, vec3d &lineStart, vec3d &lineEnd)

TEST_CASE("Vector_Add"){
	vec3d v1{1.0f, 2.0f, 3.0f};
	vec3d v2{4.0f, 5.0f, 6.0f};
	vec3d vnull{0.0f, 0.0f, 0.0f};

	SECTION("Adding two nonzero vec3ds"){
		vec3d result = Vector_Add(v1, v2);
		REQUIRE(result.x == 5.0f);
		REQUIRE(result.y == 7.0f);
		REQUIRE(result.z == 9.0f);
	}

	SECTION("Adding a zero vec3d to a nonzero vec3d"){
		vec3d result = Vector_Add(v1, vnull);
		REQUIRE(result.x == v1.x);
		REQUIRE(result.y == v1.y);
		REQUIRE(result.z == v1.z);
	}

	SECTION("Adding a nonzero vec3d to a zero vec3d"){
		vec3d result = Vector_Add(vnull, v1);
		REQUIRE(result.x == v1.x);
		REQUIRE(result.y == v1.y);
		REQUIRE(result.z == v1.z);
	}

	SECTION("Adding a zero vec3d to a zero vec3d"){
		vec3d result = Vector_Add(vnull, vnull);
		REQUIRE(result.x == 0.0f);
		REQUIRE(result.y == 0.0f);
		REQUIRE(result.z == 0.0f);
	}
}

TEST_CASE("Vector_Sub"){
	vec3d v1{1.0f, 2.0f, 3.0f};
	vec3d v2{4.0f, 5.0f, 6.0f};
	vec3d vnull{0.0f, 0.0f, 0.0f};

	SECTION("Subtracting a nonzero vec3d from a nonzero vec3d"){
		vec3d result = Vector_Sub(v1, v2);
		REQUIRE(result.x == -3.0f);
		REQUIRE(result.y == -3.0f);
		REQUIRE(result.z == -3.0f);
	}

	SECTION("Subtracting a zero vec3d from a nonzero vec3d"){
		vec3d result = Vector_Sub(v1, vnull);
		REQUIRE(result.x == v1.x);
		REQUIRE(result.y == v1.y);
		REQUIRE(result.z == v1.z);
	}

	SECTION("Subtracting a nonzero vec3d from a zero vec3d"){
		vec3d result = Vector_Sub(vnull, v1);
		REQUIRE(result.x == -v1.x);
		REQUIRE(result.y == -v1.y);
		REQUIRE(result.z == -v1.z);
	}

	SECTION("Subtracting a zero vec3d from a zero vec3d"){
		vec3d result = Vector_Add(vnull, vnull);
		REQUIRE(result.x == 0.0f);
		REQUIRE(result.y == 0.0f);
		REQUIRE(result.z == 0.0f);
	}
}

TEST_CASE("Vector_Mul"){
	vec3d v1{1.0f, 2.0f, 3.0f};
	vec3d vnull{0.0f, 0.0f, 0.0f};

	SECTION("Multiplying a nonzero vec3d with a nonzero positive float"){
		INFO("vec3ds and floats used: {" << v1.x << ", " << v1.y << ", " << v1.z << "} and float k = " << 5.0f);
		vec3d result = Vector_Mul(v1, 5.0f);
		REQUIRE(result.x == 5.0f);
		REQUIRE(result.y == 10.0f);
		REQUIRE(result.z == 15.0f);
	}

	SECTION("Multiplying a nonzero vec3d with a nonzero negative float"){
		INFO("vec3ds and floats used: {" << v1.x << ", " << v1.y << ", " << v1.z << "} and float k = " << -10.0f);
		vec3d result = Vector_Mul(v1, -10.0f);
		REQUIRE(result.x == -10.0f);
		REQUIRE(result.y == -20.0f);
		REQUIRE(result.z == -30.0f);
	}

	SECTION("Multiplying a nonzero vec3d with zero"){
		INFO("vec3ds and floats used: {" << v1.x << ", " << v1.y << ", " << v1.z << "} and float k = " << 0.0f);
		vec3d result = Vector_Mul(v1, 0.0f);
		REQUIRE(result.x == 0.0f);
		REQUIRE(result.y == 0.0f);
		REQUIRE(result.z == 0.0f);
	}

	SECTION("Multiplying a zero vec3d with zero"){
		INFO("vec3ds and floats used: {" << vnull.x << ", " << vnull.y << ", " << vnull.z << "} and float k = " << 0.0f);
		vec3d result = Vector_Mul(vnull, 0.0f);
		REQUIRE(result.x == 0.0f);
		REQUIRE(result.y == 0.0f);
		REQUIRE(result.z == 0.0f);
	}

	SECTION("Multiplying a zero vec3d with a nonzero positive float"){
		INFO("vec3ds and floats used: {" << vnull.x << ", " << vnull.y << ", " << vnull.z << "} and float k = " << 5.0f);
		vec3d result = Vector_Mul(vnull, 5.0f);
		REQUIRE(result.x == 0.0f);
		REQUIRE(result.y == 0.0f);
		REQUIRE(result.z == 0.0f);
	}

	SECTION("Multiplying a zero vec3d with a nonzero negative float"){
		INFO("vec3ds and floats used: {" << vnull.x << ", " << vnull.y << ", " << vnull.z << "} and float k = " << -10.0f);
		vec3d result = Vector_Mul(vnull, -10.0f);
		REQUIRE(result.x == 0.0f);
		REQUIRE(result.y == 0.0f);
		REQUIRE(result.z == 0.0f);
	}
}

TEST_CASE("Vector_Div"){
	vec3d v1{1.0f, 2.0f, 3.0f};
	vec3d vnull{0.0f, 0.0f, 0.0f};

	SECTION("Dividing a nonzero vec3d by a nonzero positive float"){
		INFO("vec3ds and floats used: {" << v1.x << ", " << v1.y << ", " << v1.z << "} and float k = " << 3.0f);
		vec3d result = Vector_Div(v1, 3.0f);
		REQUIRE_THAT(result.x, Catch::Matchers::WithinRel(0.333333f, 0.001f));
		REQUIRE_THAT(result.y, Catch::Matchers::WithinRel(0.666666f, 0.001f));
		REQUIRE(result.z == 1.0f);
	}

	SECTION("Dividing a nonzero vec3d by a nonzero negative float"){
		INFO("vec3ds and floats used: {" << v1.x << ", " << v1.y << ", " << v1.z << "} and float k = " << -3.0f);
		vec3d result = Vector_Div(v1, -3.0f);
		REQUIRE_THAT(result.x, Catch::Matchers::WithinRel(-0.333333f, 0.001f));
		REQUIRE_THAT(result.y, Catch::Matchers::WithinRel(-0.666666f, 0.001f));
		REQUIRE(result.z == -1.0f);
	}

	SECTION("Dividing a nonzero vec3d by zero"){
		INFO("vec3ds and floats used: {" << v1.x << ", " << v1.y << ", " << v1.z << "} and float k = " << 0.0f);
		INFO("The Vector_Div function is supposed to safely handle division by zero by returning zero");
		//IMPORTANT: This operation is supposed to fail (Dividing by zero)
		//The function should return a zero vector!
		vec3d result = Vector_Div(v1, 0.0f);
		REQUIRE(result.x == 0.0f);
		REQUIRE(result.y == 0.0f);
		REQUIRE(result.z == 0.0f);
	}

	SECTION("Dividing a zero vec3d by a nonzero positive float"){
		INFO("vec3ds and floats used: {" << vnull.x << ", " << vnull.y << ", " << vnull.z << "} and float k = " << 3.0f);
		vec3d result = Vector_Div(vnull, 3.0f);
		REQUIRE(result.x == 0.0f);
		REQUIRE(result.y == 0.0f);
		REQUIRE(result.z == 0.0f);
	}

	SECTION("Dividing a zero vec3d by a nonzero negative float"){
		INFO("vec3ds and floats used: {" << vnull.x << ", " << vnull.y << ", " << vnull.z << "} and float k = " << -3.0f);
		vec3d result = Vector_Div(vnull, -3.0f);
		REQUIRE(result.x == 0.0f);
		REQUIRE(result.y == 0.0f);
		REQUIRE(result.z == 0.0f);
	}

	SECTION("Dividing a zero vec3d by zero (READ NOTE IN unit-tests.cpp!)"){
		INFO("vec3ds and floats used: {" << vnull.x << ", " << vnull.y << ", " << vnull.z << "} and float k = " << 0.0f);
		INFO("The Vector_Div function is supposed to safely handle division by zero by returning zero");
		//IMPORTANT: This operation is supposed to fail (Dividing by zero)
		//The function should return a zero vector!
		vec3d result = Vector_Div(vnull, 0.0f);
		REQUIRE(result.x == 0.0f);
		REQUIRE(result.y == 0.0f);
		REQUIRE(result.z == 0.0f);
	}
}

TEST_CASE("Vector_CrossProduct"){
	vec3d v1{1.0f, 2.0f, 3.0f};
	vec3d v2{4.0f, 5.0f, 6.0f};
	vec3d vnull{0.0f, 0.0f, 0.0f};

	SECTION("Getting the cross product of two nonzero vec3ds"){
		INFO("vec3ds used: {" << v1.x << ", " << v1.y << ", " << v1.z << "} and {" << v2.x << ", " << v2.y << ", " << v2.z << "}.");
		vec3d result = Vector_CrossProduct(v1, v2);
		REQUIRE(result.x == -3.0f);
		REQUIRE(result.y == 6.0f);
		REQUIRE(result.z == -3.0f);
	}

	SECTION("Getting the cross product of a nonzero vec3d and a zero vec3d"){
		INFO("vec3ds used: {" << v1.x << ", " << v1.y << ", " << v1.z << "} and {" << vnull.x << ", " << vnull.y << ", " << vnull.z << "}.");
		vec3d result = Vector_CrossProduct(v1, vnull);
		REQUIRE(result.x == 0.0f);
		REQUIRE(result.y == 0.0f);
		REQUIRE(result.z == 0.0f);
	}

	SECTION("Getting the cross product of a zero vec3d and a nonzero vec3d"){
		INFO("vec3ds used: {" << vnull.x << ", " << vnull.y << ", " << vnull.z << "} and {" << v2.x << ", " << v2.y << ", " << v2.z << "}.");
		vec3d result = Vector_CrossProduct(vnull, v2);
		REQUIRE(result.x == 0.0f);
		REQUIRE(result.y == 0.0f);
		REQUIRE(result.z == 0.0f);
	}
}

TEST_CASE("Vector_DotProduct"){
	vec3d v1{1.0f, 2.0f, 3.0f};
	vec3d v2{4.0f, 5.0f, 6.0f};
	vec3d vnull{0.0f, 0.0f, 0.0f};

	SECTION("Getting the dot product of two nonzero vec3ds"){
		INFO("vec3ds used: {" << v1.x << ", " << v1.y << ", " << v1.z << "} and {" << v2.x << ", " << v2.y << ", " << v2.z << "}.");
		float result = Vector_DotProduct(v1, v2);
		REQUIRE(result == 32.0f);
	}

	SECTION("Getting the dot product of a nonzero vec3d and a zero vec3d"){
		INFO("vec3ds used: {" << v1.x << ", " << v1.y << ", " << v1.z << "} and {" << vnull.x << ", " << vnull.y << ", " << vnull.z << "}.");
		float result = Vector_DotProduct(v1, vnull);
		REQUIRE(result == 0.0f);
	}

}

TEST_CASE("Vector_Length"){
	vec3d v1{1.0f, 2.0f, 3.0f};
	vec3d vnull{0.0f, 0.0f, 0.0f};

	SECTION("Getting the length of a nonzero vec3d"){
		INFO("vec3d used: {" << v1.x << ", " << v1.y << ", " << v1.z << "}.");
		float result = Vector_Length(v1);
		REQUIRE(result == std::sqrt(Vector_DotProduct(v1, v1)));
		REQUIRE(result == std::sqrt(14.0f));
		REQUIRE_THAT(result, Catch::Matchers::WithinRel(3.74f, 0.01f));
	}

	SECTION("Getting the length of a zero vec3d"){
		INFO("vec3d used: {" << vnull.x << ", " << vnull.y << ", " << vnull.z << "}.");
		float result = Vector_Length(vnull);
		REQUIRE(result == 0.0f);
	}
}

TEST_CASE("Vector_Normalise"){
	vec3d v1{1.0f, 2.0f, 3.0f};
	vec3d vnull{0.0f, 0.0f, 0.0f};

	SECTION("Normalising a nonzero vec3d"){
		INFO("vec3d used: {" << v1.x << ", " << v1.y << ", " << v1.z << "}.");
		vec3d result = Vector_Normalise(v1);
		REQUIRE_THAT(result.x, Catch::Matchers::WithinRel(v1.x / 3.74f, 0.01f));
		REQUIRE_THAT(result.y, Catch::Matchers::WithinRel(v1.y / 3.74f, 0.01f));
		REQUIRE_THAT(result.z, Catch::Matchers::WithinRel(v1.z / 3.74f, 0.01f));

		REQUIRE_THAT(result.x, Catch::Matchers::WithinRel(0.2673f, 0.01f));
		REQUIRE_THAT(result.y, Catch::Matchers::WithinRel(0.5354f, 0.01f));
		REQUIRE_THAT(result.z, Catch::Matchers::WithinRel(0.8018f, 0.01f));
	}

	SECTION("Normalizing a zero vec3d"){
		INFO("vec3d used: {" << vnull.x << ", " << vnull.y << ", " << vnull.z << "}.");
		INFO("Pretty sure this also devides by zero.");
		vec3d result = Vector_Normalise(vnull);
		REQUIRE(result.x == 0.0f);
		REQUIRE(result.y == 0.0f);
		REQUIRE(result.z == 0.0f);
	}
}

int main_tests_runner(int argc, char* argv[]){
	Catch::Session session;
	return session.run();
	return Catch::Session().run();
}

int main_tests_runner_verbose(int argc, char* argv[]){
	Catch::Session session;
	session.configData().showSuccessfulTests = true;
	return session.run();
}
