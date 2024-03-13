#include "ArgParser.h"
#include <iostream>
#include <string>
using namespace ArgumentParser;

ArgParser::ArgParser() {}

StringArgument& ArgParser::AddStringArgument(const std::string& long_name) {
    StringArgument* arg = new StringArgument(long_name);
    string_arguments_.push_back(arg);

    return *arg;
}

StringArgument& ArgParser::AddStringArgument(const char short_name, const std::string& long_name) {
    StringArgument* arg = new StringArgument(short_name, long_name);
    string_arguments_.push_back(arg);

    return *arg;
}

std::string ArgParser::GetStringValue(const std::string& long_name) {
    for (StringArgument* arg : string_arguments_) {
        if (arg->GetLongName() == long_name) {

            return arg->GetValue();
        }
    }

    return "";
}

std::string ArgParser::GetStringValue(char key) {
    for (StringArgument* arg : string_arguments_) {
        if (arg->GetShortName() == key) {

            return arg->GetValue();
        }
    }

    return "Delete your windows :)\n";
}

bool ArgParser::ParseStringArgument(const std::string& key, const std::string& value) {

    for (StringArgument* arg : string_arguments_) {
        if (arg->GetLongName() == key) {
            arg->AddValue(value);

            return true;
        }
    }

    return false;
}

bool ArgParser::ParseStringArgument(char key, const std::string& value) {
    for (StringArgument* arg : string_arguments_) {
        if (arg->GetShortName() == key) {
            arg->AddValue(value);

            return true;
        }
    }

    return false;
}



IntArgument& ArgParser::AddIntArgument(const std::string& long_name) {
    IntArgument* argument = new IntArgument(long_name);
    int_arguments_.push_back(argument);

    return *argument;
}

IntArgument& ArgParser::AddIntArgument(char short_name, const std::string& long_name) {
    IntArgument* argument = new IntArgument(short_name, long_name);
    int_arguments_.push_back(argument);

    return *argument;
}


int ArgParser::GetIntValue(const std::string& long_name, int index) {
    for (IntArgument* argument : int_arguments_) {
        if (argument->GetLongName() == long_name) {

            return argument->GetValue(index);
        }
    }

    return -1;
}

int ArgParser::GetIntValue(char short_name, int index) {
    for (IntArgument* argument : int_arguments_) {
        if (argument->GetShortName() == short_name) {

            return argument->GetValue(index);
        }
    }
    return -1;
}



bool ArgParser::ParseIntArgument(const std::string& key, const std::string& string_value) {
    for (char symbol : string_value) {
        if (!isdigit(symbol)) {

            return false;
        }
    }
    int int_value = std::stoi(string_value);
    for (IntArgument* arg : int_arguments_) {
        if (arg->GetLongName() == key) {
            arg->AddValue(int_value);

            return true;
        }
    }

    return false;
}

bool ArgParser::ParseIntArgument(char key, const std::string& string_value) {
    for (char symbol : string_value) {
        if (!isdigit(symbol)) {

            return false;
        }
    }

    int int_value = std::stoi(string_value);
    for (IntArgument* arg : int_arguments_) {
        if (arg->GetShortName() == key) {
            arg->AddValue(int_value);

            return true;
        }
    }

    return false;
}



Flag& ArgParser::AddFlag(const std::string& long_name) {
    Flag* argument = new Flag(long_name);
    flags_.push_back(argument);

    return *argument;
}

Flag& ArgParser::AddFlag(char short_name, const std::string& long_name) {
    Flag* argument = new Flag(short_name, long_name);
    flags_.push_back(argument);

    return *argument;
}

bool ArgParser::GetFlag(char key) {
    for (Flag* argument : flags_) {
        if (argument->GetShortName() == key) {

            return argument->GetValue();
        }
    }

    return false;
}

bool ArgParser::GetFlag(const std::string& key) {
    for (Flag* argument : flags_) {
        if (argument->GetLongName() == key) {

            return argument->GetValue();
        }
    }

    return false;
}

