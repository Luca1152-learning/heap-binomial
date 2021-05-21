#include <iostream>
#include <climits>

using namespace std;

struct Nod {
    int val = 0, ordin = 0;
    Nod *pPrec = nullptr, *pFrate = nullptr, *pFiu = nullptr;
};

class HeapBinomial {
private:
    Nod *m_pStart;

    static void faSubarboreStang(Nod *viitorSubarbore, Nod *tata) {
        viitorSubarbore->pFrate = tata->pFiu;
        tata->pFiu = viitorSubarbore;
        tata->ordin++;
    }

    void consolideaza() {
        // Daca nodul de start este null, va da eroare initializarea lui pUrm. Si oricum, un heap gol
        // nu trebuie consolidat
        if (m_pStart == nullptr) {
            return;
        }

        Nod *pCurr = m_pStart, *pUrm = pCurr->pFrate, *pPrec = nullptr;
        while (pUrm) {
            if (pCurr->ordin != pUrm->ordin) {
                // Arborele curent si urmatorul nu trebuie reuniti (neavand acelasi ordin)
                pPrec = pCurr;
                pCurr = pUrm;
                pUrm = pUrm->pFrate;
            } else if (pCurr->ordin == pUrm->ordin) {
                // Arborele curent si urmatorul trebuie reuniti

                if (pUrm->ordin && pUrm->pFrate && pUrm->ordin == pUrm->pFrate->ordin) {
                    // Sunt 3 arbori care au acelasi ordin. Ultimii doi trebuie reuniti (si nu primii doi), pentru
                    // a pastra ordinea crescatoare a ordinelor arborilor
                    //
                    // In situatia asta se poate ajunge daca trebuie sa consolidam (de exemplu):
                    // 1 4 8 8 16 16 32 => 1 4 !16 16 16! 32
                    pPrec = pCurr;
                    pCurr = pUrm;
                    pUrm = pUrm->pFrate;
                } else {
                    // Sunt numai 2 arbori cu acelasi ordin ce trebuie reuniti

                    if (pCurr->val > pUrm->val) {
                        // Arborele urmator trebuie unit in cel curent
                        pCurr->pFrate = pUrm->pFrate;
                        faSubarboreStang(pUrm, pCurr);

                        // pPrec si pCurr nu se modifica, din moment ce pPrec-pCurr-pUrm devine pPrec-[pCurr->pUrm]-pUrm actualizat
                        pUrm = pUrm->pFrate;
                    } else {
                        // Arborele curent trebuie unit in cel urmator
                        if (pPrec) {
                            pPrec->pFrate = pUrm;
                        } else {
                            m_pStart = pUrm;
                        }
                        faSubarboreStang(pCurr, pUrm);

                        // pPrec nu se modifica, din moment ce pPrec-pCurr-pUrm devine pPrec-[pUrm->pCurr]-pUrm actualizat
                        pCurr = pUrm;
                        pUrm = pUrm->pFrate;
                    }
                }
            }
        }
    }

public:
    void insereaza(int val) {
        // Creeaza un nou nod (= arbore binomial de ordin 0) cu valoarea data
        Nod *pNod = new Nod();
        pNod->val = val;

        // Insereaza arborele la inceput, ca sa se pastreze ordinea crescatoare a arborilor (dpdv. al ordinului)
        pNod->pFrate = m_pStart;
        m_pStart = pNod;

        consolideaza();
    }

