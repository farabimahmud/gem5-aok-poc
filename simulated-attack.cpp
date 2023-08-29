#include <stdio.h>
#include <x86intrin.h>
#include <string>
#include <stdint.h>

using namespace std; 
#define LLC_SIZE (2<<20)
#define LLC_ASSOC 16
#define CACHELINE_SZ 64
#define SECRET 0xff
#define COUNT (512)

uint8_t buffer[COUNT * 64];

static inline __attribute__((always_inline))
void victim(int index){
    uint8_t junk = 0xff ^  buffer[index * CACHELINE_SZ];
}

static inline uint64_t rdtscp( uint32_t & aux )
{
    uint64_t rax,rdx;
    asm volatile ( "rdtscp\n" : "=a" (rax), "=d" (rdx), "=c" (aux) : : );
    return (rdx << 32) + rax;
}

int main(int argc, char* argv[]){
    if (argc != 2){
        perror("Usage: ./simulated-attack <index>");
        exit(1);
    }
    uint8_t secretbit = SECRET & atoi(argv[1]);
    uint32_t junk;
    unsigned long t, t1, t2;

    // write into buffer so initial entries are replaced by later entries
    for (int i=0; i<COUNT; i++){
        buffer[i * CACHELINE_SZ] = rand() % 0xff;
    }
    _mm_mfence();
    int index = 118;
    if (secretbit==1){
        index = 117;
    }
    t1 = rdtscp(junk);
    victim(index);
    t2 = rdtscp(junk);
    t = t2- t1;
    FILE *fp = fopen("/home/grads/f/farabi/gem5-aok-poc/results.csv","a");
    fprintf(fp,"%d,%ld,%d\n",index,t,secretbit);
    fclose(fp);

    return 0;
}