bool ArgParser::ParseFlag(char short_name) {
    for (Flag* argument : flags_) {
        if (argument->GetShortName() == short_name) {
            argument->SetValue(true);

            return true;
        }
    }

    return false;
}

bool ArgParser::ParseFlag(const std::string& long_name) {

    for (Flag* argument : flags_) {
        if (argument->GetLongName() == long_name) {
            argument->SetValue(true);

            return true;
        }
    }

    return false;
}

bool ArgParser::AddToPositional(const std::string& string_value) {
    for (StringArgument* argument : string_arguments_) {
        if (argument->IsPositional()) {
            argument->AddValue(string_value);

            return true;
        }
    }

    for (char symbol : string_value) {
        if (!isdigit(symbol)) {

            return false;
        }
    }

    int int_value = std::stoi(string_value);
    for (IntArgument* argument : int_arguments_) {
        if (argument->IsPositional()) {
            argument->AddValue(int_value);

            return true;
        }
    }

    return false;
}

bool ArgParser::AllInitialized() {
    for (StringArgument* str_argument : string_arguments_) {
        if (!str_argument->IsInit()) {

            return false;
        }
    }
    for (IntArgument* int_argument : int_arguments_) {
        if (!int_argument->IsInit()) {

            return false;
        }
    }
    for (Flag* flag : flags_) {
        if (!flag->IsInit()) {

            return false;
        }
    }

    return true;
}

bool ArgParser::Parse(int argc, char* argv[]) {

    for (int i = 1; i < argc; i++) {
        std::string argument = argv[i];

        if (argument[0] == '-') {
            int eq_index = argument.find('=');

            if (argument[1] == '-') {
                std::string long_name = argument.substr(2, eq_index - 2);

                if (eq_index != std::string::npos) {

                    std::string value = argument.substr(eq_index + 1);
                    if (!ParseStringArgument(long_name, value) &&
                        !ParseIntArgument(long_name, value)) {

                        return false;
                    }
                    ParseFlag(long_name);

                }
                else {
                    if (!ParseFlag(long_name)) {

                        return false;
                    }
                }
            }
            else {
                char short_name = argument[1];
                if (!ParseFlag(short_name)) {
                    i++;
                    argument = argv[i];
                    if (!ParseIntArgument(short_name, argument) &&
                        !ParseStringArgument(short_name, argument)) {

                        return false;
                    }
                }
            }

        }
        else {
            if (!AddToPositional(argument)) {

                return false;
            }
        }
    }

    return AllInitialized();
}

bool ArgParser::Parse(std::vector<std::string> arguments) {
    for (int i = 1; i < arguments.size(); i++) {
        std::string argument = arguments[i];
        if (argument[0] == '-') {
            int eq_index = argument.find('=');

            if (argument[1] == '-') {
                std::string long_name = argument.substr(2, eq_index - 2);

                if (eq_index != std::string::npos) {

                    std::string value = argument.substr(eq_index + 1);
                    if (!ParseStringArgument(long_name, value) &&
                        !ParseIntArgument(long_name, value)) {

                        return false;
                    }
                }
                else {
                    if (!ParseFlag(long_name)) {

                        return false;
                    }
                }
            }
            else {

                char short_name = argument[1];
                if (!ParseFlag(short_name)) {
                    i++;
                    argument = arguments[i];
                    if (!ParseIntArgument(short_name, argument) &&
                        !ParseStringArgument(short_name, argument)) {

                        return false;
                    }
                }
            }

        }
        else {
            if (!AddToPositional(argument)) {

                return false;
            }
        }

    }

    return AllInitialized();
};

ArgParser::~ArgParser() {
    for (int i = 0; i < string_arguments_.size(); i++) {
        delete string_arguments_[i];
    }
    for (int i = 0; i < int_arguments_.size(); i++) {
        delete int_arguments_[i];
    }
    for (int i = 0; i < flags_.size(); i++) {
        delete flags_[i];
    }
}