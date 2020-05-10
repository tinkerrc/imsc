#pragma once
#ifndef IMSC_FILE_H
#define IMSC_FILE_H

#include <string>
#include <stdexcept>

class FileError : public std::runtime_error {
    public:
        FileError(const std::string& s) 
            : std::runtime_error(s.c_str()) {}
};

std::string read_file(const std::string& fname);

bool write_file(const std::string& fname, const std::string& data);

bool append_to_file(const std::string& fname, const std::string& data);

// implement later
std::string encrypt(const std::string& plaintext, const std::string& key);

std::string decrypt(const std::string& cipher, const std::string& key);

std::string read_encrypted_file(const std::string& fname, const std::string& key);

bool write_encrypted_file(
        const std::string& fname,
        const std::string& data,
        const std::string& key);

#endif
