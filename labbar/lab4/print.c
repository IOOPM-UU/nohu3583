#include <stdio.h>
#include <string.h>
#include <ctype.h>

char *trim(char *str)
{
  char *start = str;
  char *end = start + strlen(str)-1;

  while (isspace(*start)) ++start;
  while (isspace(*end)) --end;

  char *cursor = str;
  for (; start <= end; ++start, ++cursor)
    {
      *cursor = *start;
    }
  *cursor = '\0';

  return str;
}

void print(char *str)
{
  printf("'%s'\n", trim(str));
}


int main(void)
{
  char str[] = "   Hej på dig!  ssafsa     ";
  print(str);
  return 0;
}