#include <stdio.h>
#include <unicorn/unicorn.h>

#include "Mrow.h"

int main(int Argc, char *Argv[]) {
  char *FileName = NULL;

  for (int Idx = 1; Idx < Argc; Idx++) {
    char *Argument = Argv[Idx];
    FileName = Argument;
  }

  if (!FileName) {
    FatalCrash("No input file");
  }

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

  MrowProcess *Process = MrowCreateProcess();
  MrowLoadProcess(FileContent, FileSize, Process);
  MrowStartProcess(Process);

  return 0;
}
