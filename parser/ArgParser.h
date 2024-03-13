#pragma once
#include "StringArgument/StringArgument.h"
#include "IntArgument/IntArgument.h"
#include "BoolArgument/BoolArgument.h"
#include <vector>
#include <string>

namespace ArgumentParser {
    class StringArgument;
    class IntArgument;
    class Flag;

    class ArgParser {
    private:

        std::vector<StringArgument*> string_arguments_;
        std::vector<IntArgument*> int_arguments_;
        std::vector<Flag*> flags_;

        bool ParseStringArgument(char key, const std::string& value);

        bool ParseStringArgument(const std::string& key, const std::string& value);

        bool ParseIntArgument(char key, const std::string& string_value);

        bool ParseIntArgument(const std::string& key, const std::string& string_value);

        bool ParseFlag(char key);

        bool ParseFlag(const std::string& key);

        bool AllInitialized();

        bool AddToPositional(const std::string& string_value);

    public:

        ArgParser();

        bool Parse(std::vector<std::string> arguments);

        bool Parse(int argc, char* argv[]);

        StringArgument& AddStringArgument(const std::string& long_name);

        StringArgument& AddStringArgument(const char short_name, const std::string& long_name);

        std::string GetStringValue(char key);

        std::string GetStringValue(const std::string& key);

        IntArgument& AddIntArgument(const std::string& long_name);

        IntArgument& AddIntArgument(char short_name, const std::string& long_name);

        int GetIntValue(char key, int index = 0);

        int GetIntValue(const std::string& key, int index = 0);

        Flag& AddFlag(const std::string& long_name);

        Flag& AddFlag(char short_name, const std::string& long_name);

        bool GetFlag(char key);

        bool GetFlag(const std::string& key);

        ~ArgParser();
    };

}