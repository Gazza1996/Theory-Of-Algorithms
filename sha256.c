// Gary Mannion
// Secure Hash Algorithm, 256 bit version
// https://ws680.nist.gov/publication/get_pdf.cfm?pub_id=910977

// input/output
#include<stdio.h>
#include<stdint.h>
#include <stdbool.h>

// functions declared
void sha256(FILE *msgf);
void fileContents();
//int checkEndian();
//_Bool endianCheck();

// Sections 4.1.2 for defintions
uint32_t sig0(uint32_t x);
uint32_t sig1(uint32_t x);

// Section 3.2 for defintion
uint32_t rotr(uint32_t n, uint16_t x);
uint32_t shr(uint32_t n, uint16_t x);

// section 4.1.2
uint32_t SIG0(uint32_t x);
uint32_t SIG1(uint32_t x);

// section 4.1.2
uint32_t Ch(uint32_t x, uint32_t y, uint32_t z);
uint32_t Maj(uint32_t x, uint32_t y, uint32_t z);

// tells the preprocesser to create the var with value of 10000
#define MAX 10000

// http://www.firmcodes.com/write-c-program-convert-little-endian-big-endian-integer/
// for converting from big to little endian
#define LittleToBig(x) (((x) >> 24) | (((x) & 0x00FF0000) >> 8) | (((x) & 0x0000FF00) << 8) | ((x) << 24))

#define BIG_ENDIAN (*(uint32_t *)"\0\xff" < 0x100)
// message block
union msgblock{
  uint8_t   e[64];
  uint32_t  t[16];
  uint64_t  s[8];
};

int bigEndian(void){
    union
    {

      uint32_t i;
      char c[4];
  } 
  e = {0x01000000};

  return e.c[0] == 1;
}

enum status {READ, PAD0, PAD1, FINISH};

// retrieve next msg block
int nextmsgBlock(FILE *msgf, union msgblock *M, enum status *S, uint64_t *nobits);

// main method
int main(int argc, char *argv[]){
  
  //header for program
  printf("\n ----------------------------------\n");
  printf("\n ---- SHA256 Hashing Algorithm ----\n");
  printf("\n ----------------------------------\n");

  // adapted from
  // https://stackoverflow.com/questions/12791864/c-program-to-check-little-vs-big-endian
  printf(" This is %s-endian\n", bigEndian() ? "big" : "little");


  // open the file given as  first cmd argument
  FILE* msgf;
  
  if(argc !=2){
    printf("file not supplied..\n.");
          return 0; 
  }
 
  msgf= fopen(argv[1], "r");
        
  if(msgf == NULL){
    printf("invalid file name\n");
          return 0;
  }

  fileContents(msgf);
  sha256(msgf);

  // close the file
  fclose(msgf);

  // must add a return method
  return 0;
}

// functions
// show file contents function
void fileContents(FILE *msgf){

  // vars
  char fileCont[MAX];  

  printf("\n ------------- File ---------------\n");

  // gets our file content
  // https://www.tutorialspoint.com/c_standard_library/c_function_fgets.htm
  while(fgets(fileCont, MAX, msgf)){
    printf(" %s\n", fileCont);
  }

  printf("\n ----------------------------------\n");
  
  // required return statement
  return;

}
// SHA256 function
void sha256(FILE *msgf){

  // k constants. Section 4.2.2
  uint32_t K[] = {
    0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5,
    0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
    0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3,
    0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
    0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc,
    0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
    0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7,
    0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
    0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13,
    0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
    0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3,
    0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
    0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5,
    0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
    0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208,
    0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2
  };

  // current message block
  union msgblock M;
  // number of bits read from the file
  uint64_t nobits = 0;
  // message block status(Padding)
  enum status S = READ;

  printf(" ---- SHA256 output from file -----  \n\n");


  // message schedule(section 6.2)
  uint32_t W[64];
  // working var (section 6.2)
  uint32_t a, b, c, d, e, f, g, h;
  // 2 temporary variables (section 6.2)
  uint32_t T1, T2;

  // hash value (section 6.2)
  // values from section 5.3.3
  uint32_t H[8] = {
     0x6a09e667
   , 0xbb67ae85
   , 0x3c6ef372
   , 0xa54ff53a
   , 0x510e527f
   , 0x9b05688c
   , 0x1f83d9ab
   , 0x5be0cd19

  };

  // current message block
  // not needed
  //uint32_t M[16] = {0, 0, 0, 0, 0, 0, 0, 0};
  
  // looping
  int i,t;

  // loop through message block as per page 22
  while(nextmsgBlock(msgf, &M, &S, &nobits)){
  
  // from page 22, W[t] = M[t] for 0 <= t <= 15
  for(t = 0; t< 16; t++)
    W[t] = M.t[t];

  
  // from page 22, W[t] = ...
  for(t = 16; t < 64; t++)
   W[t] =  sig1(W[t-2]) + W[t-7] + sig0(W[t-15]) + W[t-16];
  
  // initialize a, b, c, ... as per step 2
  a = H[0]; b = H[1]; c = H[2]; d = H[3];
  e = H[4]; f = H[5]; g = H[6]; h = H[7];

  // step 3
  for (t = 0; t < 64; t++){
    T1 = h + SIG1(e) + Ch(e, f ,g) + K[t] + W[t];
    T2 = SIG0(a) + Maj(a, b ,c);
    h = g;
    g = f;
    f = e;
    e = d + T1;
    d = c;
    c = b;
    b = a;
    a = T1 + T2;
  }
   // step 4
   H[0] = a + H[0]; H[1] = b + H[1];
   H[2] = c + H[2]; H[3] = d + H[3];
   H[4] = e + H[4]; H[5] = f + H[5];
   H[6] = g + H[6]; H[7] = h + H[7];

}

      H[1] >> (24 - i *8) & 0x000000ff;
      H[2] >> (24 - i *8) & 0x000000ff;
      H[3] >> (24 - i *8) & 0x000000ff;
      H[4] >> (24 - i *8) & 0x000000ff;
      H[5] >> (24 - i *8) & 0x000000ff;
      H[6] >> (24 - i *8) & 0x000000ff;
      H[7] >> (24 - i *8) & 0x000000ff;

      // output when converting from big to little endian and vice versa.
  if(BIG_ENDIAN){
    printf(" %08x%08x%08x%08x%08x%08x%08x%08x\n\n",H[0], H[1],H[2],H[3], H[4], H[5], H[6],  H[7]);
  }else{
    printf(" %08x%08x%08x%08x%08x%08x%08x%08x\n\n",H[0], H[1],H[2],H[3], H[4], H[5], H[6],  H[7]);
  }

  printf("\n ------ Completed Successfully ------\n");
}
// rotate right
uint32_t rotr(uint32_t x, uint16_t g){
  return (x >> g) | (x << (32 - g));
}

