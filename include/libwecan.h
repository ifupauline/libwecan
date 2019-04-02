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
 * @brief return byte indexes of msb, lsb and lsb bit offset from signal parameters
 * @param startbit [In] signal startbit = lsb in case of little-endian, msb else
 * @param length   [In] signal length in bits
 * @param endianness  INTEL if signal data is in little endian format
 * @param offset_lsb     [Out] offset (0..7) of the lsb in its containing byte
 * @param byte_index_lsb [Out] byte index of the lsb from the matrix start
 * @param byte_index_msb [Out] byte index of the msb from the matrix start
 */
void compute_indexes(const uint16_t startbit, const uint8_t length, uint8_t endianness, 
                uint16_t *offset_lsb, uint16_t *byte_index_lsb, uint16_t *byte_index_msb)
{
        uint16_t msb = 0; 
        uint16_t lsb = 0;

        if (endianness == MOTOROLA) {
                /* 
                 * big-endian, compute lsb from msb using dbc bit numbering 
                 * definition from signal msb count down length-1 time, if bit 
                 * reachs bit_0 of current byte jump to bit_7 of next byte 
                 * (left to right bit numbering from msb to lsb)
                 */
                msb =  startbit;
                uint16_t current_bit_nr = startbit;

                int i;
                for (i = 0; i < length - 1; i++) {
                        if (current_bit_nr % 8 == 0) {
                                current_bit_nr = current_bit_nr + 15;

                        } else {
                                current_bit_nr--;
                        }
                }
                lsb = current_bit_nr;
                *byte_index_lsb = lsb / 8;
                *byte_index_msb = msb / 8;
                *offset_lsb = lsb % 8;

        } else { 
                /* little-endian, right to left bit numbering from lsb to msb */
                lsb = startbit;
                msb = lsb + length - 1;
                *byte_index_msb = msb / 8;
                *byte_index_lsb = lsb / 8;
                *offset_lsb = lsb % 8;
        }
}

/*
 * @brief copy 8 bytes data chunk containing signal to a local temp var
 *        the endianness is taken in account
 * @param frame  frame data address
 * @param byte_index_lsb  byte index of the lsb from the frame[0]
 * @param byte_index_msb  byte index of the msb from the frame[0]
 * @param endianness
 * @return uint64 in little endian format containing the signal to extract
 */
uint64_t frame_to_local(const uint8_t *frame, uint16_t byte_index_lsb, 
                uint16_t byte_index_msb, uint8_t endianness)
{
        uint64_t target = frame[byte_index_lsb];
        uint8_t *dest = (uint8_t*) &target;

        /* big-endian => do a reverse copy of bytes (aka copy and swap) */
        if (endianness == MOTOROLA) {

                int i;
                for (i = byte_index_lsb - 1; i >= byte_index_msb; i--) {
                        dest++;
                        *dest = frame[i];
                }

        } else {
                /* simple bytes copy little-endian -> little-endian */
                int i;
                for (i = byte_index_lsb + 1; i <= byte_index_msb; i++) {
                        dest++;
                        *dest = frame[i];
                }
        }
        return target;
}

/*
 * @brief extract a signal value from a CAN frame
 * @param frame     frame data address
 * @param startbit  signal start bit
 * @param length    signal length in bits
 * @param signedness
 * @param endianness
 * @return signal value as uint64 ready for decoding by using Factor and Offset
 *         If the signal value is signed (and negative), just cast the extracted value to int64
 *         to get its value thanks to the algo of sign bit extension
 */

uint64_t extract(const uint8_t *frame, const uint16_t startbit, 
                const uint8_t length, uint8_t signedness, uint8_t endianness)
{
        uint16_t byte_index_msb = 0;
        uint16_t byte_index_lsb = 0;
        uint16_t offset_lsb = 0;
        uint64_t target = 0;

        compute_indexes(startbit, length, endianness, &offset_lsb, 
                        &byte_index_lsb, &byte_index_msb);

        target = frame_to_local(frame, byte_index_lsb, byte_index_msb, endianness);

#ifdef VERBOSE_DEBUG_
        printf("length: %d\n", length);
        printf("offset_lsb: %d\n", offset_lsb);
        printf("startbit: %d\n", startbit);
        printf("byte_index_lsb: %d\nbyte_index_msb: %d\n", byte_index_lsb, byte_index_msb);
#endif

        /* remove lower bits not in signal (lower than offset lsb) */
        target = target >> offset_lsb;

        /* clear higher bits not in signal (higher than msb) */
        uint64_t mask = ((uint64_t)1 << length) - 1;
        target = target & mask;

        /* sign extension */
        if (signedness == SIGNED) {
                /* 
                 * we test if msb has value 1 (negative number), do sign bit 
                 * extension all bits higher than msb set to 1, so that a cast 
                 * of target to int64 will provide a correct negative value
                 */
                if (((uint64_t)1 << (length - 1)) & target) {
                        uint64_t sign_ext_mask = ~mask;
                        target = target | sign_ext_mask;
                }
        }
        return target;
}

