#include "order_book.h"
#include <thread>
#include <chrono>

int main() {

    // ── Test 1: FIFO within same price level ──────────────────────────────────
    // Three bids at the same price, added in order 1→2→3
    // A sell that can only fill two of them should fill #1 and #2, not #3
    std::cout << "=== Test 1: FIFO — first in, first filled ===\n";
    {
        OrderBook book;
        book.addOrder({1, true, 10200, 100, now_ns()});
        std::this_thread::sleep_for(std::chrono::milliseconds(1)); // ensure different timestamps
        book.addOrder({2, true, 10200, 100, now_ns()});
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        book.addOrder({3, true, 10200, 100, now_ns()});

        book.printBook(); // should show $102 qty=300 orders=3

        // sell 200 — should fill order #1 first, then #2, #3 untouched
        book.addOrder({4, false, 10200, 200, now_ns()});
        book.printBook();
        // expected: TRADE buy#1, TRADE buy#2 — in that order
        // $102 qty=100 orders=1 remaining (order #3)
    }

    // ── Test 2: FIFO across partial fills ─────────────────────────────────────
    // Two bids at same price, sell partially fills the first one
    // Remaining of first order should still be ahead of second order
    std::cout << "=== Test 2: partial fill preserves queue position ===\n";
    {
        OrderBook book;
        book.addOrder({1, true, 10200, 100, now_ns()});
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        book.addOrder({2, true, 10200, 100, now_ns()});

        // sell 50 — should partially fill order #1 only
        book.addOrder({3, false, 10200, 50, now_ns()});
        book.printBook();
        // expected: TRADE 50 @ $102 (buy#1 x sell#3)
        // book shows $102 qty=150 orders=2 (50 remaining on #1, 100 on #2)

        // sell another 100 — should fill remaining 50 of #1, then 50 of #2
        book.addOrder({4, false, 10200, 100, now_ns()});
        book.printBook();
        // expected: TRADE 50 @ $102 (buy#1 x sell#4)
        //           TRADE 50 @ $102 (buy#2 x sell#4)
        // book shows $102 qty=50 orders=1 (50 remaining on #2)
    }

    // ── Test 3: different price levels still price-priority ───────────────────
    // Two bids at different prices — better price must fill first
    // regardless of which was added first
    std::cout << "=== Test 3: price priority beats time priority ===\n";
    {
        OrderBook book;
        // order #2 added first but at worse price
        book.addOrder({1, true, 10100, 100, now_ns()}); // bid $101 — added first
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        book.addOrder({2, true, 10300, 100, now_ns()}); // bid $103 — added second

        // sell at $101 — should fill $103 bid first (better price)
        // even though $101 bid was added earlier
        book.addOrder({3, false, 10100, 100, now_ns()});
        book.printBook();
        // expected: TRADE 100 @ $103 (buy#2 x sell#3)
        // $101 bid still resting
    }

    return 0;
}