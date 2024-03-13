#include "IntArgument.h"
using namespace ArgumentParser;

IntArgument::IntArgument(const std::string& long_name) {
    short_name_ = '\0';
    long_name_ = long_name;
    is_init_ = false;
    storage_ = nullptr;
    min_args_amount_ = 1;
    args_amount_ = 0;
    is_positional_ = false;
    is_multivalued_ = false;
}

IntArgument::IntArgument(char short_name, const std::string& long_name) {
    short_name_ = short_name;
    long_name_ = long_name;
    is_init_ = false;
    storage_ = nullptr;
    min_args_amount_ = 1;
    args_amount_ = 0;
    is_positional_ = false;
    is_multivalued_ = false;
}

bool IntArgument::IsInit() {
    return is_init_;
}

std::string IntArgument::GetLongName() {
    return long_name_;
}

char IntArgument::GetShortName() {
    return short_name_;
}

int IntArgument::GetValue(int index) {
    if (has_default_value_) {
        return default_value_;
    }
    return values_[index];
}

void IntArgument::AddValue(int value) {
    has_default_value_ = false;
    values_.push_back(value);
    if (!is_multivalued_) {
        is_init_ = true;
    }
    else {
        args_amount_++;
        if (args_amount_ >= min_args_amount_) {
            is_init_ = true;
        }
        if (storage_ != nullptr) {
            storage_->resize(values_.size());
            for (int i = 0; i < storage_->size(); i++) {
                (*storage_)[i] = values_[i];
            }
        }
    }
}

IntArgument& IntArgument::MultiValue(int min_args_amount) {
    is_multivalued_ = true;
    min_args_amount_ = min_args_amount;
    return *this;
}

void IntArgument::StoreValues(std::vector<int>& values) {
    storage_ = &values;
}

void IntArgument::Default(int value) {
    default_value_ = value;
    has_default_value_ = true;
    is_init_ = true;
    //AddValue(value);
}

IntArgument& IntArgument::Positional() {
    is_positional_ = true;
    return *this;
}

bool IntArgument::IsPositional() {
    return is_positional_;
}
