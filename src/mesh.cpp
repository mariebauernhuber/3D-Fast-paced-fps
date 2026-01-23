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
    for (const auto& tri : obj.meshData.tris) {
        for (int i = 0; i < 3; i++) {
            vertices.push_back(tri.p[i].x);
            vertices.push_back(tri.p[i].y);
            vertices.push_back(tri.p[i].z);
        }
    }

    // 2. Generate and bind buffers
    glGenVertexArrays(1, &obj.meshData.VAO);
    glGenBuffers(1, &obj.meshData.VBO);

    glBindVertexArray(obj.meshData.VAO);

    // 3. Upload data
    glBindBuffer(GL_ARRAY_BUFFER, obj.meshData.VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

    // 4. Set vertex attributes (index 0 corresponds to 'aPos' in your shader)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // 5. Unbind
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
	obj.properties["name"] = "Unnamed Object";
}
