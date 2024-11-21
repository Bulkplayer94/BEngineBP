#include "pch.h"
#include "SettingsManager.h"

void BEngine::SettingsManager::registerSetting(const Setting& setting) {
    // Überprüfen, ob die Einstellung bereits existiert
    if (m_settings.find(setting.name) != m_settings.end()) {
        // Aktualisiere die vorhandene Einstellung
        m_settings[setting.name] = setting;

        // Kategorie aktualisieren, falls sie geändert wurde
        auto& currentCategory = m_categories[m_settings[setting.name].category];
        if (std::find(currentCategory.begin(), currentCategory.end(), setting.name) == currentCategory.end()) {
            currentCategory.push_back(setting.name);
        }
        return;
    }

    // Neue Einstellung hinzufügen
    m_settings[setting.name] = setting;
    m_categories[setting.category].push_back(setting.name);
}

BEngine::SettingsManager::Setting* BEngine::SettingsManager::getSetting(const std::string& name) {
    auto it = m_settings.find(name);
    return (it != m_settings.end()) ? &it->second : nullptr;
}

void BEngine::SettingsManager::drawSettingsMenu() {
    ImGui::Begin("Settings");
    for (const auto& category : m_categories) {
        if (ImGui::CollapsingHeader(category.first.c_str())) {
            for (const auto& settingName : category.second) {
                Setting& setting = m_settings[settingName];
                switch (setting.type) {
                case SLIDER_INT:
                    ImGui::SliderInt(setting.name.c_str(), &setting.sliderIntValue, setting.sliderIntMinValue, setting.sliderIntMaxValue);
                    break;
                case SLIDER_FLOAT:
                    ImGui::SliderFloat(setting.name.c_str(), &setting.sliderFloatValue, setting.sliderFloatMinValue, setting.sliderFloatMaxValue);
                    break;
                case CHECKBOX:
                    ImGui::Checkbox(setting.name.c_str(), &setting.checkboxValue);
                    break;
                case DROPDOWN: {
                    std::vector<const char*> optionsCStr;
                    for (const auto& opt : setting.dropdownOptions)
                        optionsCStr.push_back(opt.c_str());
                    ImGui::Combo(setting.name.c_str(), &setting.dropdownIndex, optionsCStr.data(), optionsCStr.size());
                    break;
                }
                case RADIO:
                    for (size_t i = 0; i < setting.dropdownOptions.size(); ++i) {
                        ImGui::RadioButton(setting.dropdownOptions[i].c_str(), &setting.radioValue, static_cast<int>(i));
                    }
                    break;
                }
            }
        }
    }
    ImGui::End();
}

void BEngine::SettingsManager::saveSettings() {
    nlohmann::json json;
    for (const auto& [name, setting] : m_settings) {
        nlohmann::json entry;
        entry["type"] = setting.type;
        entry["category"] = setting.category;
        switch (setting.type) {
        case SLIDER_INT:
            entry["value"] = setting.sliderIntValue;
            entry["maxVal"] = setting.sliderIntMaxValue;
            entry["minVal"] = setting.sliderIntMinValue;
            break;
        case SLIDER_FLOAT:
            entry["value"] = setting.sliderFloatValue;
            entry["maxVal"] = setting.sliderFloatMaxValue;
            entry["minVal"] = setting.sliderFloatMinValue;
            break;
        case CHECKBOX:
            entry["value"] = setting.checkboxValue;
            break;
        case DROPDOWN:
            entry["value"] = setting.dropdownIndex;
            entry["options"] = setting.dropdownOptions;
            break;
        case RADIO:
            entry["value"] = setting.radioValue;
            entry["options"] = setting.dropdownOptions;
            break;
        }
        json[name] = entry;
    }
    std::ofstream file(m_settingsFilePath);
    file << json.dump(4);
}

void BEngine::SettingsManager::loadSettings() {
    std::ifstream file(m_settingsFilePath);
    if (!file.is_open()) return;

    nlohmann::json json;
    file >> json;

    // Kategorien zurücksetzen
    m_categories.clear();

    for (const auto& [name, entry] : json.items()) {
        // Wenn das Setting bereits existiert, überspringe es
        if (m_settings.find(name) != m_settings.end()) continue;

        Setting setting;
        setting.name = name;
        setting.type = static_cast<SettingType>(entry["type"].get<int>());
        setting.category = entry["category"].get<std::string>();

        switch (setting.type) {
        case SLIDER_INT:
            setting.sliderIntValue = entry["value"].get<int>();
            setting.sliderIntMaxValue = entry["maxVal"].get<int>();
            setting.sliderIntMinValue = entry["minVal"].get<int>();
            break;
        case SLIDER_FLOAT:
            setting.sliderFloatValue = entry["value"].get<float>();
            setting.sliderFloatMaxValue = entry["maxVal"].get<float>();
            setting.sliderFloatMinValue = entry["minVal"].get<float>();
            break;
        case CHECKBOX:
            setting.checkboxValue = entry["value"].get<bool>();
            break;
        case DROPDOWN:
            setting.dropdownIndex = entry["value"].get<int>();
            setting.dropdownOptions = entry["options"].get<std::vector<std::string>>();
            break;
        case RADIO:
            setting.radioValue = entry["value"].get<int>();
            setting.dropdownOptions = entry["options"].get<std::vector<std::string>>();
            break;
        }

        // Setting hinzufügen
        m_settings[name] = setting;
        m_categories[setting.category].push_back(name);
    }
}

bool BEngine::SettingsManager::isSettingRegistered(const std::string& name)
{
    return m_settings.find(name) != m_settings.end();
}
