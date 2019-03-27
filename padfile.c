#include<stdio.h>
#include<stdint.h>

union msgblock {
  uint8_t    e[64];
  uint32_t   t[16];
  uint64_t   s[8];

};

enum status {READ, PAD0, PAD1, FINISH};

int main(int argc, char *argv[]) {
  FILE* f;

  int i;

  union msgblock M;

  uint64_t nobits = 0;

  uint64_t nobytes;

  enum status S = READ;

  // read in file, taken from input when program is ran
  f = fopen(argv[1], "r");

  // not reached end of file
  while (S == READ) {
    nobytes = fread(M.e, 1, 64, f);
    // debug message
    printf("Read %2llu bytes\n", nobytes);

    nobits = nobits + (nobytes * 8);
    if (nobytes < 56) {
      printf("Found a block with less than 55 bytes!!\n");
      // adding 1
      M.e[nobytes] = 0x80;
      // get last 8 bytes
      while (nobytes < 56) {
        nobytes = nobytes + 1;
        // set bytes to zero
        M.e[nobytes] = 0x00;
      }
      // set s to nobits
    M.s[7] = nobits;
    S = FINISH;
    } else if(nobytes < 64){
      S = PAD0;
      M.e[nobytes] = 0x80;
      while (nobytes < 64){
        nobytes = nobytes + 1;
        M.e[nobytes] = 0x00;
      }
    } else if (feof(f)){
      S = PAD1;
    }
  }

  if (S == PAD0 || S == PAD1){
    for (i = 0; i < 56; i++)
      M.e[i] = 0x00;
      M.s[7] = nobits;
    }
    if (S == PAD1)
      M.e[0] = 0x80;
    
 nt i;
 17 
   18   union msgblock M;
 19 
   20   uint64_t nobits = 0;
 21 
   22   uint64_t nobytes;
 23 
   24   enum status S = READ;
 25 
   26   // read in file, taken from input when program is ran
   27   f = fopen(argv[1], "r");
 28 
   29   // not reached end of file
   30   while (S == READ) {
     31     nobytes = fread(M.e, 1, 64, f);
     32     // debug message
       33     printf("Read %2llu bytes\n", nobytes);
     34 
       35     nobits = nobits + (nobytes * 8);
     36     if (nobytes < 56) {
       37       printf("Found a block with less than 55 bytes!!\n");
       38       // adding 1
         39       M.e[nobytes] = 0x80;
       40       // get last 8 bytes
         41       while (nobytes < 56) {
           42         nobytes = nobytes + 1;
           43         // set bytes to zero
             44         M.e[nobytes] = 0x00;
         47     M.s[7] = nobits;
       48     S = FINISH;
       49     } else if(nobytes < 64){ // close the file
  fclose(f);

  for(int i = 0; i < 64; i++)
    printf("%x ", M.e[i]);
  printf("\n");

  return 0;
}
