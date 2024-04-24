#include "framework.h"
#include "hash_table/hash_table.h"
#include "hash_table/pair.h"

int fileno(FILE *stream);

#define STRING_SIZE_MAX 255
#define LINE_MAX_SZ 4096
#define WORDS_BUFFER_MAX 4096

void file_read_words(FILE *fp)
{
    hash_table_t *hash_table = hash_table_create((size_t)HASH_TABLE_INITIAL_SZ);

    struct stat file_stat;
    fstat(fileno(fp), &file_stat);
    const size_t file_max_size = file_stat.st_size + 1;
    char buff[file_max_size];
    const char *delim = " ";

    while (fgets(buff, file_max_size, fp))
    {
        char *token = strtok(buff, delim);
        while (token)
        {

            hash_table->fn_insert(hash_table->self, token);

            token = strtok(NULL, delim);
        }
    }

    // printing and cleaning table;
    hash_table->fn_print(hash_table->self);
    hash_table->fn_dtor(hash_table->self);
}

int main(int argc, char **argv)
{

    // opening and reading from file;
    if (argc < 2)
    {
        perror("Please provide filename \n");
        exit(1);
    }

    FILE *fp = fopen(argv[1], "rb");
    if (fp == NULL)
    {
        perror("Error reading file\n");
        exit(1);
    }

    file_read_words(fp);
    fclose(fp);
}