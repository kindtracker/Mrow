#include <errno.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "Mrow.h"
#include <unicorn/unicorn.h>

void MrowSyscallGetCwd(uc_engine *Unicorn, MrowProcess *Process) {
  (void)Process;

  uint64_t BufferAddress;
  uint64_t BufferSize;

  uc_reg_read(Unicorn, UC_ARM64_REG_X0, &BufferAddress);
  uc_reg_read(Unicorn, UC_ARM64_REG_X1, &BufferSize);

  if (BufferSize == 0 || BufferSize > SIZE_MAX) {
    goto Invalid;
  }

  char *Buffer = malloc((size_t)BufferSize);
  if (!Buffer) {
    goto NoMemory;
  }

  if (!getcwd(Buffer, (size_t)BufferSize)) {
    int Error = errno;
    free(Buffer);

    uint64_t ReturnValue = (uint64_t)(int64_t)-Error;
    uc_reg_write(Unicorn, UC_ARM64_REG_X0, &ReturnValue);
    return;
  }

  size_t Length = strlen(Buffer) + 1;

  if (uc_mem_write(Unicorn, BufferAddress, Buffer, Length) != UC_ERR_OK) {
    free(Buffer);
    goto Fault;
  }

  free(Buffer);

  // Linux getcwd returns the length including the null terminator.
  uint64_t ReturnValue = Length;
  uc_reg_write(Unicorn, UC_ARM64_REG_X0, &ReturnValue);
  return;

Invalid:
  {
    uint64_t ReturnValue = (uint64_t)(int64_t)-EINVAL;
    uc_reg_write(Unicorn, UC_ARM64_REG_X0, &ReturnValue);
    return;
  }

NoMemory:
  {
    uint64_t ReturnValue = (uint64_t)(int64_t)-ENOMEM;
    uc_reg_write(Unicorn, UC_ARM64_REG_X0, &ReturnValue);
    return;
  }

Fault:
  {
    uint64_t ReturnValue = (uint64_t)(int64_t)-EFAULT;
    uc_reg_write(Unicorn, UC_ARM64_REG_X0, &ReturnValue);
  }

  // TODO: Return the guest process's working directory, not Mrow's host cwd.
}
