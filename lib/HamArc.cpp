#include "HamArc.h"

HamArc::HamArc(std::string arch_name, int inf_bits_amount)
    : name_(arch_name + ".haf"), inf_bits_amount_(inf_bits_amount)
{
    parity_bits_amount_ = CountParityBits(inf_bits_amount);
    block_size_ = inf_bits_amount + parity_bits_amount_;
}

void HamArc::CreateArchive(std::vector<std::string>& file_names) {
    std::ofstream archive;
    archive.open(name_, std::ios::out | std::ios::binary | std::ios::trunc);
    for (const std::string& file_name : file_names) {
        AppendFile(archive, file_name);
    }
    archive.close();
}

void HamArc::ListFiles() {
    uint64_t archive_size = GetFileLength(name_);
    if (archive_size == 0) {
        return;
    }

    std::ifstream archive;
    archive.open(name_, std::ios::binary | std::ios::in);
    if (!archive.is_open()) {
        std::cerr << "Can not find archive!\n";
        exit(EXIT_FAILURE);
    }

    File file_header;
    while (archive.tellg() != archive_size) {
        file_header = ReadFileHeader(archive);
        std::cout << file_header.GetName() << '\n';
        SkipFile(archive, file_header);
    }
    archive.close();
}

void HamArc::ExtractAll() {
    uint64_t archive_size = GetFileLength(name_);
    if (archive_size == 0) {
        return;
    }

    std::ifstream archive;
    archive.open(name_, std::ios::binary | std::ios::in);
    if (!archive.is_open()) {
        std::cerr << "Can not find archive!\n";
        exit(EXIT_FAILURE);
    }

    File file_header;
    while (archive.tellg() != archive_size) {
        file_header = ReadFileHeader(archive);
        ExtractFile(archive, file_header);
    }
    archive.close();
}

void HamArc::ExtractFiles(const std::vector<std::string>& file_names) {
    uint64_t archive_size = GetFileLength(name_);
    if (archive_size == 0) {
        return;
    }

    std::ifstream archive;
    archive.open(name_, std::ios::binary | std::ios::in);
    if (!archive.is_open()) {
        std::cerr << "Can not find archive!\n";
        exit(EXIT_FAILURE);
    }

    File file_header;
    while (archive.tellg() != archive_size) {
        file_header = ReadFileHeader(archive);
        if (std::find(file_names.begin(), file_names.end(), file_header.GetName()) != file_names.end()) {
            ExtractFile(archive, file_header);
        }
        else {
            SkipFile(archive, file_header);
        }
    }
    archive.close();
}

void HamArc::DeleteFiles(const std::vector<std::string>& file_names) {
    uint64_t old_archive_size = GetFileLength(name_);

    std::ifstream old_archive;
    old_archive.open(name_, std::ios::in | std::ios::binary);
    if (!old_archive.is_open()) {
        std::cerr << "Can not find archive!\n";
        exit(EXIT_FAILURE);
    }

    std::ofstream temp;
    temp.open("temporary_archive_for_deleting.tmphaf", std::ios::out | std::ios::binary);

    File file_header;
    while (old_archive.tellg() != old_archive_size) {
        file_header = ReadFileHeader(old_archive);
        if (std::find(file_names.begin(), file_names.end(), file_header.GetName()) == file_names.end()) {
            uint64_t file_size = CountEncodedLength(file_header.GetSize());

            WriteFileLength(temp, file_header.GetNameSize(), file_header.GetSize());
            WriteFileName(temp, file_header.GetName());

            const int kBufSize = 256;
            HamBuffer buf(kBufSize);
            for (int i = 0; i < file_size / kBufSize; i++) {
                buf.ReadFromFile(old_archive);
                buf.WriteToFile(temp, kBufSize);
            }
            buf.ReadFromFile(old_archive, file_size % kBufSize);
            buf.WriteToFile(temp, file_size % kBufSize);

        }
        else {
            SkipFile(old_archive, file_header);
        }
    }
    old_archive.close();
    temp.close();
    std::remove(name_.c_str());
    std::rename("temporary_archive_for_deleting.tmphaf", name_.c_str());
}

int HamArc::CountParityBits(int inf_bits) {
    int parity_bits = 0;
    int pow = 1;
    while (pow - parity_bits < inf_bits + 1) {
        parity_bits++;
        pow *= 2;
    }

    return parity_bits + 1;
}

int HamArc::CountEncodedLength(int data_len) {
    int data_bit_len = data_len * 8;

    int encoded_bit_len = data_bit_len / inf_bits_amount_ * block_size_
        + data_bit_len % inf_bits_amount_ + CountParityBits(data_bit_len % inf_bits_amount_);
    int encoded_byte_len = (encoded_bit_len + 7) / 8;

    return encoded_byte_len;
}

int HamArc::GetFileLength(const std::string& file_name) {
    std::ifstream file;
    file.open(file_name, std::ios::in | std::ios::binary);
    file.seekg(0, std::ios::end);
    int file_length = file.tellg();

    return file_length;
}

