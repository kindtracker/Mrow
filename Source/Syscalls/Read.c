#include <errno.h>
#include <stdint.h>
#include <unistd.h>

#include "Mrow.h"
#include <unicorn/unicorn.h>

void MrowSyscallRead(uc_engine *Unicorn, MrowProcess *Process) {
  (void)Process;

  uint64_t Fd;
  uint64_t BufferAddress;
  uint64_t Count;

  uc_reg_read(Unicorn, UC_ARM64_REG_X0, &Fd);
  uc_reg_read(Unicorn, UC_ARM64_REG_X1, &BufferAddress);
  uc_reg_read(Unicorn, UC_ARM64_REG_X2, &Count);

  if (Count > INT64_MAX) {
    goto Invalid;
  }

  // TODO: Validate the complete guest buffer before reading from the host fd.
  // Otherwise, a guest memory fault could occur after the host fd has advanced.

  {
    size_t ChunkSize = 4096;
    char Buffer[4096];
    uint64_t TotalRead = 0;

    while (TotalRead < Count) {
      size_t Remaining = (size_t)(Count - TotalRead);
      size_t ToRead = Remaining < ChunkSize ? Remaining : ChunkSize;

      ssize_t BytesRead = read((int)Fd, Buffer, ToRead);

      if (BytesRead < 0) {
        if (TotalRead == 0) {
          goto HostError;
        }
        break;
      }

      if (BytesRead == 0) {
        break;
      }

      if (uc_mem_write(Unicorn, BufferAddress + TotalRead, Buffer,
                       (size_t)BytesRead) != UC_ERR_OK) {
        goto Fault;
      }

      TotalRead += (uint64_t)BytesRead;

      if ((size_t)BytesRead < ToRead) {
        break;
      }
    }

    uint64_t ReturnValue = TotalRead;
    uc_reg_write(Unicorn, UC_ARM64_REG_X0, &ReturnValue);
    return;
  }

Invalid:
  {
    uint64_t ReturnValue = (uint64_t)(int64_t)-EINVAL;
    uc_reg_write(Unicorn, UC_ARM64_REG_X0, &ReturnValue);
    return;
  }

HostError:
  {
    uint64_t ReturnValue = (uint64_t)(int64_t)-errno;
    uc_reg_write(Unicorn, UC_ARM64_REG_X0, &ReturnValue);
    return;
  }

Fault:
  {
    uint64_t ReturnValue = (uint64_t)(int64_t)-EFAULT;
    uc_reg_write(Unicorn, UC_ARM64_REG_X0, &ReturnValue);
    return;
  }
}
