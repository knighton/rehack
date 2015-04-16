#include "file.h"

#include <cstdio>

bool File::IsFile(const string& file_name) {
    FILE* f = fopen(file_name.c_str(), "rb");
    if (!f) {
        return false;
    }

    fclose(f);
    return true;
}

bool File::FileToString(const string& file_name, string* text) {
    FILE* f = fopen(file_name.c_str(), "rb");
    if (!f) {
        return false;
    }

    fseek(f, 0, SEEK_END);
    size_t size = (size_t)ftell(f);
    rewind(f);

    char* buf = new char[size];
    if (!buf) {
        return false;
    }

    if (fread(buf, sizeof(*buf), size, f) != size) {
        delete buf;
        fclose(f);
        return false;
    }

    text->assign(buf, size);
    delete buf;
    fclose(f);
    return true;
}

bool File::StringToFile(const string& text, const string& file_name) {
    FILE* f = fopen(file_name.c_str(), "wb");
    if (!f) {
        return false;
    }

    if (fwrite(text.data(), 1, text.size(), f) != text.size()) {
        return false;
    }

    fclose(f);
    return true;
}

bool File::ListDir(const string& dir_name, vector<string>* names) {
    DIR* dir = opendir(dir_name.c_str());
    if (!dir) {
        return false;
    }

    dirent* ent;
    while ((ent = readdir(dir))) {
        names->emplace_back(ent->d_name);
    }
    closedir(dir);
    return true;
}