/*
 * @brief insert a signal value into a CAN frame
 * @param frame     [In/Out] frame data address
 * @param startbit  signal start bit
 * @param length    signal length in bits
 * @param value     signal value
 * @param endianness
 */
void insert(uint8_t *frame, uint16_t startbit, uint8_t length, uint64_t can_value, 
                uint8_t endianness)
{
        uint16_t byte_index_msb = 0;
        uint16_t byte_index_lsb = 0;
        uint16_t offset_lsb = 0;
        uint64_t target = 0;

        compute_indexes(startbit, length, endianness, &offset_lsb, 
                        &byte_index_lsb, &byte_index_msb);

        target = frame_to_local(frame, byte_index_lsb, byte_index_msb, endianness);
        

        /* create mask to erase current signal value */
        uint64_t mask = ((uint64_t)1 << length) - 1;
        uint64_t erase_mask = ~(mask << offset_lsb);

        /* erase and insert signal value into local data chunk */
        target = (target & erase_mask) | (can_value << offset_lsb);

#ifdef VERBOSE_DEBUG_
        printf("length: %d\n", length);
        printf("offset_lsb: %d\n", offset_lsb);
        printf("startbit: %d\n", startbit);
        printf("byte_index_lsb: %d\nbyte_index_msb: %d\n", byte_index_lsb, byte_index_msb);
#endif

        /* 
         * copy back data chunk containing signal into CAN frame by taken into
         * account endianess
         */
        uint8_t *src = (uint8_t*) &target;
        if (endianness == MOTOROLA) {
                /* big-endian, reverse order copy (copy and swap) */
                int i;
                for (i = byte_index_lsb; i >= byte_index_msb; i--) {
                        frame[i] = *src;
                        src++;
                }

        } else {
                /* little-endian, simple bytes copy */
                int i;
                for (i = byte_index_lsb; i <= byte_index_msb; i++) {
                        frame[i] = *src;
                        src++;
                }
        }
}

/*
 * @brief encode a physical value into CAN format and insert it into a frame
 */
void encode_uint64_t(uint8_t *frame, uint64_t physical_value, uint16_t startbit,
                uint8_t length, uint8_t endianness, double factor, double offset)
{
        uint64_t can_value = (uint64_t)(((double)physical_value - offset) / factor);
        insert(frame, startbit, length, can_value, endianness);
}

void encode_int64_t(uint8_t *frame, int64_t physical_value, uint16_t startbit,
                uint8_t length, uint8_t endianness, double factor, double offset)
{
        uint64_t can_value = (int64_t)(((double)physical_value - offset) / factor);
        insert(frame, startbit, length, can_value, endianness);
}

void encode_double(uint8_t *frame, double physical_value, uint16_t startbit,
                uint8_t length, uint8_t endianness, double factor, double offset)
{
        uint64_t can_value = (int64_t)((physical_value - offset) / factor);
        insert(frame, startbit, length, can_value, endianness);
}

void encode_float(uint8_t *frame, float physical_value, uint16_t startbit,
                uint8_t length, uint8_t endianness, double factor, double offset)
{
        uint64_t can_value = (int64_t)(((double)physical_value - offset) / factor);
        insert(frame, startbit, length, can_value, endianness);
}

/*
 * @brief extract a signal and decode it into a physical value
 */
uint64_t decode_uint64_t(uint8_t *frame, uint16_t startbit, uint8_t length, 
                uint8_t endianness, double factor, double offset)
{
        uint64_t can_value = extract(frame, startbit, length, UNSIGNED, endianness);
        return (uint64_t)(((double)can_value * factor) + offset);
}

int64_t decode_int64_t(uint8_t *frame, uint16_t startbit, uint8_t length, 
                uint8_t endianness, double factor, double offset)
{
        int64_t can_value = (int64_t)extract(frame, startbit, length, SIGNED, endianness);
        return (int64_t)(((double)can_value * factor) + offset);
}

double decode_double(uint8_t *frame, uint16_t startbit, uint8_t length, 
                uint8_t endianness, double factor, double offset)
{
        int64_t can_value = (int64_t)extract(frame, startbit, length, SIGNED, endianness);
        return (double)(((double)can_value * factor) + offset);
}

float decode_float(uint8_t *frame, uint16_t startbit, uint8_t length, 
                uint8_t endianness, double factor, double offset)
{
        int64_t can_value = (int64_t)extract(frame, startbit, length, SIGNED, endianness);
        return (float)(((double)can_value * factor) + offset);
}
#endif /* LIBWECAN_H */
