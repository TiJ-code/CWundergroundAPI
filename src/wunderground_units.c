#include "../include/wunderground_units.h"

char wu_unit_to_char(const wu_unit_t unit) {
    switch (unit) {
        default:
        case WU_UNIT_METRIC:    return 'm';
        case WU_UNIT_IMPERIAL:  return 'e';
        case WU_UNIT_HYBRID:    return 'h';
    }
}