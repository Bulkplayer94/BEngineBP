#pragma once

#include "ImGui/imgui.h" 
#include <utility>
#include <vector>
#include <string>

namespace BEngine {
	struct ErrorReporter {

		typedef enum ErrorLevel {
			ErrorLevel_LOW, // GREEN
			ErrorLevel_MEDIUM, // YELLOW
			ErrorLevel_HIGH, // RED
			ErrorLevel_ASSERT // MAGENTA
		};

		std::vector<std::pair<ErrorLevel, std::string>> errorList;

		void Report(ErrorLevel errLevel, std::string errMsg);
		void Draw();

	} extern errorReporter;
}