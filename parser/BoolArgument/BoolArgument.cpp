#include "BoolArgument.h"

using namespace ArgumentParser;

Flag::Flag(const std::string& long_name) {
    short_name_ = '\0';
    long_name_ = long_name;
    is_init_ = false;
}

Flag::Flag(char short_name, const std::string& long_name) {
    short_name_ = short_name;
    long_name_ = long_name;
    is_init_ = false;
}

bool Flag::IsInit() {
    return is_init_;
}

char Flag::GetShortName() {
    return short_name_;
}

std::string Flag::GetLongName() {
    return long_name_;
}

bool Flag::GetValue() {
    return value_;
}

void Flag::SetValue(bool value) {
    value_ = value;
    is_init_ = true;
}

void Flag::Default(bool value) {
    SetValue(value);
}
