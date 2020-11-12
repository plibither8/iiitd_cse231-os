#include <sys/syscall.h>

#define RTNICE_ID 440

int rtnice(int pid, int rtnice_val) {
  return syscall(RTNICE_ID, pid, rtnice_val);
}
