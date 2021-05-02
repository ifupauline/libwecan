#include "libwecan.h"

/*
 * @brief return byte indexes of msb, lsb and lsb bit offset from signal params
 * @param startbit [In] signal startbit = lsb in case of little-endian, msb else
 * @param len      [In] signal len in bits
 * @param endianness     INTEL if signal data is in little endian format
 * @param offset_lsb     [Out] offset (0..7) of the lsb in its containing byte
 * @param byte_idx_lsb [Out] byte index of the lsb from the matrix start
 * @param byte_idx_msb [Out] byte index of the msb from the matrix start
 */
static void compute_indexes(const uint16_t startbit, const uint8_t len, 
                uint8_t endianness, uint16_t *offset_lsb, 
                uint16_t *byte_idx_lsb, uint16_t *byte_idx_msb)
{
        int i = 0;
        uint16_t msb = 0; 
        uint16_t lsb = 0;
        uint16_t current_bit_nr = 0;

        if (endianness == MOTOROLA) {
                /* 
                 * big-endian, compute lsb from msb using dbc bit numbering 
                 * definition from signal msb count down len-1 time, if bit 
                 * reachs bit_0 of current byte jump to bit_7 of next byte 
                 * (left to right bit numbering from msb to lsb)
                 */
                msb =  startbit;
                current_bit_nr = startbit;

                for (i = 0; i < len - 1; i++) {
                        if (current_bit_nr % 8 == 0) {
                                current_bit_nr = current_bit_nr + 15;

                        } else {
                                current_bit_nr--;
                        }
                }
                lsb = current_bit_nr;
                *byte_idx_lsb = lsb / 8;
                *byte_idx_msb = msb / 8;
                *offset_lsb = lsb % 8;

        } else { 
                /* little-endian, right to left bit numbering from lsb to msb */
                lsb = startbit;
                msb = lsb + len - 1;
                *byte_idx_msb = msb / 8;
                *byte_idx_lsb = lsb / 8;
                *offset_lsb = lsb % 8;
        }
}

/*
 * @brief copy 8 bytes data chunk containing signal to a local temp var
 *        the endianness is taken in account
 * @param frame  frame data address
 * @param byte_idx_lsb  byte index of the lsb from the frame[0]
 * @param byte_idx_msb  byte index of the msb from the frame[0]
 * @param endianness
 * @return uint64 in little endian format containing the signal to extract
 */
static uint64_t frame_to_local(const uint8_t *frame, uint16_t byte_idx_lsb, 
                uint16_t byte_idx_msb, uint8_t endianness)
{
        int i = 0;
        uint64_t target = 0;
        uint8_t *dest = NULL;
        target = frame[byte_idx_lsb];
        dest = (uint8_t*) &target;

        /* big-endian => do a reverse copy of bytes (aka copy and swap) */
        if (endianness == MOTOROLA) {
                for (i = byte_idx_lsb - 1; i >= byte_idx_msb; i--) {
                        dest++;
                        *dest = frame[i];
                }

        } else {
                /* simple bytes copy little-endian -> little-endian */
                for (i = byte_idx_lsb + 1; i <= byte_idx_msb; i++) {
                        dest++;
                        *dest = frame[i];
                }
        }
        return target;
}

uint64_t extract(const uint8_t *frame, const uint16_t startbit, 
                const uint8_t len, uint8_t signedness, uint8_t endianness)
{
        uint16_t byte_idx_msb = 0;
        uint16_t byte_idx_lsb = 0;
        uint16_t offset_lsb = 0;
        uint64_t target = 0;
        uint64_t mask = 0;
        uint64_t sign_ext_mask = 0;

        compute_indexes(startbit, len, endianness, &offset_lsb, 
                        &byte_idx_lsb, &byte_idx_msb);

        target = frame_to_local(frame, byte_idx_lsb, byte_idx_msb, 
                        endianness);

#ifdef VERBOSE_DEBUG_
        printf("len: %d\n", len);
        printf("offset_lsb: %d\n", offset_lsb);
        printf("startbit: %d\n", startbit);
        printf("byte_idx_lsb: %d\nbyte_idx_msb: %d\n", 
                        byte_idx_lsb, byte_idx_msb);
#endif

        /* remove lower bits not in signal (lower than offset lsb) */
        target = target >> offset_lsb;

        /* clear higher bits not in signal (higher than msb) */
        mask = ((uint64_t)1 << len) - 1;
        target = target & mask;

        /* sign extension */
        if (signedness == SIGNED) {
                /* 
                 * we test if msb has value 1 (negative number), do sign bit 
                 * extension all bits higher than msb set to 1, so that a cast 
                 * of target to int64 will provide a correct negative value
                 */
                if (((uint64_t)1 << (len - 1)) & target) {
                        sign_ext_mask = ~mask;
                        target = target | sign_ext_mask;
                }
        }
        return target;
}

