#include <iostream>

using namespace std;

int n, q, op, m, x, a, b;

int main() {
    freopen("mergeheap.in", "r", stdin);
    freopen("mergeheap.out", "w", stdout);

    // Input rapid
    ios_base::sync_with_stdio(false);
    cin.tie(nullptr);

    cin >> n >> q;

    for (int i = 0; i < q; i++) {
        cin >> op;
        if (op == 1) {
            cin >> m >> x;
        } else if (op == 2) {
            cin >> m;
        } else if (op == 3) {
            cin >> a >> b;
        }
    }

    return 0;
}
