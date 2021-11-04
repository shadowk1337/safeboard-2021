#include "scandata.h"

ScanData::ScanData() {
    _d = new Data;
    _vd = new VarData;
}

void ScanData::setProgramName(char *name) {
    if (name == nullptr) {
        return;
    }
    _vd->programName = name;
}

void ScanData::setData(int readSize, char **argv) {
    for (int i = 0; i < readSize; ++i) {
        _vd->dirNames.insert(argv[i]);
    }
}
