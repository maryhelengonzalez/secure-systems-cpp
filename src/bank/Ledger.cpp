#include "bank/Ledger.hpp"
#include <utility>

namespace bank {

void Ledger::append(TxType type, int from, int to, int64_t cents, std::string note) {
  std::lock_guard<std::mutex> lock(mu_);
  entries_.push_back(LedgerEntry{type, from, to, cents, std::move(note)});
}

// ✅ ADD THIS:
std::vector<LedgerEntry> Ledger::snapshot() const {
  std::lock_guard<std::mutex> lock(mu_);
  return entries_; // returns a copy (safe)
}

} // namespace bank
