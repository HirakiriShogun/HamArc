#pragma once
#include <iostream>
#include <vector>
#include <cstring>

namespace ArgumentParser {

    class StringArgument {
    public:
        friend class ArgParser;

        void Default(const std::string& value);

        StringArgument& Positional();

        void StoreValue(std::string& storage);

        void StoreValues(std::vector<std::string>& storage);

        StringArgument& MultiValue(int min_args_amount = 1);

    private:
        char short_name_;
        std::string long_name_;
        std::vector<std::string> values_;

        std::string* storage_;
        std::vector<std::string>* long_storage_;

        int min_args_amount_;
        int args_amount_;
        std::string default_value_;

        bool is_init_;
        bool is_positional_;
        bool has_default_value_;
        bool is_multivalued_;

        StringArgument(const std::string& long_name);

        StringArgument(const char short_name, const std::string& long_name);

        std::string GetLongName();

        char GetShortName();

        std::string GetValue(int index = 0);

        void AddValue(const std::string& value);

        bool IsPositional();

        bool IsInit();
    };

}