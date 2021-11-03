#ifndef SAFEBOARD_SCANPROCESSOR_H
#define SAFEBOARD_SCANPROCESSOR_H

#include <string>

#if __cplusplus >= 201703L
#include <filesystem>
#include <fstream>
#else
#include <dirent.h>
#endif

#include "scandata.h"

class ScanInputProcessor;
class ScanInputManProcessor;
class ScanDataProcessor;
class ScanOutputProcessor;

struct ScanExecBehavior {
    virtual bool exec() = 0;
};

class ScanProcessor : public ScanExecBehavior {
public:
    ScanProcessor(int argc, char **argv);

public:
    bool exec() override;

private:
    ScanData *_data = nullptr;

    ScanInputProcessor *_inputProcessor;
    ScanDataProcessor *_dataProcessor;
    ScanOutputProcessor *_outputProcessor;
};

class ScanInputProcessor : public ScanExecBehavior {
public:
    ScanInputProcessor(int argc, char **argv, ScanData *data);

public:
    bool exec() override;

private:
    bool manEnabled() const;

private:
    int _argc;
    char **_argv;
    ScanData *_data;
    ScanInputManProcessor *_manProcessor;
};

class ScanInputManProcessor : public ScanExecBehavior {
public:
    explicit ScanInputManProcessor(ScanData *data);

public:
    bool exec() override;

private:
    ScanData *_data;
};

class ScanDataProcessor : public ScanExecBehavior {
public:
    explicit ScanDataProcessor(ScanData *data);

public:
    bool exec() override;

private:
    void processDirs();
    void processFile(const std::string &filename) const;

private:
    std::string getExtension(const std::string &filename) const;

private:
    ScanData *_data;
};

class ScanOutputProcessor : public ScanExecBehavior {
public:
    explicit ScanOutputProcessor(ScanData *data);

public:
    bool exec() override;

private:
    std::string parseTime() const;
    void generateOutput(std::ostream &os) const;

private:
    ScanData *_data;
};

#endif //SAFEBOARD_SCANPROCESSOR_H
