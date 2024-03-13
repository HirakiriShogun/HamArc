#include "StringArgument.h"
#include <iostream>
using namespace ArgumentParser;

StringArgument::StringArgument(const std::string& long_name) {
    short_name_ = '\0';
    long_name_ = long_name;
    storage_ = nullptr;
    long_storage_ = nullptr;
    is_multivalued_ = false;
    is_positional_ = false;
    is_init_ = false;
    has_default_value_ = false;
}

StringArgument::StringArgument(const char short_name, const std::string& long_name) {
    short_name_ = short_name;
    long_name_ = long_name;
    storage_ = nullptr;
    long_storage_ = nullptr;
    is_init_ = false;
    is_positional_ = false;
    is_multivalued_ = false;
    has_default_value_ = false;
}

std::string StringArgument::GetLongName() {

    return long_name_;
}

char StringArgument::GetShortName() {

    return short_name_;
}

std::string StringArgument::GetValue(int index) {

    return values_[index];
}

bool ArgumentParser::StringArgument::IsPositional()
{
    return is_positional_;
}

void StringArgument::AddValue(const std::string& value) {
    values_.emplace_back(value);
    if (is_multivalued_) {
        if (long_storage_ != nullptr) {
            long_storage_->emplace_back(value);
        }
        if (values_.size() >= min_args_amount_) {
            is_init_ = true;
        }
    }
    else {
        if (storage_ != nullptr) {
            *storage_ = value;
        }
        is_init_ = true;
    }
}

bool StringArgument::IsInit() {
    return is_init_;
}

void StringArgument::Default(const std::string& value) {
    has_default_value_ = true;
    default_value_ = value;
    AddValue(value);
}

ArgumentParser::StringArgument& ArgumentParser::StringArgument::Positional() {
    is_positional_ = true;
    return *this;
}

void StringArgument::StoreValue(std::string& storage) {
    storage_ = &storage;
}

void StringArgument::StoreValues(std::vector<std::string>& storage) {
    long_storage_ = &storage;
}

StringArgument& StringArgument::MultiValue(int min_args_amount) {
    is_multivalued_ = true;
    min_args_amount_ = min_args_amount;
    if (min_args_amount == 0) {
        is_init_ = true;
    }
    return *this;
}