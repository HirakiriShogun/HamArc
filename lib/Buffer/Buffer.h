#pragma once
#include <fstream>
#include <iostream>
#include <cstdint>
#include <cstring>

class File {
public:
    File();

    File(const std::string& name, uint64_t size);

    std::string GetName() const;

    uint64_t GetSize() const;

    uint8_t GetNameSize() const;

private:
    std::string name_;
    uint64_t name_size_;
    uint64_t size_;
};

class HamBuffer {
public:
    explicit HamBuffer(int size);

    explicit HamBuffer(const char* buf, int size);

    int GetSize() const;

    void Clear();

    void ReadFromFile(std::ifstream& file);

    void ReadFromFile(std::ifstream& file, int size);

    void WriteToFile(std::ofstream& file, int data_size) const;

    void EncodeTo(HamBuffer& buffer, int block_size) const;

    void DecodeTo(HamBuffer& buffer, int block_size);

    uint8_t GetFileNameLength() const;

    long long GetFileLength() const;

    std::string GetString() const;

    ~HamBuffer();

private:
    char* buf_;
    int size_;

    int GetBit(uint64_t pos) const;

    void SetOne(uint64_t pos);

    void SetZero(uint64_t pos);

    void InvertBit(uint64_t pos);

    void SetParityBits(int block_size);

    void CountBlockParity(uint64_t offset, int block_size);

    bool CorrectErrors(int block_size);

    bool CorrectBlock(uint64_t offset, int block_size);
};