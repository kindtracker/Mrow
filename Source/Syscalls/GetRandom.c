#include "Mrow.h"

#include <errno.h>
#include <stdint.h>
#include <sys/random.h>

#define MROW_MAX_RANDOM_SIZE (1024 * 1024)

void MrowSyscallGetRandom(uc_engine *Unicorn, MrowProcess *Process) {
  uint64_t Address;
  uint64_t Size;
  uint64_t Flags;

  uc_reg_read(Unicorn, UC_ARM64_REG_X0, &Address);
  uc_reg_read(Unicorn, UC_ARM64_REG_X1, &Size);
  uc_reg_read(Unicorn, UC_ARM64_REG_X2, &Flags);

  // TODO: Validate supported flags (GRND_NONBLOCK and GRND_RANDOM).
  // TODO: Return -EINVAL for unsupported flags.
  // TODO: Validate the guest memory address.
  // TODO: Handle requests larger than the maximum supported size.
  // TODO: Consider implementing guest-specific random state.

  (void)Process;

  if (Size > MROW_MAX_RANDOM_SIZE) {
    uint64_t Result = (uint64_t)-EINVAL;
    uc_reg_write(Unicorn, UC_ARM64_REG_X0, &Result);
    return;
  }

  if (Size == 0) {
    uint64_t Result = 0;
    uc_reg_write(Unicorn, UC_ARM64_REG_X0, &Result);
    return;
  }

  char *Buffer = Malloc(Size);

  if (Buffer == NULL) {
    uint64_t Result = (uint64_t)-ENOMEM;
    uc_reg_write(Unicorn, UC_ARM64_REG_X0, &Result);
    return;
  }

  ssize_t Generated = getrandom(Buffer, Size, (unsigned int)Flags);
  int ErrorNumber = errno;

  if (Generated < 0) {
    Free(Buffer);

    uint64_t Result = (uint64_t)-ErrorNumber;
    uc_reg_write(Unicorn, UC_ARM64_REG_X0, &Result);
    return;
  }

  uc_err Error = uc_mem_write(Unicorn, Address, Buffer, Generated);

  Free(Buffer);

  if (Error != UC_ERR_OK) {
    uint64_t Result = (uint64_t)-EFAULT;
    uc_reg_write(Unicorn, UC_ARM64_REG_X0, &Result);
    return;
  }

  uint64_t Result = (uint64_t)Generated;
  uc_reg_write(Unicorn, UC_ARM64_REG_X0, &Result);
}
