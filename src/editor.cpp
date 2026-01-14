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
    static int selectedIndex = 0;

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
        ImGui::BeginListBox("Select Object", ImVec2(0, 80));
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
    
    if (ImGui::CollapsingHeader("Object3D Editor")) {
        // Object selector listbox
	//
        ImGui::SameLine();
        if (ImGui::Button("Add Object")) {
            objects.emplace_back();
            selectedIndex = objects.size() - 1;
        }
        ImGui::SameLine();
        if (ImGui::Button("Delete Selected") && objects.size() > 0) {
            objects.erase(objects.begin() + selectedIndex);
            if (selectedIndex >= objects.size()) {
                selectedIndex = objects.size() - 1;
            }
            if (selectedIndex < 0) selectedIndex = 0;
        }

        // Edit selected object
        if (selectedIndex >= 0 && selectedIndex < objects.size()) {
            Object3D& object = objects[selectedIndex];
            
            // Transform editing section
            if (ImGui::CollapsingHeader("Transform")) {
                ImGui::DragFloat3("Position", &object.position.x, 0.1f);
                ImGui::DragFloat3("Rotation", &object.rotation.x, 0.1f);
                ImGui::DragFloat3("Rotation / Tick", &object.rotationPerTick.x, 0.1f);
                ImGui::DragFloat3("Scale", &object.scale.x, 0.1f);
            }

            // Properties table (same as before)
            if (ImGui::CollapsingHeader("Custom Properties")) {
                if (ImGui::BeginTable("Object Properties", 3, 
                    ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders | ImGuiTableFlags_Resizable)) {
                    ImGui::TableSetupColumn("Property");
                    ImGui::TableSetupColumn("Value");
                    ImGui::TableSetupColumn("Action");
                    ImGui::TableHeadersRow();

                    for (auto it = object.properties.begin(); it != object.properties.end(); ) {
                        ImGui::TableNextRow();
                        ImGui::TableNextColumn();

                        ImGui::PushID(it->first.c_str());
                        char keyBuf[256];
                        strcpy(keyBuf, it->first.c_str());
                        ImGui::InputText("##key", keyBuf, sizeof(keyBuf));

                        if (strlen(keyBuf) != it->first.size() || strcmp(keyBuf, it->first.c_str()) != 0) {
                            if (strlen(keyBuf) > 0) {
                                object.properties[keyBuf] = it->second;
                                if (strcmp(keyBuf, it->first.c_str()) != 0) {
                                    object.properties.erase(it->first);
                                }
                                it = object.properties.find(keyBuf);
                            }
                        }

                        ImGui::TableNextColumn();
                        char valBuf[256];
                        strcpy(valBuf, it->second.c_str());
                        ImGui::InputText("##value", valBuf, sizeof(valBuf));
                        it->second = valBuf;

                        ImGui::TableNextColumn();
                        if (ImGui::Button("Delete")) {
                            object.properties.erase(it->first);
                            ImGui::PopID();
                            continue;
                        }
                        ImGui::PopID();
                        ++it;
                    }
                    ImGui::EndTable();
                }

                // Add new property
                static char newKey[256] = "", newVal[256] = "";
                ImGui::InputText("New Property", newKey, sizeof(newKey));
                ImGui::InputText("Value", newVal, sizeof(newVal));
                if (ImGui::Button("Add") && strlen(newKey) > 0) {
                    object.properties[newKey] = newVal;
                    newKey[0] = newVal[0] = '\0';
                }
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
