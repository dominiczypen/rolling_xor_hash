#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>

// Function declarations:
void hash(char* s, int len, uint64_t* hash_p);
void hash_rolling(char delete_char, char add_char, int len, uint64_t* hash_p);
int compare_hashes(char* file_content, int len, char* search_string);

//-----------------------------------------------------
void hash(char* s, int len, uint64_t* hash_p)
  // takes string s of length <len> and 
  // produces a Pearson-like hash: but no table / lookup needed
  // instead use BIJECTION : right-rotation of uint64_t
  // Output: 64bit hash
{
  *hash_p = 0;
  int i = 0;
  int count_to_8 = 0; // to address the 8 byte-parts of uint64_t

  // walk through string and hash consecutive groups of <len> bytes

  while (i < len)
  {
      // Step 1 of Pearson's original algo: XOR what we have
      // with character just read: 
    *(hash_p) = *(hash_p) ^ (((uint64_t)(*s)) << (count_to_8 * 8));
    count_to_8++;
      // Step 2 of Pearson: bijection. Here we use IDENTITY -> do nothing!
    if (count_to_8 == 8)  // back to 0
    { 
      count_to_8 = 0;
    } 
    s++; // walk 1 step further in string!
    i++;
  } // end of loop
}
//-----------------------------------------------------
void hash_rolling(char delete_char, char add_char, int len, uint64_t* hash_p)
{
  // Step 1: XOR with delete char at right end
  *hash_p = *hash_p ^ (uint64_t)delete_char;
   
  // Step 2: Nachruecken nach rechts 
  uint64_t rightmost_byte = *hash_p & ((1 << 8) - 1);
  *hash_p = (*hash_p >> 8) | (rightmost_byte << 56);

  // Step 3: add new character appearing in window 
  *hash_p = *hash_p ^ ((uint64_t)add_char << ((len - 1) % 8 ) * 8);
}
//-----------------------------------------------------
int compare_hashes(char* content_p, int len, char* search_string_p)
 // gives back 0 (no match)  or 1 (at least one match)
 // WARNING: <search_string> has to be terminated by '\0'
{
  // Hash of search string
  //uint64_t search_hash = 0;
  //hash(search_string, &search_hash);

  // Hash of file string
  uint64_t window_hash = 0;
  uint64_t window_hash_r = 0; // _r -> "rolling"

  int i = 0;

  hash(content_p, len, &window_hash);
  window_hash_r = window_hash;
  int match = (window_hash == window_hash_r);
  int loop_counter = 0;
  printf("Hash  : %" PRIx64 "\n", window_hash); // DEBUG
  printf("Hash_r: %" PRIx64 "\n", window_hash_r); // DEBUG

  while ((loop_counter < 8) && (match == 1) && (*(content_p + len) != '\0'))
  {
    content_p++;
    hash(content_p, len, &window_hash);     // calculate new file hash!
                                     // search_hash stays THE SAME throughout!
    hash_rolling(*(content_p-1), *(content_p + len - 1), 
            len, &window_hash_r);
    match = (window_hash == window_hash_r);

    printf("Hash  : %" PRIx64 "\n", window_hash); // DEBUG
    printf("Hash_r: %" PRIx64 "\n", window_hash_r); // DEBUG
    loop_counter++;
  }
  return loop_counter;
}
//------------------------------
int main()
{
  //search_file("meintext.txt", "hallo", 6);
  //printf("Length of file meintext.txt %d", length_of_file("meintext.txt"));

  char mystring[] = "abcdefghijklmnopqrstuvwxyz01234567899 \
       Das hier wuerde eigentlich in einem File stehen, \
       aber wir lesen es jetzt mal so ein, inklusive Newlines \
       und etc. 012343423459383.";
  int loop_counter = compare_hashes(mystring, 11, "hallo welt,"); // 11 characters wie im beispiel
  printf("Loop_counter == %d \n", loop_counter);
  printf("(Cancel at loop counter == 8 ....");
  return 0;
}
