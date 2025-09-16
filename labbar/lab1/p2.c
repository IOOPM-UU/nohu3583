#include <stdio.h>

int main(void)
{
  int i = 1;              // deklaration och initiering av iterationsvariabeln
  for(i = 10; i >= 1; i--) // for-loop som itererar från 10 till 1.
  {
      printf("%d\n", i); ; // skriver ut i varje iteration.
  }
  return 0;
}