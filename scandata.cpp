#include "scandata.h"

ScanData::ScanData() {
    _d = new Data;
    _vd = new VarData;
}

bool ScanData::setProgramName(char *name) {
    if (name == nullptr) {
        return false;
    }
    _vd->programName = name;

    return true;
}

bool ScanData::setData(char **argv) {
  while (*argv) {
      if (*argv == nullptr) {
          return false;
      }
      _vd->dirNames.insert(*argv++);
  }

  return true;
}
