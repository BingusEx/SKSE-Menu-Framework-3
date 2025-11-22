#include "Config.h"
#include "Application.h"

unsigned int Config::ToggleKey = 0x3B;
uint8_t Config::ToggleMode = 0;
unsigned int Config::ToggleKeyGamePad = 0;
uint8_t Config::ToggleModeGamePad = 0;
bool Config::FreezeTimeOnMenu = true;
MenuStyle Config::MenuStyle = MenuStyle::Skyrim;
bool Config::BlurBackgroundOnMenu = true;
std::string Config::PrimaryFont = "CN.ttf";  
bool Config::EnableChinese = true;                                
bool Config::EnableJapanese = false;
bool Config::EnableKorean = false;
bool Config::EnableCyrillic = false;
bool Config::EnableThai = false;

char* toUpperCase(const char* str) {
    if (str == nullptr) {
        return nullptr;
    }

    char* upper_str = new char[strlen(str) + 1];
    if (upper_str == nullptr) {
        return nullptr; 
    }
    for (int i = 0; str[i] != '\0'; i++) {
        upper_str[i] = std::toupper((unsigned char)str[i]);
    }
    upper_str[strlen(str)] = '\0';

    return upper_str;
}

void Config::Init() {
	const auto ini = new Utils::Ini("SKSEMenuFramework.ini");
    ini->SetSection("General");

    ToggleKey = GetKeyBinding(ini->GetString("ToggleKey", "f1"));
    ToggleMode = GetToggleMode(ini->GetString("ToggleMode", "SinglePress"));

    ToggleKeyGamePad = GetKeyBinding(ini->GetString("ToggleKeyGamePad", ""),RE::INPUT_DEVICE::kGamepad);
    ToggleModeGamePad = GetToggleMode(ini->GetString("ToggleModeGamePad", "DoublePress"));

    FreezeTimeOnMenu = ini->GetBool("FreezeTimeOnMenu", true);
    BlurBackgroundOnMenu = ini->GetBool("BlurBackgroundOnMenu", true);
    auto menuStyleStr = toUpperCase(ini->GetString("MenuStyle", "SKYRIM"));

    if (strcmp(menuStyleStr, "MODERN") == 0) {
        Config::MenuStyle = MenuStyle::Modern;
    } else if(strcmp(menuStyleStr, "DEFAULT") == 0) {
        Config::MenuStyle = MenuStyle::Classic;
    }

    ini->SetSection("Fonts");  
    PrimaryFont = ini->GetString("PrimaryFont", "MainFont.ttf");
    EnableChinese = ini->GetBool("EnableChinese", true);
    EnableJapanese = ini->GetBool("EnableJapanese", false);
    EnableKorean = ini->GetBool("EnableKorean", false);
    EnableCyrillic = ini->GetBool("EnableCyrillic", false);
    EnableThai = ini->GetBool("EnableThai", false);



    delete ini;
    delete[] menuStyleStr;
}

void Config::Save() {
    const auto ini = new Utils::Ini("SKSEMenuFramework.ini");

    // General Section
    ini->SetSection("General");
    // Note: You'll need helper functions to convert key bindings and toggle modes back to strings
    // For now, these are placeholders - implement KeyBindingToString() and ToggleModeToString()
    // ini->SetString("ToggleKey", KeyBindingToString(ToggleKey).c_str());
    // ini->SetString("ToggleMode", ToggleModeToString(ToggleMode).c_str());
    // ini->SetString("ToggleKeyGamePad", KeyBindingToString(ToggleKeyGamePad).c_str());
    // ini->SetString("ToggleModeGamePad", ToggleModeToString(ToggleModeGamePad).c_str());

    ini->SetBool("FreezeTimeOnMenu", FreezeTimeOnMenu);
    ini->SetBool("BlurBackgroundOnMenu", BlurBackgroundOnMenu);

    const char* menuStyleStr = "SKYRIM";
    if (Config::MenuStyle == MenuStyle::Modern) {
        menuStyleStr = "MODERN";
    } else if (Config::MenuStyle == MenuStyle::Classic) {
        menuStyleStr = "DEFAULT";
    }
    if (ToggleMode == 0) {
        ini->SetString("ToggleMode", "SINGLEPRESS");
    }
    else if (ToggleMode == 1) {
        ini->SetString("ToggleMode", "HOLD");
    }
    else if (ToggleMode == 2) {
        ini->SetString("ToggleMode", "DOUBLEPRESS");
    }

    if (ToggleModeGamePad == 0) {
        ini->SetString("ToggleModeGamePad", "SINGLEPRESS");
    } else if (ToggleModeGamePad == 1) {
        ini->SetString("ToggleModeGamePad", "HOLD");
    } else if (ToggleModeGamePad == 2) {
        ini->SetString("ToggleModeGamePad", "DOUBLEPRESS");
    }

    ini->SetString("ToggleKey", GetKeyName(ToggleKey, RE::INPUT_DEVICE::kKeyboard).c_str());
    ini->SetString("ToggleKeyGamePad", GetKeyName(ToggleKeyGamePad, RE::INPUT_DEVICE::kGamepad).c_str());


    ini->SetString("MenuStyle", menuStyleStr);

    // Fonts Section
    ini->SetSection("Fonts");
    ini->SetString("PrimaryFont", PrimaryFont.c_str());
    ini->SetBool("EnableChinese", EnableChinese);
    ini->SetBool("EnableJapanese", EnableJapanese);
    ini->SetBool("EnableKorean", EnableKorean);
    ini->SetBool("EnableCyrillic", EnableCyrillic);
    ini->SetBool("EnableThai", EnableThai);

    // Save to file
    if (!ini->Save()) {
        // Log error if save fails
        // logger::error("Failed to save configuration to INI file");
    }

    delete ini;
}