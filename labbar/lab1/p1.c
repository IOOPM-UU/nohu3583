#include <stdio.h>

int main(void)
{
  int i = 1;              // deklaration och initiering av iterationsvariabeln
  for(i = 1; i <= 10; i++) // for-loop som itererar från 1 till 10
  {
      printf("%d\n", i); ; // skriver ut i varje iteration.
  }
  return 0;
}