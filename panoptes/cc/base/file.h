#ifndef CC_BASE_FILE_H_
#define CC_BASE_FILE_H_

class File {
  public:
    // Whether the path is a file.
    static bool IsFile(const string& file_name);

    // Read the entire file to a string.
    static bool FileToString(const string& file_name, string* text);

    // Write the entire string to a file.
    static bool StringToFile(const string& text, const string& file_name);

    // List a directory.
    static bool ListDir(const string& dir_name, vector<string>* names);
};

#endif  // CC_BASE_FILE_H_
