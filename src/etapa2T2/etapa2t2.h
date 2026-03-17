#ifndef PROJETOARQUITETURA_ETAPA2T2_H
#define PROJETOARQUITETURA_ETAPA2T2_H

#include <cstdint>
    

class etapa2t2 {
    // Os 9 registradores de 32 bits
    int32_t H, OPC, TOS, CPP, LV, SP, PC, MDR, MAR;

    // O registrador de 8 bits
    int8_t MBR;
    int IR;
    public:
    etapa2t2(){
        H = OPC = TOS = CPP = LV = SP = PC = MDR = MAR = 0;
        MBR = 0;
    }
    void run();
};

#endif