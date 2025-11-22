#include "Licence.h"

const char* contents =
    "# SKSEMenuFramework - SKSE Plugin"
    "© 2025 A. Thiago Kaqiue. All Rights Reserved."

    "Unauthorized distribution of the compiled **SKSEMenuFramework.dll** is strictly prohibited."
    "This DLL **may not be shared, resold, repackaged, or included in any other distribution without prior written "
    "consent** from the author."

    "By using this software, you agree to these terms."

    "# LEGAL NOTICE: DO NOT REMOVE THIS FILE"
    "This software **must** be distributed with this LICENSE.md."
    "Removing this file **does NOT bypass legal restrictions**."

    "Unauthorized distribution, modification, or reverse-engineering of `SKSEMenuFramework.dll`"
    "is strictly prohibited and may result in **legal action**.";


// Helper function to remove all whitespace from a string
std::string removeWhitespace(const std::string& str) {
    std::string result;
    result.reserve(str.size());
    for (char c : str) {
        if (c != '\n' && c != '\r') {
            result += c;
        }
    }
    return result;
}

bool fileContentsEqual(const std::string& filename, const std::string& expected) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        return false;
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string contents = buffer.str();
    file.close();

    // Compare without whitespace
    return removeWhitespace(contents) == removeWhitespace(expected);
}

bool Licence::Validate() {
    if (!fileContentsEqual("Data\\SKSE\\plugins\\SKSEMenuFramework.LICENSE.md", contents)) {
        logger::error("SKSEMenuFramework.LICENSE.md.md file is missing or has been modified!");
        return false;
    }
    return true;
}
