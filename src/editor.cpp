#include "../include/editor.hpp"
#include <SDL3/SDL_init.h>
#include <imgui.h>
#include "../imguizmo/ImGuizmo.h"
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/common.hpp>
#include <glm/gtc/type_ptr.hpp>

extern float deltaTime;
extern float deltaTimeMod;
extern float newDeltaTime;
extern float targetFrameRate;
extern bool is_running;
extern GLuint fbo, textureColorBuffer;
extern vec3d vCamera;
extern mat4x4 matView, matProj;
glm::mat4 gridMatrix = glm::mat4(1.0f); 

void DrawObjectEditor(std::vector<Object3D>& objects) {
    static int selectedIndex = -1;

    ImGui::DockSpaceOverViewport(0, ImGui::GetMainViewport());

    ImGui::Begin("Game Viewport");
	ImGui::Image((void*)(intptr_t)textureColorBuffer, ImVec2(ImGui::GetContentRegionAvail().x, (ImGui::GetContentRegionAvail().x /16)*9), ImVec2(0, 0), ImVec2(1, -1));
//	ImGuizmo::SetDrawlist(); 
	float* viewPtr = (float*)&matView;
	float* projPtr = (float*)&matProj;
//	ImGuizmo::DrawGrid(viewPtr, projPtr, glm::value_ptr(gridMatrix), 100.0f);
	//ImGuizmo::SetRect(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y, ImGui::GetWindowSize().x, ImGui::GetWindowSize().y);
    ImGui::End();

    ImGui::Begin("Temp window");
    ImGui::End();

    ImGui::Begin("Scene tree");
        // Object selector listbox
        ImGui::BeginListBox(" ", ImVec2(500, 1000));
        for (int i = 0; i < objects.size(); ++i) {
            std::string displayName = "Unnamed Object " + std::to_string(i);
            auto nameIt = objects[i].properties.find("name");
            if (nameIt != objects[i].properties.end()) {
                displayName = nameIt->second;
            }
            if (ImGui::Selectable(displayName.c_str(), selectedIndex == i)) {
                selectedIndex = i;
            }
        }
        ImGui::EndListBox();
    ImGui::End();

    ImGui::Begin("Object Editor", NULL, ImGuiViewportFlags());

    	if(ImGui::Button("Exit App")){
		is_running = false;
	}

    	ImGui::DragFloat("deltaTimeForCalc", &newDeltaTime);
    	ImGui::DragFloat("Actual deltaTime", &deltaTime);
	ImGui::DragFloat("DeltaTime Modifier", &deltaTimeMod);
	ImGui::DragFloat("Target Framerate", &targetFrameRate);
    if(selectedIndex != -1){
    if (ImGui::CollapsingHeader("Object3D Editor")) {
        // Object selector listbox
	//
        if (ImGui::Button("Add Object")) {
            objects.emplace_back();
	    selectedIndex = -1;
        }
        if (ImGui::Button("Delete Selected") && objects.size() > 0) {
            objects.erase(objects.begin() + selectedIndex);
                selectedIndex = -1;
            }
        }

        // Edit selected object
        if (selectedIndex >= 0 && selectedIndex < objects.size()) {
            Object3D& object = objects[selectedIndex];
            
            // Transform editing section
            if (ImGui::CollapsingHeader("Transform")) {
                ImGui::DragFloat3("Position", &objects[selectedIndex].position.x, 0.1f);
                ImGui::DragFloat3("Rotation", &object.rotation.x, 0.1f);
                ImGui::DragFloat3("Rotation / Tick", &objects[selectedIndex].rotationPerTick.x, 0.1f);
                ImGui::DragFloat3("Scale", &object.scale.x, 0.1f);
            }
        } else {
            ImGui::Text("No objects available");
        }
    }

	if(ImGui::CollapsingHeader("Camera and Player Settings")){
		ImGui::DragFloat3("Camera Pos", &vCamera.x, 0.1f);
	}

	ImGui::End();
}
