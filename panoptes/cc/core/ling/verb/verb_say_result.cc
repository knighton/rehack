#include "verb_say_result.h"

void VerbSayResult::Dump() const {
/*
        printf("Verb Say Result (");
        for (size_t i = 0; i < pre_words.size() - 1; ++i) {
            printf("%s ", pre_words[i].c_str());
        }
        if (pre_words.size()) {
            printf("%s", pre_words[pre_words.size() - 1].c_str());
        }
        printf(") (");
        for (size_t i = 0; i < main_words.size() - 1; ++i) {
            printf("%s ", main_words[i].c_str());
        }
        if (main_words.size()) {
            printf("%s", main_words[main_words.size() - 1].c_str());
        }
        printf(")\n");
*/
/*
        printf("Verb Say Result (");
        for (size_t i = 0; i < pre_words.size() - 1; ++i) {
            //printf("%zu ", pre_words[i].size());
            printf(".");
        }
        if (pre_words.size()) {
            //printf("%zu", pre_words[pre_words.size() - 1].size());
            printf(".");
        }
        printf(") (");
        for (size_t i = 0; i < main_words.size() - 1; ++i) {
            //printf("%zu ", main_words[i].size());
            printf(".");
        }
        if (main_words.size()) {
            //printf("%zu", main_words[main_words.size() - 1].size());
            printf(".");
        }
        printf(")\n");
    }
*/

    printf("[VerbSayResult::Dump] %zu %zu\n", pre_words.size(), main_words.size());
}
