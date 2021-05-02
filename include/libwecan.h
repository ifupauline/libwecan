#ifndef LIBWECAN_H
#define LIBWECAN_H

#include <stdio.h>
#include <unistd.h>
#include <stdint.h>

#define FALSE     0
#define TRUE      1
#define UNSIGNED  2
#define SIGNED    3
#define INTEL     4
#define MOTOROLA  5

/*
 * @brief extract a signal value from a CAN frame
 * @param frame     frame data address
 * @param startbit  signal start bit
 * @param len       signal len in bits
 * @param signedness
 * @param endianness
 * @return signal value as uint64 ready for decoding by using Factor and Offset
 *         If the signal value is signed (and negative), just cast the extracted 
 *         value to int64 to get its value thanks to the algo of sign bit 
 *         extension
 */
uint64_t extract(const uint8_t *frame, const uint16_t startbit, 
                const uint8_t len, uint8_t signedness, uint8_t endianness);

/*
 * @brief insert a signal value into a CAN frame
 * @param frame     [In/Out] frame data address
 * @param startbit  signal start bit
 * @param len       signal len in bits
 * @param value     signal value
 * @param endianness
 */
void insert(uint8_t *frame, uint16_t startbit, uint8_t len, 
                uint64_t can_value, uint8_t endianness);

/*
 * @brief encode a physical value into CAN format and insert it into a frame
 */
void encode_uint64_t(uint8_t *frame, uint64_t phy_value, uint16_t startbit,
                uint8_t len, uint8_t endianness, 
                double factor, double offset);

void encode_int64_t(uint8_t *frame, int64_t phy_value, uint16_t startbit,
                uint8_t len, uint8_t endianness, 
                double factor, double offset);

void encode_double(uint8_t *frame, double phy_value, uint16_t startbit,
                uint8_t len, uint8_t endianness, 
                double factor, double offset);

void encode_float(uint8_t *frame, float phy_value, uint16_t startbit,
                uint8_t len, uint8_t endianness, 
                double factor, double offset);

/*
 * @brief extract a signal and decode it into a physical value
 */
uint64_t decode_uint64_t(uint8_t *frame, uint16_t startbit, uint8_t len, 
                uint8_t endianness, double factor, double offset);

int64_t decode_int64_t(uint8_t *frame, uint16_t startbit, uint8_t len, 
                uint8_t endianness, double factor, double offset);

double decode_double(uint8_t *frame, uint16_t startbit, uint8_t len, 
                uint8_t endianness, double factor, double offset);

float decode_float(uint8_t *frame, uint16_t startbit, uint8_t len, 
                uint8_t endianness, double factor, double offset);

#endif /* LIBWECAN_H */
