#include "pch.h"
#include "ErrorReporter.h"

using namespace BEngine;

void ErrorReporter::Report(ErrorLevel errLevel, std::string errMsg)
{
	errorList.push_back(std::pair<ErrorLevel, std::string>(errLevel, errMsg));
}

void ErrorReporter::Draw()
{
	ImGui::Begin("Error Reporter");

	for (auto& I : errorList) {
		ImColor textCol;
		switch (I.first) {
		case(ErrorLevel_LOW):
			textCol = ImColor(0, 255, 0);
			break;

		case(ErrorLevel_MEDIUM):
			textCol = ImColor(100, 100, 0);
			break;

		case(ErrorLevel_HIGH):
			textCol = ImColor(255, 0, 0);
			break;

		case(ErrorLevel_ASSERT):
			textCol = ImColor(0, 0, 255);
			break;

		default:
			textCol = ImColor(255, 255, 255);
		}

		ImGui::TextColored(textCol, I.second.c_str());
	}

	ImGui::End();
}
