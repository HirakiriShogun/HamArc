#include "../lib/HamArc.h"
#include "../parser/ArgParser.h"


int main(int argc, char* argv[]) {
    ArgumentParser::ArgParser parser;

    std::string archive_name;
    std::vector <std::string> file_names;

    parser.AddStringArgument('f', "file").StoreValue(archive_name);
    parser.AddStringArgument("filename").MultiValue(0).Positional().StoreValues(file_names);
    parser.AddIntArgument('b', "block").Default(11);

    parser.AddFlag('c', "create").Default(false);
    parser.AddFlag('l', "list").Default(false);
    parser.AddFlag('x', "extract").Default(false);
    parser.AddFlag('d', "delete").Default(false);
    parser.AddFlag('a', "append").Default(false);
    parser.AddFlag('m', "merge").Default(false);

    if (!parser.Parse(argc, argv)) {
        std::cerr << "Incorrect input\n";
        std::cout << "Archive name: " << archive_name;
        exit(EXIT_FAILURE);
    }

    HamArc archer(parser.GetStringValue("file"), parser.GetIntValue("block"));

    if (parser.GetFlag("create")) {
        std::cout << "Creating, wait please!\n";
        archer.CreateArchive(file_names);
    }
    else if (parser.GetFlag("list")) {
        std::cout << "Files in " << parser.GetStringValue("file") << ":\n";
        archer.ListFiles();
    }
    else if (parser.GetFlag("extract")) {
        std::cout << "Extracting, wait please!\n";
        if (file_names.empty()) {
            archer.ExtractAll();
        }
        else {
            archer.ExtractFiles(file_names);
        }
    }
    else if (parser.GetFlag("delete")) {
        std::cout << "Deleting, wait please!\n";
        archer.DeleteFiles(file_names);
    }
    else if (parser.GetFlag("append")) {
        std::cout << "Appending, wait please!\n";
        archer.AppendFiles(file_names);
    }
    else if (parser.GetFlag("merge")) {
        std::cout << "Merging, wait please!";
        archer.Merge(file_names);
    }
}