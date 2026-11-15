#include <stdio.h>
#include <string.h>
#include <unicorn/unicorn.h>

#include "Mrow.h"

int main(int Argc, char *Argv[]) {
  if (Argc < 2) {
    FatalCrash("No input file");
  }

  char *FileName = Argv[1];

  FILE *FilePtr = fopen(FileName, "rb");
  if (!FilePtr) {
    FatalCrash("%s: No such file or directory", FileName);
  }

  fseek(FilePtr, 0, SEEK_END);
  size_t FileSize = ftell(FilePtr);
  fseek(FilePtr, 0, SEEK_SET);

  char *FileContent = Malloc(FileSize + 1);
  fread(FileContent, 1, FileSize, FilePtr);
  FileContent[FileSize] = '\0';
  fclose(FilePtr);

  memmove(&Argv[0], &Argv[1], sizeof(char *) * (Argc - 1));
  Argc--;
  Argv[0] = "busybox";

  MrowProcess *Process = MrowCreateProcess();
  MrowLoadProcess(FileContent, FileSize, Process);
  MrowStartProcess(Process, Argc, Argv);

  return 0;
}
