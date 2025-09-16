#include <stdio.h>

int main(void)
{
  int i = 1;              // deklaration och initiering av iterationsvariabeln
  int sum = 0;            // deklaration och initiering av sum variabeln
  char symbol = '*';      // deklaration och initiering av symbol variabeln
  for(i = 1; i <= 10; i++) // for-loop som itererar från 1 till 10
  {
      for (int a = 1; a <=i; a++) // for-loop som itererar från 1 till i, inner loop
          printf("%c", symbol);   // skriver ut symbolen '*' varje iteration
          printf("\n");           // skriver ut en radbrytning efter varje inre loop är klar
      sum += i;                   // lägger till i till sum varje iteration 

  }
  printf("The sum is: %d\n", sum); // skriver ut summan efter loopen är klar
  return 0;
}