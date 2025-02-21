#include "defilter.h"

static int deapply_sub(byte *new_line, byte *old_line, size_t length, size_t bpp) {
    for (size_t i = 0; i < bpp; ++i) {
        new_line[i] = old_line[i];
    }

    for (size_t i = bpp; i < length; ++i) {
        new_line[i] = old_line[i] + new_line[i - bpp];
    }

    return 0;
}

static int deapply_up(byte *new_line, byte *old_line, byte *prev_line, size_t length) {
    for (size_t i = 0; i < length; ++i) {
        new_line[i] = old_line[i] + prev_line == NULL ? 0 : prev_line[i];
    }

    return 0;
}

static int deapply_average(byte *new_line, byte *old_line, byte *prev_line, size_t length, size_t bpp) {
    // For operations with additional bits used, replacing byte with unsigned short
    unsigned short raw, prior;

    for (size_t i = 0; i < length; ++i) {
        raw = i < bpp ? 0 : old_line[i - bpp];
        prior = prev_line == NULL ? 0 : prev_line[i];

        new_line[i] = old_line[i] + (byte)((raw + prior) >> 1); // <=> floor((raw + prior) / 2)
    }

    return 0;
}

static byte paeth_predictor(byte left, byte above, byte above_left) {
    unsigned short p = (unsigned short)left + above - above_left;
    unsigned short p_left = (unsigned short)abs(p - left);
    unsigned short p_above = (unsigned short)abs(p - above);
    unsigned short p_above_left = (unsigned short)abs(p - above_left);

    if (p_left <= p_above && p_left <= p_above_left)
        return left;
    else if (p_above <= p_above_left)
        return above;
    else
        return above_left;
}

static int deapply_paeth(byte *new_line, byte *old_line, byte *prev_line, size_t length, size_t bpp) {
    for (size_t i = 0; i < bpp; ++i) {
        new_line[i] = old_line[i] + paeth_predictor(0, prev_line[i], 0);
    }

    for (size_t i = bpp; i < length; ++i) {
        new_line[i] = old_line[i] + paeth_predictor(old_line[i - bpp], prev_line[i], prev_line[i - bpp]);
    }

    return 0;
}

static int defilter_line(byte *new_line, byte *old_line, size_t length, size_t bpp) {
    byte *prev_line = old_line - length;
    
    FilterType type = old_line[0];
    ++old_line;

    switch (type) {
        case NONE:
            return 0;
        case SUB:
            return deapply_sub(new_line, old_line, length, bpp);
        case UP:
            return deapply_up(new_line, old_line, prev_line, length);
        case AVERAGE:
            return deapply_average(new_line, old_line, prev_line, length, bpp);
        case PAETH:
            return deapply_paeth(new_line, old_line, prev_line, length, bpp);
        default:
            return -1;
    } 
}

byte *defilter(byte *old_data, size_t *size, size_t height, size_t bpp) {
    byte *new_data = malloc((*size - height) * sizeof(byte));
    size_t scanline_length = *size / height - 1;

    for (size_t i = 1; i < height; ++i) {
        defilter_line(new_data + scanline_length * i, old_data + (scanline_length + 1) * i,
                      scanline_length, bpp);
    }

    *size -= height;
    return new_data;
}