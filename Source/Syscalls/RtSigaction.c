#include <errno.h>
#include <signal.h>
#include <stdint.h>
#include <string.h>

#include "Mrow.h"
#include <unicorn/unicorn.h>

typedef struct {
  uint64_t Handler;
  uint64_t Flags;
  uint64_t Restorer;
  uint64_t Mask;
} MrowSigaction;

void MrowSyscallRtSigaction(uc_engine *Unicorn, MrowProcess *Process) {
  (void)Process;

  uint64_t Signal;
  uint64_t ActionAddress;
  uint64_t OldActionAddress;
  uint64_t SignalSetSize;

  uc_reg_read(Unicorn, UC_ARM64_REG_X0, &Signal);
  uc_reg_read(Unicorn, UC_ARM64_REG_X1, &ActionAddress);
  uc_reg_read(Unicorn, UC_ARM64_REG_X2, &OldActionAddress);
  uc_reg_read(Unicorn, UC_ARM64_REG_X3, &SignalSetSize);

  if (SignalSetSize != 8 || Signal == 0 || Signal >= NSIG ||
      Signal == SIGKILL || Signal == SIGSTOP) {
    goto Invalid;
  }

  struct sigaction HostAction;
  struct sigaction OldHostAction;

  memset(&HostAction, 0, sizeof(HostAction));
  memset(&OldHostAction, 0, sizeof(OldHostAction));

  if (ActionAddress) {
    MrowSigaction GuestAction;

    if (uc_mem_read(Unicorn, ActionAddress, &GuestAction,
                    sizeof(GuestAction)) != UC_ERR_OK) {
      goto Fault;
    }

    HostAction.sa_handler = (void (*)(int))(uintptr_t)GuestAction.Handler;
    HostAction.sa_flags = (int)GuestAction.Flags;
    HostAction.sa_restorer = (void (*)(void))(uintptr_t)GuestAction.Restorer;

    sigemptyset(&HostAction.sa_mask);

    // TODO: Translate the guest signal mask into a host sigset_t.
  }

  if (sigaction((int)Signal, ActionAddress ? &HostAction : NULL,
                OldActionAddress ? &OldHostAction : NULL) < 0) {
    goto HostError;
  }

  if (OldActionAddress) {
    MrowSigaction GuestOldAction = {
        .Handler = (uint64_t)(uintptr_t)OldHostAction.sa_handler,
        .Flags = (uint64_t)OldHostAction.sa_flags,
        .Restorer = (uint64_t)(uintptr_t)OldHostAction.sa_restorer,
        .Mask = 0};

    if (uc_mem_write(Unicorn, OldActionAddress, &GuestOldAction,
                     sizeof(GuestOldAction)) != UC_ERR_OK) {
      goto Fault;
    }
  }

  {
    uint64_t ReturnValue = 0;
    uc_reg_write(Unicorn, UC_ARM64_REG_X0, &ReturnValue);
    return;
  }

Invalid:
  {
    uint64_t ReturnValue = (uint64_t)(int64_t)-EINVAL;
    uc_reg_write(Unicorn, UC_ARM64_REG_X0, &ReturnValue);
    return;
  }

Fault:
  {
    uint64_t ReturnValue = (uint64_t)(int64_t)-EFAULT;
    uc_reg_write(Unicorn, UC_ARM64_REG_X0, &ReturnValue);
    return;
  }

HostError:
  {
    uint64_t ReturnValue = (uint64_t)(int64_t)-errno;
    uc_reg_write(Unicorn, UC_ARM64_REG_X0, &ReturnValue);
  }

  // TODO: Keep signal handlers in the guest process instead of installing
  // guest addresses as handlers in Mrow's host process.
  // TODO: Translate guest signal masks and flags.
  // TODO: Implement guest signal delivery and sigreturn.
}
