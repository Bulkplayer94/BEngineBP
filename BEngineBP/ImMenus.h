#pragma once

#include "ImGui\imgui.h"
#include "MeshManager.h"
#include <vector>
#include <array>

namespace BEngine {

	// Not the OS
	namespace ImMenus {
		struct ImMenu {
			virtual void Initialize() {}
			virtual void Draw() {}
			virtual void Shutdown() {}
		};

		struct ModelSelectMenu : ImMenu {

			void Initialize() override {
				
			}

			void Draw() override {
				ImGui::Begin("ModelSelecter 3000");
				ImGui::Text("Select an Model to Spawn");
				if (ImGui::BeginListBox("Model Selector"))
				{
					for (auto& I : meshManager.meshList)
					{
						if (!I.second->isStatic) {
							if (ImGui::Button(I.first.c_str())) {
								entityManager.RegisterEntity(I.second, playerCamera.position);
							}
						}	
					}
					
					ImGui::EndListBox();
				}
				ImGui::End();
			}

		} inline modelSelectMenu;

		std::array<ImMenu*, 1> menuList = { &modelSelectMenu };

		void DrawImMenus() {
			for (auto& I : menuList)
			{
				I->Draw();
			}
		}
	}
}