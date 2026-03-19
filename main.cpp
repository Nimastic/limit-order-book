#include "order_book.h"

int main() {
    OrderBook book;

    // add some bids
    book.addOrder({1, true,  10200, 100, 1000}); // buy 100 @ $102.00
    book.addOrder({2, true,  10100, 200, 1001}); // buy 200 @ $101.00
    book.addOrder({3, true,  10200,  50, 1002}); // buy  50 @ $102.00 (same level)
    book.addOrder({3, true,  10200,  50, 1002}); // buy  50 @ $102.00 (same level)
    book.addOrder({3, true,  10200,  50, 1002}); // buy  50 @ $102.00 (same level)
    book.addOrder({3, true,  10200,  50, 1002}); // buy  50 @ $102.00 (same level)


    // add some asks
    book.addOrder({4, false, 10200,  80, 1003}); // sell  80 @ $103.00
    book.addOrder({4, false, 10200,  80, 1003}); // sell  80 @ $103.00
    book.addOrder({4, false, 10300,  80, 1003}); // sell  80 @ $103.00
    book.addOrder({4, false, 10300,  80, 1003}); // sell  80 @ $103.00
    book.addOrder({5, false, 10400, 150, 1004}); // sell 150 @ $104.00

    book.printBook();
    return 0;
}