# Limit Order Book — Built Components Summary

This document provides a concise overview of the data structures, features, order types, matching algorithms, and testing frameworks built in this repository.

---

## 1. Core Data Structures & Architecture

- **Fixed-Point Integer Prices**:
  - Prices are represented as integers (e.g. cents, `$103.00` $\rightarrow$ `10300`) in [`Order`](file:///c:/Users/jerie/Documents/GitHub/limit-order-book-cpp/order_book.h#L37-L46) to eliminate IEEE 754 floating-point inaccuracies during matching.
- **Price-Level Indexing**:
  - **Bids**: [`std::map<int, std::deque<Order>, std::greater<int>> bids`](file:///c:/Users/jerie/Documents/GitHub/limit-order-book-cpp/order_book.h#L121) (highest price level first).
  - **Asks**: [`std::map<int, std::deque<Order>> asks`](file:///c:/Users/jerie/Documents/GitHub/limit-order-book-cpp/order_book.h#L127) (lowest price level first).
  - Upgraded queue container from `std::queue` to `std::deque` to allow iteration without copying queues during display or inspection.
- **$\mathcal{O}(1)$ Lookup & Cancel Index**:
  - [`std::unordered_map<int, Order*> index`](file:///c:/Users/jerie/Documents/GitHub/limit-order-book-cpp/order_book.h#L134) maps order IDs directly to order pointers inside the price-level queues.
- **Trade History**:
  - [`std::vector<Trade> trades`](file:///c:/Users/jerie/Documents/GitHub/limit-order-book-cpp/order_book.h#L139) records executed transactions sequentially.

---

## 2. Order Types & Lifecycle Management

Order statuses are tracked via [`OrderStatus`](file:///c:/Users/jerie/Documents/GitHub/limit-order-book-cpp/order_book.h#L18-L23): `Open`, `PartiallyFilled`, `Filled`, and `Cancelled`.

Supported order types in [`OrderType`](file:///c:/Users/jerie/Documents/GitHub/limit-order-book-cpp/order_book.h#L25-L31):
1. **Limit Order**: Sweeps opposite matching orders and rests any unfilled remainder in the book.
2. **Market Order**: Sweeps liquidity using sentinel price levels (`INT_MAX` for buy market orders, `0` for sell market orders).
3. **ELO (Enhanced Limit Order)**: Sweeps up to 10 ticks/levels relative to the best opposite quote, resting any unfilled remainder on the book. Rejects if the price is $\ge 10$ ticks away from the best quote.
4. **SLO (Special Limit Order)**: Sweeps up to 10 ticks/levels and automatically cancels any unfilled remainder instead of resting on the book.
5. **AON (All-or-Nothing)**: Scans available cumulative depth upfront across eligible price levels. Fully fills if total available quantity $\ge$ order quantity; otherwise rejects immediately without modifying the order book.

---

## 3. Matching & Cancellation Engine

Implemented in [`order_book.cpp`](file:///c:/Users/jerie/Documents/GitHub/limit-order-book-cpp/order_book.cpp):

- **Continuous Matching ([`matchBuy`](file:///c:/Users/jerie/Documents/GitHub/limit-order-book-cpp/order_book.cpp#L59-L92) & [`matchSell`](file:///c:/Users/jerie/Documents/GitHub/limit-order-book-cpp/order_book.cpp#L95-L125))**:
  - Greedily matches incoming orders against the best opposite price level using FIFO priority.
  - Decrements remaining quantities and emits trade logs via `recordTrade`.
- **Cancellation ([`cancelOrder`](file:///c:/Users/jerie/Documents/GitHub/limit-order-book-cpp/order_book.cpp#L30-L53))**:
  - Performs $\mathcal{O}(1)$ lookup via hash map index, marks `status = OrderStatus::Cancelled`, and erases the index entry.
- **Lazy Purging ([`purgeCancelledFront`](file:///c:/Users/jerie/Documents/GitHub/limit-order-book-cpp/order_book.cpp#L9-L14))**:
  - Cleans cancelled orders lazily from the front of price queues prior to matching or iterating.
- **Book Display ([`printBook`](file:///c:/Users/jerie/Documents/GitHub/limit-order-book-cpp/order_book.cpp#L246-L296))**:
  - Prints aggregated depth for Ask (sellers) and Bid (buyers) sides along with active order counts, skipping sentinel price levels.

---

## 4. Verification & Testing

- **Order Type Scenarios** ([`main.cpp`](file:///c:/Users/jerie/Documents/GitHub/limit-order-book-cpp/main.cpp)):
  - Comprehensive unit test cases for ELO sweep & rest, boundary tick rejections, SLO remainder cancellations, and AON rejection/execution logic.
- **Stress Testing & Invariant Hooks** ([`stress_test.cpp`](file:///c:/Users/jerie/Documents/GitHub/limit-order-book-cpp/stress_test.cpp)):
  - Simulates 10,000 randomized order submissions and cancellations.
  - Validates key engine invariants via [`checkInvariants`](file:///c:/Users/jerie/Documents/GitHub/limit-order-book-cpp/stress_test.cpp#L14-L47):
    1. *No Crossed Book*: `Best Bid < Best Ask`.
    2. *Volume Balance*: Total buy filled quantity equals total sell filled quantity.
    3. *Index Integrity*: Cancelled orders never remain live in the lookup index.

---

## 5. Directory Structure & Key Files

| File / Folder | Purpose |
| :--- | :--- |
| [`order_book.h`](file:///c:/Users/jerie/Documents/GitHub/limit-order-book-cpp/order_book.h) | Structs (`Order`, `Trade`), Enums (`OrderStatus`, `OrderType`), `now_ns()` timer, and `OrderBook` class declaration. |
| [`order_book.cpp`](file:///c:/Users/jerie/Documents/GitHub/limit-order-book-cpp/order_book.cpp) | Core engine matching, insertion, cancellation, lazy purging, and book display logic. |
| [`main.cpp`](file:///c:/Users/jerie/Documents/GitHub/limit-order-book-cpp/main.cpp) | Demonstrations and test cases for ELO, SLO, and AON orders. |
| [`stress_test.cpp`](file:///c:/Users/jerie/Documents/GitHub/limit-order-book-cpp/stress_test.cpp) | 10,000 order simulation & invariant assertion checker (`checkInvariants`). |
| [`README.md`](file:///c:/Users/jerie/Documents/GitHub/limit-order-book-cpp/README.md) | High-level mental models, data structure decisions, and reference materials. |
| [`Roadmap.md`](file:///c:/Users/jerie/Documents/GitHub/limit-order-book-cpp/Roadmap.md) | Layer 1 (Correctness), Layer 2 (Performance), and Layer 3 (Distribution) master plan. |
| [`Notes/`](file:///c:/Users/jerie/Documents/GitHub/limit-order-book-cpp/Notes) | Detailed conceptual notes on HKEX rules, CME MDP 3.0, memory architectures, and matching performance. |
