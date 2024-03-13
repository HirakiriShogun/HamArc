#include "Buffer.h"

HamBuffer::HamBuffer(int size)
    : size_(size)
{
    buf_ = new char[size];
    std::fill(buf_, buf_ + size, 0);
}

HamBuffer::HamBuffer(const char* str, int size)
    : size_(size)
{
    buf_ = new char[size];
    std::memcpy(buf_, str, size);
}

int HamBuffer::GetSize() const {
    return size_;
}

void HamBuffer::ReadFromFile(std::ifstream& file) {
    Clear();
    file.read(buf_, size_);
}

void HamBuffer::ReadFromFile(std::ifstream& file, int size) {
    Clear();
    file.read(buf_, size);
}

void HamBuffer::Clear() {
    std::fill(buf_, buf_ + size_, 0);
}

void HamBuffer::WriteToFile(std::ofstream& file, int data_size) const {
    file.write(buf_, data_size);
}

void HamBuffer::EncodeTo(HamBuffer& buffer, int block_size) const {
    buffer.Clear();
    int ptr1 = 0;
    int ptr2 = 0;

    int pow = 1;
    int pos = 0;
    int buf_bit_len = this->size_ * 8;

    while (ptr1 < buf_bit_len) {
        if (pos == block_size) {
            pos = 0;
            pow = 1;
        }
        if (pos != 0 && pos != pow) {
            if (this->GetBit(ptr1) == 1) {
                buffer.SetOne(ptr2);
            }
            ptr1++;
        }
        else if (pos == pow) {
            pow *= 2;
        }

        pos++;
        ptr2++;
    }
    buffer.SetParityBits(block_size);
}

void HamBuffer::DecodeTo(HamBuffer& buffer, int block_size) {
    buffer.Clear();
    if (!CorrectErrors(block_size)) {
        exit(EXIT_FAILURE);
    }

    int ptr1 = 0;
    int ptr2 = 0;

    int pow = 1;
    int pos = 0;
    int buf_bit_len = buffer.size_ * 8;

    while (ptr2 < buf_bit_len) {

        if (ptr1 % block_size == 0) {
            pos = 0;
            pow = 1;
        }

        if (pos != 0 && pos != pow) {
            if (this->GetBit(ptr1) == 1) {
                buffer.SetOne(ptr2);
            }
            ptr2++;

        }
        else if (pos == pow) {
            pow *= 2;
        }
        ptr1++;
        pos++;
    }
}

uint8_t HamBuffer::GetFileNameLength() const {
    return static_cast<uint8_t>(buf_[0]);
}

long long HamBuffer::GetFileLength() const {
    long long file_size = 0;
    std::memcpy(&file_size, buf_ + 1, 8);

    return file_size;
}

std::string HamBuffer::GetString() const {
    return std::string(buf_, size_);
}

HamBuffer::~HamBuffer() {
    delete[] buf_;
}

int HamBuffer::GetBit(uint64_t pos) const {
    if (pos >= size_ * 8) {
        std::cerr << "GetBit index out of range\n";
        exit(EXIT_FAILURE);
    }
    char byte = buf_[pos / 8];
    if (byte & (1 << (pos % 8))) {
        return 1;
    }
    return 0;
}

void HamBuffer::SetOne(uint64_t pos) {
    if (pos >= size_ * 8) {
        std::cerr << "SetOne index out of range\n";
        exit(EXIT_FAILURE);
    }
    buf_[pos / 8] = (buf_[pos / 8] | (1 << (pos % 8)));
}

void HamBuffer::SetZero(uint64_t pos) {
    if (pos >= size_ * 8) {
        std::cerr << "SetZero index out of range\n";
        exit(EXIT_FAILURE);
    }
    buf_[pos / 8] = (buf_[pos / 8] ^ (1 << (pos % 8)));
}

void HamBuffer::InvertBit(uint64_t pos) {
    if (pos >= size_ * 8) {
        std::cerr << "InvertBit index out of range\n";
        exit(EXIT_FAILURE);
    }
    if (GetBit(pos) == 0) {
        SetOne(pos);
    }
    else {
        SetZero(pos);
    }
}

void HamBuffer::SetParityBits(int block_size) {
    for (int offset = 0; offset < size_ * 8; offset += block_size) {
        CountBlockParity(offset, block_size);
    }
}

void HamBuffer::CountBlockParity(uint64_t offset, int block_size) {
    uint64_t parity = 0;
    int common_parity = 0;

    // Счет позиций единичных контрольных битов
    for (int index = 1; index < block_size && offset + index < size_ * 8; index++) {
        if (GetBit(offset + index)) {
            parity ^= index;
            common_parity++;
        }
    }

    // Вставка позиции
    uint64_t parity_pos = 1;
    while (parity_pos < block_size) {
        if (parity_pos & parity) {
            SetOne(offset + parity_pos);
            common_parity++;
        }
        parity_pos <<= 1;
    }

    // Вставка общей четности
    if (common_parity % 2 == 1) {
        SetOne(offset);
    }
    CorrectBlock(offset, block_size);
}

bool HamBuffer::CorrectErrors(int block_size) {
    for (int offset = 0; offset < 8 * size_; offset += block_size) {
        if (!CorrectBlock(offset, block_size)) {

            return false;
        }
    }

    return true;
}

bool HamBuffer::CorrectBlock(uint64_t offset, int block_size) {

    uint64_t parity = 0;
    uint64_t parity_bit_pos = 1;
    int common_parity = 0;

    for (int index = 1; index < block_size && offset + index < size_ * 8; index++) {
        if (GetBit(offset + index)) {
            parity ^= index;
            common_parity++;
        }
    }

    if (parity == 0) {

        return true;
    }

    parity_bit_pos = 1;
    uint64_t error_pos = 0;
    while (parity_bit_pos < block_size) {
        if (parity & parity_bit_pos) {
            error_pos += parity_bit_pos;
        }
        parity_bit_pos <<= 1;
    }

    if (error_pos != 0) {
        if (common_parity % 2 != GetBit(offset)) {
            InvertBit(offset + error_pos);
            std::cout << "Error corrected!\n";
        }
        else {
            std::cerr << "Data is too corrupted!\n";

            return false;
        }
    }
    else if (common_parity % 2 != GetBit(offset + 0)) {
        InvertBit(offset + 0);
        std::cout << "Error corrected!\n";
    }

    return true;
}


File::File()
    : size_(0), name_size_(0)
{}

File::File(const std::string& name, uint64_t size)
    : name_(name), size_(size), name_size_(static_cast<uint8_t>(name.size()))
{}

std::string File::GetName() const {
    return name_;
}

uint64_t File::GetSize() const {
    return size_;
}

uint8_t File::GetNameSize() const {
    return name_size_;
}
