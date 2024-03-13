#pragma once
#include <vector>
#include <string>

namespace ArgumentParser {

    class IntArgument {
    public:
        friend class ArgParser;

        void Default(int value);

        IntArgument& MultiValue(int min_args_amount = 1);

        IntArgument& Positional();

        void StoreValues(std::vector<int>& values);

    private:
        char short_name_;
        std::string long_name_;

        std::vector<int> values_;

        int min_args_amount_;
        int args_amount_;
        int values_amount_;
        int default_value_;

        std::vector<int>* storage_;

        bool is_init_;
        bool is_positional_;
        bool has_default_value_;
        bool is_multivalued_;

        std::string GetLongName();

        char GetShortName();

        int GetValue(int index = 0);

        void AddValue(int value);

        IntArgument(const std::string& long_name);

        IntArgument(char short_name, const std::string& long_name);

        bool IsInit();

        bool IsPositional();
    };

}