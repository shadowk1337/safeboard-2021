#include <algorithm>
#include <chrono>
#include <cstring>

#include "scanprocessor.h"

ScanProcessor::ScanProcessor(int argc, char **argv) {
    _data = new ScanData();

    _inputProcessor = new ScanInputProcessor(argc, argv, _data);
    _dataProcessor = new ScanDataProcessor(_data);
    _outputProcessor = new ScanOutputProcessor(_data);
}

bool ScanProcessor::exec() {
    if (!_inputProcessor->exec()) {
        return false;
    }
    if (!_dataProcessor->exec()) {
        return false;
    }
    if (!_outputProcessor->exec()) {
        return false;
    }

    return true;
}

ScanInputProcessor::ScanInputProcessor(int argc, char **argv, ScanData *data) : _argc(argc), _argv(argv), _data(data),
                                                                    _manProcessor(new ScanInputManProcessor(data)) {}

bool ScanInputProcessor::exec() {
    auto argIt = _argv;

    _data->setProgramName(*argIt++);

    if (_argc == 1) {
        char *tmp = nullptr;
        tmp = new char[256];
#if __cplusplus >= 201703L || defined(_WIN32)
        auto curPath = std::filesystem::current_path();
        tmp = strdup(curPath.string().c_str());
#else
        if (getcwd(tmp, 256) == nullptr) {
            std::cerr << "Can't open directory: " << *tmp << std::endl;
        }
#endif
        char **tmp2 = &tmp;
        _data->setData(1, tmp2);
    } else {
        _data->setData(_argc - 1, argIt);
    }

    if (manEnabled()) {
        _manProcessor->exec();
        return false;
    }

    return true;
}

bool ScanInputProcessor::manEnabled() const {
    return (_argc == 2 && (!strcmp(_argv[1], "-h") || !strcmp(_argv[1], "--help")));
}

ScanInputManProcessor::ScanInputManProcessor(ScanData *data) : _data(data) {}

bool ScanInputManProcessor::exec() {
    std::cout << _data->data()->man << std::endl;

    return true;
}

ScanDataProcessor::ScanDataProcessor(ScanData *data) : _data(data) {}

bool ScanDataProcessor::exec() {
    auto start = std::chrono::steady_clock::now();
    processDirs();
    auto end = std::chrono::steady_clock::now();
    _data->varData()->execTime = std::chrono::duration<double, std::milli>(end - start).count();

    return true;
}

void ScanDataProcessor::processDirs() {
#if __cplusplus >= 201703L || defined(_WIN32)
    for (const auto &dir : _data->varData()->dirNames) {
        try {
            for (const auto &file: std::filesystem::directory_iterator(dir)) {
                processFile(file.path().string());
            }
        } catch(std::filesystem::filesystem_error const &ex) {
            std::cerr << "Can't open directory: " << dir << std::endl;
        } catch(...) {
            continue;
        }
    }
#else
    for (char *dir : _data->varData()->dirNames) {
        DIR *curDir = nullptr;
        dirent *ent = nullptr;

        curDir = opendir(dir);
        if (curDir) {
            while ((ent = readdir(curDir)) != nullptr) {
                std::string filename = std::string(dir) + "/" + std::string(ent->d_name);
                if (strcmp(ent->d_name, ".") != 0 && strcmp(ent->d_name, "..") != 0) {
                    processFile(filename);
                }
            }
        } else {
            std::cerr << "Can't open directory: " << dir << std::endl;
        }
    }
#endif
}

void ScanDataProcessor::processFile(const std::string &filename) const {
    std::ifstream in(filename);
    if (in.is_open()) {
        std::string extension = getExtension(filename);
        auto evilPattern = _data->data()->evilPatterns.find(extension);

        if (evilPattern != _data->data()->evilPatterns.end()) {
            std::string tmpLine;
            while (std::getline(in, tmpLine)) {
                if (tmpLine.find(evilPattern->second) != -1) {
                    if (extension == "js") {
                        _data->varData()->JSdetected++;
                        break;
                    } else if (extension == "cmd" || extension == "bat") {
                        _data->varData()->CMDdetected++;
                        break;
                    } else if (extension == "exe" || extension == "dll") {
                        _data->varData()->EXEdetected++;
                        break;
                    }
                }
            }
        }
        in.close();
    } else {
        _data->varData()->errors++;
    }
    _data->varData()->fileCount++;
}

std::string ScanDataProcessor::getExtension(const std::string &filename) const {
    std::string extension = filename.substr(filename.find_last_of('.') + 1);
    std::transform(extension.begin(), extension.end(), extension.begin(), [](unsigned char c) {
        return std::tolower(c); });

    return extension;
}

ScanOutputProcessor::ScanOutputProcessor(ScanData *data) : _data(data) {}

bool ScanOutputProcessor::exec() {
    generateOutput(std::cout);

    return true;
}

std::string ScanOutputProcessor::parseTime() const {
    uint64_t totalTime = _data->varData()->execTime;
    uint64_t minutes = totalTime / 60000;
    totalTime %= 60000;
    uint64_t seconds = totalTime / 1000;
    uint64_t milliseconds = totalTime % 1000;

    std::string minutesS = (minutes < 10 ? "0" : "") + std::to_string(minutes);
    std::string secondsS = (seconds < 10 ? "0" : "") + std::to_string(seconds);
    std::string millisecondsS = (milliseconds < 10 ? milliseconds < 100 ? "00" : "0" : "") +
            std::to_string(milliseconds);

    return minutesS + ":" + secondsS + ":" + millisecondsS;
}

void ScanOutputProcessor::generateOutput(std::ostream &os) const {
    os << "\n====== Scan result ======\n";
    os << "Processed files: "   << _data->varData()->fileCount     << '\n';
    os << "Js detects: "        << _data->varData()->JSdetected    << '\n';
    os << "CMD detects: "       << _data->varData()->CMDdetected   << '\n';
    os << "EXE detects: "       << _data->varData()->EXEdetected   << '\n';
    os << "Errors: "            << _data->varData()->errors        << '\n';
    os << "Execution time: "    << parseTime() << '\n';
    os << "=========================\n";
}