void insert(uint8_t *frame, uint16_t startbit, uint8_t len, 
                uint64_t can_value, uint8_t endianness)
{
        int i = 0;
        uint8_t *src = NULL;
        uint16_t byte_idx_msb = 0;
        uint16_t byte_idx_lsb = 0;
        uint16_t offset_lsb = 0;
        uint64_t target = 0;
        uint64_t mask = 0;
        uint64_t erase_mask = 0;

        compute_indexes(startbit, len, endianness, &offset_lsb, 
                        &byte_idx_lsb, &byte_idx_msb);

        target = frame_to_local(frame, byte_idx_lsb, byte_idx_msb, 
                        endianness);

        /* create mask to erase current signal value */
        mask = ((uint64_t)1 << len) - 1;
        erase_mask = ~(mask << offset_lsb);

        /* erase and insert signal value into local data chunk */
        target = (target & erase_mask) | (can_value << offset_lsb);

#ifdef VERBOSE_DEBUG_
        printf("len: %d\n", len);
        printf("offset_lsb: %d\n", offset_lsb);
        printf("startbit: %d\n", startbit);
        printf("byte_idx_lsb: %d\nbyte_idx_msb: %d\n", 
                        byte_idx_lsb, byte_idx_msb);
#endif
        /* 
         * copy back data chunk containing signal into CAN frame by taken into
         * account endianess
         */
        src = (uint8_t*)&target;
        if (endianness == MOTOROLA) {
                /* big-endian, reverse order copy (copy and swap) */
                for (i = byte_idx_lsb; i >= byte_idx_msb; i--) {
                        frame[i] = *src;
                        src++;
                }

        } else {
                /* little-endian, simple bytes copy */
                for (i = byte_idx_lsb; i <= byte_idx_msb; i++) {
                        frame[i] = *src;
                        src++;
                }
        }
}

void encode_uint64_t(uint8_t *frame, uint64_t phy_value, uint16_t startbit,
                uint8_t len, uint8_t endianness, 
                double factor, double offset)
{
        uint64_t can_value = 0;
        can_value = (uint64_t)((phy_value - offset) / factor);
        insert(frame, startbit, len, can_value, endianness);
}

void encode_int64_t(uint8_t *frame, int64_t phy_value, uint16_t startbit,
                uint8_t len, uint8_t endianness, 
                double factor, double offset)
{
        uint64_t can_value = 0;
        can_value = (int64_t)((phy_value - offset) / factor);
        insert(frame, startbit, len, can_value, endianness);
}

void encode_double(uint8_t *frame, double phy_value, uint16_t startbit,
                uint8_t len, uint8_t endianness, 
                double factor, double offset)
{
        uint64_t can_value = 0;
        can_value = (int64_t)((phy_value - offset) / factor);
        insert(frame, startbit, len, can_value, endianness);
}

void encode_float(uint8_t *frame, float phy_value, uint16_t startbit,
                uint8_t len, uint8_t endianness, 
                double factor, double offset)
{
        uint64_t can_value = 0;
        can_value = (int64_t)((phy_value - offset) / factor);
        insert(frame, startbit, len, can_value, endianness);
}

/*
 * @brief extract a signal and decode it into a physical value
 */
uint64_t decode_uint64_t(uint8_t *frame, uint16_t startbit, uint8_t len, 
                uint8_t endianness, double factor, double offset)
{
        uint64_t can_value = 0;
        can_value = extract(frame, startbit, len, UNSIGNED, endianness);
        return (uint64_t)((can_value * factor) + offset);
}

int64_t decode_int64_t(uint8_t *frame, uint16_t startbit, uint8_t len, 
                uint8_t endianness, double factor, double offset)
{
        int64_t can_value = 0;
        can_value = (int64_t)extract(frame, startbit, len, SIGNED, endianness);
        return (int64_t)((can_value * factor) + offset);
}

double decode_double(uint8_t *frame, uint16_t startbit, uint8_t len, 
                uint8_t endianness, double factor, double offset)
{
        int64_t can_value = 0;
        can_value = (int64_t)extract(frame, startbit, len, SIGNED, endianness);
        return (double)((can_value * factor) + offset);
}

float decode_float(uint8_t *frame, uint16_t startbit, uint8_t len, 
                uint8_t endianness, double factor, double offset)
{
        int64_t can_value = 0;
        can_value = (int64_t)extract(frame, startbit, len, SIGNED, endianness);
        return (float)((can_value * factor) + offset);
}
