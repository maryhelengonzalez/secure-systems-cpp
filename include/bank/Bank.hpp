#include <memory>
#pragma once
#include "bank/Ledger.hpp"
#include <cstdint>
#include <mutex>
#include <string>
#include <vector>

namespace bank {

struct Account {
  int id;
  int64_t balance;       // cents
  mutable std::mutex mu; // per-account lock
  Account(int id_, int64_t bal_) : id(id_), balance(bal_) {}

};

class Bank {
 public:
  explicit Bank(int num_accounts, int64_t initial_balance_cents);

  // Returns false if invalid account, negative amount, or insufficient funds
  bool deposit(int acct, int64_t cents, std::string note = "");
  bool withdraw(int acct, int64_t cents, std::string note = "");

  // Deadlock-safe transfer using std::scoped_lock over two mutexes
  bool transfer(int from, int to, int64_t cents, std::string note = "");

  int64_t get_balance(int acct) const;
  int64_t total_balance() const; // invariant checker helper

  const Ledger& ledger() const { return ledger_; }

 private:
  bool valid(int acct) const;
  std::vector<std::unique_ptr<Account>> accounts_;
  Ledger ledger_;
};

} // namespace bank
