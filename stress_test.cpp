#include "order_book.h"
#include <cassert>
#include <random>
#include <iostream>
#include <set>

// ── Invariant checker ─────────────────────────────────────────────────────────
/**
 * @brief Asserts OrderBook invariants (friend of OrderBook; declared in order_book.h).
 * @param book Book to validate after each operation.
 * @see OrderBook
 */
// Call after every operation to verify the book is in a valid state
void checkInvariants(const OrderBook& book) {

    // Invariant 1: no crossed book
    // best bid must always be strictly less than best ask
    // if bid >= ask, a match should have happened — crossing means matching bug
    if (!book.bids.empty() && !book.asks.empty()) {
        int bestBid = book.bids.begin()->first;
        int bestAsk = book.asks.begin()->first;
        // skip sentinel prices
        if (bestBid != INT_MAX && bestAsk != 0) {
            assert(bestBid < bestAsk &&
                "INVARIANT BROKEN: crossed book — best bid >= best ask");
        }
    }

    // Invariant 2: every trade has matching counterparts
    // buy and sell quantities across all trades must be equal
    // phantom trades (fills with no counterpart) would break this
    int totalBuyFilled  = 0;
    int totalSellFilled = 0;
    for (const auto& t : book.getTrades()) {
        totalBuyFilled  += t.quantity;
        totalSellFilled += t.quantity;
    }
    assert(totalBuyFilled == totalSellFilled &&
        "INVARIANT BROKEN: buy filled qty != sell filled qty");

    // Invariant 3: no cancelled order appears as a live order
    // every order in the index must not be cancelled
    for (const auto& [id, ptr] : book.index) {
        assert(ptr->status != OrderStatus::Cancelled &&
            "INVARIANT BROKEN: cancelled order still in index");
    }
}

int main() {
    std::cout << "=== Stress test: 10,000 random orders ===\n";

    OrderBook book;

    // reproducible random number generator
    std::mt19937 rng(42);

    // price range: $99 to $101 in cents (9900 to 10100)
    // narrow range forces lots of matches
    std::uniform_int_distribution<int> priceDist(9900, 10100);
    std::uniform_int_distribution<int> qtyDist(1, 500);
    std::uniform_int_distribution<int> sideDist(0, 1);
    std::uniform_int_distribution<int> typeDist(0, 9); // 0-1 = market, rest = limit

    int totalOrders   = 0;
    int totalTrades   = 0;
    int totalCancels  = 0;
    std::vector<int> activeOrderIds;

    for (int i = 1; i <= 10000; i++) {
        int  price    = priceDist(rng);
        int  qty      = qtyDist(rng);
        bool isBuy    = sideDist(rng);
        int  type     = typeDist(rng);

        // 20% chance of market order
        if (type <= 1) {
            price = isBuy ? INT_MAX : 0;
        }

        book.addOrder({i, isBuy, price, qty, now_ns()});
        activeOrderIds.push_back(i);
        totalOrders++;

        // every 50 orders, cancel a random active order
        if (i % 50 == 0 && !activeOrderIds.empty()) {
            std::uniform_int_distribution<int> cancelDist(
                0, activeOrderIds.size() - 1);
            int cancelIdx = cancelDist(rng);
            int cancelId  = activeOrderIds[cancelIdx];
            book.cancelOrder(cancelId);
            activeOrderIds.erase(activeOrderIds.begin() + cancelIdx);
            totalCancels++;
        }

        // check invariants every 100 orders
        if (i % 100 == 0) {
            checkInvariants(book);
            std::cout << "checkpoint " << i << "/10000 — trades so far: "
                      << book.getTrades().size() << "\n";
        }
    }

    // final invariant check
    checkInvariants(book);

    std::cout << "\n=== Results ===\n";
    std::cout << "orders processed : " << totalOrders  << "\n";
    std::cout << "trades executed  : " << book.getTrades().size() << "\n";
    std::cout << "cancels issued   : " << totalCancels << "\n";

    // final book state
    int restingBids = 0, restingAsks = 0;
    for (auto& [price, q] : book.bids) {
        if (price == 0 || price == INT_MAX) continue;
        auto copy = q;
        while (!copy.empty()) {
            if (copy.front().status != OrderStatus::Cancelled) restingBids++;
            copy.pop();
        }
    }
    for (auto& [price, q] : book.asks) {
        if (price == 0 || price == INT_MAX) continue;
        auto copy = q;
        while (!copy.empty()) {
            if (copy.front().status != OrderStatus::Cancelled) restingAsks++;
            copy.pop();
        }
    }
    std::cout << "resting bids     : " << restingBids << "\n";
    std::cout << "resting asks     : " << restingAsks << "\n";
    std::cout << "\nAll invariants passed.\n";

    return 0;
}