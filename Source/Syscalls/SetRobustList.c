#include "Mrow.h"

void MrowSyscallSetRobustList(uc_engine *Unicorn, MrowProcess *Process) {
  uint64_t Head;
  uint64_t Length;

  uc_reg_read(Unicorn, UC_ARM64_REG_X0, &Head);
  uc_reg_read(Unicorn, UC_ARM64_REG_X1, &Length);

  // TODO: Validate that Head is a valid userspace address.
  // TODO: Validate Length against sizeof(struct robust_list_head).
  // TODO: Store the robust list head per thread.
  Process->RobustList = Head;

  // TODO: Return -EINVAL if Length is invalid.

  uint64_t Result = 0;

  uc_reg_write(Unicorn, UC_ARM64_REG_X0, &Result);
}
