#include "Mrow.h"

void MrowSyscallSetTidAddress(uc_engine *Unicorn, MrowProcess *Process) {
  uint64_t Address;

  uc_reg_read(Unicorn, UC_ARM64_REG_X0, &Address);

  // TODO: Validate that Address is a valid userspace address.
  Process->ClearChildTid = Address;

  // TODO: Return the calling thread's actual TID.
  // TODO: Implement proper thread management.
  uint64_t Result = Process->Tid;

  uc_reg_write(Unicorn, UC_ARM64_REG_X0, &Result);
}

// TODO: When a thread exits:
// 1. Write zero to ClearChildTid.
// 2. Perform a FUTEX_WAKE operation on ClearChildTid.
// 3. Release the thread's TLS block.
// 4. Release other thread-specific resources.
