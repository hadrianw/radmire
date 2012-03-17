#include "utils.h"
#include <stdlib.h>

struct Level {
        size_t offset;
	unsigned char mask;
	unsigned char val;
};

static struct Level levels[] = {
	{0, 0x80, 0},
	{1 << 7, 0xE0, 0xC0},
	{1 << 11, 0xF0, 0xE0}
        // last 1 << 16
};
static unsigned char BMASK = 0xC0;
static unsigned char BVAL= 0x80;

size_t utf8offset(const char *buff, size_t *offset)
{
        if(!buff || !buff[0] || !offset)
                return 0;

        char bytes = 1;
        struct Level *level = levels;
        for(; bytes <= LENGTH(levels); level++, bytes++)
                if((buff[0] & level->mask) == level->val)
                        break;
        if(bytes > LENGTH(levels))
                goto err;

        *offset = (buff[0] & ~level->mask) << 6 * (bytes - 1);
        for(int i = 1; i < bytes; i++) {
                if((buff[i] & BMASK) != BVAL)
                        goto err;
                *offset |= (buff[i] & ~BMASK) << 6 * (bytes - i - 1);
        }

        offset += level->offset;
        return bytes;

err:
        *offset = 0;
        return 0;
}
