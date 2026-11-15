#include "Mrow.h"

#include <errno.h>
#include <stdint.h>

void MrowSyscallBrk(uc_engine *Unicorn, MrowProcess *Process) {
  uint64_t Address;

  uc_reg_read(Unicorn, UC_ARM64_REG_X0, &Address);

  if (Address == 0 || Address < Process->HeapStart) {
    uc_reg_write(Unicorn, UC_ARM64_REG_X0, &Process->ProgramBreak);
    return;
  }

  uint64_t OldEnd = (Process->ProgramBreak + PAGE_SIZE - 1) & ~(PAGE_SIZE - 1);
  uint64_t NewEnd = (Address + PAGE_SIZE - 1) & ~(PAGE_SIZE - 1);

  if (NewEnd > OldEnd) {
    uc_err Error = uc_mem_map(Unicorn, OldEnd, NewEnd - OldEnd,
                              UC_PROT_READ | UC_PROT_WRITE);

    if (Error != UC_ERR_OK) {
      uc_reg_write(Unicorn, UC_ARM64_REG_X0, &Process->ProgramBreak);
      return;
    }
  } else if (NewEnd < OldEnd) {
    uc_err Error = uc_mem_unmap(Unicorn, NewEnd, OldEnd - NewEnd);

    if (Error != UC_ERR_OK) {
      uc_reg_write(Unicorn, UC_ARM64_REG_X0, &Process->ProgramBreak);
      return;
    }
  }

  Process->ProgramBreak = Address;

  uc_reg_write(Unicorn, UC_ARM64_REG_X0, &Process->ProgramBreak);
}
