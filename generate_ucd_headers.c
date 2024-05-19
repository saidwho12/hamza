/*
    This file is part of Hamza.

    Hamza is free software: you can redistribute it and/or modify it under the terms of the GNU Lesser General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
    
    Hamza is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.
    You should have received a copy of the GNU Lesser General Public License along with Hamza. If not, see <https://www.gnu.org/licenses/>. 
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#if _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

#include <regex.h>
#include <assert.h>

#define INTS_PER_LINE 6

typedef struct {
    uint32_t codepoint;
    char *comment;
    int comment_len;
    char joining_type;
    char *joining_group;
    int joining_group_len;
} arabic_joining_data_t;

typedef struct {
    uint32_t codepoint;
    char name[25];
} jamo_t;

typedef struct {
    char *txt[5];
    size_t sizes[5];
} property_value_alias_t;

#define BUCKET_MAX_INDICES 64

typedef struct {
    int size;
    uint32_t indices[BUCKET_MAX_INDICES];
} mph_bucket_t;

typedef struct {
    int32_t *k2;
    void *values;
    size_t value_size;
    size_t size;
} mph_table_t;

// 32-bit hash functions
uint32_t hash2_lowbias32(uint32_t k1, uint32_t k2)
{
    uint32_t h = k1;
    h ^= h >> 16;
    h *= 0x7feb352d;
    h ^= h >> 15;
    h *= 0x846ca68b;
    h ^= h >> 16;

    h ^= k2;
    h ^= h >> 16;
    h *= 0x7feb352d;
    h ^= h >> 15;
    h *= 0x846ca68b;
    h ^= h >> 16;
    return h;
}

int compare_mph_buckets(const void *lhs, const void *rhs)
{
    return ((const mph_bucket_t*)rhs)->size - ((const mph_bucket_t*)lhs)->size;
}

int popvalue(int *arr, int *size, int index)
{
    int val = arr[index];
    for (int i = index; i < (*size); ++i) {
        arr[i] = arr[i+1];
    }

    (*size) --;
    return val;
}

int binsearch(int *arr,int size, int val) {
    int low,mid,high;
    low = 0; high = size;

    while (low <= high) {
        mid = (low + high) >> 1;
        if (val < arr[mid]) { // less
            high = mid-1;
        } else if (val > arr[mid]) { // greater
            low = mid+1;
        } else { // equal
            return mid;
        }
    }

    return -1;
}

void arraypush(int *arr, int *size, int val) {
    arr[(*size)++] = val;
}

void bininsert(int *arr, int *size, int val) {
    int low,mid,high;
    low = 0; high = *size;
    mid = (low + high) >> 1;

    while (low <= high) {
        mid = (low + high) >> 1;
        if (val < arr[mid]) { // lesser
            high = mid-1;
        } else if (val > arr[mid]) { // greater
            low = mid+1;
        } else { // equal
            break;
        }
    }

    (*size) ++;
    for (int i = (*size)-1; i > mid; --i) {
        arr[i] = arr[i-1];
    }

    arr[mid] = val;
}

int occurcnt(int *arr, int size, int val)
{
    int cnt = 0;
    for (int i =0; i < size; ++i) {
        cnt += (arr[i] == val);
    }
    return cnt;
}

void mph_table_init(mph_table_t t[static 1], uint32_t *keys, void *values,
                    size_t value_size, size_t num_values, uint32_t (*hash)(uint32_t, uint32_t))
{
    mph_bucket_t *buckets = calloc(num_values, sizeof(mph_bucket_t)); // Index buckets
    t->k2 = calloc(num_values, sizeof(int32_t)); // Secondary Hash Array (for Collision Resolution)
    t->value_size = value_size;
    t->size = num_values;
    t->values = malloc(num_values * value_size); // Ordered values

    // Store keys in appropriate buckets
    for (int i=0; i<num_values; ++i) {
        uint32_t h = hash(keys[i],0)%num_values;
        mph_bucket_t *bucket = &buckets[h];
        bucket->indices[bucket->size++] = i;
    }

    // Sort the buckets and process in decreasing order (largest -> smallest)
    qsort(buckets, num_values, sizeof(mph_bucket_t), compare_mph_buckets);

    // Freeslots array
    int *freeslots = malloc(num_values * sizeof(int));
    int freeslots_size = num_values;
    for (int i = 0; i < num_values; ++i) {
        freeslots[i] = i;
    }

    int ones = 0, multi = 0;
    for (int i = 0; i < num_values; ++i) {
        if (buckets[i].size < 1)
            break;
        else if (buckets[i].size == 1) {
            // Only buckets with one item remain, place them directly into a free slot
            // k2 list already initialized to 0 so no need to set here
            int idx = buckets[i].indices[0];
            uint32_t k1 = keys[idx];
            // uint32_t k1 = keys[idx];

            assert(freeslots_size);
            int slot = popvalue(freeslots, &freeslots_size, 0);
            // printf("%d\n", slot);
            // We subtract one to ensure it's negative even if the zeroeth slot was
            // used.
            t->k2[hash(k1,0)%num_values] = -(int32_t)(slot+1);
            memcpy((char*)t->values + slot*value_size, (char*)values+idx*value_size, value_size);//Store value in slot
            ++ones;
        } else {
            // Try different values of k2 until we find a hash function that places
            // all the items in the buckets into free slots
            int32_t k2 = 0;
            int slots[BUCKET_MAX_INDICES];
            int slots_size = 0;
            
            while (slots_size < buckets[i].size) {
                int idx = buckets[i].indices[slots_size];
                uint32_t k1 = keys[idx];
                int slot = hash(k1,k2)%num_values;
                if (occurcnt(slots, slots_size, slot) >= 1 || binsearch(freeslots, freeslots_size, slot) == -1) {
                    ++k2;
                    slots_size = 0; // Clear
                } else {
                    slots[slots_size] = slot;
                    ++slots_size;
                }
            }

            // printf("SLOTS SIZE: %d\n", slots_size);
            // for (int z = 0; z < slots_size; ++z) {
            //     printf("%d ", slots[z]);
            // }printf("\n\n");

            assert(slots_size == buckets[i].size);

            // Set k2 param for bucket
            uint32_t k1 = keys[buckets[i].indices[0]];
            t->k2[hash(k1,0)%num_values] = k2;

            // Copy values from source table to their slots
            for (int j = 0; j < slots_size; ++j){
                int idx = buckets[i].indices[j];
                memcpy((char*)t->values + slots[j]*value_size, (char*)values+idx*value_size, value_size); // Store value in slot
                int slot_index = binsearch(freeslots, freeslots_size, slots[j]);
                assert(slot_index != -1);
                int popped = popvalue(freeslots, &freeslots_size, slot_index);
                // printf("[%d] : %d\n", slot_index, popped);
            }

            multi += slots_size;
        }
    }

    // Verify if MPH table was built correctly
    int matches = 0, mismatches = 0;
    for (int i = 0; i < num_values; ++i) {
        uint32_t k1 = keys[i];
        uint32_t h = hash(k1,0)%num_values;
        int32_t k2 = t->k2[h];
        uint32_t slot;
        if (k2 < 0) {
            slot = (-k2)-1;
        } else {
            slot = hash(keys[i],k2)%num_values;
        }

        char *val = (char*)t->values + slot*value_size;
        uint32_t codepoint = ((arabic_joining_data_t*) val)->codepoint;
        if (codepoint != k1) {
            mismatches ++;
        } else {
            matches ++;
        }
    }

    printf(" MATCHES: %d/%d , multi:%d, ones:%d\n", matches, mismatches, multi,ones);

    free(freeslots);
    free(buckets);
}

void mph_table_deinit(mph_table_t *t)
{
    free(t->k2);
    free(t->values);
}


int generate() {
    int err;
    char errbuf[512];
    regex_t ver_regex,
            arabic_shaping_regex,
            jamo_regex,
            property_value_aliases_regex;

    if ((err = regcomp(&ver_regex, "^([0-9]+)\\.([0-9]+)(?:\\.([0-9]+)|-(Update(?:[0-9]+)?))$", REG_EXTENDED|REG_NEWLINE)) != 0) {
        regerror(err, &arabic_shaping_regex, errbuf, 512);
        fprintf(stderr, "Failed to compile regex err:\n%s\n", errbuf);
        return -1;
    }

    if ((err = regcomp(&arabic_shaping_regex, "^([[:xdigit:]]*)[[:space:]]*;[[:space:]]*((?:[[:alnum:]][[:space:]]?)*)[[:space:]]*;[[:space:]]*([[:alpha:]])[[:space:]]*;[[:space:]]*((?:[[:space:]]?[[:alpha:]_])*)[[:space:]]*$", REG_EXTENDED|REG_NEWLINE)) != 0) {
        regerror(err, &arabic_shaping_regex, errbuf, 512);
        fprintf(stderr, "Failed to compile regex err:\n%s\n", errbuf);
        return -1;
    }

    if ((err = regcomp(&jamo_regex, "^([[:xdigit:]]*)[[:space:]]*;[[:space:]]*([[:alpha:]]*)[[:space:]#^\\n]*", REG_EXTENDED|REG_NEWLINE)) != 0) {
        regerror(err, &jamo_regex, errbuf, 512);
        fprintf(stderr, "Failed to compile regex err:\n%s\n", errbuf);
        return -1;
    }

    if ((err = regcomp(&property_value_aliases_regex, "^([[:alnum:]_\\-\\.]*)(?:[[:space:]]*;[[:space:]]*([[:alnum:]_\\-\\.\\/\\\\]*)(?:[[:space:]]*;[[:space:]]*([[:alnum:]_\\-\\.\\/\\\\]*)(?:[[:space:]]*;[[:space:]]*([[:alnum:]_\\-\\.\\/\\\\]*)[[:space:]]*)?)?)(?:[[:space:]]*#[^\\n]*)?", REG_EXTENDED|REG_NEWLINE)) != 0) {
        regerror(err, &property_value_aliases_regex, errbuf, 512);
        fprintf(stderr, "Failed to compile regex err:\n%s\n", errbuf);
        return -1;
    }

#ifdef _WIN32
    WIN32_FIND_DATA fdFile;
    HANDLE hFind;

    char *sDir = ".\\UCD";
    char sPath[2048] = ".\\UCD\\*.*";
    
    if((hFind = FindFirstFile(sPath, &fdFile)) == INVALID_HANDLE_VALUE) {
        printf("Path not found: [%s]\n", sDir);
        return -1;
    }

    printf("Path found: [%s]\n", sDir);
    do {
        //Find first file will always return "."
        //    and ".." as the first two directories.
        if(strcmp(fdFile.cFileName, ".") != 0
                && strcmp(fdFile.cFileName, "..") != 0) {
            //Build up our file path using the passed in
            //  [sDir] and the file/foldername we just found:
            snprintf(sPath, 2048, "%s\\%s", sDir, fdFile.cFileName);

            //Is the entity a Folder?
            if(fdFile.dwFileAttributes &FILE_ATTRIBUTE_DIRECTORY) {
                printf("Directory: %s\n", sPath);
                regmatch_t match[5];
                if (regexec(&ver_regex, fdFile.cFileName, 5, match, 0) != REG_NOMATCH) {
                    // Regex match, generate UCD file
                    char filename[100];
                    int major = atoi(fdFile.cFileName + match[1].rm_so);
                    int minor = atoi(fdFile.cFileName + match[2].rm_so);
                    int patch = atoi(fdFile.cFileName + match[3].rm_so);
                    snprintf(filename,sizeof(filename),"hz_ucd_%d_%d_%.*s%d.h", major, minor,
                            match[4].rm_eo-match[4].rm_so, fdFile.cFileName + match[4].rm_so,
                            patch);

                    char filename2[100];
                    snprintf(filename2,100,"./hz/%s", filename);
                    FILE *f = fopen(filename2, "w+");
                    if (f) {
                        char macro[100]; strcpy(macro,filename);
                        for (char *p = macro; *p != '\0'; ++p) {
                            if (*p == '.') *p = '_';
                        }
                        strupr(macro);

                        fprintf(f, "#ifndef %s\n", macro);
                        fprintf(f, "#define %s\n\n", macro);
                        fprintf(f, "#include <stdint.h>\n\n");
                        fprintf(f, "#define HZ_UCD_VERSION HZ_MAKE_VERSION(%d,%d,%d)\n\n",major,minor,patch);

                        {
                            // Add enum definitions from PropertyValueAliases.txt
                            char tmp[100];
                            snprintf(tmp,100,"%s\\ucd\\PropertyValueAliases.txt", sPath);
                            printf("%s\n", tmp);
                            FILE *property_value_aliases = fopen(tmp,"r");
                            if (property_value_aliases) { // Read entire file data, then split into lines and parse
                                fseek(property_value_aliases,0,SEEK_END);
                                size_t filesize = ftell(property_value_aliases);
                                fseek(property_value_aliases,0,SEEK_SET);
                                char *str = malloc(filesize+1); 
                                fread(str, 1, filesize, property_value_aliases);
                                str[filesize] = '\0';

                                // Allocate big enough buffer for joining groups
                                #define MAX_PROPERTY_VALUE_ALIASES 2048
                                property_value_alias_t *aliases = malloc(sizeof(aliases[0]) * MAX_PROPERTY_VALUE_ALIASES);
                                char *s = str;
                                int off, len;
                                int cnt;
                                for (cnt=0; ; ++cnt) {
                                    regmatch_t match[6];
                                    if (regexec(&property_value_aliases_regex, s, 6, match, 0) == REG_NOMATCH)
                                        break;

                                    off = match[0].rm_so + (s - str);
                                    len = match[0].rm_eo - match[0].rm_so;

                                    property_value_alias_t alias;
                                    for (int k = 0; k < 5; ++k) {
                                        alias.txt[k] = s + match[k+1].rm_so;
                                        alias.sizes[k] = match[k+1].rm_eo - match[k+1].rm_so;
                                    }
                                    aliases[cnt] = alias;
                                    s += match[0].rm_eo;
                                }

                                fprintf(f, "typedef enum {");
                                int v = 16;
                                for (int j = 0; j < cnt; ++j) {
                                    if (aliases[j].sizes[0] == 2 && !strncmp(aliases[j].txt[0], "jt", 2)) {
                                        char enum_name[50];
                                        snprintf(enum_name, 50, "\n    HZ_JOINING_TYPE_%.*s = 1 << %d,", aliases[j].sizes[1], aliases[j].txt[1], v);
                                        fprintf(f, strupr(enum_name));
                                        ++v;
                                    }
                                }
                                fprintf(f, "\n} hz_joining_type_t;\n\n");

                                fprintf(f, "typedef enum {");
                                fprintf(f, "\n    HZ_JOINING_GROUP_NONE,");
                                for (int j = 0; j < cnt; ++j) {
                                    if (aliases[j].sizes[0] == 2 && !strncmp(aliases[j].txt[0], "jg", 2)) {
                                        char enum_name[50]="";
                                        strncat(enum_name, aliases[j].txt[2], aliases[j].sizes[2]);
                                        enum_name[aliases[j].sizes[2]] = '\0';
                                        if (strcmp(enum_name,"No_Joining_Group")) {
                                            fprintf(f,"\n    HZ_JOINING_GROUP_%s,", strupr(enum_name));
                                        }
                                    }
                                }
                                fprintf(f, "\n} hz_joining_group_t;\n\n");

                                free(aliases);
                                fclose(property_value_aliases);
                            }
                        }

                        if (0) { // Add Korean Jamo table from Jamo.txt
                            char tmp[100];
                            snprintf(tmp,100,"%s\\ucd\\Jamo.txt", sPath);
                            printf("%s\n", tmp);
                            FILE *jamo = fopen(tmp,"r");
                            if (jamo) { // Read entire file data, then split into lines and parse
                                mph_table_t jamo_mph_table;
                                fseek(jamo,0,SEEK_END);
                                size_t filesize = ftell(jamo);
                                fseek(jamo,0,SEEK_SET);
                                char *str = malloc(filesize+1); 
                                fread(str,1,filesize,jamo);
                                str[filesize] = '\0';
                                // Allocate big enough buffer for joining data

                                #define JAMO_SIZE 2048
                                jamo_t *jamo_data = malloc(sizeof(*jamo_data) * JAMO_SIZE);
                                uint32_t *jamo_data_keys = malloc(sizeof(uint32_t) * JAMO_SIZE);

                                char *s = str;
                                int off, len, cnt;
                                for (cnt=0; ; ++cnt) {
                                    regmatch_t match[3];
                                    if (regexec(&jamo_regex, s, 3, match, 0) == REG_NOMATCH)
                                        break;

                                    off = match[0].rm_so + (s - str);
                                    len = match[0].rm_eo - match[0].rm_so;

                                    jamo_data_keys[cnt] = strtol(s+match[1].rm_so,NULL,16);
                                    jamo_data[cnt].codepoint = jamo_data_keys[cnt];
                                    memset(jamo_data[cnt].name, 0, sizeof jamo_data[cnt].name);
                                    snprintf(jamo_data[cnt].name, sizeof jamo_data[cnt].name, "%.*s", match[2].rm_eo - match[2].rm_so, s + match[2].rm_so);

                                    printf("%s\n", jamo_data[cnt].name);

                                    s += match[0].rm_eo;
                                }

                                mph_table_init(&jamo_mph_table,
                                                jamo_data_keys,
                                                jamo_data,
                                                sizeof(jamo_data[0]),
                                                cnt,
                                                hash2_lowbias32);


/*
                                fprintf(f, "int32_t hz_ucd_jamo_short_names_k2[%d] = {", cnt);
                                for (int i = 0; i < jamo_mph_table.size; ++i) {
                                    int k2 = jamo_mph_table.k2[i];
                                    if (!(i % INTS_PER_LINE)) fprintf(f,"\n    ");
                                    fprintf(f, "%4d,", k2);
                                }

                                fprintf(f,"\n};\n\n");

                                // Short Names
                                fprintf(f, "uint32_t hz_ucd_jamo_short_names[%d] = {",cnt);
                                for (int i = 0; i < jamo_mph_table.size; ++i) {
                                    jamo_t dat = ((jamo_t*)jamo_mph_table.values)[i];
                                    fprintf(f, "\n    HZ_JAMO_SHORT_NAME_%s,", dat.name);
                                }

                                fprintf(f,"\n};\n\n");
                                */
                            }

                        }

                        { // Add arabic table
                            char tmp[100];
                            snprintf(tmp,100,"%s\\ucd\\ArabicShaping.txt", sPath);
                            printf("%s\n", tmp);
                            FILE *arabic_shaping = fopen(tmp,"r");
                            if (arabic_shaping) { // Read entire file data, then split into lines and parse
                                mph_table_t arabic_shaping_table;
                                fseek(arabic_shaping,0,SEEK_END);
                                size_t filesize = ftell(arabic_shaping);
                                fseek(arabic_shaping,0,SEEK_SET);
                                char *str = malloc(filesize+1); 
                                fread(str,1,filesize,arabic_shaping);
                                str[filesize] = '\0';
                                // Allocate big enough buffer for joining data

                                #define ARABIC_JOINING_SIZE 2048
                                arabic_joining_data_t *arabic_data = malloc(sizeof(*arabic_data) * ARABIC_JOINING_SIZE);
                                uint32_t *arabic_data_keys = malloc(sizeof(uint32_t) * ARABIC_JOINING_SIZE);

                                char *s = str;
                                int off, len, cnt;
                                for (cnt=0; ; ++cnt) {
                                    regmatch_t match[5];
                                    if (regexec(&arabic_shaping_regex, s, 5, match, 0) == REG_NOMATCH)
                                        break;

                                    off = match[0].rm_so + (s - str);
                                    len = match[0].rm_eo - match[0].rm_so;

                                    // Create joining data
                                    arabic_joining_data_t jd;
                                    memset(&jd,0,sizeof(jd));
                                    jd.codepoint = strtol(s + match[1].rm_so, NULL, 16);
                                    // printf("%d -> %.*s : %04x\n", cnt, match[1].rm_eo - match[1].rm_so, s + match[1].rm_so, jd.codepoint);
                                    jd.joining_type = s[match[3].rm_so]; // Joining type
                                    if (match[2].rm_so != -1) {
                                        jd.comment = s + match[2].rm_so;
                                        jd.comment_len = match[2].rm_eo - match[2].rm_so;
                                    }
                                    if (match[4].rm_so != -1) {
                                        jd.joining_group = s + match[4].rm_so;
                                        jd.joining_group_len =  match[4].rm_eo - match[4].rm_so;
                                    }

                                    arabic_data[cnt] = jd;
                                    arabic_data_keys[cnt] = jd.codepoint;

                                    s += match[0].rm_eo;
                                }

                                mph_table_init(&arabic_shaping_table,
                                                arabic_data_keys,
                                                arabic_data,
                                                sizeof(arabic_joining_data_t),
                                                cnt,
                                                hash2_lowbias32);

                                printf("ArabicShaping.txt MPH table created\n");

                                fprintf(f, "static int32_t hz_ucd_arabic_joining_k2[%d] = {", cnt);
                                for (int i = 0; i < arabic_shaping_table.size; ++i) {
                                    int k2 = arabic_shaping_table.k2[i];
                                    if (!(i % INTS_PER_LINE)) fprintf(f,"\n    ");
                                    fprintf(f, "%4d,", k2);
                                }

                                fprintf(f,"\n};\n\n");

                                // UCS 
                                fprintf(f, "static uint32_t hz_ucd_arabic_joining_ucs_codepoints[%d] = {",cnt);
                                for (int i = 0; i < arabic_shaping_table.size; ++i) {
                                    arabic_joining_data_t dat = ((arabic_joining_data_t*)arabic_shaping_table.values)[i];
                                    if (!(i % INTS_PER_LINE))
                                        fprintf(f,"\n    ");
                                    fprintf(f, "%8d,", dat.codepoint);
                                }

                                fprintf(f,"\n};\n\n");
                                printf("UCS Done\n");

                                fprintf(f, "static uint32_t hz_ucd_arabic_joining_data[%d] = {",cnt);
                                for (int i = 0; i < arabic_shaping_table.size; ++i) {
                                    arabic_joining_data_t dat = ((arabic_joining_data_t*)arabic_shaping_table.values)[i];
                                    fprintf(f,"\n    ");
                                    char jg[50]="";
                                    strncat(jg, dat.joining_group, dat.joining_group_len);
                                    if (!strcmp("No_Joining_Group", jg)) {
                                        fprintf(f, "HZ_JOINING_TYPE_%c | HZ_JOINING_GROUP_NONE,", dat.joining_type);
                                    } else {
                                        strupr(jg); // uppercase
                                        // Change spaces to underscores
                                        for (char *p = jg; *p != '\0'; ++p) {
                                            if (*p == ' ') *p = '_';
                                        }
                                        fprintf(f, "HZ_JOINING_TYPE_%c | HZ_JOINING_GROUP_%s,", dat.joining_type, jg);
                                    }
                                }

                                fprintf(f,"\n};\n\n");
#if 0
                                fprintf(f, "static char const* const hz_ucd_arabic_joining_ucs_names[%d] = {",cnt);
                                for (int i = 0; i < arabic_shaping_table.size; ++i) {
                                    arabic_joining_data_t dat = ((arabic_joining_data_t*)arabic_shaping_table.values)[i];
                                    fprintf(f,"\n    ");
                                    fprintf(f, "\"%.*s\",", dat.comment_len, dat.comment);
                                }

                                fprintf(f,"\n};\n\n");
#endif
                                free(arabic_data);
                                free(str);
                                mph_table_deinit(&arabic_shaping_table);
                                fclose(arabic_shaping);
                            }
                        }

                        fprintf(f, "#endif /* %s */", macro);
                        fclose(f);
                    }
                    printf("%s\n",filename);
                }
            }
        }
    }
    while(FindNextFile(hFind, &fdFile)); //Find the next file.
#endif
}

int main(int argc, char *argv[]) {
    generate();
    return EXIT_SUCCESS;
}