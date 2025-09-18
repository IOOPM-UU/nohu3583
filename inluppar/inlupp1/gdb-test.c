typedef struct entry entry_t;
typedef struct hash_table ioopm_hash_table_t;

struct entry
{
  int key;       // holds the key
  char *value;   // holds the value
  entry_t *next; // points to the next entry (possibly NULL)
};

struct hash_table
{
  entry_t *buckets[17];
};

int main(int argc, char *argv[])
{
  entry_t a = { .key = 1, .value = argv[1] };
  entry_t b = { .key = 2, .value = argv[2], .next = &a };
  entry_t c = { .key = 3, .value = argv[3], .next = &b };
  entry_t d = { .key = 4, .value = argv[4] };
  entry_t e = { .key = 5, .value = argv[5] };
  entry_t f = { .key = 6, .value = argv[6], .next = &e };
  ioopm_hash_table_t ht = { .buckets = { 0 } };
  ht.buckets[3] = &c;
  ht.buckets[8] = &d;
  ht.buckets[15] = &f;

  return 0;
}
