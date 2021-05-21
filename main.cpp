#include <iostream>
#include "HeapBinomial.h"

using namespace std;

const int nMax = 105;

HeapBinomial h[nMax];

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
