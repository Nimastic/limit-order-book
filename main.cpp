#include "order_book.h"

int main() {

    // ── Test 1: clean full match ──────────────────────────────────────────────
    std::cout << "=== Test 1: full match ===\n";
    {
        OrderBook book;
        book.addOrder({1, false, 10300, 100, 1000}); // sell 100 @ $103
        book.addOrder({2, true,  10300, 100, 1001}); // buy  100 @ $103 → should match
        book.printBook(); // book should be empty
    }

    // ── Test 2: partial fill ──────────────────────────────────────────────────
    std::cout << "=== Test 2: partial fill ===\n";
    {
        OrderBook book;
        book.addOrder({1, false, 10300,  60, 1000}); // sell  60 @ $103
        book.addOrder({2, true,  10300, 100, 1001}); // buy  100 @ $103 → fills 60, 40 rests
        book.printBook(); // should show bid 40 @ $103, ask side empty
    }

    // ── Test 3: sweep multiple levels ─────────────────────────────────────────
    std::cout << "=== Test 3: sweep multiple ask levels ===\n";
    {
        OrderBook book;
        book.addOrder({1, false, 10300,  80, 1000}); // sell  80 @ $103
        book.addOrder({2, false, 10400, 150, 1001}); // sell 150 @ $104
        book.addOrder({3, true,  10500, 300, 1002}); // buy  300 @ $105 → sweeps both
        book.printBook(); // should show bid 70 @ $105 resting, asks empty
    }

    // ── Test 4: no cross ─────────────────────────────────────────────────────
    std::cout << "=== Test 4: no cross, orders rest ===\n";
    {
        OrderBook book;
        book.addOrder({1, false, 10400, 100, 1000}); // sell @ $104
        book.addOrder({2, true,  10300, 100, 1001}); // buy  @ $103 → no cross
        book.printBook(); // both sides should show their orders
    }

    return 0;
}