// shift right
uint32_t shr(uint32_t n, uint16_t x){
  return (x >> n);
}

uint32_t sig0(uint32_t x){
  // sections 3.1 and  4.1.2 for definitions
  return (rotr(x, 7) ^ rotr(x, 18) ^ shr(x, 3));
}

uint32_t sig1(uint32_t x){
  return (rotr(x, 17) ^ rotr(x, 19) ^ shr(x, 10));
}

uint32_t SIG0(uint32_t x){
  return (rotr(x, 2) ^ rotr(x, 13) ^ rotr(x, 22));
}

uint32_t SIG1(uint32_t x){
  return (rotr(x, 6) ^ rotr(x, 11) ^ rotr(x, 25));
}

// choose
uint32_t Ch(uint32_t x, uint32_t y, uint32_t z){
  return ((x & y) ^ (~(x) & z));
}
// Majority
uint32_t Maj(uint32_t x, uint32_t y, uint32_t z){
  return ((x & y) ^ (x & z) ^ (y & z));
}

int nextmsgBlock(FILE *msgf, union msgblock *M, enum status *S, uint64_t *nobits){
  // for looping
  int i;
  // number of bytes from fread
  uint64_t nobytes;

  // if we have finished all msgblocks, S should FINISH
  if(*S == FINISH){
    return 0;
  }
  
  // otherwise check if we need another block full of padding
  if (*S == PAD0 || *S == PAD1){
      // set first 56 bytes to 0 bits
      for (i = 0; i < 56; i++){
        M->e[i] = 0x00;
      }
      
      // set last 64 bits to the number of bits in the file(should be big endian)
      M->s[7] = *nobits;
      // tell S we are finished
      *S = FINISH;
       
      if (*S == PAD1){
       M->e[0] = 0x80;
      // keep the loop in sha256 for one more iteration
       return 1;
      }
  }
    // get down here we haven't finished reading the file(S == READ)
    nobytes = fread(M->e, 1, 64, msgf);
    
    // Kepp track of the number of bytes we have read
    *nobits = *nobits + (nobytes * 8);

    // if less than 56 bytes is read, all padding can be put in this block
    if (nobytes < 56) {
      // adding 1
      M->e[nobytes] = 0x80;
     
      // add 0 until last 64
      while (nobytes < 56) {
        nobytes = nobytes + 1;
        // set bytes to zero
        M->e[nobytes] = 0x00;
      }
      
      // append the file size in bits as an unsigned 64 bit int
      M->s[7] = *nobits;
      // S is finished
      *S = FINISH;
    // otherwise, check id can put some padding in this block
    } else if(nobytes < 64){
      // tell S we need another block with padding but no one bit
      *S = PAD0;
      // put one bit into the current block
      M->e[nobytes] = 0x80;
      // pad the rest with 0 bits
      while (nobytes < 64){
        nobytes = nobytes + 1;
        M->e[nobytes] = 0x00;
      }
    // check if we're at the end of the file  
    } else if (feof(msgf)){
      // tell s we need another msg block with all the padding
      *S = PAD1;
    }

    // if we get down here, return 1 so so function is called again
    return 1;
  }
