#ifndef H_FILE
#define H_FILE

#include <string>
#include <fstream>

#include <crypto++/aes.h>

using std::string;
using std::ifstream;
using std::ofstream;

class FileError : public std::runtime_error {
    public:
        FileError(const string& s) 
            : std::runtime_error(s.c_str()) {}
};

string read_file(const string& fname) {
    ifstream ifs(fname);
    if (!ifs.good()) {
        throw FileError("read_file(): error reading file");
    }

    string data = "";
    ifs >> data;
    return data;
}

bool write_file(const string& fname, const string& data) {
    ofstream ofs(fname, ofstream::out | ofstream::trunc);
    if (!ofs.good()) return false;

    ofs << data;
    ofs.flush();
    return true;
}

bool append_to_file(const string& fname, const string& data) {
    ofstream ofs(fname, ofstream::out | ofstream::app);
    if (!ofs.good()) return false;

    ofs << data;
    ofs.flush();
    return true;
}

// implement later
string encrypt(const string& plaintext, const string& key) {
    return plaintext;
}

string decrypt(const string& cipher, const string& key) {
    return cipher;
}

string read_encrypted_file(const string& fname, const string& key) {
    string cipher = read_file(fname);
    return decrypt(cipher, key);
}

bool write_encrypted_file(
        const string& fname,
        const string& data,
        const string& key) {

    string cipher = encrypt(data, key);
    return write_file(fname, cipher);
}

#endif
