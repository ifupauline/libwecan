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

void compute_indexes(const uint16_t startbit, const uint8_t length, uint8_t endianness, 
                uint8_t *offset_lsb, uint8_t *byte_index_lsb, uint8_t *byte_index_msb)
{
        uint16_t msb = 0; 
        uint16_t lsb = 0;

        if (endianness == MOTOROLA) {
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

        } else { /* is little endian */
                /* calcul index */
                lsb = startbit;
                msb = lsb + length - 1;
                *byte_index_msb = msb / 8;
                *byte_index_lsb = lsb / 8;
                *offset_lsb = lsb % 8;
        }
}

uint64_t frame_to_local(const uint8_t *frame, uint8_t byte_index_lsb, 
                uint8_t byte_index_msb, uint8_t endianness)
{
        uint64_t target = 0;

        /* 
         * do a cross copy of bytes containing signal (to little endian)
         * 0F EA FF --> FF EA 0F 
         */
        uint8_t *dest = (uint8_t*) &target;
        target = frame[byte_index_lsb];
        if (endianness == MOTOROLA) {

                int i;
                for (i = byte_index_lsb - 1; i >= byte_index_msb; i--) {
                        dest++;
                        *dest = frame[i];
                }

        } else { /* is little endian */
                /* copy signal to target */
                int i;
                for (i = byte_index_lsb + 1; i <= byte_index_msb; i++) {
                        dest++;
                        *dest = frame[i];
                }
        }
        return target;
}

uint64_t extract(const uint8_t *frame, const uint16_t startbit, 
                const uint8_t length, uint8_t signedness, uint8_t endianness)
{
        /* 
         * function returns the signal value in the can frame 
         * if signal is negative, cast to int64_t 
         */
        uint8_t byte_index_msb = 0;
        uint8_t byte_index_lsb = 0;
        uint8_t offset_lsb = 0;
        uint64_t target = 0;

        compute_indexes(startbit, length, endianness, &offset_lsb, 
                        &byte_index_lsb, &byte_index_msb);

        target = frame_to_local(frame, byte_index_lsb, byte_index_msb, endianness);

        /* debug */
        //printf("length is: %d\n", length);
        //printf("offset_lsb is: %d\n", offset_lsb);
        //printf("startbit is: %d\n", startbit);
        //printf("byte_index_lsb: %d\nbyte_index_msb: %d\n", byte_index_lsb, byte_index_msb);

        /* shift the signal to b0 of target */
        target = target >> offset_lsb;

        /* clear bits after msb  ex:1111 1111 --> 0000 1111 */
        uint64_t mask = ((uint64_t)1 << length) - 1;
        target = target & mask;

        /* sign extension */
        if (signedness == SIGNED) {
                /*  we test if msb is at 1 (negative number), sign extension */
                if (((uint64_t)1 << (length - 1)) & target) {
                        uint64_t sign_ext_mask = ~mask;
                        target = target | sign_ext_mask;
                }
        }
        return target;
}

void insert(uint8_t *frame, uint16_t startbit, uint8_t length, uint64_t value, 
                uint8_t endianness)
{
        uint8_t byte_index_msb = 0;
        uint8_t byte_index_lsb = 0;
        uint8_t offset_lsb = 0;
        uint64_t target = 0;

        compute_indexes(startbit, length, endianness, &offset_lsb, 
                        &byte_index_lsb, &byte_index_msb);

        target = frame_to_local(frame, byte_index_lsb, byte_index_msb, endianness);

        /* mask */
        uint64_t mask = ((uint64_t)1 << length) - 1;
        uint64_t erase_mask = ~(mask << offset_lsb);
        target = (target & erase_mask) | (value << offset_lsb);

        /* debug */
        //printf("length is: %d\n", length);
        //printf("offset_lsb is: %d\n", offset_lsb);
        //printf("startbit is: %d\n", startbit);
        //printf("byte_index_lsb: %d\nbyte_index_msb: %d\n", byte_index_lsb, byte_index_msb);

        /* copy local to frame */
        uint8_t *src = (uint8_t*) &target;
        if (endianness == MOTOROLA) {

                int i;
                for (i = byte_index_lsb; i >= byte_index_msb; i--) {
                        frame[i] = *src;
                        src++;
                }

        } else { /* is little endian */

                int i;
                for (i = byte_index_lsb; i <= byte_index_msb; i++) {
                        frame[i] = *src;
                        src++;
                }
        }
}

void encode_uint64_t(uint8_t *frame, uint64_t physical_value, uint16_t startbit,
                uint8_t length, uint8_t endianness, double factor, double offset)
{
        uint64_t can_value = (physical_value - offset) / factor;
        insert(frame, startbit, length, can_value, endianness);
}

void encode_int64_t(uint8_t *frame, int64_t physical_value, uint16_t startbit,
                uint8_t length, uint8_t endianness, double factor, double offset)
{
        uint64_t can_value = (physical_value - offset) / factor;
        insert(frame, startbit, length, can_value, endianness);
}

void encode_double(uint8_t *frame, double physical_value, uint16_t startbit,
                uint8_t length, uint8_t endianness, double factor, double offset)
{
        uint64_t can_value = (uint64_t)((physical_value - offset) / factor);
        insert(frame, startbit, length, can_value, endianness);
}

void encode_float(uint8_t *frame, float physical_value, uint16_t startbit,
                uint8_t length, uint8_t endianness, double factor, double offset)
{
        uint64_t can_value = (uint64_t)((physical_value - offset) / factor);
        insert(frame, startbit, length, can_value, endianness);
}

uint64_t decode_uint64_t(uint8_t *frame, uint16_t startbit, uint8_t length, 
                uint8_t endianness, double factor, double offset)
{
        uint64_t can_value = extract(frame, startbit, length, UNSIGNED, endianness);
        return (can_value * factor) + offset;
}

int64_t decode_int64_t(uint8_t *frame, uint16_t startbit, uint8_t length, 
                uint8_t endianness, double factor, double offset)
{
        int64_t can_value = (int64_t)extract(frame, startbit, length, SIGNED, endianness);
        return (can_value * factor) + offset;
}

double decode_double(uint8_t *frame, uint16_t startbit, uint8_t length, 
                uint8_t endianness, double factor, double offset)
{
        int64_t can_value = (int64_t)extract(frame, startbit, length, SIGNED, endianness);
        return (can_value * factor) + offset;
}

float decode_float(uint8_t *frame, uint16_t startbit, uint8_t length, 
                uint8_t endianness, double factor, double offset)
{
        int64_t can_value = (int64_t)extract(frame, startbit, length, SIGNED, endianness);
        return (can_value * factor) + offset;
}
#endif /* LIBWECAN_H */
