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

typedef struct {
  uc_engine *Unicorn;
  uint64_t Entry;
} MrowProcess;

extern uc_err UcError;

void *Malloc(size_t Size);
void Free(void *Pointer);

MrowProcess *MrowCreateProcess(void);
void MrowLoadProcess(char *FileContent, size_t FileSize, MrowProcess *Process);
