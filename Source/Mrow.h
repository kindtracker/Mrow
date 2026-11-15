#pragma once
#include <stddef.h>
#include <stdio.h>
#include <unicorn/unicorn.h>

#define Crash(...)                                                             \
  do {                                                                         \
    fflush(stdout);                                                            \
    fprintf(stderr, "\x1b[1;37mMrow:\x1b[0m "                                  \
                    "\x1b[1;31mError:\x1b[0m ");                               \
    fprintf(stderr, __VA_ARGS__);                                              \
    fputc('\n', stderr);                                                       \
    exit(1);                                                                   \
  } while (0)

#define FatalCrash(...)                                                        \
  do {                                                                         \
    fflush(stdout);                                                            \
    fprintf(stderr, "\x1b[1;37mMrow:\x1b[0m "                                  \
                    "\x1b[1;31mFatal Error:\x1b[0m ");                         \
    fprintf(stderr, __VA_ARGS__);                                              \
    fputc('\n', stderr);                                                       \
    exit(1);                                                                   \
  } while (0)

#define STACK_ADDRESS 0x10000000
#define STACK_SIZE 0x100000
#define PAGE_SIZE 0x1000

typedef struct {
  uc_engine *Unicorn;
  uint64_t Entry;
  uint64_t HeapStart;
  uint64_t ProgramBreak;
  uint64_t Tid;
  uint64_t ClearChildTid;
  uint64_t RobustList;
} MrowProcess;

extern uc_err UcError;

void *Malloc(size_t Size);
void Free(void *Pointer);

MrowProcess *MrowCreateProcess(void);
void MrowLoadProcess(char *FileContent, size_t FileSize, MrowProcess *Process);
void MrowStartProcess(MrowProcess *Process);
void MrowSyscall(uc_engine *Unicorn, MrowProcess *Process);

void MrowSyscallWrite(uc_engine *Unicorn, MrowProcess *Process);
void MrowSyscallExit(uc_engine *Unicorn, MrowProcess *Process);
void MrowSyscallBrk(uc_engine *Unicorn, MrowProcess *Process);
void MrowSyscallMmap(uc_engine *Unicorn, MrowProcess *Process);
void MrowSyscallExitGroup(uc_engine *Unicorn, MrowProcess *Process);
void MrowSyscallSetTidAddress(uc_engine *Unicorn, MrowProcess *Process);
void MrowSyscallSetRobustList(uc_engine *Unicorn, MrowProcess *Process);
void MrowSyscallRseq(uc_engine *Unicorn, MrowProcess *Process);
void MrowSyscallPrlimit64(uc_engine *Unicorn, MrowProcess *Process);
void MrowSyscallGetRandom(uc_engine *Unicorn, MrowProcess *Process);
void MrowSyscallReadlinkat(uc_engine *Unicorn, MrowProcess *Process);
void MrowSyscallMprotect(uc_engine *Unicorn, MrowProcess *Process);
void MrowSyscallFstat(uc_engine *Unicorn, MrowProcess *Process);
