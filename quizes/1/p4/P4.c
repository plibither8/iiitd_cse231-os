#include <stdio.h>
#include <stdlib.h>

/* Comment/uncomment this to use/disable long_add */
int long_add(int a, int b, int c, int d);

/* Comment/uncomment this to use/disable extended_add */
// long extended_add(long a, long b, long c, long d);

int main()
{
  long a, b, c, d;
  scanf("%ld %ld %ld %ld", &a, &b, &c, &d);

  /* Comment/uncomment this to use/disable long_add */
  printf("%d\n", long_add(a, b, c, d));

  /* Comment/uncomment this to use/disable extended_add */
  // printf("%ld\n", extended_add(a, b, c, d));

  return 0;
}
