#include <stdio.h>
#include <inttypes.h>

int64_t add(int64_t, int64_t);

int main() {
  long a, b;
  scanf("%ld %ld", &a, &b);
  printf("%ld", add(a, b));
  return 0;
}
