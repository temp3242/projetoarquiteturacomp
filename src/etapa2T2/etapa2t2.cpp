#include "etapa2t2.h"
#include <iostream>
#include <fstream>
#include <string>
#include <bitset>
#include <iomanip>
#include <filesystem>
#include <vector>

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
        out << "001101000000110000010\n";
        out << "001101000000010000011\n";
        out << "001111011000000000111\n\n";
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
        out << "h = " << std::bitset<32>(H) << "\n\n";
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
    int ciclo = 1;

    while (getline(instrucoes, inst_str)){

        uint32_t inst = std::stoi(inst_str, nullptr, 2);
        IR = inst;
        int ula = inst >> 15;
        
        // Separação dos 21 bits
        int alu_ctrl = (inst >> 13) & 0xFF; // Os 8 bits mais á esquerda
        int c_bus = (inst >> 4) & 0x1FF; // Os 9 bits do meio
        int b_bus = inst & 0xF; // Os 4 bits mais á direita

        

        std::cout << "Cycle " << ciclo << "\n";
        std::cout << "ir = " <<  std::bitset<8>(alu_ctrl) << " " <<  std::bitset<9>(c_bus) << " " <<  std::bitset<4>(b_bus) << "\n\n";
        log << "Cycle " << ciclo << "\n";
        log << "ir = " <<  std::bitset<8>(alu_ctrl) << " " <<  std::bitset<9>(c_bus) << " " <<  std::bitset<4>(b_bus) << "\n\n";
        
        int32_t op_a = H;
        int32_t op_b = 0;
        std::string nome_reg_b = "";

        switch(b_bus){
            case 0 : op_b = MDR; nome_reg_b = "mdr"; break;
            case 1 : op_b = PC; nome_reg_b = "pc"; break;
            case 2 : op_b = static_cast<uint32_t>(static_cast<uint8_t>(MBR)); nome_reg_b = "mbr"; break;
            case 3 : op_b = static_cast<uint32_t>(static_cast<int8_t>(MBR)); nome_reg_b = "mbru"; break;
            case 4 : op_b = SP; nome_reg_b = "sp"; break;
            case 5 : op_b = LV; nome_reg_b = "lv"; break;
            case 6 : op_b = CPP; nome_reg_b = "cpp"; break;
            case 7 : op_b = TOS; nome_reg_b = "tos"; break;
            case 8 : op_b = OPC; nome_reg_b = "opc";

            default : op_b = 0; nome_reg_b = "Nenhum"; break;
  
        }

        std::cout << "b_bus = " << nome_reg_b << "\n";
        log << "b_bus = " << nome_reg_b << "\n";
        
        std::vector<std::string> escritos;
        
        if (c_bus & (1 << 8)) escritos.push_back("h");
        if (c_bus & (1 << 7)) escritos.push_back("opc");
        if (c_bus & (1 << 6)) escritos.push_back("tos");
        if (c_bus & (1 << 5)) escritos.push_back("cpp");
        if (c_bus & (1 << 4)) escritos.push_back("lv");
        if (c_bus & (1 << 3)) escritos.push_back("sp");
        if (c_bus & (1 << 2)) escritos.push_back("pc");
        if (c_bus & (1 << 1)) escritos.push_back("mdr");
        if (c_bus & (1 << 0)) escritos.push_back("mar");
        
        std::cout << "c_bus = ";
        log << "c_bus = ";
        if (escritos.empty()) {
            std::cout << "nenhum";
            log << "nenhum";
        } else {
            for (size_t i = 0; i < escritos.size(); ++i) {
                std::cout << escritos[i];
                log << escritos[i];
                    if (i < escritos.size() - 1) {
                        std::cout << ", ";
                        log << ", ";
                }
            }
        }
        std::cout << std::endl << std::endl;
        log << std::endl << std::endl;

        auto print_registradores_antes_depois = [&](std::ostream& out, bool before = false) {
        if (before) out << "> Registers before instrucion\n";
        if (!before) out << "> Registers after instrucion\n";
        out << "mar = " << std::bitset<32>(MAR) << "\n";
        out << "mdr = " << std::bitset<32>(MDR) << "\n";
        out << "pc = " << std::bitset<32>(PC) << "\n";
        out << "mbr = " << std::bitset<8>(MBR) << "\n";
        out << "sp = " << std::bitset<32>(SP) << "\n";
        out << "lv = " << std::bitset<32>(LV) << "\n";
        out << "cpp = " << std::bitset<32>(CPP) << "\n";
        out << "tos = " << std::bitset<32>(TOS) << "\n";
        out << "opc = " << std::bitset<32>(OPC) << "\n";
        out << "h = " << std::bitset<32>(H) << "\n\n";

        };
        
        print_registradores_antes_depois(std::cout, true);
        print_registradores_antes_depois(log, true);

        uint32_t alu_out = alu(alu_ctrl, H, op_b).s;

        if (c_bus & (1 << 8)) H = alu_out; 
        if (c_bus & (1 << 7)) OPC = alu_out;
        if (c_bus & (1 << 6)) TOS = alu_out;
        if (c_bus & (1 << 5)) CPP = alu_out;
        if (c_bus & (1 << 4)) LV = alu_out;
        if (c_bus & (1 << 3)) SP = alu_out;
        if (c_bus & (1 << 2)) PC = alu_out;
        if (c_bus & (1 << 1)) MDR = alu_out;
        if (c_bus & (1 << 0)) MAR = alu_out;
    
        print_registradores_antes_depois(std::cout, false);
        print_registradores_antes_depois(log, false);

        std::cout << "=====================================================\n";
        log << "=====================================================\n";
        ciclo++;
    }


    std::cout << "Cycle " << ciclo << "\n";
    std::cout << "No more lines, EOP.\n";

    log << "Cycle " << ciclo << "\n";
    log << "No more lines, EOP.\n";

    instrucoes.close();
    arq_registradores.close();
    log.close();
}