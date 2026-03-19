#pragma once
#include <map>
#include <queue>
#include <unordered_map>
#include <functional>
#include <iostream>

// namespace using std;
// bad to use this

struct Order {
    int id;
    bool isBuy;
    int price;
    int quantity;
    long long timestamp; // or time priority

};

class OrderBook {

public:
    void addOrder(Order o);
    void printBook() const;

private:
    // bids: highest price first
    // for this bid price, queue of Orders at this price (FIFO)
    std::map<int, std::queue<Order>, std::greater<int>> bids;
    // asks: lowest price first
    // for this ask price, queue of Orders at this price (FIF0)
    std::map<int, std::queue<Order>> asks;
    // O(1) lookup by order id, needed for cancel later
    std::unordered_map<int, Order*> index; // id and pointer to order
    //? what is this for
};
