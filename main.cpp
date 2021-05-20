#include <iostream>

using namespace std;

struct Nod {
    int val = 0, ordin = 0;
    Nod *pFrate = nullptr, *pFiu = nullptr, *pTata = nullptr;

    void reset() {
        val = ordin = 0;
        pFrate = pFiu = pTata = nullptr;
    }
};

class HeapBinomial {
private:
    Nod *m_pStart;
    Nod *m_pMaxim;

    static void faSubarboreStang(Nod *viitorSubarbore, Nod *tata) {
        viitorSubarbore->pFrate = tata->pFiu;
        viitorSubarbore->pTata = tata;
        tata->pFiu = viitorSubarbore;
        tata->ordin++;
    }

    void consolideaza() {
        // Daca nodul de start este null, va da eroare initializarea lui pUrm. Si oricum, un heap gol
        // nu trebuie consolidat
        if (m_pStart == nullptr) {
            return;
        }

        // Recalculeaza maximul (de la 0)
        m_pMaxim = m_pStart;

        Nod *pCurr = m_pStart, *pUrm = pCurr->pFrate, *pPrec = nullptr;
        while (pUrm) {
            // Actualizeaza maximul
            if (pUrm->val > m_pMaxim->val) {
                m_pMaxim = pUrm;
            }

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
                        Nod *pUrmUrm = pUrm->pFrate;
                        faSubarboreStang(pUrm, pCurr);
                        pCurr->pFrate = pUrmUrm;
                    } else {
                        // Arborele curent trebuie unit in cel urmator

                        if (pPrec) {
                            pPrec->pFrate = pUrm;
                        } else {
                            m_pStart = pUrm;
                        }
                        faSubarboreStang(pCurr, pUrm);
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

        // Actualizeaza maximul (stocat pentru a avea lookup in O(1))
        if (!m_pMaxim || (m_pMaxim && pNod->val > m_pMaxim->val)) {
            m_pMaxim = pNod;
        }

        // Insereaza arborele la inceput, ca sa se pastreze ordinea crescatoare a arborilor din colectie
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
        // Inainte de a sterge nodul de minim, trebuie sa vedem ce facem cu subarborii...
        // Ideea este sa ii punem intr-un nou heap binomial pe care sa il reunim cu cel curent
        HeapBinomial *subarbori = new HeapBinomial();

        // Creeaza heap-ul din subarbori, sortandu-i in ordine crescatoare, dupa ordin.
        // Subarborii unui nod sunt in ordine descrescatoare a ordinului (fiii lui B_k sunt B_(k-1), B_(k-2), ..., B_0),
        // deci ordinea lor va trebui inversata
        Nod *pCurr = m_pMaxim->pFiu;
        while (pCurr) {
            Nod *pFrate = pCurr->pFrate;

            pCurr->pFrate = subarbori->m_pStart;
            pCurr->pTata = nullptr;
            subarbori->m_pStart = pCurr;

            pCurr = pFrate;
        }

        // Sterge nodul de maxim, inlocuindu-l cu urmatorul arbore din heap
        Nod *temp = m_pMaxim->pFrate;
        if (m_pMaxim->pFrate) {
            *m_pMaxim = *m_pMaxim->pFrate;
        } else {
            if (m_pStart == m_pMaxim) {
                m_pStart = nullptr;
                delete m_pMaxim;
                m_pMaxim = nullptr;
            } else {
                Nod *nodIt = m_pStart;
                while (nodIt->pFrate != m_pMaxim) {
                    nodIt = m_pStart->pFrate;
                }
                nodIt->pFrate = nullptr;
            }
        }
        delete temp;

        // Reuneste heap-ul curent si cel al subarborilor
        HeapBinomial *pHeapReunit = reuneste(this, subarbori);
        *this = *pHeapReunit;
        delete pHeapReunit;
        delete subarbori;
    }

    int getMaxim() const {
        if (m_pMaxim) {
            return m_pMaxim->val;
        } else {
            return -1;
        }
    }

    void reset() {
        m_pStart = nullptr;
        m_pMaxim = nullptr;
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

            cout << h[m].getMaxim() << "\n";
            h[m].extrageMaxim();
        } else if (op == 3) {
            cin >> a >> b;

            h[a] = *HeapBinomial::reuneste(&(h[a]), &(h[b]));
            h[b].reset();
        }
    }

    return 0;
}
