#include "Mrow.h"

void MrowSyscallWrite(uc_engine *Unicorn) {
  uint64_t Fd;
  uint64_t Address;
  uint64_t Size;

  uc_reg_read(Unicorn, UC_ARM64_REG_X0, &Fd);
  uc_reg_read(Unicorn, UC_ARM64_REG_X1, &Address);
  uc_reg_read(Unicorn, UC_ARM64_REG_X2, &Size);

  if (Size > 1024 * 1024) {
    Size = 1024 * 1024;
  }

  char *Buffer = Malloc(Size);

  UcError = uc_mem_read(Unicorn, Address, Buffer, Size);

  if (UcError != UC_ERR_OK) {
    Free(Buffer);

    uint64_t Result = (uint64_t)-14;
    uc_reg_write(Unicorn, UC_ARM64_REG_X0, &Result);
    return;
  }

  ssize_t Written = -1;

  if (Fd == STDOUT_FILENO || Fd == STDERR_FILENO) {
    Written = write(Fd, Buffer, Size);
  }

  Free(Buffer);

  uint64_t Result = Written >= 0 ? (uint64_t)Written : (uint64_t)-9;

  uc_reg_write(Unicorn, UC_ARM64_REG_X0, &Result);
}
