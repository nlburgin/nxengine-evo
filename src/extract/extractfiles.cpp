
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <sys/stat.h>

#if defined(_WIN32)
#include <direct.h>
#include <io.h>
#endif

#include "crc.h"
#include "extractfiles.h"

#define HEADER_LEN 54
#define MAX_FILE_SIZE 62768
#define CREDIT_SIZE 39518

// Windows .bmp resources don't include the BMP-file headers
//Jenka's Nightmare appears to use a different BMP variant than the original
const uint8_t credit_header[] = {0x42, 0x4D, 0x1A, 0x65, 0x0A, 0x00, 0x00, 0x00, 0x00, 0x00, 0x36, 0x04, 0x00,
                                 0x00, 0x28, 0x00, 0x00, 0x00, 0xA0, 0x00, 0x00, 0x00, 0xF0, 0x00, 0x00, 0x00, 
                                 0x01, 0x00, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x28, 0x9A, 0x00, 0x00, 0xC3, 
                                 0x0E, 0x00, 0x00, 0xC3, 0x0E, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00};

const uint8_t pixel_header[] = {0x42, 0x4D, 0x1A, 0x65, 0x0A, 0x00, 0x00, 0x00, 0x00, 0x00, 0x36, 0x04, 0x00,
                                 0x00, 0x28, 0x00, 0x00, 0x00, 0xA0, 0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00, 
                                 0x01, 0x00, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x28, 0x9A, 0x00, 0x00, 0xC3, 
                                 0x0E, 0x00, 0x00, 0xC3, 0x0E, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00};
                                /*{0x42, 0x4D, 0x76, 0x05, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x76, 0x00, 0x00,
                                0x00, 0x28, 0x00, 0x00, 0x00, 0xA0, 0x00, 0x00, 0x00, 0x10, 0x00, 0x00};*/


static struct
{
  const char *filename;
  uint32_t offset;
  uint32_t length;
  uint32_t crc; //unused
  const uint8_t *header;
} files[] = {{"data/endpic/credit01.bmp", 0x133B1C, CREDIT_SIZE, 0, credit_header},
             {"data/endpic/credit02.bmp", 0x13D544, CREDIT_SIZE, 0, credit_header},
             {"data/endpic/credit03.bmp", 0x146F6C, CREDIT_SIZE, 0, credit_header},
             {"data/endpic/credit04.bmp", 0x150994, CREDIT_SIZE, 0, credit_header},
             {"data/endpic/credit05.bmp", 0x15A3BC, CREDIT_SIZE, 0, credit_header},
             {"data/endpic/credit06.bmp", 0x163DE4, CREDIT_SIZE, 0, credit_header},
             {"data/endpic/credit07.bmp", 0x16D80C, CREDIT_SIZE, 0, credit_header},
             {"data/endpic/credit08.bmp", 0x177234, CREDIT_SIZE, 0, credit_header},
             {"data/endpic/credit09.bmp", 0x180C5C, CREDIT_SIZE, 0, credit_header},
             {"data/endpic/credit10.bmp", 0x18A684, CREDIT_SIZE, 0, credit_header},
             {"data/endpic/credit11.bmp", 0x1940AC, CREDIT_SIZE, 0, credit_header},
             {"data/endpic/credit12.bmp", 0x19DAD4, CREDIT_SIZE, 0, credit_header},
             {"data/endpic/credit14.bmp", 0x1A74FC, CREDIT_SIZE, 0, credit_header},
             {"data/endpic/credit15.bmp", 0x1B0F24, CREDIT_SIZE, 0, credit_header},
             {"data/endpic/credit16.bmp", 0x1BA94C, CREDIT_SIZE, 0, credit_header},
             {"data/endpic/credit17.bmp", 0x1C4374, CREDIT_SIZE, 0, credit_header},
             {"data/endpic/credit18.bmp", 0x1CDD9C, CREDIT_SIZE, 0, credit_header},
             {"data/endpic/pixel.bmp", 1931204, 3706, 0, pixel_header},
             {"data/wavetable.dat", 0x12D11C, 25600, 0xb3a3b7ef, NULL},
             {"data/org/access.org", 664412, 16988, 0, NULL}, //"Halloween 2"
             {"data/org/balcony.org", 690596, 3082, 0, NULL},
             {"data/org/ginsuke.org", 780248, 5970, 0, NULL},
             {"data/org/bdown.org", 697252, 2570, 0, NULL},
             {"data/org/cemetery.org", 699824, 4578, 0, NULL},
             {"data/org/lastbtl.org", 903948, 27212, 0, NULL}, //"Recharge", a massively extended remix of the original
             {"data/org/ending.org", 716412, 17898, 0, NULL},
             {"data/org/wanpaku.org", 1160664, 19626, 0, NULL},
             {"data/org/fireeye.org", 757752, 21354, 0, NULL},
             {"data/org/fanfale1.org", 754836, 914, 0, NULL},
             {"data/org/fanfale2.org", 755752, 1074,0, NULL},
             {"data/org/fanfale3.org", 756676, 922, 0, NULL},
             {"data/org/gameover.org", 779108, 1138, 0, NULL},
             {"data/org/grand.org", 786220, 13466, 0, NULL},
             {"data/org/vivi.org", 1134208, 10458, 0, NULL},
             {"data/org/gravity.org", 799688, 20578, 0, NULL},
             {"data/org/weed.org", 1180292, 23706, 0, NULL},
             {"data/org/hell.org", 820268, 18386, 0, NULL},
             {"data/org/requiem.org", 1054536, 61188, 0, NULL},//Super Secret Easter Egg Track
             {"data/org/jenka.org", 852108, 8306, 0, NULL},
             {"data/org/jenka2.org", 860416, 11986, 0, NULL},
             {"data/org/maze.org", 966248, 14786, 0, NULL},
             {"data/org/lastbt3.org", 882824, 21122, 0, NULL},
             {"data/org/lastcave.org", 931160, 18122, 0, NULL},
             {"data/org/mdown2.org", 981036, 21074, 0, NULL},
             {"data/org/ironh.org", 838656, 13450, 0, NULL},
             {"data/org/oside.org", 1012748, 25634, 0, NULL},
             {"data/org/plant.org", 1038384, 11378, 0, NULL},
             {"data/org/kodou.org", 872404, 10418, 0, NULL},
             {"data/org/quiet.org", 1049764, 4770, 0, NULL},
             {"data/org/escape.org", 734312, 20524, 0, NULL},//"Desperation"
             {"data/org/anzen.org", 681400, 9194, 0, NULL},
             {"data/org/wanpak2.org", 1144668, 15994, 0, NULL},
             {"data/org/ballos.org", 693680, 3570, 0, NULL},
             {"data/org/curly.org", 704404, 9844, 0, NULL}, //"Pier Walk"
             {"data/org/toroko.org", 1115724, 18482, 0, NULL},
             {"data/org/mura.org", 1002112, 10634, 0, NULL},
             {"data/org/dr.org", 714248, 2162, 0, NULL},
             {"data/org/marine.org", 949284, 16962, 0, NULL},
             {"data/org/white.org", 1204000, 23714, 0, NULL},
             {"data/org/zonbie.org", 1227832, 5346, 0, NULL},
             {NULL, 0, 0, 0, NULL}};

