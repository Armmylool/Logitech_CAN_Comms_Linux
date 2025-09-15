#include "G29.h"
#include "Port.h"


/** @brief checksum for Brake cmd. */
uint8_t calculate_checksum(uint8_t *data, size_t length) ;

/** @brief process_data*/
void process_data(int raw_data) ;
