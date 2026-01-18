#include "bank/Bank.hpp"
#include <cassert>
#include <iostream>
#include <thread>
#include <vector>

using bank::Bank;

static void test_basic_ops() {
  Bank b(3, 10000); // $100.00 each if cents
  assert(b.deposit(0, 500, "paycheck"));
  assert(b.withdraw(1, 250, "coffee"));
  assert(!b.withdraw(1, 999999, "too much"));
  assert(b.transfer(0, 2, 1000, "rent"));

  assert(b.get_balance(0) == 10000 + 500 - 1000);
  assert(b.get_balance(1) == 10000 - 250);
  assert(b.get_balance(2) == 10000 + 1000);

  auto txs = b.ledger().snapshot();
  assert(txs.size() == 3);
}

static void test_concurrent_transfers_invariant() {
  const int N = 10;
  Bank b(N, 100000); // $1000 each
  const int64_t start_total = b.total_balance();

  const int threads = 8;
  const int ops_per_thread = 5000;

  auto worker = [&]() {
    for (int i = 0; i < ops_per_thread; ++i) {
      int from = i % N;
      int to = (i * 7 + 3) % N;
      if (from == to) to = (to + 1) % N;
      b.transfer(from, to, 1, "xfer"); // 1 cent
    }
  };

  std::vector<std::thread> ts;
  ts.reserve(threads);
  for (int t = 0; t < threads; ++t) ts.emplace_back(worker);
  for (auto& th : ts) th.join();

  const int64_t end_total = b.total_balance();
  assert(start_total == end_total); // invariant: transfers preserve total
  std::cout << "[PASS] invariant preserved under concurrent transfers\n";
}

int main() {
  test_basic_ops();
  test_concurrent_transfers_invariant();
  std::cout << "ALL BANK TESTS PASSED\n";
  return 0;
}