void createdir(const char *fname)
{
#if defined(_WIN32)
  char *dir = _strdup(fname);
#else
  char *dir = strdup(fname);
#endif
  char *ptr = strrchr(dir, '/');
  if (ptr)
  {
    *ptr = 0;

#if defined(_WIN32)
    _mkdir(dir);
#else
    mkdir(dir, S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH);
#endif
  }

  free(dir);
}

bool extract_files(FILE *exefp)
{
  uint8_t *buffer;
  uint8_t *file;
  uint32_t length;
  uint32_t crc;
  //bool check_crc = false;//Disable this for now.
  // bool first_crc_failure = true;

  buffer = (uint8_t *)malloc(MAX_FILE_SIZE);
  crc_init();

  for (int i = 0;; i++)
  {
    const char *outfilename = files[i].filename;
    if (!outfilename)
      break;

    printf("[ %s ]\n", outfilename);

    // initialize header if any
    file   = buffer;
    length = files[i].length;

    if (files[i].header)
    {
      memcpy(buffer, files[i].header, HEADER_LEN);
      file += HEADER_LEN;
      length += HEADER_LEN;
    }

    // read data from exe
    fseek(exefp, files[i].offset, SEEK_SET);
    fread(file, files[i].length, 1, exefp);

    #if defined(CHECK_CRC)
    {
      crc = crc_calc(file, files[i].length);
      if (crc != files[i].crc)
      {
        printf("File '%s' failed CRC check.\n", outfilename);

        //				first_crc_failure = false;
      }
    }
    #endif

    // write out the file
    createdir(outfilename);

    FILE *fp = fopen(outfilename, "wb");
    if (!fp)
    {
      printf("Failed to open '%s' for writing.\n", outfilename);
      free(buffer);
      return 1;
    }

    fwrite(buffer, length, 1, fp);
    fclose(fp);
  }

  free(buffer);
  return 0;
}
