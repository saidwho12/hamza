/*
    This file is part of Hamza.

    Hamza is free software: you can redistribute it and/or modify it under the terms of the GNU Lesser General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
    
    Hamza is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.
    You should have received a copy of the GNU Lesser General Public License along with Hamza. If not, see <https://www.gnu.org/licenses/>. 
*/

#include <stdint.h>
#include <regex.h>
#include <curl/curl.h>

struct callback_data {
    FILE *output;
    char *folder;
};

static long file_is_coming(struct curl_fileinfo *finfo,
                           struct callback_data *data,
                           int remains)
{
  printf("%3d %40s %10luB ", remains, finfo->filename,
         (unsigned long)finfo->size);
 
  switch(finfo->filetype) {
  case CURLFILETYPE_DIRECTORY:
    printf(" DIR\n");
    break;
  case CURLFILETYPE_FILE:
    printf("FILE ");
    break;
  default:
    printf("OTHER\n");
    break;
  }
 
  if(finfo->filetype == CURLFILETYPE_FILE) {
    char path[150];
    snprintf(path,150,"./UCD/%s/",data->folder );
    for (char *p = path; *p != '\0'; ++p) {
        if (*p == '/') *p = '\\';
    }

    // MakeDirTree(path);
    char syscall[200];
    snprintf(syscall,200,"mkdir %s",path);

    system(syscall);
    strcat(path, finfo->filename);
    printf("%s\n",path);

    data->output = fopen(path, "wb+");
    if(!data->output) {
      return CURL_CHUNK_BGN_FUNC_FAIL;
    }
  }
 
  return CURL_CHUNK_BGN_FUNC_OK;
}
 
static long file_is_downloaded(struct callback_data *data)
{
  if(data->output) {
    printf("DOWNLOADED\n");
    fclose(data->output);
    data->output = 0x0;
  }
  return CURL_CHUNK_END_FUNC_OK;
}
 
static size_t write_it(char *buff, size_t size, size_t nmemb,
                       void *cb_data)
{
  struct callback_data *data = cb_data;
  size_t written = 0;
  if(data->output)
    written = fwrite(buff, size, nmemb, data->output);
  else
    /* listing output */
    written = fwrite(buff, size, nmemb, stdout);
  return written;
}

void wildcard_download(char *folder, const char* url) {
    CURL *curl;
    FILE *fp = NULL;
    CURLcode res;

    struct callback_data data = { 0, folder };  

    curl = curl_easy_init();
    if (curl) {
        //turn on wildcard matching
        curl_easy_setopt(curl, CURLOPT_WILDCARDMATCH, 1L);
        // callback is called before download of concrete file started
        curl_easy_setopt(curl, CURLOPT_CHUNK_BGN_FUNCTION, file_is_coming);
        // callback is called after data from the file have been transferred
        curl_easy_setopt(curl, CURLOPT_CHUNK_END_FUNCTION, file_is_downloaded);
        // this callback will write contents into files
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_it);
        // put transfer data into callbacks
        curl_easy_setopt(curl, CURLOPT_CHUNK_DATA, &data);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &data);

        curl_easy_setopt(curl, CURLOPT_URL, url);

        res = curl_easy_perform(curl);

        curl_easy_cleanup(curl);
    }
}

// https://www.regular-expressions.info/posix.html
// https://www.regular-expressions.info/posixbrackets.html

// POSIX compliant regex
// #define UC_VERSION_REGEX "(([0-9]+)\\.([0-9]+)((?:\\.|-Update)[0-9]+)?[\n\f\r\x85\u2028\u2029])+"
#define UC_VERSION_REGEX "([0-9]+)\\.([0-9]+)(?:\\.([0-9]+)|-(Update(?:[0-9]+)?))"
#define ARABIC_SHAPING_REGEX "^([[:xdigit:]]*)[[:space:]]*;[[:space:]]*((?:[[:alpha:]][[:space:]]{0,1})+)[[:space:]]*;[[:space:]]*([[:alpha:]])[[:space:]]*;[[:space:]]*([[:alpha:]]*).*$"

size_t nlst_callback(void *data, size_t size, size_t count, void *arg) {
    int status;
    regex_t preg;
    if (regcomp(&preg, UC_VERSION_REGEX, REG_EXTENDED|REG_NEWLINE) != 0) {
        fprintf(stderr, "Failed to compile regex.\n");
        return -1;
    }

    // Match Unicode version folders on multiple lines
    char *str = (char *)data;
    char *s = str;
    int off, len;
    
    for (int i = 0; ; i++) {
        regmatch_t match[5];
        if (regexec(&preg, s, 5, match, 0) == REG_NOMATCH)
            break;
        
        off = match[0].rm_so + (s - str);
        len = match[0].rm_eo - match[0].rm_so;
        printf("#%d:\n", i);
        printf("offset = %jd; length = %jd\n", (uint32_t) off,
                (uint32_t) len);
        for (int j = 0; j < 5; ++j) {
            printf("match #%d = \"%.*s\"\n", j, match[j].rm_eo - match[j].rm_so, s + match[j].rm_so);
        }

        // if there's a ucd subfolder, download .txt files
        char folder[40];
        snprintf(folder,100,"%.*s/ucd",len, str+off);

        char wildcard[150];
        snprintf(wildcard, 100, "ftp://ftp.unicode.org/Public/%s/*.txt", folder);
        printf("%s\n", wildcard);

        wildcard_download(folder, wildcard);

        s += match[0].rm_eo;
    }

    regfree(&preg);
    return count*size;
}

void pull()
{
    CURL* curl;
    CURLcode res;
    
    curl_global_init(CURL_GLOBAL_ALL);
    curl = curl_easy_init();

    if (curl) {
        // NLST directory
        curl_easy_setopt(curl, CURLOPT_URL, "ftp://ftp.unicode.org/Public/");

        curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "NLST");
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, nlst_callback);
        curl_easy_setopt(curl, CURLOPT_DIRLISTONLY, 1L);

        // verbose output
        curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);

        res = curl_easy_perform(curl);

        if(CURLE_OK != res) {
            /* We failed */
            fprintf(stderr, "curl_easy_perform() returned %s\n", curl_easy_strerror(res));
        }

        /* always cleanup */
        curl_easy_cleanup(curl);
    }

    curl_global_cleanup();
}

int main(int argc, char *argv[]) {
    pull();
    return 0;
}
