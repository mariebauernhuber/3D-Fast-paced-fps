#define CATCH_CONFIG_RUNNER
#include <catch2/catch.hpp>
#include <catch2/catch_reporter_automake.hpp>
#include "../include/unit-tests.hpp"

//TODO TESTS:
//Vector_IntersectPlane(vec3d &plane_p, vec3d &plane_n, vec3d &lineStart, vec3d &lineEnd)

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
