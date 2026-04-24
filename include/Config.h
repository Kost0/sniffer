#pragma once
#include <string>


struct Config {
    std::string interface;
    std::string filter;   
    int         count   = 0; 
    bool        verbose = false;

    static Config& getInstance() {
        static Config instance;
        return instance;
    }

    void parse(int argc, char** argv);

    Config(const Config&)            = delete;
    Config& operator=(const Config&) = delete;

private:
    Config() = default;
};
