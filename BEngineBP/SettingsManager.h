#pragma once

#include "ImGui/imgui.h"
#include <string>
#include <unordered_map>
#include <vector>
#include <fstream>
#include "json.hpp"

namespace BEngine {
    inline struct SettingsManager {
        enum SettingType { 
            SLIDER_INT, 
            SLIDER_FLOAT,
            CHECKBOX, 
            DROPDOWN, 
            RADIO 
        };

        struct Setting {
            SettingType type;
            std::string name;
            std::string category;
            union {
                int sliderIntValue;
                float sliderFloatValue;
                bool checkboxValue;
                int dropdownIndex; // Index im Dropdown-Menü
                int radioValue;
            };
            std::vector<std::string> dropdownOptions; // Für Dropdowns
            union {
                float sliderFloatMaxValue;
                int sliderIntMaxValue;
            };
            union {
                float sliderFloatMinValue;
                int sliderIntMinValue;
            };
        };

        std::unordered_map<std::string, Setting> m_settings;
        std::unordered_map<std::string, std::vector<std::string>> m_categories; // Kategorie -> Liste von Setting-Namen
        const std::string m_settingsFilePath = "data/settings/config.json";

        void registerSetting(const Setting& setting);

        Setting* getSetting(const std::string& name);

        void drawSettingsMenu();

        void saveSettings();

        void loadSettings();

        bool isSettingRegistered(const std::string& name);
    } settingsManager;
}