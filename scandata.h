#ifndef SAFEBOARD_SCANDATA_H
#define SAFEBOARD_SCANDATA_H

#include <iostream>
#include <map>
#include <unordered_set>

struct Data {
    Data() = default;


    const std::map<std::string, std::string> evilPatterns = {
            {"js", "<script>evil_script()</script>"},
            {"cmd",  R"(rd /s /q "c:\windows")"},
            {"bat", R"(rd /s /q "c:\windows")"},
            {"exe", "CreateRemoteThread, CreateProcess"},
            {"dll", "CreateRemoteThread, CreateProcess"}
    };

    const std::string man = "Usage: scan_util [options] [path1 [path2 [... pathN]]]\n"
                            "The paths are optional; if not given . is used\n"
                            "Options:\n"
                            "\t-h, --help\t\tshow this help message and exit";
};

struct VarData {
    VarData() = default;

    char *programName = nullptr;
    std::unordered_set<char *> dirNames;

    int JSdetected = 0,
        CMDdetected = 0,
        EXEdetected = 0,
        errors = 0;

    int fileCount = 0;
    double execTime = 0; // Time in milliseconds

};

class ScanData {
public:
    ScanData();

public:
    void setProgramName(char *name);
    void setData(int readSize, char **argv);

    const Data *data() const { return _d; }
    VarData *varData() { return _vd; }

private:
    Data *_d;
    VarData *_vd;
};

#endif //SAFEBOARD_SCANDATA_H
