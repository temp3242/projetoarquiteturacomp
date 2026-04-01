#include <filesystem>
#include <iostream>

#include "config.h"
#include "etapa1/etapa1.h"
#include "etapa2T1/etapa2.h"
#include "etapa2T2/etapa2t2.h"
#include "etapa3T1/etapa3t1.h"
#include "etapa3e/etapa3e.h"


int main() {

    std::filesystem::create_directory(std::string(PROJECT_ROOT_DIR) + "/out");

    etapa1 etapa1;
    etapa2 etapa2;
    etapa2t2 etapa2t2;
    etapa3t1 etapa3t1;
    etapa3e etapa3e;

    // etapa1.run();
    // etapa2.run();
    // etapa2t2.run();
    // etapa3t1.run();
    etapa3e.run();

    return 0;
}
