#ifndef CWUNDERGROUNDAPI_WUNDERGROUND_UNITS_H
#define CWUNDERGROUNDAPI_WUNDERGROUND_UNITS_H

/**
 * @enum wu_unit_t
 * @brief Units system for Wunderground API responses.
 *
 * These values are passed to the API as single-letter codes:
 *  - WU_UNIT_METRIC    -> "m" (Celsius, km/h, hPa)
 *  - WU_UNIT_IMPERIAL  -> "e" (Fahrenheit, mph, inHg)
 *  - WU_UNIT_HYBRID    -> "h" (mixed system: Celsius + mph)
 */
typedef enum {
    WU_UNIT_METRIC = 0,   /**< Metric units ("m") */
    WU_UNIT_IMPERIAL = 1, /**< Imperial units ("e") */
    WU_UNIT_HYBRID = 2    /**< Hybrid units ("h") */
} wu_unit_t;

#endif //CWUNDERGROUNDAPI_WUNDERGROUND_UNITS_H