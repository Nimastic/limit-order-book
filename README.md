# limit-order-book

## Mental Model
- a limit book is just two sorted collections
- one high-to-low (bids)
- one low-to-high (asks)
- imagine (low <- high (buyers)) | matching point / difference is spread | ((sellers) low -> high)
- at same prive level, match in FIFO
- unmatched orders stays in the book until it is cancelled/expires/filled
- Tight spreads (0.01) mean a liquid, efficient market. Wide spreads mean thin or illiquid.

![Order book diagram](./assets/order_book_structure.svg)

## Lifecycle Of An Order
![Order Lifecycle](./assets/order_lifecycle_flowchart.svg)

## Hierachy of Exchange, Broker, Market Data Vendor:
- Exchange (HKEX, NYSE, SGX) — runs the matching engine, owns the order book
- Broker (moomoo, Saxo, Interactive Brokers) — takes your order and sends it to the exchange
- Market data vendor (Bloomberg, Refinitiv) — resells the exchange's data feed

## Learning Resources
### Before Writing Code
[KHEX Trading Mechanism](https://www.hkex.com.hk/Services/Trading/Securities/Overview/Trading-Mechanism?sc_lang=en) How an exchange describes price-time priority
- Why HKEX? Public transparency. HKEX publishes its trading rules, matching logic, and tick size tables in plain English, for free, without a login. HKEX's documentation is unusually clean and pedagogically useful.

[Building a Market Data Feed with Liquibook](https://objectcomputing.com/resources/publications/sett/august-2013-building-a-market-data-feed-with-liquibook) How the book connects to the wider exchange system, order gateway, market data feed etc

### After Creating Basic Order Book
[Liquibook C++ matching Engine](https://github.com/enewhuis/liquibook) Study after completing your own version

[How To Build a Fast Limit Order book](https://web.archive.org/web/20110205154238/http://howtohft.blogspot.com/2011/02/how-to-build-fast-limit-order-book.html)

[CME Group MDP3.0 Spec](https://cmegroupclientsite.atlassian.net/wiki/spaces/EPICSANDBOX/pages/457219613/CME+MDP+3.0+Market+Data) How real market data feeds work

### C++ Skills Reference
[C++ Ref](https://en.cppreference.com/)

## Data Structure Decision 
![Data Structure Decision](/assets/data_structure_comparison.svg)
- Map (RB tree)
- Map with iterator hint (RB tree)
- Skip List (layed linked list)
- Intrusive List + Hash Map (production HFT)

