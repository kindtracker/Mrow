#include "Mrow.h"

#include <errno.h>
#include <stdint.h>

void MrowSyscallRseq(uc_engine *Unicorn, MrowProcess *Process) {
  uint64_t Address;
  uint64_t Length;
  uint64_t Flags;
  uint64_t Signature;

  uc_reg_read(Unicorn, UC_ARM64_REG_X0, &Address);
  uc_reg_read(Unicorn, UC_ARM64_REG_X1, &Length);
  uc_reg_read(Unicorn, UC_ARM64_REG_X2, &Flags);
  uc_reg_read(Unicorn, UC_ARM64_REG_X3, &Signature);

  // TODO: Validate the rseq structure address.
  // TODO: Validate Length against sizeof(struct rseq).
  // TODO: Validate Flags and Signature.
  // TODO: Store the rseq registration per thread.
  // TODO: Implement restartable sequence handling.
  (void)Process;
  (void)Address;
  (void)Length;
  (void)Flags;
  (void)Signature;

  // TODO: Return -EINVAL for invalid arguments.
  // TODO: Return -EBUSY if already registered.

  uint64_t Result = (uint64_t)-ENOSYS;
  uc_reg_write(Unicorn, UC_ARM64_REG_X0, &Result);
}
