#include <filesystem>
#include <iostream>
#include "etapa1/etapa1.h"
#include "etapa2/etapa2.h"

int main() {

    std::filesystem::create_directory("../out");

    etapa1 etapa1;
    etapa2 etapa2;

    etapa1.run();
    // etapa2.run();

    return 0;
}
