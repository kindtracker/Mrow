#pragma once
#include <stddef.h>
#include <stdio.h>

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

void *Malloc(size_t Size);
