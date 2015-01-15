/*
 * !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
 * Author : emptyhua@gmail.com
 * Create : 2011.9.26
 * !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pinyin.h"
#include "pinyin_data.h"

int pinyin_get_tones_by_unicode(wchar_t uni, char **tones_out)
{
    if (!pinyin_ishanzi(uni))
    {
        *tones_out = NULL;
        return 0;
    }

    int count = pinyin_count[uni - 19968];

    if (count == 0)
    {
        *tones_out = NULL;
        return 0;
    }

    char *tones = NULL;
    tones = strdup(pinyin_tones[uni - 19968]);

    *tones_out = tones;
    return count;

}

int pinyin_get_pinyins_by_unicode(wchar_t uni, const char ***pinyins_out)
{
    if (!pinyin_ishanzi(uni))
    {
        *pinyins_out = NULL;
        return 0;
    }
   

    int count = pinyin_count[uni - 19968];

    if (count == 0)
    {
        *pinyins_out = NULL;
        return 0;
    }

    /*
    fprintf(stderr, "wchar_t uni: %u\n", uni);
    fprintf(stderr, "%s: count: %d\n", __func__, count);
    */

    const char *indexs = pinyin_index[uni - 19968];
    const char **pinyins = (const char **)malloc(sizeof(const char *) * count);
    int i = 0;
    for (i = 0; i < count; i++)
    {
        int start = i * 2;
        pinyins[i] = pinyin_all_pinyin[(unsigned char)indexs[start] + (unsigned char)indexs[start + 1]];
    }

    *pinyins_out = pinyins;
    return count;
}
