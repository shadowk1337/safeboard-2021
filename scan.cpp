#include "scanprocessor.h"

int main(int argc, char **argv) {
    auto scan = new ScanProcessor(argc, argv);
    scan->exec();

    return 0;
}
