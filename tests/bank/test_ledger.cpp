#include "bank/Ledger.hpp"

#include <cassert>
#include <iostream>
#include <thread>
#include <vector>

using namespace bank;

static void test_record_and_snapshot() {
  Ledger ledger;

  ledger.record(1, +500, 1000, "deposit");
  ledger.record(1, -200, 1001, "withdraw");
  ledger.record(2, +999, 1002, "deposit");

  auto all = ledger.snapshot();
  assert(all.size() == 3);

  auto a1 = ledger.for_account(1);
  assert(a1.size() == 2);
  assert(a1[0].delta_cents == 500);
  assert(a1[1].delta_cents == -200);

  auto a2 = ledger.for_account(2);
  assert(a2.size() == 1);
  assert(a2[0].delta_cents == 999);
}

static void test_thread_safety_append_only() {
  Ledger ledger;

  constexpr int kThreads = 8;
  constexpr int kPerThread = 2000;

  std::vector<std::thread> threads;
  threads.reserve(kThreads);

  for (int t = 0; t < kThreads; ++t) {
    threads.emplace_back([&, t] {
      for (int i = 0; i < kPerThread; ++i) {
        ledger.record(static_cast<AccountId>(t), 1, 1000 + i, "t");
      }
    });
  }

  for (auto& th : threads) th.join();

  const auto total = ledger.size();
  assert(total == static_cast<std::size_t>(kThreads * kPerThread));

  // Spot-check: per-account counts should match.
  for (int t = 0; t < kThreads; ++t) {
    auto entries = ledger.for_account(static_cast<AccountId>(t));
    assert(entries.size() == static_cast<std::size_t>(kPerThread));
  }
}

int main() {
  test_record_and_snapshot();
  test_thread_safety_append_only();
  std::cout << "ALL LEDGER TESTS PASSED\n";
  return 0;
}
