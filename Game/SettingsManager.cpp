#include "SettingsManager.hpp"
#include <string>

SettingsManager::SettingsManager()
{
    mSettings["Fullscreen"] = false;
    mSettings["Width"] = 800;
    mSettings["Height"] = 600;
    mSettings["Bind"] = false;

    mSettings["Music Volume"] = 50.f;
    mSettings["Sound Volume"] = 75.f;
}

SettingsManager::~SettingsManager()
{
}

void SettingsManager::handleArgs(std::list<std::string>& stack)
{
    while (!stack.empty())
    {
        std::string val = stack.back();
        stack.pop_back();

        if (val == "-f")
        {
            mSettings["Fullscreen"] = true;
        }
        else if (val == "-b")
        {
            mSettings["Bind"] = true;
        }
        else if (val == "-r")
        {
            std::string res = stack.back();
            stack.pop_back();

            size_t f = res.find('x');

            if (f != std::string::npos)
            {
                std::string width = res.substr(0, f);
                std::string height = res.substr(f+1);

                mSettings["Width"] = atoi(width.c_str());
                mSettings["Height"] = atoi(height.c_str());
            }
        }
        else if (val == "-m")
        {
            std::string vol = stack.back();
            stack.pop_back();

            mSettings["Music Volume"] = atof(vol.c_str());
        }
        else if (val == "-s")
        {
            std::string vol = stack.back();
            stack.pop_back();

            mSettings["Sound Volume"] = atof(vol.c_str());
        }
    }
}

bool SettingsManager::getBool(const std::string& n)
{
    return boost::any_cast<bool>(mSettings[n]);
}
float SettingsManager::getFloat(const std::string& n)
{
    return boost::any_cast<float>(mSettings[n]);
}
int SettingsManager::getInt(const std::string& n)
{
    return boost::any_cast<int>(mSettings[n]);
}
std::string SettingsManager::getString(const std::string& n)
{
    return boost::any_cast<std::string>(mSettings[n]);
}