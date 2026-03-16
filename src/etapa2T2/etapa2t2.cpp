#include "etapa2t2.h"
#include <iostream>
#include <fstream>
#include <string>
#include <bitset>
#include <iomanip>
#include <filesystem>
#include <vector>

void etapa2t2::run() {
    // Cria a pasta de saída, se não existir
    std::filesystem::create_directory("../out/etapa2T2");

    // Abre os arquivos de entrada e saída
    std::ifstream arq_registradores("../progs/etapa2/registradores_etapa2_tarefa2.txt");
    std::ifstream instrucoes("../progs/etapa2/programa_etapa2_tarefa2.txt");
    std::ofstream log("../out/etapa2T2/log_tarefa2.txt");

    if (!instrucoes.is_open() || !log.is_open() || !arq_registradores.is_open()) {
        std::cout << "Erro ao abrir os arquivos da Etapa 2 Tarefa 2.\n";
        return;
    }

    // Valores iniciais baseados no exemplo
    H   = 1;
    OPC = 0;
    TOS = 2;
    CPP = 0;
    LV  = 0;
    SP  = 0;
    PC  = 0;
    MDR = 0;
    MAR = 0;
    MBR = static_cast<int8_t>(0b10000001); 

    auto print_registradores = [&](std::ostream& out) {
        out << "=====================================================\n";
        out << "> Initial register states\n";
        out << "mar = " << std::bitset<32>(MAR) << "\n";
        out << "mdr = " << std::bitset<32>(MDR) << "\n";
        out << "pc = " << std::bitset<32>(PC) << "\n";
        out << "mbr = " << std::bitset<8>(MBR) << "\n";
        out << "sp = " << std::bitset<32>(SP) << "\n";
        out << "lv = " << std::bitset<32>(LV) << "\n";
        out << "cpp = " << std::bitset<32>(CPP) << "\n";
        out << "tos = " << std::bitset<32>(TOS) << "\n";
        out << "opc = " << std::bitset<32>(OPC) << "\n";
        out << "h = " << std::bitset<32>(H) << "\n";
        out << "=====================================================\n";
    };

    // Chamando a função para imprimir na tela e no log
    print_registradores(std::cout);
    print_registradores(log);

    std::cout << "Start of program\n";
    std::cout << "=====================================================\n";
    log << "Start of program\n";
    log << "=====================================================\n";

    std::string inst_str;


    instrucoes.close();
    arq_registradores.close();
    log.close();
}