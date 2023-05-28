#include <stdio.h>
#include <stdlib.h>

#define HZ_IMPLEMENTATION
#include <hz/hz.h>

char *read_entire_file(const char *filename, size_t *out_size) {
    FILE *fp = fopen(filename,"rb");
    char *data = NULL;

    if (fp) {
        fseek(fp,0,SEEK_END);
        *out_size = ftell(fp);
        fseek(fp,0,SEEK_SET);
        data = malloc(*out_size);
        fread(data,1,*out_size,fp);
        fclose(fp);
    }

    return data;
}

int main(int argc, char**argv) {
    size_t input_size;
    char *input = read_entire_file("../../data/arabic-paragraph.txt",&input_size);

    hz_config_t cfg = {.ucd_version = HZ_MAKE_VERSION(15,0,0)};

    if (!hz_init(&cfg)) {
        return EXIT_FAILURE;
    }

    hz_memory_arena_t memory_arena;
    char *arena_memory = malloc(1024*1024*4);
    hz_memory_arena_init(&memory_arena,arena_memory,1024*1024*4);

    hz_buffer_t buffer;
    hz_buffer_init(&buffer);
    hz_buffer_load_utf8_unaligned(&buffer,&memory_arena,input,input_size);

    free(arena_memory);
    free(input);
    return EXIT_SUCCESS;
}