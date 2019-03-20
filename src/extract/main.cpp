#include "extractfiles.h"
#include "extractpxt.h"
#include "extractstages.h"

#include <cstdio>
#include <cstdlib>

static const char *filename = "JenkasNightmare.exe";

int main(int argc, char *argv[])
{
  FILE *fp;

  fp = fopen(filename, "rb");
  if (!fp)
  {
    return 1;
  }

  if (extract_pxt(fp))
    printf("pxt failed!");
  if (extract_files(fp))
    printf("files failed!");
  if (extract_stages(fp))
    printf("stages failed!");
  fclose(fp);
  printf("Sucessfully extracted.\n");
  return 0;
}
