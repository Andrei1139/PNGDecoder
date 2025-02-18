#include "defilter.h"
#include "image.h"

static int defilter_line(byte *line, size_t length) {
    FilterType type = line[0];
    ++line;

    switch (type) {
        case NONE:
            return 0;
        case SUB:
            
    }
}

int defilter(byte *data, size_t width, size_t height) {
    
}