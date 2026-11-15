#include <string.h>
#include <unicorn/unicorn.h>

#include "Mrow.h"

static void HookCode(uc_engine *Unicorn, uint64_t Address, uint32_t Size,
                     void *UserData) {
  uint32_t Instruction;

  uc_mem_read(Unicorn, Address, &Instruction, sizeof(Instruction));

  if ((Instruction & 0xffe0001f) == 0xd4000001) {
    MrowSyscall(Unicorn);

    uint64_t NewPc = Address + 4;
    uc_reg_write(Unicorn, UC_ARM64_REG_PC, &NewPc);
  }
}

MrowProcess *MrowCreateProcess(void) {
  MrowProcess *Process = Malloc(sizeof(MrowProcess));
  memset(Process, 0, sizeof(MrowProcess));

  uc_engine *Unicorn;
  UcError = uc_open(UC_ARCH_ARM64, UC_MODE_ARM, &Unicorn);
  if (UcError != UC_ERR_OK) {
    Crash("Failed to create unicorn engine: %s", uc_strerror(UcError));
  }

  Process->Unicorn = Unicorn;

  return Process;
}

void MrowStartProcess(MrowProcess *Process) {
  uc_err Error = uc_mem_map(Process->Unicorn, STACK_ADDRESS, STACK_SIZE,
                            UC_PROT_READ | UC_PROT_WRITE);
  if (Error != UC_ERR_OK) {
    Crash("Failed to map stack: %s", uc_strerror(Error));
  }

  uint64_t StackPointer = STACK_ADDRESS + STACK_SIZE;
  StackPointer &= ~0xFULL;
  uc_reg_write(Process->Unicorn, UC_ARM64_REG_SP, &StackPointer);

  uc_hook Hook;
  uc_hook_add(Process->Unicorn, &Hook, UC_HOOK_CODE, (void *)HookCode, NULL, 1,
              0);

  UcError = uc_emu_start(Process->Unicorn, Process->Entry, 0, 0, 0);
  if (UcError != UC_ERR_OK) {
    Crash("Failed to start process: %s", uc_strerror(UcError));
  }
}
