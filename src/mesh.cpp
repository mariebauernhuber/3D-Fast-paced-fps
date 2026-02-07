#include "../include/mesh.hpp"
#include "../include/geometry.hpp"

extern std::vector<Object3D> objects;

mat4x4 Object3D::GetWorldMatrix()  {

	mat4x4 matRotX = Matrix_MakeRotationX(rotation.x);
	mat4x4 matRotY = Matrix_MakeRotationY(rotation.y);
	mat4x4 matRotZ = Matrix_MakeRotationZ(rotation.z);

	mat4x4 matWorld = Matrix_MultiplyMatrix(matRotZ, matRotX);
	matWorld = Matrix_MultiplyMatrix(matWorld, matRotY);
	mat4x4 matTrans = Matrix_MakeTranslation(position.x, position.y, position.z);
	matWorld = Matrix_MultiplyMatrix(matWorld, matTrans);
	return matWorld;
}

void InitializeObjectGPU(Object3D &obj) {
    std::vector<float> vertices;
    vertices.reserve(obj.meshData.tris.size() * 15);  // 3pos×3 + 2uv×3 = 15 floats/tri

    for (const auto& tri : obj.meshData.tris) {
        for (int i = 0; i < 3; ++i) {
            // Position (existing)
            vertices.push_back(tri.p[i].x);
            vertices.push_back(tri.p[i].y);
            vertices.push_back(tri.p[i].z);
            
            // NEW: UVs
            vertices.push_back(tri.t[i].x);
            vertices.push_back(tri.t[i].y);
        }
    }

    glGenVertexArrays(1, &obj.meshData.VAO);
    glGenBuffers(1, &obj.meshData.VBO);
    glBindVertexArray(obj.meshData.VAO);

    glBindBuffer(GL_ARRAY_BUFFER, obj.meshData.VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void SetObjDefaults(Object3D &obj){
	obj.scale = {1.0f, 1.0f, 1.0f};
	obj.position = {0.0f, 0.0f, 0.0f};
	obj.rotationPerTick = {0.0f, 0.0f, 0.0f};
	obj.positionPerTick = {0.0f, 0.0f, 0.0f};
	obj.relativePositionPerTick = {0.0f, 0.0f, 0.0f};
	obj.rotation = {0.0f, 0.0f, 0.0f};
	obj.farthestPoint = obj.position;
	obj.plen = 0.0f;
	obj.cullingFrontFace = GL_CCW;
	obj.cullingMode = GL_BACK;
	obj.properties["name"] = "Unnamed Object";
}

bool Object3D::CreateBroadCollisionCircle(){ // MUST BE CALLED AFTER CREATING MESHDATA
	triangle farthestTri; // This is the triangle which has the farthest point from the center
	vec3d x;
	float xlen;
	farthestTri = meshData.tris.at(0);
	plen = 0.0f;
	for(auto& tri : meshData.tris){
		for(int i = 0; i < 3; i++){
			x = {0.0f, 0.0f, 0.0f, 0.0f};
			x.x = tri.p[i].x - position.x;
			x.y = tri.p[i].y - position.y;
			x.z = tri.p[i].z - position.z;
			xlen = Vector_Length(x);
			if(xlen > plen){
				plen = xlen;
				farthestPoint = x;
				farthestTri = tri;
			}
		}
	}
	return 0;
}
