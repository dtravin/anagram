#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>

#define ALPHABET_SIZE 256

long get_time_microseconds() {
    struct timeval currentTime;
    gettimeofday(&currentTime, NULL);
    return currentTime.tv_sec * (int)1e6 + currentTime.tv_usec;
}

char* read_file(char* fname, unsigned long* fileLength) {
    FILE* file;
    char* buffer;
    unsigned long fileLen;

    //Open file
    file = fopen(fname, "r");
    if (!file)
    {
        fprintf(stderr, "Unable to open file %s", fname);
        return NULL;
    }

    fseek(file, 0, SEEK_END);
    fileLen=ftell(file);
    fseek(file, 0, SEEK_SET);

    buffer=(char*)malloc(fileLen+1);
    if (!buffer)
    {
        fprintf(stderr, "Cannot allocate memory");
        fclose(file);
        return NULL;
    }

    if (fread(buffer, fileLen, 1, file) == 0) {
        fprintf(stderr, "Unable to read dictionary");
        fclose(file);
        return NULL;
    }
    fclose(file);
    *fileLength = fileLen;
    return buffer;
}

int main(int argc, char* argv[])
{
    long start_time = get_time_microseconds();

    char result[1024];
    result[0] = 0;
    if (argc < 3) {
        fprintf(stderr, "Usage: anagram path_to_dictionary word\n");
        return 1;
    }

    char lookup_letters[ALPHABET_SIZE];
    memset(lookup_letters, 0, ALPHABET_SIZE*sizeof(char));

    int i=0;
    while (argv[2][i] != '\0') {
        lookup_letters[argv[2][i]]++;
        i++;
    }
    int lookup_len = i;

    char candidate_letters[ALPHABET_SIZE];

    // Optimization #1
    // Read entire file into memory
    unsigned long fileLen;
    char* buffer = read_file(argv[1], &fileLen);
    if (buffer == NULL) {
        return 1;
    }

    int start, end = 0;
    int new_word = 1;
    int skip_this_word = 0;
    for (int p=0;p<fileLen;p++) {
        if (new_word) {
            start = p;
            end = p;
            new_word = 0;
        }
        char c = buffer[p];
        if (c != '\n' && c != '\r' && c != '\t' && c != ' ') {
            end++;
            // Optimization #2
            // Skip if number of letters in the word
            // is less than number of letters in dictionary word
            if (end - start > lookup_len) {
                skip_this_word = 1;
            }
            // Optimization #3
            // Skip dictionary word if not allowed character found
            if (lookup_letters[c] == 0) {
                skip_this_word = 1;
            }
        } else {
            if (skip_this_word) {
                skip_this_word = 0;
            } else {
                // Optimization #4
                // memset faster than loop
                memset(candidate_letters, 0, ALPHABET_SIZE*sizeof(char));
                for (int k=start;k<end;k++) {
                    candidate_letters[buffer[k]]++;
                }
                // Optimization #5
                // memory compare by blocks
                if (memcmp(lookup_letters, candidate_letters, ALPHABET_SIZE) == 0) {
                    strcat(result, ",");
                    buffer[p] = 0;
                    char* word = buffer + start;
                    strcat(result, word);
                }
            }
            new_word = 1;
        }
    }
    free(buffer);

    long end_time = get_time_microseconds();

    printf("%ld%s\n", (end_time-start_time), result);

    return 0;
}
