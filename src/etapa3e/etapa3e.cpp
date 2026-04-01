#include "etapa3e.h"
#include "../config.h"

#include <filesystem>
#include <fstream>
#include <iostream>
#include <vector>
#include <bitset>
#include <vector>
#include <sstream>

struct regfile {
    uint32_t H, OPC, TOS, CPP, LV, SP, PC, MDR, MAR;
    uint8_t MBR;
};

struct alu_result {
    uint32_t s;
    bool c_out, n, z;
};

void print_memory_3e(std::ostream& out, const std::vector<uint32_t>& mem){
    out << "*******************************\n";
    for (uint32_t val : mem){
        out << std::bitset<32>(val) << "\n";
    }    std::ofstream log(std::string(PROJECT_ROOT_DIR) + "/out/etapa3T1/log_tarefa1.txt");
    out << "*******************************\n";
}

void print_regs_3e(std::ostream& out, const regfile& regs){
    out << "*******************************\n";
    out << "mar = " << std::bitset<32>(regs.MAR) << "\n";
    out << "mdr = " << std::bitset<32>(regs.MDR) << "\n";
    out << "pc = " << std::bitset<32>(regs.PC) << "\n";
    out << "mbr = " << std::bitset<8>(regs.MBR) << "\n";
    out << "sp = " << std::bitset<32>(regs.SP) << "\n";
    out << "lv = " << std::bitset<32>(regs.LV) << "\n";
    out << "cpp = " << std::bitset<32>(regs.CPP) << "\n";
    out << "tos = " << std::bitset<32>(regs.TOS) << "\n";
    out << "opc = " << std::bitset<32>(regs.OPC) << "\n";
    out << "h = " << std::bitset<32>(regs.H) << "\n";
}

std::vector<std::string> split_3e(const std::string& str, const std::string& delimiter) {
    std::vector<std::string> tokens;
    size_t start = 0;
    size_t end = str.find(delimiter);

    // Loop through the string, finding each delimiter
    while (end != std::string::npos) {
        // Extract the token and add it to our vector
        tokens.push_back(str.substr(start, end - start));
        
        // Move the start position past the delimiter
        start = end + delimiter.length();
        
        // Find the next occurrence of the delimiter
        end = str.find(delimiter, start);
    }
    
    // Don't forget to grab the last piece of the string!
    tokens.push_back(str.substr(start));

    return tokens;
}

// ALU:
// OUT A = 00011000
// OUT B = 00010100
// A+1 = 00111001
// B+1 = 00110101

