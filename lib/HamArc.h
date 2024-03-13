#pragma once
#include "Buffer/Buffer.h"

#include <iostream>
#include <cstring>
#include <vector>
#include <fstream>
#include <algorithm>

class HamArc {
public:
    explicit HamArc(std::string arch_name, int inf_bits_amount);

    void CreateArchive(std::vector<std::string>& file_names);

    void ListFiles();

    void ExtractAll();

    void ExtractFiles(const std::vector<std::string>& file_names);

    void DeleteFiles(const std::vector<std::string>& file_names);

    void AppendFiles(const std::vector<std::string>& file_names);

    void Merge(const std::vector<std::string>& file_names);

private:

    std::string name_;

    int block_size_;
    int parity_bits_amount_;
    int inf_bits_amount_;

    int CountParityBits(int ibits);

    int CountEncodedLength(int data_len);

    int GetFileLength(const std::string& file_name);

    void AppendFile(std::ofstream& archive, const std::string& file_name);

    void WriteFileLength(std::ofstream& archive, uint8_t name_length, uint64_t file_length);

    void WriteFileName(std::ofstream& archive, const std::string& file_name);

    void WriteFile(std::ofstream& archive, std::ifstream& file, uint64_t file_len);

    void ExtractFile(std::ifstream& archive, File& file_header);

    void SkipFile(std::ifstream& archive, File& file_header);

    File ReadFileHeader(std::ifstream& archive);
};