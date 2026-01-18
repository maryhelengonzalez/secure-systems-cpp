#pragma once
#include "bank/Types.hpp"
#include <mutex>
#include <string>
#include <vector>

namespace bank {

struct LedgerEntry {
  TxType type;
  int from;
  int to;
  int64_t cents;
  std::string note;
};

class Ledger {
 public:
  void append(TxType type, int from, int to, int64_t cents, std::string note);

  // ✅ ADD THIS:
  std::vector<LedgerEntry> snapshot() const;

 private:
  mutable std::mutex mu_;
  std::vector<LedgerEntry> entries_;
};

} // namespace bank
