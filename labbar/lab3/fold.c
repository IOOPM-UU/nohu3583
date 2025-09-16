#include <stdio.h>

typedef int int_fold_func(int, int);

int foldl_int_int(int numbers[], int numbers_siz, int_fold_func *f)
{
  int result = 0;

  // Loopa över arrayen och för varje element e utför result = f(result, e)
  for (int i = 0; i < numbers_siz; ++i)
  {
    result = f(result, numbers[i]);
  }

  return result;
}

int add(int a, int b)
{
  return a + b;
}

int sum(int numbers[], int numbers_siz)
{
  return foldl_int_int(numbers, numbers_siz, add);
}

int main() {
  int numbers[] = {1, 2, 3, 4, 5};
  int numbers_siz = sizeof(numbers) / sizeof(numbers[0]);
  int result = sum(numbers, numbers_siz);
  printf("Sum: %d\n", result); 
  if (result == 15) {
    printf("Test passed!\n");
  } else {
    printf("Test failed!\n");
  }
  return 0;
}