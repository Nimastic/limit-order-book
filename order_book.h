#pragma once // tells compilers to include once per CU
#include <map>
#include <queue>
#include <unordered_map>
#include <functional>
#include <iostream>
#include <algorithm>

// namespace using std;
// bad to use this in header files

struct Order {
    int id;
    bool isBuy;
    int price;
    int quantity;
    long long timestamp; // for time priority

};

struct Trade {
    int buyOrderId;
    int sellOrderId;
    int price;
    int quantity;
};

class OrderBook {

public:
    void addOrder(Order o);
    void printBook() const;
    const std::vector<Trade>& getTrades() const { return trades; }


private:
    // bids: highest price first
    // for this bid price, queue of Orders at this price (FIFO)
    std::map<int, std::queue<Order>, std::greater<int>> bids;
    // asks: lowest price first
    // for this ask price, queue of Orders at this price (FIF0)
    std::map<int, std::queue<Order>> asks;
    
    // O(1) lookup by order id, needed for cancel later
    std::unordered_map<int, Order*> index; // id and pointer to order

    std::vector<Trade> trades;

    void matchBuy(Order& buy);
    void matchSell(Order& sell);
    void recordTrade(int qty, int price, int buyId, int sellId);
};
