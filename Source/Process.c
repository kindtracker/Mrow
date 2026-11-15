#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <unicorn/unicorn.h>

#include "Mrow.h"

#define CHECK_UC(Expression, Message)                                          \
  do {                                                                         \
    UcError = (Expression);                                                    \
    if (UcError != UC_ERR_OK) {                                                \
      Crash("%s: %s", Message, uc_strerror(UcError));                          \
    }                                                                          \
  } while (0)

static void HookCode(uc_engine *Unicorn, uint64_t Address, uint32_t Size,
                     MrowProcess *Process) {
  (void)Size;

  uint32_t Instruction;

  CHECK_UC(uc_mem_read(Unicorn, Address, &Instruction, sizeof(Instruction)),
           "Failed to read instruction");

  if ((Instruction & 0xffe0001f) == 0xd4000001) {
    MrowSyscall(Unicorn, Process);

    uint64_t NewPC = Address + 4;

    CHECK_UC(uc_reg_write(Unicorn, UC_ARM64_REG_PC, &NewPC),
             "Failed to advance PC");
  }
}

static bool HookMemoryInvalid(uc_engine *Unicorn, uc_mem_type Type,
                              uint64_t Address, int Size, int64_t Value,
                              void *UserData) {
  (void)Unicorn;
  (void)Value;
  (void)UserData;

  const char *Operation = "Unknown";

  switch (Type) {
  case UC_MEM_READ_UNMAPPED:
    Operation = "Read from unmapped memory";
    break;
  case UC_MEM_WRITE_UNMAPPED:
    Operation = "Write to unmapped memory";
    break;
  case UC_MEM_FETCH_UNMAPPED:
    Operation = "Instruction fetch from unmapped memory";
    break;
  case UC_MEM_READ_PROT:
    Operation = "Read protection violation";
    break;
  case UC_MEM_WRITE_PROT:
    Operation = "Write protection violation";
    break;
  case UC_MEM_FETCH_PROT:
    Operation = "Execute protection violation";
    break;
  default:
    break;
  }

  Crash("Memory: %s at 0x%016llx (Size=%d)", Operation,
        (unsigned long long)Address, Size);

  return false;
}

static void HookInterrupt(uc_engine *Unicorn, uint32_t Number, void *UserData) {
  (void)Unicorn;
  (void)UserData;

  printf("Interrupt: Number=%u\n", Number);
}

static bool HookInvalidInstruction(uc_engine *Unicorn, void *UserData) {
  (void)UserData;

  uint64_t PC = 0;
  uc_reg_read(Unicorn, UC_ARM64_REG_PC, &PC);

  Crash("Invalid instruction at PC=0x%016llx", (unsigned long long)PC);

  return false;
}

MrowProcess *MrowCreateProcess(void) {
  MrowProcess *Process = Malloc(sizeof(MrowProcess));
  memset(Process, 0, sizeof(MrowProcess));

  uc_engine *Unicorn;

  UcError = uc_open(UC_ARCH_ARM64, UC_MODE_ARM, &Unicorn);

  if (UcError != UC_ERR_OK) {
    Crash("Failed to create Unicorn engine: %s", uc_strerror(UcError));
  }

  Process->Unicorn = Unicorn;

  return Process;
}

void MrowStartProcess(MrowProcess *Process) {
  CHECK_UC(uc_mem_map(Process->Unicorn, STACK_ADDRESS, STACK_SIZE,
                      UC_PROT_READ | UC_PROT_WRITE),
           "Failed to map stack");

  uint64_t StackPointer = STACK_ADDRESS + STACK_SIZE - 0x20;
  StackPointer &= ~0xFULL;

  CHECK_UC(uc_reg_write(Process->Unicorn, UC_ARM64_REG_SP, &StackPointer),
           "Failed to initialize stack pointer");

  uc_hook CodeHook;
  uc_hook InvalidMemoryHook;
  uc_hook InterruptHook;
  uc_hook InvalidInstructionHook;

  CHECK_UC(uc_hook_add(Process->Unicorn, &CodeHook, UC_HOOK_CODE,
                       (void *)HookCode, Process, 1, 0),
           "Failed to install code hook");

  CHECK_UC(uc_hook_add(Process->Unicorn, &InvalidMemoryHook,
                       UC_HOOK_MEM_INVALID, (void *)HookMemoryInvalid, NULL, 1,
                       0),
           "Failed to install invalid memory hook");

  CHECK_UC(uc_hook_add(Process->Unicorn, &InterruptHook, UC_HOOK_INTR,
                       (void *)HookInterrupt, NULL, 1, 0),
           "Failed to install interrupt hook");

  CHECK_UC(uc_hook_add(Process->Unicorn, &InvalidInstructionHook,
                       UC_HOOK_INSN_INVALID, (void *)HookInvalidInstruction,
                       NULL, 1, 0),
           "Failed to install invalid instruction hook");

  UcError = uc_emu_start(Process->Unicorn, Process->Entry, 0, 0, 0);

  if (UcError != UC_ERR_OK) {
    Crash("Process: Execution failed: %s", uc_strerror(UcError));
  }
}
