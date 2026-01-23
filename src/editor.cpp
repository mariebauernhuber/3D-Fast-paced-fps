#include "../include/editor.hpp"
#include <SDL3/SDL_init.h>
#include "../imgui/imgui.h"
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/common.hpp>
#include <glm/gtc/type_ptr.hpp>

extern float deltaTime;
extern float deltaTimeMod;
extern float newDeltaTime;
extern float targetFrameRate;
extern float realFrameRate;
extern bool is_running;
extern GLuint fbo, textureColorBuffer;
extern vec3d vCamera;
extern mat4x4 matView, matProj;
glm::mat4 gridMatrix = glm::mat4(1.0f); 
bool isImGuiGameViewportInFocus = false;
bool isImGuiGameViewportInMouseLock = false;
extern float secondsElapsedSinceStartup;
extern float secondTiming;

void DrawObjectEditor(std::vector<Object3D>& objects) {
        static int selectedIndex = -1;
        ImGui::DockSpaceOverViewport(0, ImGui::GetMainViewport());
        ImGui::Begin("Game Viewport (press F1 to toggle focus)");
    	ImGui::Image(textureColorBuffer, ImVec2(ImGui::GetContentRegionAvail().x, (ImGui::GetContentRegionAvail().x /16)*9), ImVec2(0, 0), ImVec2(1, -1));
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
	ImGui::DragFloat("SecondsElapsed", (float*)&secondTiming);
	ImGui::DragFloat("frameRate", (float*)&realFrameRate);

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
            // Transform editing section
            if (ImGui::CollapsingHeader("Transform")) {
                ImGui::DragFloat3("Position", &objects[selectedIndex].position.x, 0.1f);
                ImGui::DragFloat3("Rotation", &objects[selectedIndex].rotation.x, 0.1f);
                ImGui::DragFloat3("Rotation / Tick", &objects[selectedIndex].rotationPerTick.x, 0.1f);
                ImGui::DragFloat3("Position / Tick", &objects[selectedIndex].positionPerTick.x, 0.1f);
                ImGui::DragFloat3("Scale", &objects[selectedIndex].scale.x, 0.1f);
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
