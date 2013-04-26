#include <boost/any.hpp>
#include <unordered_map>
#include <list>

#pragma once

class SettingsManager
{
public:
    SettingsManager();
    ~SettingsManager();

    void handleArgs(std::list<std::string>& args);
    
    bool        getBool(const std::string&);
    float       getFloat(const std::string&);
    int         getInt(const std::string&);
    std::string getString(const std::string&);

private:
    std::unordered_map<std::string, boost::any> mSettings;
};