#include <filesystem>
#include <iostream>
#include "etapa1/etapa1.h"
#include "etapa2T1/etapa2.h"
#include "etapa2T2/etapa2t2.h"

int main() {

    std::filesystem::create_directory("../out");

    etapa1 etapa1;
    etapa2 etapa2;
    etapa2t2 etapa2t2;

    // etapa1.run();
    // etapa2.run();
    etapa2t2.run();

    return 0;
}
