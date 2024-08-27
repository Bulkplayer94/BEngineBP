#pragma once

#include "ImGui\imgui.h"
#include "MeshManager.h"
#include <vector>
#include <array>

namespace BEngine {
	namespace ImMenus {
		struct ImMenu {
			virtual void Initialize() {}
			virtual void Draw() {}
			virtual void Shutdown() {}
		};

		struct ModelSelectMenu : ImMenu {

			void Draw() override {
				if (ImGui::Begin("ModelSelecter 3000##ModelSelectMenu"))
				{
					ImGui::Text("Select an Model to Spawn");
					if (ImGui::BeginListBox("Model Selector##ModelSelectMenu"))
					{
						for (auto& I : meshManager.meshList)
						{
							if (!I.second->isStatic) {
								if (ImGui::Button((I.first + "##ModelSelectMenu").c_str())) {
									entityManager.RegisterEntity(I.second, playerCamera.position);
								}
							}	
						}
						ImGui::EndListBox();
					}
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