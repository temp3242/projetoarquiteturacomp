#include "etapa3t1.h"

#include <filesystem>
#include <fstream>
#include <iostream>

struct regfile {
    uint32_t H, OPC, TOS, CPP, LV, SP, PC, MDR, MAR;
    uint8_t MBR;
};

struct alu_result {
    uint32_t s;
    bool c_out, n, z;
};

alu_result alu(unsigned const char opcode, const uint32_t a, const uint32_t b) {
    int c_in, c_out, s;

    const bool sll_8 = (opcode >> 7) & 0x1;
    const bool sra_1 = (opcode >> 6) & 0x1;

    const int funcao = (opcode >> 4) & 0x3;
    const bool en_a = (opcode >> 3) & 0x1;
    const bool en_b = (opcode >> 2) & 0x1;
    const bool inv_a = (opcode >> 1) & 0x1;
    const bool inc = opcode & 0x1;

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
            s = 0;
            break;
    }

    uint32_t s_d = s;

    if (sll_8) {
        // SLL8: Shift Left Logical de 8 bits. Empurra tudo para a esquerda e preenche com zeros.
        s_d = s << 8;
    } else if (sra_1) {
        // SRA1: Shift Right Arithmetic de 1 bit. Empurra para a direita, mas MANTÉM o sinal.
        s_d = static_cast<uint32_t>(static_cast<int32_t>(s) >> 1);
    }

    // A flag Z (Zero) assume nível alto (1) quando a saída é exatamente zero.
    bool Z = (s_d == 0);

    // A flag N (Negativo) assume nível alto (1) quando a saída é negativa.
    // Em complemento de 2, um número é negativo se o seu bit mais significativo (o 31º) for 1.
    bool N = (s_d >> 31) & 0x1;

    alu_result result{};
    result.c_out = c_out;
    result.s = s_d;
    result.n = N;
    result.z = Z;

    return result;
}

void write_to_regs(regfile *regs, const uint16_t c_bus_mask, const uint32_t value) {
    if (c_bus_mask & (1 << 0)) regs->H = value;
    if (c_bus_mask & (1 << 1)) regs->OPC = value;
    if (c_bus_mask & (1 << 2)) regs->TOS = value;
    if (c_bus_mask & (1 << 3)) regs->CPP = value;
    if (c_bus_mask & (1 << 4)) regs->LV = value;
    if (c_bus_mask & (1 << 5)) regs->SP = value;
    if (c_bus_mask & (1 << 6)) regs->PC = value;
    if (c_bus_mask & (1 << 7)) regs->MDR = value;
    if (c_bus_mask & (1 << 8)) regs->MAR = value;
}

void etapa3t1::run() {
    // Cria a pasta de saída, se não existir
    std::filesystem::create_directory("../out/etapa2T2");

    // Abre os arquivos de entrada e saída
    std::ifstream arq_registradores("../progs/etapa3/registradores_etapa3_tarefa1.txt");
    std::ifstream instrucoes("../progs/etapa3/programa_etapa3_tarefa1.txt");
    std::ofstream log("../out/etapa3T1/log_tarefa1.txt");

    if (!instrucoes.is_open() || !log.is_open() || !arq_registradores.is_open()) {
        std::cout << "Erro ao abrir os arquivos da Etapa 3 Tarefa 1.\n";
        return;
    }

    std::string inst_str;

    regfile regs{};

    while (getline(instrucoes, inst_str)) {
        int inst = std::stoi(inst_str);
        int ula = inst >> 15;
        int c_bus = inst >> 6 & 0x1FF;
        int mem = inst >> 4 & 0x3;
        int b_bus = inst & 0xF;

        uint32_t alu_b;

        switch (b_bus) {
            case 0:
                alu_b = regs.MDR;
                break;
            case 1:
                alu_b = regs.PC;
                break;
            case 2:
                alu_b = regs.MBR;
                break;
            case 3:
                alu_b = regs.MBR << 24;
                alu_b >>= 24;
                break;
            case 4:
                alu_b = regs.SP;
                break;
            case 5:
                alu_b = regs.LV;
                break;
            case 6:
                alu_b = regs.CPP;
                break;
            case 7:
                alu_b = regs.TOS;
                break;
            case 8:
                alu_b = regs.OPC;
                break;
            default: alu_b = 0;
        }

        alu_result alu_out = alu(ula, regs.H, alu_b);

        write_to_regs(&regs, c_bus, alu_out.s);
    }
}
