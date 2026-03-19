#include "order_book.h"

using namespace std; // no overhead to use this

// -- Core Matching

void OrderBook::recordTrade(int qty, int price, int buyId, int sellId) {
    trades.push_back({buyId, sellId, price, qty});
    cout << "TRADE: " << qty
              << " @ $" << price / 100.0
              << "  (buy#" << buyId << " x sell#" << sellId << ")\n";
}


void OrderBook::matchBuy(Order& buy) {
    while (buy.quantity > 0 && !asks.empty()) {
        auto best = asks.begin();          // lowest ask
        if (buy.price < best->first) break; // price doesn't cross, stop

        Order& sell = best->second.front(); // oldest order at this level
        int fill = std::min(buy.quantity, sell.quantity);

        recordTrade(fill, best->first, buy.id, sell.id);

        buy.quantity  -= fill;
        sell.quantity -= fill;

        if (sell.quantity == 0) {
            index.erase(sell.id);
            best->second.pop();
            if (best->second.empty()) asks.erase(best);
        }
    }
}

void OrderBook::matchSell(Order& sell) {
    while (sell.quantity > 0 && !bids.empty()) {
        auto best = bids.begin();            // highest bid
        if (sell.price > best->first) break; // price doesn't cross, stop

        Order& buy = best->second.front();
        int fill = std::min(sell.quantity, buy.quantity);

        recordTrade(fill, best->first, buy.id, sell.id);

        sell.quantity -= fill;
        buy.quantity  -= fill;

        if (buy.quantity == 0) {
            index.erase(buy.id);
            best->second.pop();
            if (best->second.empty()) bids.erase(best);
        }
    }
}

// Public Interface

void OrderBook::addOrder(Order o) {
    if (o.isBuy) { // if this order isbuy
        matchBuy(o);
        if (o.quantity > 0) bids[o.price].push(o); // rest remainder //  we put into buy map and push order into that price's queue
    } else {
        matchSell(o);
        if (o.quantity > 0) asks[o.price].push(o);
    }
}

// Display

void OrderBook::printBook() const {
    cout << "\n=== ASK SIDE (sellers) ===\n";
    // prints highest ask first 
    for (auto it = asks.rbegin(); it != asks.rend(); ++it) { // from the highest bid first we iterate down
        auto q = it->second; // get the queue
        int total = 0;
        int count = 0;
        while (!q.empty()) { // while queue is not empty
            total += q.front().quantity; //add quantity of order to total
            q.pop();
            count++;
        }
        cout << " $" << it->first / 100.0
             << " qty=" << total
             << " orders=" << count << "\n";
    }

    cout << "------=== SPREAD ===------\n";
    // using [price, q] : bids] vs auto it = asks.rbegin(); it != asks.rend(); ++it has no performance difference
    
    for (auto& [price, q] : bids) {
        auto copy = q;
        int total = 0;
        int count = 0;
        while (!copy.empty()) {
            total += copy.front().quantity;
            copy.pop();
            count++;
        }
        cout << " $" << price / 100.0
             << " qty=" << total
             << " orders=" << count << "\n";
    }
    cout << "=== BID SIDE (buyers) ===\n\n";
}