    static HeapBinomial *reuneste(HeapBinomial *a, HeapBinomial *b) {
        HeapBinomial *pHeapReunit = new HeapBinomial();

        // Seteaza nodul de start al heap-ului reunit
        Nod *pNodA = a->m_pStart, *pNodB = b->m_pStart;
        if ((pNodA && pNodB && pNodA->ordin <= pNodB->ordin) || (pNodA && !pNodB)) {
            pHeapReunit->m_pStart = pNodA;
            pNodA = pNodA->pFrate;
        } else if ((pNodA && pNodB && pNodB->ordin < pNodA->ordin) || (!pNodA && pNodB)) {
            pHeapReunit->m_pStart = pNodB;
            pNodB = pNodB->pFrate;
        } else {
            return pHeapReunit;
        }

        Nod *pUlt = pHeapReunit->m_pStart;

        while (pNodA && pNodB) {
            if (pNodA->ordin < pNodB->ordin) {
                pUlt->pFrate = pNodA;
                pUlt = pUlt->pFrate;
                pNodA = pNodA->pFrate;
            } else {
                pUlt->pFrate = pNodB;
                pUlt = pUlt->pFrate;
                pNodB = pNodB->pFrate;
            }
        }
        while (pNodA) {
            pUlt->pFrate = pNodA;
            pUlt = pUlt->pFrate;
            pNodA = pNodA->pFrate;
        }
        while (pNodB) {
            pUlt->pFrate = pNodB;
            pUlt = pUlt->pFrate;
            pNodB = pNodB->pFrate;
        }

        pHeapReunit->consolideaza();

        return pHeapReunit;
    }

    void extrageMaxim() {
        // Problema ne asigura ca nu se va extrage dintr-un heap vid, deci sigur exista primul element
        int maxValue = m_pStart->val;
        Nod *pPrecMaxim = nullptr, *pMaxim = m_pStart;

        Nod *pIt = m_pStart;
        while (pIt && pIt->pFrate) {
            if (pIt->pFrate->val > maxValue) {
                pPrecMaxim = pIt;
                maxValue = pIt->pFrate->val;
                pMaxim = pIt->pFrate;
            }
            pIt = pIt->pFrate;
        }

        // Inainte de a sterge nodul de minim, trebuie sa vedem ce facem cu subarborii...
        // Ideea este sa ii punem intr-un nou heap binomial pe care sa il reunim cu cel curent
        HeapBinomial *subarbori = new HeapBinomial();

        // Creeaza heap-ul din subarbori, sortandu-i in ordine crescatoare, dupa ordin.
        // Subarborii unui nod sunt in ordine descrescatoare a ordinului (fiii lui B_k sunt B_(k-1), B_(k-2), ..., B_0),
        // deci ordinea lor va trebui inversata
        Nod *pCurr = pMaxim->pFiu;
        while (pCurr) {
            Nod *pFrate = pCurr->pFrate;

            pCurr->pFrate = subarbori->m_pStart;
            subarbori->m_pStart = pCurr;

            pCurr = pFrate;
        }

        // Sterge nodul de maxim, inlocuindu-l cu urmatorul arbore din heap
        if (pPrecMaxim == nullptr) {
            m_pStart = pMaxim->pFrate;
        } else {
            pPrecMaxim->pFrate = pMaxim->pFrate;
        }

        // Reuneste heap-ul curent si cel al subarborilor
        HeapBinomial *pHeapReunit = reuneste(this, subarbori);
        *this = *pHeapReunit;
    }

    Nod *getMaxim() const {
        int maxValue = INT_MIN;
        Nod *pNodMax = nullptr;

        Nod *pIt = m_pStart;
        while (pIt) {
            if (pIt->val > maxValue) {
                maxValue = pIt->val;
                pNodMax = pIt;
            }
            pIt = pIt->pFrate;
        }

        return pNodMax;
    }

    void reset() {
        m_pStart = nullptr;
    }
};

HeapBinomial h[105];

int main() {
    freopen("mergeheap.in", "r", stdin);
    freopen("mergeheap.out", "w", stdout);

    // Input rapid
    ios_base::sync_with_stdio(false);
    cin.tie(nullptr);

    int n, q, op, m, x, a, b;
    cin >> n >> q;

    for (int i = 0; i < q; i++) {
        cin >> op;
        if (op == 1) {
            cin >> m >> x;

            h[m].insereaza(x);
        } else if (op == 2) {
            cin >> m;

            cout << h[m].getMaxim()->val << "\n";
            h[m].extrageMaxim();
        } else if (op == 3) {
            cin >> a >> b;

            h[a] = *HeapBinomial::reuneste(&(h[a]), &(h[b]));
            h[b].reset();
        }
    }

    return 0;
}
