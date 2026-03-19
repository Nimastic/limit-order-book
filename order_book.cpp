#include "order_book.h"

using namespace std;

void OrderBook::addOrder(Order o) {
    if (o.isBuy) { // if this order isbuy
        bids[o.price].push(o); //  we put into buy map and push order into that price's queue
    } else {
        asks[o.price].push(o); // asks
    }
}

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