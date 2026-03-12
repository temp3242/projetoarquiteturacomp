#include "etapa2.h"

#include <bitset>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>

void etapa2::run() {
    std::filesystem::create_directory("../out/etapa2");
    std::ifstream instrucoes("../progs/etapa2/programa_etapa2.txt");
    std::ofstream log("../out/etapa2/log.txt");
    std::ofstream saida("../out/etapa2/saida_etapa2.txt");

    if (!instrucoes.is_open()) {
        std::cout << "Erro ao abrir o arquivo programa_etapa1.txt\n";
        return;
    }
    if (!log.is_open()) {
        std::cout << "Erro ao abrir o arquivo log.txt\n";
        return;
    }
    if (!saida.is_open()) {
        std::cout << "Erro ao abrir o arquivo saida_etapa1.txt\n";
        return;
    }

    a = 0x80000000;
    b = 1;
    PC = 1;

    std::cout << "b = " << std::bitset<32>(b) << "\n";
    std::cout << "a = " << std::bitset<32>(a) << "\n\n";
    std::cout << "Start of Program\n";
    std::cout << "============================================================\n";

    log << "b = " << std::bitset<32>(b) << "\n";
    log << "a = " << std::bitset<32>(a) << "\n\n";
    log << "Start of Program\n";
    log << "============================================================\n";

    std::string inst_str;
    while (getline(instrucoes, inst_str)) {

        int inst = std::stoi(inst_str, nullptr, 2);
        IR = inst;

        bool sll_8 = inst >> 7;
        bool sra_1 = (inst >> 6) & 0x1;
        int funcao = (inst >> 4) & 0x3;
        bool en_a  = (inst >> 3) & 0x1;
        bool en_b  = (inst >> 2) & 0x1;
        bool inv_a = (inst >> 1) & 0x1;
        bool inc   = inst & 0x1;

        uint32_t op_a = en_a ? a : 0;
        uint32_t op_b = en_b ? b : 0;
        op_a = inv_a ? ~op_a : op_a;
        c_in = inc ? 1 : 0;

        switch (funcao) {
            case 0: // AND
                s = op_a & op_b;
                c_out = 0;
                break;
            case 1: // OR
                s = op_a | op_b;
                c_out = 0;
                break;
            case 2: // NOT B
                s = ~op_b;
                c_out = 0;
                break;
            case 3: {
                uint64_t sum = static_cast<uint64_t>(op_a) + static_cast<uint64_t>(op_b) + c_in;
                s = static_cast<uint32_t>(sum);
                c_out = (sum >> 32) & 1;
                break;
            }
            default:
                c_out = 0;
                break;
        }


        auto print_state = [&](std::ostream& out, const std::string& ir_str) {
            out << "PC = " << PC << "\n";
            out << "IR = " << ir_str << "\n";
            out << "b = " << std::bitset<32>(b) << "\n";
            out << "a = " << std::bitset<32>(a) << "\n";

            out << "s = " << std::bitset<32>(s) << "\n";
            out << "co = " << c_out << "\n";
            out << "============================================================\n";
        };

        std::cout << "Cycle " << PC << "\n\n";
        print_state(std::cout, inst_str);

        log << "Cycle " << PC << "\n\n";
        print_state(log, inst_str);

        saida << s << "\n";
        PC += 1;
    }

    std::cout << "Cycle " << PC << "\n\n";
    std::cout << "PC = " << PC << "\n";
    std::cout << "> Line is empty, EOP.\n";

    log << "Cycle " << PC << "\n\n";
    log << "PC = " << PC << "\n";
    log << "> Line is empty, EOP.\n";

    instrucoes.close();
    log.close();
    saida.close();
}
