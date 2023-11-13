#include <stdio.h>

int main (int argc, void** argv)
{
  float a = 0.f;
  double b = 0.f;
  int c = 3;
  printf ("hello world\n");
  sscanf ("3.14\n", "%f\n", &a);
  printf ("finished sscanf\n");
  sscanf ("3.14\n", "%lf\n", &b);
  printf ("a: %f\n", a);
  printf ("b: %lf\n", b);
  printf ("c: %d\n", c);
}
