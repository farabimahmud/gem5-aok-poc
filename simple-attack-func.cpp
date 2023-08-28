#include <pthread.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <x86intrin.h>

pthread_barrier_t barrier;  // Barrier for synchronization
pthread_barrier_t read_barrier;
#define COUNT (512)

size_t size = 4;
uint8_t arr1[256 * 64];
uint8_t arr2[512 * 64];  // idx 0 * 64 -> node 2+
uint8_t SECRET = 0xff;
struct spy_thread_args
{
  int index;
  bool ignore;
  uint8_t secretbit;
};
struct spy_thread_args * spa;

void* spy_thread(void* arg) {
  cpu_set_t cpuset;
  CPU_ZERO(&cpuset);        // Clear the CPU set
  CPU_SET(1, &cpuset);      // Set CPU core 1

  spa = (spy_thread_args *) arg;
  int i = spa->index;
  bool ignore = spa->ignore;
  uint8_t secretbit = spa->secretbit;
  FILE *fp = fopen("/data/gem5/results.csv","a");
  // fprintf(fp, "params: %d,%d,%d\n",i,ignore,secretbit);

  _mm_clflush(&arr2[i * 64]);
  pthread_barrier_wait(&barrier);
  unsigned int junk;
  uint8_t x;
  pthread_barrier_wait(&read_barrier);
  unsigned long time1 = __rdtscp(&junk);
  x ^= arr2[i * 64];
  unsigned long time2 = __rdtscp(&junk);
  if (!ignore){
    
    fprintf(fp, "%d,%ld,%d\n", i, time2 - time1,secretbit);
    
  }
  fclose(fp);
  
  return NULL;
}

void* victim_thread(void* arg) {
  cpu_set_t cpuset;
  CPU_ZERO(&cpuset);        // Clear the CPU set
  CPU_SET(63, &cpuset);      // Set CPU core 7
  int i = *((int *) arg);

  pthread_barrier_wait(&barrier);
  uint8_t val = arr2[i * 64];

  for (int i = 0; i < 10; i++) {
    val ^= arr2[i * 64];
  }
  pthread_barrier_wait(&read_barrier);
  return NULL;
}

void launch(int id, bool ignore, uint8_t sbit){
  pthread_t spy_t, victim_t;
  struct spy_thread_args spyargs = {id, ignore, sbit};

  pthread_barrier_init(&barrier, NULL, 2);
  pthread_barrier_init(&read_barrier, NULL, 2);

  pthread_create(&spy_t, NULL, spy_thread, &spyargs);
  pthread_create(&victim_t, NULL, victim_thread, &id);

  pthread_join(spy_t, NULL);
  pthread_join(victim_t, NULL);

  pthread_barrier_destroy(&barrier);
  pthread_barrier_destroy(&read_barrier);
}

int main(int argc, char* argv[]) {
  int idx_to_read = 4;
  uint8_t secretbit = SECRET & 0x1;
  if (argc == 2) {
    secretbit  = atoi(argv[1]) & 0x1;
  }

  if (secretbit == 0){
    idx_to_read = 126;
  }
  else{
    idx_to_read = 4;
  }
  launch(idx_to_read, true, secretbit);
  for (int i=0; i<COUNT; i++)
    _mm_clflush(&arr2[i*64]);
  _mm_mfence();

  launch(idx_to_read, false, secretbit);
  // for (int i=0; i<COUNT; i++)
  //   _mm_clflush(&arr2[i*64]);
  // _mm_mfence();
  // launch(idx_to_read, false, secretbit);
  // for (int i=0; i<COUNT; i++)
  //   _mm_clflush(&arr2[i*64]);
  // _mm_mfence();
  // launch(idx_to_read, false, secretbit);
  // for (int i=0; i<COUNT; i++)
  //   _mm_clflush(&arr2[i*64]);
  // _mm_mfence();


  return 0;
}