alu_result alu_3e(unsigned const char opcode, const uint32_t a, const uint32_t b) {
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

void write_memory_3e(regfile *regs, std::vector<uint32_t>& memory) {
    if (regs->MAR < memory.size()) {
        memory[regs->MAR] = regs->MDR;
    } else {
        memory.resize(regs->MAR + 1, 0);
        memory[regs->MAR] = regs->MDR;
    }
}

void read_memory_3e(regfile *regs, const std::vector<uint32_t>& memory) {
    if (regs->MAR < memory.size()) {
        regs->MDR = memory[regs->MAR];
    } else {
        regs->MDR = 0;
    }
}

void write_to_regs_3e(regfile *regs, const uint16_t c_bus_mask, const uint32_t value) {
    if (c_bus_mask & (1 << 0)) regs->MAR = value;
    if (c_bus_mask & (1 << 1)) regs->MDR = value;
    if (c_bus_mask & (1 << 2)) regs->PC = value;
    if (c_bus_mask & (1 << 3)) regs->SP = value;
    if (c_bus_mask & (1 << 4)) regs->LV = value;
    if (c_bus_mask & (1 << 5)) regs->CPP = value;
    if (c_bus_mask & (1 << 6)) regs->TOS = value;
    if (c_bus_mask & (1 << 7)) regs->OPC = value;
    if (c_bus_mask & (1 << 8)) regs->H = value;
}

void run_microinstruction(const int inst, regfile& regs, std::vector<uint32_t>& memory, std::ostream& log, int& ciclo) {
    int ula = inst >> 15;
    int c_bus = inst >> 6 & 0x1FF;
    int mem = inst >> 4 & 0x3;
    int b_bus = inst & 0xF;

    uint32_t alu_b;
    std::string str_b_bus = "";

    switch (b_bus) {
        case 0:
            alu_b = regs.MDR;
            if (mem == 3) str_b_bus = "mbr (fetch)";
            else str_b_bus = "mdr";
            break;
        case 1:
            alu_b = regs.PC; str_b_bus = "pc";
            break;
        case 2:
            alu_b = regs.MBR; str_b_bus = "mbr";
            break;
        case 3:
            alu_b = regs.MBR << 24; str_b_bus = "h";
            alu_b >>= 24;
            break;
        case 4:
            alu_b = regs.SP; str_b_bus = "sp";
            break;
        case 5:
            alu_b = regs.LV; str_b_bus = "lv";
            break;
        case 6:
            alu_b = regs.CPP; str_b_bus = "cpp";
            break;
        case 7:
            alu_b = regs.TOS; str_b_bus = "tos";
            break;
        case 8:
            alu_b = regs.OPC; str_b_bus = "opc";
            break;
        default: alu_b = 0;
    }

    std::vector<std::string> c_regs;
    
    if (c_bus & (1 << 8)) c_regs.push_back("h");
    if (c_bus & (1 << 7)) c_regs.push_back("opc");
    if (c_bus & (1 << 6)) c_regs.push_back("tos");
    if (c_bus & (1 << 5)) c_regs.push_back("cpp");
    if (c_bus & (1 << 4)) c_regs.push_back("lv");
    if (c_bus & (1 << 3)) c_regs.push_back("sp");
    if (c_bus & (1 << 2)) c_regs.push_back("pc");
    if (c_bus & (1 << 1)) c_regs.push_back("mdr");
    if (c_bus & (1 << 0)) c_regs.push_back("mar");

    if (mem == 3) { // fetch
        c_regs.push_back("mbr");
        c_regs.push_back("h");
    }

    std::string str_c_bus = c_regs.empty() ? "nenhum" : c_regs[0];
    for (size_t i = 1; i < c_regs.size(); i++) str_c_bus += ", " + c_regs[i];

    auto print_both = [&](const std::string& str){
        std::cout << str;
        log << str;
    };

    print_both("Cycle " + std::to_string(ciclo) + "\n");
    print_both("ir = " + std:: bitset<8>(ula).to_string() + " " +
                        std:: bitset<9>(c_bus).to_string() + " " + 
                        std:: bitset<2>(mem).to_string() + " " +
                        std:: bitset<4>(b_bus).to_string() + "\n");

    print_both("b = " + str_b_bus + "\n");
    print_both("c = " + str_c_bus + "\n\n");

    print_both("> Registers before instruction\n");
    print_regs_3e(std::cout, regs);
    print_regs_3e(log, regs);
    print_both("\n");


    if (mem == 3) { // fetch
        // "ula" é o byte (8 bits) mais significativo dos 23 bits da instrucao.
        regs.MBR = ula;
        regs.H = ula;
    } else {
        alu_result alu_out = alu_3e(ula, regs.H, alu_b);
        write_to_regs_3e(&regs, c_bus, alu_out.s);
    }

    if (mem == 2) {
        write_memory_3e(&regs, memory);
    } else if (mem == 1) {
        read_memory_3e(&regs, memory);
    }

    print_both ("> Registers after instruction\n");
    print_regs_3e(std::cout, regs);
    print_regs_3e(log, regs);
    print_both("\n");

    print_both ("> Memory after instruction\n");
    print_memory_3e(std::cout, memory);
    print_memory_3e(log, memory);
    print_both("============================================================\n");

    ciclo++;
}

// Formato microinstrucao (23 bits): UUUUUUUUCCCCCCCCCMMBBBB
// Legenda: U = ULA
// C = Barramento C
// M = Memoria
// B = Barramento B

void generate_microinstructions(std::string& inst, regfile& regs, std::vector<uint32_t>& memory, std::ostream& log, int& ciclo) {
    
    std::vector<std::string> inst_split = split_3e(inst, " ");
    std::vector<uint32_t> microinstructions;

    if (inst_split[0] == "ILOAD") {
        microinstructions = {0b00010100100000000000101}; // H = LV
        for (int i = 0; i < stoi(inst_split[1]); i++) {
            microinstructions.push_back(0b00111001100000000000000); // H = H + 1
        }
        microinstructions.push_back(0b00011000000000001010000); // MAR = H; rd
        microinstructions.push_back(0b00110101000001001100100); // MAR = SP = SP+1; wr
        microinstructions.push_back(0b00010100001000000000000); // TOS = MDR
    } else if (inst_split[0] == "DUP") {
        microinstructions = {
            0b00110101000001001000100, // MAR = SP = SP + 1
            0b00010100000000010100111 // MDR = TOS; wr
        };
    } else if (inst_split[0] == "BIPUSH") {
        std::string byte = inst_split[1];
        microinstructions = {
            0b00110101000001001000100, // MAR = SP = SP + 1
            ((stoul(byte, nullptr, 2) << 15) | 0b00000000000000000110000), // MBR = H = byte
            0b00011000001000010100000 // MDR = TOS = H; wr
        };
    }

    for (const auto& microinst : microinstructions) {
        run_microinstruction(microinst, regs, memory, log, ciclo);
    }
}


void etapa3e::run() {
    // Cria a pasta de saída, se não existir
    std::filesystem::create_directory(std::string(PROJECT_ROOT_DIR) + "/out/etapa3e");

    // Abre os arquivos de entrada e saída
    std::ifstream arq_registradores(std::string(PROJECT_ROOT_DIR) + "/progs/etapa3/registradores_etapa3_tarefa1.txt");
    std::ifstream instrucoes(std::string(PROJECT_ROOT_DIR) + "/progs/etapa3/instrucoes.txt");
    std::ifstream mem_file(std::string(PROJECT_ROOT_DIR) + "/progs/etapa3/dados_etapa3_tarefa1.txt");
    std::ofstream log(std::string(PROJECT_ROOT_DIR) + "/out/etapa3e/log_entregavel.txt");

    if (!instrucoes.is_open() || !log.is_open() || !arq_registradores.is_open() || !mem_file.is_open()) {
        std::cout << "Erro ao abrir os arquivos da Etapa 3 Tarefa 1.\n";
        return;
    }

    std::vector<uint32_t> memory;
    std::string line;

    while (getline(mem_file, line)) {
        memory.push_back(std::stoul(line, nullptr, 2));
    }

    mem_file.close();

    
    regfile regs{};
    std::string reg_line;

    while (getline(arq_registradores, reg_line)) {
        auto parts = split_3e(reg_line, " = ");
        std::string reg_name = parts[0];
        uint32_t reg_value = std::stoul(parts[1], nullptr, 2);

        if (reg_name == "h") regs.H = reg_value;
        else if (reg_name == "opc") regs.OPC = reg_value;
        else if (reg_name == "tos") regs.TOS = reg_value;
        else if (reg_name == "cpp") regs.CPP = reg_value;
        else if (reg_name == "lv") regs.LV = reg_value;
        else if (reg_name == "sp") regs.SP = reg_value;
        else if (reg_name == "pc") regs.PC = reg_value;
        else if (reg_name == "mdr") regs.MDR = reg_value;
        else if (reg_name == "mar") regs.MAR = reg_value;
        else if (reg_name == "mbr") regs.MBR = static_cast<uint8_t>(reg_value);
    }

    arq_registradores.close();
    
    auto print_both = [&](const std::string& str){
        std::cout << str;
        log << str;
    };

    print_both("============================================================\n");
    print_both("Initial memory state\n");
    print_memory_3e(std::cout, memory);
    print_memory_3e(log, memory);
    print_both("Initial register state\n");
    print_regs_3e(std::cout, regs);
    print_regs_3e(log, regs);
    print_both("============================================================\n");
    print_both("Start of Program\n");
    print_both("============================================================\n");

    std::string inst_str;
    int ciclo = 1;

    while (getline(instrucoes, inst_str)) {
        generate_microinstructions(inst_str, regs, memory, log, ciclo);
    }
    
    print_both("Cycle " + std::to_string(ciclo) + "\n");
    print_both("No more lines, EOP.\n");

    instrucoes.close();
    log.close();
}
