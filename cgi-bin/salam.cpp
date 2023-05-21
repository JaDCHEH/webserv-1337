#include "cgi.hpp"

char** convertMapToCharArray(const mapstring& map) {
    // Create a char** array to hold the converted data
    char** charArray = new char*[map.size() + 1]; // +1 for the terminating nullptr
    
    // Convert each key-value pair to "KEY=VALUE" format
    size_t i = 0;
    mapstring::const_iterator iter;
    for (iter = map.begin(); iter != map.end(); ++iter) {
        string envVar = iter->first + "=" + iter->second;
        
        // Allocate memory for the current environment variable
        charArray[i] = new char[envVar.size() + 1];
        
        // Copy the environment variable string to charArray
        std::strcpy(charArray[i], envVar.c_str());
        ++i;
    }
    
    // Set the last element of the array as nullptr
    charArray[map.size()] = NULL;
    return charArray;
}

int main() {
    std::map<std::string, std::string> envMap;
    envMap["VAR1"] = "value1";
    envMap["VAR2"] = "value2";
    envMap["VAR3"] = "value3";
    char** envArray = convertMapToCharArray(envMap);
    
    // Print the converted char** array
    for (size_t i = 0; envArray[i] != NULL; ++i) {
        std::cout << envArray[i] << std::endl;
    }
    // Free the allocated memory
    for (size_t i = 0; envArray[i] != NULL; ++i) {
        delete[] envArray[i];
    }
    delete[] envArray;
    
    return 0;
}