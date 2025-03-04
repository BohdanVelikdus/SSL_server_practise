#pragma once

#include<fstream>
#include<filesystem>
#include<string>
#include<optional>
#include<iostream>

inline bool isValidFile(std::string path)
{
    std::filesystem::path path_ = std::filesystem::absolute(path);
    if(std::filesystem::exists(path) && std::filesystem::is_regular_file(path_))
    {
        return true;
    }
    return false;
}

inline std::optional<std::string> GetInputFromConsoleString()
{
    std::string input;
    std::cin >> input;
    if(std::cin.eof())
        return std::nullopt;
    return input;
}