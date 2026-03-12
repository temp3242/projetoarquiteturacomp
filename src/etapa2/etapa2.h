#ifndef PROJETOARQUITETURA_ETAPA2_H
#define PROJETOARQUITETURA_ETAPA2_H


class etapa2 {
    int a, b, s, c_in, c_out, z, n, s_d;
    int IR, PC;
public:
    etapa2() {
        a = b = s = 0;
        c_in = c_out = 0;
        IR = PC = 0;
        z = n = 0;
        s_d = 0;
    }
    void run();
};


#endif //PROJETOARQUITETURA_ETAPA2_H