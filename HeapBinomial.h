#ifndef HEAP_BINOMIAL_HEAPBINOMIAL_H
#define HEAP_BINOMIAL_HEAPBINOMIAL_H

#include <climits>

using namespace std;

struct Nod {
    int val = 0, ordin = 0;
    Nod *pFrate = nullptr, *pFiu = nullptr;
};

class HeapBinomial {
private:
    Nod *m_pStart = nullptr;

    static void faSubarboreStang(Nod *viitorSubarbore, Nod *tata);

    void consolideaza();

public:
    void insereaza(int val);

    static HeapBinomial *reuneste(HeapBinomial *a, HeapBinomial *b);

    void extrageMaxim();

    Nod *getMaxim() const;

    void reset();
};

#endif //HEAP_BINOMIAL_HEAPBINOMIAL_H
