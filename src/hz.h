#ifndef HZ_H
#define HZ_H

#include "hz-ot.h"
#include "hz-script-table.h"

#ifdef __cplusplus
extern "C" {
#endif
    hz_index_t
    hz_face_map_unicode_to_id(hz_face_t *face, hz_unicode_t c);

    const hz_language_map_t *
    hz_get_language_map(hz_language_t lang);

    hz_language_t
    hz_lang(const char *tag);
    
    void
    hz_shape(hz_font_t *font, hz_sequence_t *sequence, hz_feature_t *features, unsigned int num_features);

#ifdef __cplusplus
}
#endif

#endif /* HZ_H */