void HamArc::WriteFileLength(std::ofstream& archive, uint8_t name_length, uint64_t file_length) {
    char name_buf[9];
    name_buf[0] = name_length;
    std::memcpy(name_buf + 1, &file_length, 8);

    int encoded_data_len = CountEncodedLength(9);

    HamBuffer buf1(name_buf, 9);
    HamBuffer buf2(encoded_data_len);

    buf1.EncodeTo(buf2, block_size_);
    buf2.WriteToFile(archive, encoded_data_len);
}

void HamArc::WriteFileName(std::ofstream& archive, const std::string& file_name) {
    int length = file_name.size();
    int encoded_data_length = CountEncodedLength(length);

    HamBuffer buf1(file_name.c_str(), length);
    HamBuffer buf2(CountEncodedLength(length));

    buf1.EncodeTo(buf2, block_size_);
    buf2.WriteToFile(archive, encoded_data_length);
}

void HamArc::WriteFile(std::ofstream& archive, std::ifstream& file, uint64_t file_len) {

    HamBuffer buf1(inf_bits_amount_);
    HamBuffer buf2(block_size_);

    for (int i = 0; i < (file_len / inf_bits_amount_); i++) {
        buf1.ReadFromFile(file);
        buf1.EncodeTo(buf2, block_size_);
        buf2.WriteToFile(archive, block_size_);
    }

    buf1.ReadFromFile(file, file_len % inf_bits_amount_);
    buf1.EncodeTo(buf2, block_size_);
    buf2.WriteToFile(archive, CountEncodedLength(file_len % inf_bits_amount_));
}

void HamArc::ExtractFile(std::ifstream& archive, File& file_header) {
    std::ofstream fout;
    fout.open(file_header.GetName(), std::ios::out | std::ios::binary);
    if (!fout.is_open()) {
        std::cerr << "Can`t open file for extracting\n";
        exit(EXIT_FAILURE);
    }

    HamBuffer buf1(block_size_);
    HamBuffer buf2(inf_bits_amount_);

    for (int i = 0; i < (file_header.GetSize() / inf_bits_amount_); i++) {
        buf1.ReadFromFile(archive);
        buf1.DecodeTo(buf2, block_size_);

        buf2.WriteToFile(fout, inf_bits_amount_);
        buf2.Clear();
    }


    if (file_header.GetSize() % inf_bits_amount_ != 0) {
        buf1.ReadFromFile(archive, CountEncodedLength(file_header.GetSize() % inf_bits_amount_));
        buf1.DecodeTo(buf2, block_size_);

        buf2.WriteToFile(fout, file_header.GetSize() % inf_bits_amount_);
        buf2.Clear();
    }

    fout.close();
}

void HamArc::SkipFile(std::ifstream& archive, File& file_header) {
    archive.seekg(CountEncodedLength(file_header.GetSize()), std::ios::cur);
}

File HamArc::ReadFileHeader(std::ifstream& archive) {
    HamBuffer len_buf1(CountEncodedLength(9));
    len_buf1.Clear();
    HamBuffer len_buf2(9);
    len_buf2.Clear();

    len_buf1.ReadFromFile(archive);
    len_buf1.DecodeTo(len_buf2, block_size_);

    uint8_t name_length = len_buf2.GetFileNameLength();
    uint64_t file_length = len_buf2.GetFileLength();

    HamBuffer name_buf1(CountEncodedLength(name_length));
    name_buf1.Clear();
    HamBuffer name_buf2(name_length);
    name_buf2.Clear();

    name_buf1.ReadFromFile(archive);
    name_buf1.DecodeTo(name_buf2, block_size_);
    return File(name_buf2.GetString(), file_length);
}

void HamArc::AppendFile(std::ofstream& archive, const std::string& file_name) {
    std::ifstream file;
    file.open(file_name, std::ios::in | std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Error: can`t open file for reading\n";
        exit(EXIT_FAILURE);
    }
    int file_len = GetFileLength(file_name);

    WriteFileLength(archive, file_name.size(), file_len);
    WriteFileName(archive, file_name);
    WriteFile(archive, file, file_len);

    file.close();
}

void HamArc::AppendFiles(const std::vector<std::string>& file_names) {
    std::ofstream archive;
    archive.open(name_, std::ios::app | std::ios::binary);
    for (std::string file_name : file_names) {
        AppendFile(archive, file_name);
    }
    archive.close();
}

void HamArc::Merge(const std::vector<std::string>& file_names) {
    std::ofstream archive;
    archive.open(name_, std::ios::out | std::ios::binary);

    for (const std::string& file_name : file_names) {
        uint64_t file_size = GetFileLength(file_name);

        std::ifstream file;
        file.open(file_name, std::ios::in | std::ios::binary);
        if (!file.is_open()) {
            std::cerr << "Can not open file while merging!\n";
            exit(EXIT_FAILURE);
        }
        const int kBufSize = 256;
        HamBuffer buf(kBufSize);
        for (int i = 0; i < file_size / kBufSize; i++) {
            buf.ReadFromFile(file);
            buf.WriteToFile(archive, kBufSize);
        }
        buf.ReadFromFile(file, file_size % kBufSize);
        buf.WriteToFile(archive, file_size % kBufSize);

        file.close();
    }
    archive.close();
}
