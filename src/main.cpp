#include <iostream>



#include "nlohmann/json.hpp"

int main()
{
    
    std::cout << "Done\n";
    try {
        // Create a JSON object
        nlohmann::json j;
        j["name"] = "John";
        j["age"] = 30;
        j["city"] = "New York";

        // Print the JSON object
        std::cout << "JSON object: " << j.dump(4) << std::endl;

        // Check if a value exists and access it
        if (j.contains("name")) {
            std::cout << "Name: " << j["name"] << std::endl;
        }

        if (j.contains("age")) {
            std::cout << "Age: " << j["age"] << std::endl;
        }
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}