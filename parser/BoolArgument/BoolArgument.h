#pragma once
#include <vector>
#include <string>
namespace ArgumentParser {

    class Flag {
        friend class ArgParser;
    private:
        char short_name_;
        std::string long_name_;
        bool value_;
        bool has_default_value_;
        bool default_value_;

        bool is_init_;

        Flag(const std::string& long_name);

        Flag(char short_name, const std::string& long_name);

        bool IsInit();

        char GetShortName();

        std::string GetLongName();

        bool GetValue();

        void SetValue(bool value);

    public:

        void Default(bool value);
    };

}