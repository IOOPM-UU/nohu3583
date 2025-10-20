
# Initial Profiling Results
For each input, what are the top 3 functions?
    1k-long words:
    The top three functions are hash_str, lowercase_inplace and ioopm_hash_table_destroy.
    10k words:
    The top three functions are hash_str, ioopm_hash_table_insert_freq and lowercase_inplace.
    16k words:
    The top three functions are ioopm_linked_list_get, hash_str and ioopm_hash_table_insert_freq.
    Small:
    ioopm_hash_table_destroy, ioopm_hash_table_keys, hash_str. 

For each input, are the top 3 functions in your code (that you have written), or is it in library functions?
All of the functions are onces that i have writen(these use library functions however).

Are the top 3 functions in your code consistent across the inputs? Why? Why not?
They are not, for example in the 1k long words the most time consuming function is hash_str. This since the function iterates through the word char by char. For long words this will take time, same for lowecase_inplace function. Also the ioopm_hash_table_destroy will take up alot of the time since there are not a lot of words to be added which results the average amount of calls per hash_table being less. In the larger files we spend more time inserting since there are many more words and then having to remove the hashtables less often (same amount of times but less percent of time)
Is there some kind of trend? (Possibly several)
My hash_int function could be improved, takes long to iterato throug, also that iterating trough the linked list takes along time.
Do the results correspond with your expectations?
Yes it did, i knew that my function for hashing was not the fastest, also i knew it would take more time for the bigger file to handle the keys list. 
Based on these results, do you see a way to make your program go faster?
I realised that my size functions was incorrect which, so i fixed it which made the code go faster. I also saw that changing the amount of buckets played a big part, so i optimized it. More on this later. 