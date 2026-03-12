#ifndef PROJETOARQUITETURA_ETAPA1_H
#define PROJETOARQUITETURA_ETAPA1_H


class etapa1 {
    int a, b, s, c_in, c_out;
    int IR, PC;
    public:
    etapa1() {
        a = b = s = 0;
        c_in = c_out = 0;
        IR = PC = 0;
    }
    void run();
};


#endif //PROJETOARQUITETURA_ETAPA1_H