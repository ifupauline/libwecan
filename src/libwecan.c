#include <math.h>
#include "libwecan.h"

/*
 * @brief return a mask with the len lower bits set (len in 1..64)
 */
static uint64_t len_mask(const uint8_t len)
{
        return (len >= 64) ? UINT64_MAX : (((uint64_t)1 << len) - 1);
}

/*
 * @brief return lsb byte index, lsb bit offset and window size from signal
 *        params
 * @param startbit [In] signal startbit = lsb in case of little-endian, msb else
 * @param len      [In] signal len in bits (1..64)
 * @param endianness     INTEL if signal data is in little endian format
 * @param offset_lsb     [Out] offset (0..7) of the lsb in its containing byte
 * @param byte_idx_lsb   [Out] byte index of the lsb from the frame start
 * @param nbytes         [Out] number of bytes covered by the signal (1..9)
 */
static void compute_indexes(const uint16_t startbit, const uint8_t len,
                uint8_t endianness, uint16_t *offset_lsb,
                uint16_t *byte_idx_lsb, uint16_t *nbytes)
{
        uint32_t seq_msb = 0;
        uint32_t seq_lsb = 0;
        uint32_t msb = 0;

        if (endianness == MOTOROLA) {
                /*
                 * big-endian, dbc "sawtooth" bit numbering: convert the msb
                 * to a sequential numbering (bit 0 = msb of byte 0, left to
                 * right), the lsb is then simply len - 1 bits further
                 */
                seq_msb = (startbit / 8u) * 8u + (7u - startbit % 8u);
                seq_lsb = seq_msb + len - 1u;
                *byte_idx_lsb = (uint16_t)(seq_lsb / 8u);
                *offset_lsb = (uint16_t)(7u - seq_lsb % 8u);
                *nbytes = (uint16_t)(*byte_idx_lsb - startbit / 8u + 1u);

        } else {
                /* little-endian, right to left bit numbering from lsb to msb */
                msb = (uint32_t)startbit + len - 1u;
                *byte_idx_lsb = startbit / 8u;
                *offset_lsb = startbit % 8u;
                *nbytes = (uint16_t)(msb / 8u - *byte_idx_lsb + 1u);
        }
}

/*
 * @brief frame index of the k-th byte of the signal window, counted from the
 *        byte containing the lsb (msb side is higher for INTEL, lower for
 *        MOTOROLA)
 */
static uint16_t window_idx(uint16_t byte_idx_lsb, uint16_t k,
                uint8_t endianness)
{
        return (endianness == MOTOROLA) ? byte_idx_lsb - k : byte_idx_lsb + k;
}

/*
 * Note: values are assembled byte by byte with shifts, so the result does not
 * depend on the host endianness and signals spanning 9 bytes (len up to 64
 * with a non aligned lsb) are handled.
 */
uint64_t extract(const uint8_t *frame, const uint16_t startbit,
                const uint8_t len, uint8_t signedness, uint8_t endianness)
{
        int shift = 0;
        uint16_t k = 0;
        uint16_t nbytes = 0;
        uint16_t byte_idx_lsb = 0;
        uint16_t offset_lsb = 0;
        uint64_t byte = 0;
        uint64_t target = 0;
        uint64_t mask = 0;

        if (len == 0 || len > 64)
                return 0;

        compute_indexes(startbit, len, endianness, &offset_lsb,
                        &byte_idx_lsb, &nbytes);

#ifdef VERBOSE_DEBUG_
        printf("len: %d\n", len);
        printf("offset_lsb: %d\n", offset_lsb);
        printf("startbit: %d\n", startbit);
        printf("byte_idx_lsb: %d\nnbytes: %d\n", byte_idx_lsb, nbytes);
#endif

        for (k = 0; k < nbytes; k++) {
                byte = frame[window_idx(byte_idx_lsb, k, endianness)];
                shift = 8 * k - offset_lsb;
                if (shift < 0)
                        target |= byte >> -shift;
                else if (shift < 64)
                        target |= byte << shift;
        }

        /* clear higher bits not in signal (higher than msb) */
        mask = len_mask(len);
        target = target & mask;

        /* sign extension */
        if (signedness == SIGNED && len < 64) {
                /*
                 * we test if msb has value 1 (negative number), do sign bit
                 * extension all bits higher than msb set to 1, so that a cast
                 * of target to int64 will provide a correct negative value
                 */
                if ((target >> (len - 1)) & 1u)
                        target = target | ~mask;
        }
        return target;
}

/*
 * Note: can_value is truncated to len bits before insertion so that negative
 * (sign extended) or out of range values never overwrite neighbour signals.
 */
void insert(uint8_t *frame, uint16_t startbit, uint8_t len,
                uint64_t can_value, uint8_t endianness)
{
        int shift = 0;
        uint16_t i = 0;
        uint16_t k = 0;
        uint16_t nbytes = 0;
        uint16_t byte_idx_lsb = 0;
        uint16_t offset_lsb = 0;
        uint64_t mask = 0;
        uint64_t value = 0;
        uint8_t byte_mask = 0;
        uint8_t byte_value = 0;

        if (len == 0 || len > 64)
                return;

        compute_indexes(startbit, len, endianness, &offset_lsb,
                        &byte_idx_lsb, &nbytes);

#ifdef VERBOSE_DEBUG_
        printf("len: %d\n", len);
        printf("offset_lsb: %d\n", offset_lsb);
        printf("startbit: %d\n", startbit);
        printf("byte_idx_lsb: %d\nnbytes: %d\n", byte_idx_lsb, nbytes);
#endif

        mask = len_mask(len);
        value = can_value & mask;

        /* erase and insert signal bits byte by byte */
        for (k = 0; k < nbytes; k++) {
                i = window_idx(byte_idx_lsb, k, endianness);
                shift = 8 * k - offset_lsb;
                if (shift < 0) {
                        byte_mask = (uint8_t)(mask << -shift);
                        byte_value = (uint8_t)(value << -shift);
                } else if (shift < 64) {
                        byte_mask = (uint8_t)(mask >> shift);
                        byte_value = (uint8_t)(value >> shift);
                } else {
                        continue;
                }
                frame[i] = (uint8_t)((frame[i] & ~byte_mask) | byte_value);
        }
}

void encode_uint64_t(uint8_t *frame, uint64_t phy_value, uint16_t startbit,
                uint8_t len, uint8_t endianness, 
                double factor, double offset)
{
        uint64_t can_value = 0;
        can_value = (uint64_t)round((phy_value - offset) / factor);
        insert(frame, startbit, len, can_value, endianness);
}

void encode_int64_t(uint8_t *frame, int64_t phy_value, uint16_t startbit,
                uint8_t len, uint8_t endianness, 
                double factor, double offset)
{
        uint64_t can_value = 0;
        can_value = (int64_t)round((phy_value - offset) / factor);
        insert(frame, startbit, len, can_value, endianness);
}

void encode_double(uint8_t *frame, double phy_value, uint16_t startbit,
                uint8_t len, uint8_t endianness, 
                double factor, double offset)
{
        uint64_t can_value = 0;
        can_value = (int64_t)round((phy_value - offset) / factor);
        insert(frame, startbit, len, can_value, endianness);
}

void encode_float(uint8_t *frame, float phy_value, uint16_t startbit,
                uint8_t len, uint8_t endianness, 
                double factor, double offset)
{
        uint64_t can_value = 0;
        can_value = (int64_t)round((phy_value - offset) / factor);
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
