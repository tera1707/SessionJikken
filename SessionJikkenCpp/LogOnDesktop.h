#pragma once
#include <string>
#include <filesystem>
#include <mutex>
#include <iostream>

class LogOnDesktop
{
private:
    std::mutex mtx;
    std::filesystem::path AppendLogFilePathOnDesktopString(std::wstring filename);
    std::wstring GetTimeString();

public:
    void WriteLine(std::wstring txt);
};
