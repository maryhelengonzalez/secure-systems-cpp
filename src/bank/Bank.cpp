#include "bank/Bank.hpp"
#include "bank/Types.hpp"
#include <mutex>
#include <utility>

namespace bank {

Bank::Bank(int num_accounts, int64_t initial_balance_cents) {
  accounts_.reserve(num_accounts);
  for (int i = 0; i < num_accounts; ++i) {
    accounts_.push_back(std::make_unique<Account>(i, initial_balance_cents));

  }
}

bool Bank::valid(int acct) const {
  return acct >= 0 && acct < static_cast<int>(accounts_.size());
}

bool Bank::deposit(int acct, int64_t cents, std::string note) {
  if (!valid(acct) || cents <= 0) return false;
  auto& a = *accounts_[acct];
  {
    std::lock_guard<std::mutex> lk(a.mu);
    a.balance += cents;
  }
  ledger_.append(TxType::Deposit, -1, acct, cents, std::move(note));
  return true;
}

bool Bank::withdraw(int acct, int64_t cents, std::string note) {
  if (!valid(acct) || cents <= 0) return false;
  auto& a = *accounts_[acct];
  bool ok = false;
  {
    std::lock_guard<std::mutex> lk(a.mu);
    if (a.balance >= cents) {
      a.balance -= cents;
      ok = true;
    }
  }
  if (ok) ledger_.append(TxType::Withdraw, acct, -1, cents, std::move(note));
  return ok;
}

bool Bank::transfer(int from, int to, int64_t cents, std::string note) {
  if (!valid(from) || !valid(to) || from == to || cents <= 0) return false;

  auto& A = *accounts_[from];
  auto& B = *accounts_[to];

  // scoped_lock locks both without deadlock
  std::scoped_lock lk(A.mu, B.mu);

  if (A.balance < cents) return false;

  A.balance -= cents;
  B.balance += cents;

  ledger_.append(TxType::Transfer, from, to, cents, std::move(note));
  return true;
}

int64_t Bank::get_balance(int acct) const {
  if (!valid(acct)) return 0;
  auto& a = *accounts_[acct];
  std::lock_guard<std::mutex> lk(a.mu);
  return a.balance;
}

int64_t Bank::total_balance() const {
  int64_t total = 0;
  // lock each account briefly; OK for debugging/invariants
  for (auto& p : accounts_) {
  auto& a = *p;
  std::lock_guard<std::mutex> lk(a.mu);
  total += a.balance;
}

  return total;
}

} // namespace bank
