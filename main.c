#include <hz.h>
#include <hz-ft.h>

int main(int argc, char *argv[]) {
    {
        hz_language_t lang = hz_lang("ara");
        hz_language_map_t *lang_map = hz_get_language_map(lang);
        printf("%s\n", lang_map->language_name);
    }

    {
        hz_language_t lang = hz_lang("urd");
        hz_language_map_t *lang_map = hz_get_language_map(lang);
        printf("%s\n", lang_map->language_name);
    }

    {
        hz_language_t lang = hz_lang("zho");
        hz_language_map_t *lang_map = hz_get_language_map(lang);
        printf("%s\n", lang_map->language_name);
    }

    {
        hz_language_t lang = hz_lang("fra");
        hz_language_map_t *lang_map = hz_get_language_map(lang);
        printf("%s\n", lang_map->language_name);
    }

    return 0;
}
