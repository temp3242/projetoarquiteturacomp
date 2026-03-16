#include "etapa2.h"

#include <bitset>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>

void etapa2::run()
{
    std::filesystem::create_directory("../out/etapa2");
    std::ifstream instrucoes("../progs/etapa2/programa_etapa2_tarefa1.txt");
    std::ofstream log("../out/etapa2/log_tarefa1.txt");
    std::ofstream saida("../out/etapa2/saida_etapa2_tarefa1.txt");

    if (!instrucoes.is_open())
    {
        std::cout << "Erro ao abrir o arquivo programa_etapa1.txt\n";
        return;
    }
    if (!log.is_open())
    {
        std::cout << "Erro ao abrir o arquivo log.txt\n";
        return;
    }
    if (!saida.is_open())
    {
        std::cout << "Erro ao abrir o arquivo saida_etapa1.txt\n";
        return;
    }

    a = 1;
    b = 0x80000000;
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
    while (getline(instrucoes, inst_str))
    {

        int inst = std::stoi(inst_str, nullptr, 2);
        IR = inst;

        // Extraição dos bits 7 e 6 para os deslocadores
        bool sll_8 = (inst >> 7) & 0x1;
        bool sra_1 = (inst >> 6) & 0x1;

        // Verifica se o SLL8 e SRA1 têm nivel alto ao mesmo tempo
        if(sll_8 && sra_1){
            auto print_error = [&](std::ostream& out) {
                out << "Cycle " << PC << "\n\n";
                out << "PC = " << PC << "\n";
                out << "IR = " << inst_str << "\n";
                out << "> Error, invalid control signals.\n";
                out << "============================================================\n";
            };

            print_error(std::cout);
            print_error(log);
            
            PC += 1;
            continue;
        }

        int funcao = (inst >> 4) & 0x3;
        bool en_a = (inst >> 3) & 0x1;
        bool en_b = (inst >> 2) & 0x1;
        bool inv_a = (inst >> 1) & 0x1;
        bool inc = inst & 0x1;

        uint32_t op_a = en_a ? a : 0;
        uint32_t op_b = en_b ? b : 0;
        op_a = inv_a ? ~op_a : op_a;
        c_in = inc ? 1 : 0;

        switch (funcao)
        {
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
        case 3:
        {
            uint64_t sum = static_cast<uint64_t>(op_a) + static_cast<uint64_t>(op_b) + c_in;
            s = static_cast<uint32_t>(sum);
            c_out = (sum >> 32) & 1;
            break;
        }
        default:
            c_out = 0;
            break;
        }
    
        uint32_t s_d = s; 

        if (sll_8)
        {
            // SLL8: Shift Left Logical de 8 bits. Empurra tudo para a esquerda e preenche com zeros.
            s_d = s << 8;
        }
        else if (sra_1)
        {
            // SRA1: Shift Right Arithmetic de 1 bit. Empurra para a direita, mas MANTÉM o sinal.
            s_d = static_cast<uint32_t>(static_cast<int32_t>(s) >> 1);
        }

        // A flag Z (Zero) assume nível alto (1) quando a saída é exatamente zero.
        bool Z = (s_d == 0) ? 1 : 0;

        // A flag N (Negativo) assume nível alto (1) quando a saída é negativa.
        // Em complemento de 2, um número é negativo se o seu bit mais significativo (o 31º) for 1.
        bool N = (s_d >> 31) & 0x1;

            auto print_state = [&](std::ostream &out, const std::string &ir_str)
            {
                out << "PC = " << PC << "\n";
                out << "IR = " << ir_str << "\n";
                out << "b = " << std::bitset<32>(b) << "\n";
                out << "a = " << std::bitset<32>(a) << "\n";

                out << "s = " << std::bitset<32>(s) << "\n";
                out << "sd = " << std::bitset<32>(s_d) << "\n";
                out << "n = " << N << "\n";
                out << "z = " << Z << "\n";
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
