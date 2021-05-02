#include "libwecan.h"
#include <assert.h>
#include <string.h>

#define PRECISION 0.00001

static void display_one(uint8_t *frame, uint8_t dlc)
{
        int i;

        printf("\n");
        printf("encoded frame: ");
        for (i = 0; i < dlc; i++) {
                printf("%02x ", frame[i]);
        }
        printf("\n");
}

static void display_two(uint8_t *expected_frame, uint8_t *frame, uint8_t dlc)
{
        int i;

        printf("\n");
        printf("expected frame: ");
        for (i = 0; i < dlc; i++) {
                printf("%02x ", expected_frame[i]);
        }
        printf("\n");

        printf("frame:          ");
        for (i = 0; i < dlc; i++) {
                printf("%02x ", frame[i]);
        }
        printf("\n");
}

static int cmp_float(float f1, float f2)
{
        if (((f1 - PRECISION) < f2) && ((f1 + PRECISION) > f2))
                return TRUE;
        else 
                return FALSE;
}

static int cmp_double(double d1, double d2)
{
        if (((d1 - PRECISION) < d2) && ((d1 + PRECISION) > d2))
                return TRUE;
        else 
                return FALSE;
}

static int asserteq(uint8_t *expected_frame, uint8_t *frame, uint8_t dlc)
{
        int i;

        for (i = 0; i < dlc; i++) {
                if (expected_frame[i] == frame[i]) 
                        continue;
                else 
                        return FALSE;
        }
        return TRUE;
}

int main(void)
{
        int64_t sphy_value              = 0;
        int64_t value_signed            = 0;
        int64_t expected_value_signed   = 0;
        int64_t value_one_byte_signed   = 0;
        int64_t value_two_bytes_signed  = 0;
        int64_t value_four_bytes_signed = 0;

        uint16_t startbit                   = 0;
        uint64_t uphy_value                 = 0;
        uint64_t value_unsigned             = 0;
        uint64_t expected_value_unsigned    = 0;
        uint64_t value_one_byte_unsigned    = 0;
        uint64_t value_two_bytes_unsigned   = 0;
        uint64_t value_seven_bytes_unsigned = 0;

        float fphy_value  = 0.0;
        double factor     = 0.0;
        double offset     = 0.0;
        double dphy_value = 0.0;

        uint8_t len               = 0;
        uint8_t frame[8]          = {0};
        uint8_t frame_fd0[40]     = {0};
        uint8_t frame_fd1[56]     = {0};
        uint8_t frame_fd2[48]     = {0};
        uint8_t frame_fd3[64]     = {0};
        uint8_t frame_fd4[24]     = {0};
        uint8_t expected_frame[8] = {0};
        /*
         ***********************************************************************
         * EXTRACT MOTOROLA 
         ***********************************************************************
         */
        printf("===========================================================\n");
        printf("EXTRACT MOTOROLA\n");
        printf("===========================================================\n");

        /* 
         * step 1 (1 byte) 
         * step 1.1 all signal on 1 byte, unsigned 
         */
        startbit = 7;
        len = 8;
        expected_value_unsigned = 255;
        frame[0] = 0xFF;
        value_unsigned = extract(frame, startbit, len, UNSIGNED, MOTOROLA);
        printf("step 1.1\n");
        printf("expected_value_unsigned: %ld\n", expected_value_unsigned);
        printf("current_value:           %ld\n", value_unsigned);
        assert(expected_value_unsigned == value_unsigned);

        /* step 1.2 all signal on 1 byte, signed */
        startbit = 15;
        len = 8;
        expected_value_signed = -3;
        frame[1] = 0xFD;
        value_signed = (int64_t)extract(frame, startbit, len, SIGNED, MOTOROLA);
        printf("\nstep 1.2\n");
        printf("expected_value_signed:   %ld\n", expected_value_signed);
        printf("current_value:           %ld\n", value_signed);
        assert(expected_value_signed == value_signed);

        /* step 1.3 (lsb middle of byte), unsigned */
        startbit = 27;
        len = 3;
        expected_value_unsigned = 7;
        frame[3] = 0x0E;
        value_unsigned = extract(frame, startbit, len, UNSIGNED, MOTOROLA);
        printf("\nstep 1.3\n");
        printf("expected_value_unsigned:   %ld\n", expected_value_unsigned);
        printf("current_value:             %ld\n", value_unsigned);
        assert(expected_value_unsigned == value_unsigned);

        /*
         * step x.x (lsb middle of byte), signed , signed signals always encoded on
         * start of byte 
         */

        /* step 1.4 (lsb start of byte), unsigned */
        startbit = 21;
        len = 6;
        expected_value_unsigned = 63;
        frame[2] = 0x3F;
        value_unsigned = extract(frame, startbit, len, UNSIGNED, MOTOROLA);
        printf("\nstep 1.4\n");
        printf("expected_value_unsigned:   %ld\n", expected_value_unsigned);
        printf("current_value:             %ld\n", value_unsigned);
        assert(expected_value_unsigned == value_unsigned);

        /* step 1.5 (lsb start of byte), signed */
        startbit = 35;
        len = 4;
        expected_value_signed = -5;
        frame[4] = 0x0B;
        value_signed = (int64_t)extract(frame, startbit, len, SIGNED, MOTOROLA);
        printf("\nstep 1.5\n");
        printf("expected_value_signed:     %ld\n", expected_value_signed);
        printf("current_value:             %ld\n", value_signed);
        assert(expected_value_signed == value_signed);

        /* step 2 (2 bytes) */
        /* step 2.1 (all signal on 2 bytes), unsigned */
        startbit = 55;
        len = 16;
        expected_value_unsigned = 52651;
        frame[6] = 0xCD;
        frame[7] = 0xAB;
        value_unsigned = extract(frame, startbit, len, UNSIGNED, MOTOROLA);
        printf("\nstep 2.1\n");
        printf("expected_value_unsigned:     %ld\n", expected_value_unsigned);
        printf("current_value:               %ld\n", value_unsigned);
        assert(expected_value_unsigned == value_unsigned);

        /* step 2.2 (all signal on 2 bytes), signed */
        startbit = 39;
        len = 16;
        expected_value_signed = -9;
        frame[4] = 0xFF;
        frame[5] = 0xF7;
        value_signed = (int64_t)extract(frame, startbit, len, SIGNED, MOTOROLA);
        printf("\nstep 2.2\n");
        printf("expected_value_signed:       %ld\n", expected_value_signed);
        printf("current_value:               %ld\n", value_signed);
        assert(expected_value_signed == value_signed);

        /* step 2.3 (lsb middle of byte), unsigned */
        startbit = 26;
        len = 9;
        expected_value_unsigned = 511;
        frame[3] = 0x07;
        frame[4] = 0xFC;
        value_unsigned = extract(frame, startbit, len, UNSIGNED, MOTOROLA);
        printf("\nstep 2.3\n");
        printf("expected_value_unsigned:       %ld\n", expected_value_unsigned);
        printf("current_value:                 %ld\n", value_unsigned);
        assert(expected_value_unsigned == value_unsigned);

        /*
         * step x.x (lsb middle of byte), signed , signed signals always encoded 
         * on start of byte 
         */

        /* step 2.4 (lsb start of byte), unsigned */
        startbit = 29;
        len = 14;
        expected_value_unsigned = 16383;
        frame[3] = 0x3F;
        frame[4] = 0xFF;
        value_unsigned = extract(frame, startbit, len, UNSIGNED, MOTOROLA);
        printf("\nstep 2.4\n");
        printf("expected_value_unsigned:       %ld\n", expected_value_unsigned);
        printf("current_value:                 %ld\n", value_unsigned);
        assert(expected_value_unsigned == value_unsigned);

        /* step 2.5 (lsb start of byte), signed */
        startbit = 18;
        len = 11;
        expected_value_signed = -789;
        frame[2] = 0x04;
        frame[3] = 0xEB;
        value_signed = (int64_t)extract(frame, startbit, len, SIGNED, MOTOROLA);
        printf("\nstep 2.5\n");
        printf("expected_value_signed:         %ld\n", expected_value_signed);
        printf("current_value:                 %ld\n", value_signed);
        assert(expected_value_signed == value_signed);

        /* step 2.6 (signal on 7 bytes, lsb start of byte), unsigned */
        startbit = 7;
        len = 56;
        expected_value_unsigned = 72057594037927935;
        frame[0] = 0xFF;
        frame[1] = 0xFF;
        frame[2] = 0xFF;
        frame[3] = 0xFF;
        frame[4] = 0xFF;
        frame[5] = 0xFF;
        frame[6] = 0xFF;
        value_unsigned = extract(frame, startbit, len, UNSIGNED, MOTOROLA);
        printf("\nstep 2.6\n");
        printf("expected_value_unsigned:         %ld\n", 
                        expected_value_unsigned);
        printf("current_value:                   %ld\n", value_unsigned);
        assert(expected_value_unsigned == value_unsigned);

        /* step 2.7 (signal on 4 bytes, lsb start of byte), signed */
        startbit = 39;
        len = 32;
        expected_value_signed = -2345634;
        frame[4] = 0xFF;
        frame[5] = 0xDC;
        frame[6] = 0x35;
        frame[7] = 0x5E;
        value_signed = (int64_t)extract(frame, startbit, len, SIGNED, MOTOROLA);
        printf("\nstep 2.7\n");
        printf("expected_value_signed:         %ld\n", expected_value_signed);
        printf("current_value:                 %ld\n", value_signed);
        assert(expected_value_signed == value_signed);

        /*
         ***********************************************************************
         * EXTRACT INTEL
         ***********************************************************************
         */
        printf("\n\n\n");
        printf("===========================================================\n");
        printf("EXTRACT INTEL\n");
        printf("===========================================================\n");

        /* step 3 (1 byte) 
         * step 3.1 all signal on 1 byte, unsigned */
        startbit = 0;
        len = 8;
        expected_value_unsigned = 255;
        frame[0] = 0xFF;
        value_unsigned = extract(frame, startbit, len, UNSIGNED, INTEL);
        printf("\nstep 3.1\n");
        printf("expected_value_unsigned:         %ld\n", 
                        expected_value_unsigned);
        printf("current_value:                   %ld\n", value_unsigned);
        assert(expected_value_unsigned == value_unsigned);

        /* step 3.2 all signal on 1 byte, signed */
        startbit = 40;
        len = 8;
        expected_value_signed = -33;
        frame[5] = 0xDF;
        value_signed = (int64_t)extract(frame, startbit, len, SIGNED, INTEL);
        printf("\nstep 3.2\n");
        printf("expected_value_signed:         %ld\n", expected_value_signed);
        printf("current_value:                 %ld\n", value_signed);
        assert(expected_value_signed == value_signed);

        /* step 3.3 (lsb middle of byte), unsigned */
        startbit = 17;
        len = 7;
        expected_value_unsigned = 47;
        frame[2] = 0x5E;
        value_unsigned = extract(frame, startbit, len, UNSIGNED, INTEL);
        printf("\nstep 3.3\n");
        printf("expected_value_unsigned:         %ld\n", 
                        expected_value_unsigned);
        printf("current_value:                   %ld\n", value_unsigned);
        assert(expected_value_unsigned == value_unsigned);

        /*
         * step x.x (lsb middle of byte), signed , signed signals always encoded 
         * on start of byte 
         */

        /* step 3.4 (lsb start of byte), unsigned */
        startbit = 48;
        len = 7;
        expected_value_unsigned = 118;
        frame[6] = 0x76;
        value_unsigned = extract(frame, startbit, len, UNSIGNED, INTEL);
        printf("\nstep 3.4\n");
        printf("expected_value_unsigned:         %ld\n", 
                        expected_value_unsigned);
        printf("current_value:                   %ld\n", value_unsigned);
        assert(expected_value_unsigned == value_unsigned);

        /* step 3.5 (lsb start of byte), signed */
        startbit = 32;
        len = 8;
        expected_value_signed = -45;
        frame[4] = 0xD3;
        value_signed = (int64_t)extract(frame, startbit, len, SIGNED, INTEL);
        printf("\nstep 3.5\n");
        printf("expected_value_signed:         %ld\n", expected_value_signed);
        printf("current_value:                 %ld\n", value_signed);
        assert(expected_value_signed == value_signed);

        /* step 4 (2 bytes) */
        /* step 4.1 (all signal on 2 bytes), unsigned */
        startbit = 24;
        len = 16;
        expected_value_unsigned = 53754;
        frame[3] = 0xFA;
        frame[4] = 0xD1;
        value_unsigned = extract(frame, startbit, len, UNSIGNED, INTEL);
        printf("\nstep 4.1\n");
        printf("expected_value_unsigned:         %ld\n", 
                        expected_value_unsigned);
        printf("current_value:                   %ld\n", value_unsigned);
        assert(expected_value_unsigned == value_unsigned);

        /* step 4.2 (all signal on 2 bytes), signed */
        startbit = 48;
        len = 16;
        expected_value_signed = -999;
        frame[6] = 0x19;
        frame[7] = 0xFC;
        value_signed = (int64_t)extract(frame, startbit, len, SIGNED, INTEL);
        printf("\nstep 4.2\n");
        printf("expected_value_signed:         %ld\n", expected_value_signed);
        printf("current_value:                 %ld\n", value_signed);
        assert(expected_value_signed == value_signed);

        /* step 4.3 (lsb middle of byte), unsigned */
        startbit = 2;
        len = 12;
        expected_value_unsigned = 3387;
        frame[0] = 0xEC;
        frame[1] = 0x34;
        value_unsigned = extract(frame, startbit, len, UNSIGNED, INTEL);
        printf("\nstep 4.3\n");
        printf("expected_value_unsigned:         %ld\n", 
                        expected_value_unsigned);
        printf("current_value:                   %ld\n", value_unsigned);
        assert(expected_value_unsigned == value_unsigned);

        /* 
         * step x.x (lsb middle of byte), signed , signed signals always encoded 
         * on start of byte 
         */

        /* step 4.4 (lsb start of byte), unsigned */
        startbit = 16;
        len = 11;
        expected_value_unsigned = 885;
        frame[2] = 0x75;
        frame[3] = 0x03;
        value_unsigned = extract(frame, startbit, len, UNSIGNED, INTEL);
        printf("\nstep 4.4\n");
        printf("expected_value_unsigned:         %ld\n", 
                        expected_value_unsigned);
        printf("current_value:                   %ld\n", value_unsigned);
        assert(expected_value_unsigned == value_unsigned);

        /* step 4.5 (lsb start of byte), signed */
        startbit = 40;
        len = 16;
        expected_value_signed = -6666;
        frame[5] = 0xF6;
        frame[6] = 0xE5;
        value_signed = (int64_t)extract(frame, startbit, len, SIGNED, INTEL);
        printf("\nstep 4.5\n");
        printf("expected_value_signed:         %ld\n", expected_value_signed);
        printf("current_value:                 %ld\n", value_signed);
        assert(expected_value_signed == value_signed);

        /* step 4.6 (signal on 7 bytes, lsb start of byte), unsigned */
        startbit = 0;
        len = 56;
        expected_value_unsigned = 48413335211474859;
        frame[0] = 0xAB;
        frame[1] = 0xFF;
        frame[2] = 0xAB;
        frame[3] = 0xFF;
        frame[4] = 0xAB;
        frame[5] = 0xFF;
        frame[6] = 0xAB;
        value_unsigned = extract(frame, startbit, len, UNSIGNED, INTEL);
        printf("\nstep 4.6\n");
        printf("expected_value_unsigned:         %ld\n", 
                        expected_value_unsigned);
        printf("current_value:                   %ld\n", value_unsigned);
        assert(expected_value_unsigned == value_unsigned);

        /* step 4.7 (signal on 4 bytes, lsb start of byte), signed */
        startbit = 0;
        len = 32;
        expected_value_signed = -1666666;
        frame[0] = 0x96;
        frame[1] = 0x91;
        frame[2] = 0xE6;
        frame[3] = 0xFF;
        value_signed = (int64_t)extract(frame, startbit, len, SIGNED, INTEL);
        printf("\nstep 4.7\n");
        printf("expected_value_signed:         %ld\n", expected_value_signed);
        printf("current_value:                 %ld\n", value_signed);
        assert(expected_value_signed == value_signed);

        /*
         ***********************************************************************
         * INSERT MOTOROLA
         ***********************************************************************
         */
        printf("\n\n\n");
        printf("===========================================================\n");
        printf("INSERT MOTOROLA\n");
        printf("===========================================================\n");

        /* step 5 (1 byte) 
         * step 5.1 all signal on 1 byte, unsigned */
        memset(frame, 0, 8);
        memset(expected_frame, 0, 8);
        value_one_byte_unsigned = 6;
        startbit = 31;
        len = 8;
        expected_frame[3] = 0x06;
        insert(frame, startbit, len, value_one_byte_unsigned, MOTOROLA);
        printf("\nstep 5.1");
        display_two(expected_frame, frame, 8);
        assert(asserteq(expected_frame, frame, 8));

        /* step 5.2 all signal on 1 byte, signed */
        memset(frame, 0, 8);
        memset(expected_frame, 0, 8);
        value_one_byte_signed = -15;
        startbit = 31;
        len = 8;
        expected_frame[3] = 0xF1;
        insert(frame, startbit, len, value_one_byte_signed, MOTOROLA);
        printf("\nstep 5.2");
        display_two(expected_frame, frame, 8);
        assert(asserteq(expected_frame, frame, 8));

        /* step 5.3 (lsb middle of byte), unsigned */
        memset(frame, 0, 8);
        memset(expected_frame, 0, 8);
        value_one_byte_unsigned = 63;
        startbit = 7;
        len = 6;
        expected_frame[0] = 0xFC;
        insert(frame, startbit, len, value_one_byte_unsigned, MOTOROLA);
        printf("\nstep 5.3");
        display_two(expected_frame, frame, 8);
        assert(asserteq(expected_frame, frame, 8));

        /*
         * step x.x (lsb middle of byte), signed , signed signals always encoded 
         * on start of byte 
         */

        /* step 5.4 (lsb start of byte), unsigned */
        memset(frame, 0, 8);
        memset(expected_frame, 0, 8);
        value_one_byte_unsigned = 113;
        startbit = 47;
        len = 8;
        expected_frame[5] = 0x71;
        insert(frame, startbit, len, value_one_byte_unsigned, MOTOROLA);
        printf("\nstep 5.4");
        display_two(expected_frame, frame, 8);
        assert(asserteq(expected_frame, frame, 8));

        /* step 5.5 (lsb start of byte), signed */
        memset(frame, 0, 8);
        memset(expected_frame, 0, 8);
        value_one_byte_signed = -113;
        startbit = 23;
        len = 8;
        expected_frame[2] = 0x8F;
        insert(frame, startbit, len, value_one_byte_signed, MOTOROLA);
        printf("\nstep 5.5");
        display_two(expected_frame, frame, 8);
        assert(asserteq(expected_frame, frame, 8));

        /* step 6 (2 bytes) */
        /* step 6.1 (all signal on 2 bytes), unsigned */
        memset(frame, 0, 8);
        memset(expected_frame, 0, 8);
        value_two_bytes_unsigned = 30126;
        startbit = 55;
        len = 16;
        expected_frame[6] = 0x75;
        expected_frame[7] = 0xAE;
        insert(frame, startbit, len, value_two_bytes_unsigned, MOTOROLA);
        printf("\nstep 6.1");
        display_two(expected_frame, frame, 8);
        assert(asserteq(expected_frame, frame, 8));

        /* step 6.2 (all signal on 2 bytes), signed */
        memset(frame, 0, 8);
        memset(expected_frame, 0, 8);
        value_two_bytes_signed = -59595;
        startbit = 39;
        len = 16;
        expected_frame[4] = 0x17;
        expected_frame[5] = 0x35;
        insert(frame, startbit, len, value_two_bytes_signed, MOTOROLA);
        printf("\nstep 6.2");
        display_two(expected_frame, frame, 8);
        assert(asserteq(expected_frame, frame, 8));

        /* step 6.3 (lsb middle of byte), unsigned */
        memset(frame, 0, 8);
        memset(expected_frame, 0, 8);
        value_two_bytes_unsigned = 189;
        startbit = 21;
        len = 9;
        expected_frame[2] = 0x17;
        expected_frame[3] = 0xA0;
        insert(frame, startbit, len, value_two_bytes_unsigned, MOTOROLA);
        printf("\nstep 6.3");
        display_two(expected_frame, frame, 8);
        assert(asserteq(expected_frame, frame, 8));

        /*
         * step x.x (lsb middle of byte), signed , signed signals always encoded 
         * on start of byte 
         */

        /* step 6.4 (lsb start of byte), unsigned */
        memset(frame, 0, 8);
        memset(expected_frame, 0, 8);
        value_two_bytes_unsigned = 1390;
        startbit = 34;
        len = 11;
        expected_frame[4] = 0x05;
        expected_frame[5] = 0x6E;
        insert(frame, startbit, len, value_two_bytes_unsigned, MOTOROLA);
        printf("\nstep 6.4");
        display_two(expected_frame, frame, 8);
        assert(asserteq(expected_frame, frame, 8));

        /* step 6.5 (lsb start of byte), signed */
        memset(frame, 0, 8);
        memset(expected_frame, 0, 8);
        value_two_bytes_signed = -24244;
        startbit = 7;
        len = 16;
        expected_frame[0] = 0xA1;
        expected_frame[1] = 0x4C;
        insert(frame, startbit, len, value_two_bytes_signed, MOTOROLA);
        printf("\nstep 6.5");
        display_two(expected_frame, frame, 8);
        assert(asserteq(expected_frame, frame, 8));

        /* step 6.6 (signal on 7 bytes, lsb start of byte), unsigned */
        memset(frame, 0, 8);
        memset(expected_frame, 0, 8);
        value_seven_bytes_unsigned = 48413335211474859;
        startbit = 7;
        len = 56;
        expected_frame[0] = 0xAB;
        expected_frame[1] = 0xFF;
        expected_frame[2] = 0xAB;
        expected_frame[3] = 0xFF;
        expected_frame[4] = 0xAB;
        expected_frame[5] = 0xFF;
        expected_frame[6] = 0xAB;
        insert(frame, startbit, len, value_seven_bytes_unsigned, MOTOROLA);
        printf("\nstep 6.6");
        display_two(expected_frame, frame, 8);
        assert(asserteq(expected_frame, frame, 8));

        /* step 6.7 (signal on 4 bytes, lsb start of byte), signed */
        memset(frame, 0, 8);
        memset(expected_frame, 0, 8);
        value_four_bytes_signed = -489;
        startbit = 39;
        len = 32;
        expected_frame[4] = 0xFF;
        expected_frame[5] = 0xFF;
        expected_frame[6] = 0xFE;
        expected_frame[7] = 0x17;
        insert(frame, startbit, len, value_four_bytes_signed, MOTOROLA);
        printf("\nstep 6.7");
        display_two(expected_frame, frame, 8);
        assert(asserteq(expected_frame, frame, 8));

        /*
         ***********************************************************************
         * INSERT INTEL
         ***********************************************************************
         */
        printf("\n\n\n");
        printf("===========================================================\n");
        printf("INSERT INTEL\n");
        printf("===========================================================\n");

        /* step 7 (1 byte) 
         * step 7.1 all signal on 1 byte, unsigned */
        memset(frame, 0, 8);
        memset(expected_frame, 0, 8);
        value_one_byte_unsigned = 240;
        startbit = 16;
        len = 8;
        expected_frame[2] = 0xF0;
        insert(frame, startbit, len, value_one_byte_unsigned, INTEL);
        printf("\nstep 7.1");
        display_two(expected_frame, frame, 8);
        assert(asserteq(expected_frame, frame, 8));

        /* step 7.2 all signal on 1 byte, signed */
        memset(frame, 0, 8);
        memset(expected_frame, 0, 8);
        value_one_byte_signed = -202;
        startbit = 32;
        len = 8;
        expected_frame[4] = 0x36;
        insert(frame, startbit, len, value_one_byte_signed, INTEL);
        printf("\nstep 7.2");
        display_two(expected_frame, frame, 8);
        assert(asserteq(expected_frame, frame, 8));

        /* step 7.3 (lsb middle of byte), unsigned */
        memset(frame, 0, 8);
        memset(expected_frame, 0, 8);
        value_one_byte_unsigned = 7;
        startbit  = 29;
        len = 3;
        expected_frame[3] = 0xE0;
        insert(frame, startbit, len, value_one_byte_unsigned, INTEL);
        printf("\nstep 7.3");
        display_two(expected_frame, frame, 8);
        assert(asserteq(expected_frame, frame, 8));

        /*
         * step x.x (lsb middle of byte), signed , signed signals always encoded 
         * on start of byte 
         */

        /* step 7.4 (lsb start of byte), unsigned */
        memset(frame, 0, 8);
        memset(expected_frame, 0, 8);
        value_one_byte_unsigned = 23;
        startbit  = 56;
        len = 5;
        expected_frame[7] = 0x17;
        insert(frame, startbit, len, value_one_byte_unsigned, INTEL);
        printf("\nstep 7.4");
        display_two(expected_frame, frame, 8);
        assert(asserteq(expected_frame, frame, 8));

        /* step 7.5 (lsb start of byte), signed */
        memset(frame, 0, 8);
        memset(expected_frame, 0, 8);
        value_one_byte_signed = -199;
        startbit  = 40;
        len = 8;
        expected_frame[5] = 0x39;
        insert(frame, startbit, len, value_one_byte_signed, INTEL);
        printf("\nstep 7.5");
        display_two(expected_frame, frame, 8);
        assert(asserteq(expected_frame, frame, 8));

        /* step 8 (2 bytes) */
        /* step 8.1 (all signal on 2 bytes), unsigned */
        memset(frame, 0, 8);
        memset(expected_frame, 0, 8);
        value_two_bytes_unsigned = 52077;
        startbit = 16;
        len = 16;
        expected_frame[2] = 0x6D;
        expected_frame[3] = 0xCB;
        insert(frame, startbit, len, value_two_bytes_unsigned, INTEL);
        printf("\nstep 8.1");
        display_two(expected_frame, frame, 8);
        assert(asserteq(expected_frame, frame, 8));

        /* step 8.2 (all signal on 2 bytes), signed */
        memset(frame, 0, 8);
        memset(expected_frame, 0, 8);
        value_two_bytes_signed = -48666;
        startbit = 32;
        len = 16;
        expected_frame[4] = 0xE6;
        expected_frame[5] = 0x41;
        insert(frame, startbit, len, value_two_bytes_signed, INTEL);
        printf("\nstep 8.2");
        display_two(expected_frame, frame, 8);
        assert(asserteq(expected_frame, frame, 8));

        /* step 8.3 (lsb middle of byte), unsigned */
        memset(frame, 0, 8);
        memset(expected_frame, 0, 8);
        value_two_bytes_unsigned = 1707;
        startbit = 44;
        len = 11;
        expected_frame[5] = 0xB0;
        expected_frame[6] = 0x6A;
        insert(frame, startbit, len, value_two_bytes_unsigned, INTEL);
        printf("\nstep 8.3");
        display_two(expected_frame, frame, 8);
        assert(asserteq(expected_frame, frame, 8));

        /*
         * step x.x (lsb middle of byte), signed , signed signals always encoded 
         * on start of byte 
         */

        /* step 8.4 (lsb start of byte), unsigned */
        memset(frame, 0, 8);
        memset(expected_frame, 0, 8);
        value_two_bytes_unsigned = 1023;
        startbit = 8;
        len = 10;
        expected_frame[1] = 0xFF;
        expected_frame[2] = 0x03;
        insert(frame, startbit, len, value_two_bytes_unsigned, INTEL);
        printf("\nstep 8.4");
        display_two(expected_frame, frame, 8);
        assert(asserteq(expected_frame, frame, 8));

        /* step 8.5 (lsb start of byte), signed */
        memset(frame, 0, 8);
        memset(expected_frame, 0, 8);
        value_two_bytes_signed = -59821;
        startbit = 48;
        len = 16;
        expected_frame[6] = 0x53;
        expected_frame[7] = 0x16;
        insert(frame, startbit, len, value_two_bytes_signed, INTEL);
        printf("\nstep 8.5");
        display_two(expected_frame, frame, 8);
        assert(asserteq(expected_frame, frame, 8));

        /* step 8.6 (signal on 7 bytes, lsb start of byte), unsigned */
        memset(frame, 0, 8);
        memset(expected_frame, 0, 8);
        value_seven_bytes_unsigned = 48413335211474859;
        startbit = 0;
        len = 56;
        expected_frame[0] = 0xAB;
        expected_frame[1] = 0xFF;
        expected_frame[2] = 0xAB;
        expected_frame[3] = 0xFF;
        expected_frame[4] = 0xAB;
        expected_frame[5] = 0xFF;
        expected_frame[6] = 0xAB;
        insert(frame, startbit, len, value_seven_bytes_unsigned, INTEL);
        printf("\nstep 8.6");
        display_two(expected_frame, frame, 8);
        assert(asserteq(expected_frame, frame, 8));

        /* step 8.7 (signal on 4 bytes, lsb start of byte), signed */
        memset(frame, 0, 8);
        memset(expected_frame, 0, 8);
        value_four_bytes_signed = -1339;
        startbit = 7;
        len = 32;
        expected_frame[0] = 0xFF;
        expected_frame[1] = 0xFF;
        expected_frame[2] = 0xFA;
        expected_frame[3] = 0xC5;
        insert(frame, startbit, len, value_four_bytes_signed, MOTOROLA);
        printf("\nstep 8.7");
        display_two(expected_frame, frame, 8);
        assert(asserteq(expected_frame, frame, 8));

        /*
         ***********************************************************************
         * ENCODE DECODE MOTOROLA double value
         ***********************************************************************
         */
        printf("\n\n\n");
        printf("===========================================================\n");
        printf("ENCODE DECODE MOTOROLA double value\n");
        printf("===========================================================\n");

        memset(frame, 0, 8);
        dphy_value = 66.66666;
        startbit = 7;
        len = 32;
        factor = 0.0000001;
        offset = 0;
        printf("\nstep 9.1\n\n");
        printf("value to be encoded: %.5lf\n", dphy_value);
        encode_double(frame, dphy_value, startbit, len, MOTOROLA, 
                        factor, offset);
        display_one(frame, 8);
        printf("\ndecoded value:       %.5lf\n", 
                        decode_double(frame, startbit, len, MOTOROLA, 
                                factor, offset));
        assert(cmp_double(decode_double(frame, startbit, len, MOTOROLA, 
                                        factor, offset), dphy_value));

        /*
         ***********************************************************************
         * ENCODE DECODE MOTOROLA double float negative value
         ***********************************************************************
         */
        printf("\n\n\n");
        printf("===========================================================\n");
        printf("ENCODE DECODE MOTOROLA double negative value\n");
        printf("===========================================================\n");

        memset(frame, 0, 8);
        dphy_value = -50.6164129;
        startbit = 7;
        len = 32;
        factor = 0.0000001;
        offset = 0;
        printf("\nstep 9.2\n\n");
        printf("value to be encoded: %.7lf\n", dphy_value);
        encode_double(frame, dphy_value, startbit, len, MOTOROLA, 
                        factor, offset);
        display_one(frame, 8);
        printf("\ndecoded value:       %.7lf\n", 
                        decode_double(frame, startbit, len, MOTOROLA, 
                                factor, offset));
        assert(cmp_double(decode_double(frame, startbit, len, MOTOROLA, 
                                        factor, offset), dphy_value));

        /*
         ***********************************************************************
         * ENCODE DECODE MOTOROLA unsigned int value
         ***********************************************************************
         */
        printf("\n\n\n");
        printf("===========================================================\n");
        printf("ENCODE DECODE MOTOROLA unsigned int value\n");
        printf("===========================================================\n");

        memset(frame, 0, 8);
        uphy_value = 666666666;
        startbit = 7;
        len = 32;
        factor = 1;
        offset = 0;
        printf("\nstep 9.3\n\n");
        printf("value to be encoded: %ld\n", uphy_value);
        encode_uint64_t(frame, uphy_value, startbit, len, MOTOROLA, 
                        factor, offset);
        display_one(frame, 8);
        printf("\ndecoded value:       %ld\n", 
                        decode_uint64_t(frame, startbit, len, MOTOROLA, 
                                factor, offset));
        assert(uphy_value == decode_uint64_t(frame, startbit, len, MOTOROLA, 
                                factor, offset));

        /*
         ***********************************************************************
         * ENCODE DECODE INTEL double value
         ***********************************************************************
         */
        printf("\n\n\n");
        printf("===========================================================\n");
        printf("ENCODE DECODE INTEL double value\n");
        printf("===========================================================\n");

        memset(frame, 0, 8);
        dphy_value = 8.4939123;
        startbit = 0;
        len = 32;
        factor = 0.0000001;
        offset = 0;
        printf("\nstep 9.4\n\n");
        printf("value to be encoded: %.7lf\n", dphy_value);
        encode_double(frame, dphy_value, startbit, len, INTEL, factor, offset);
        display_one(frame, 8);
        printf("\ndecoded value:       %.7lf\n",
                        decode_double(frame, startbit, len, INTEL, 
                                factor, offset));
        assert(cmp_double(decode_double(frame, startbit, len, INTEL, 
                                        factor, offset), dphy_value));

        /*
         ***********************************************************************
         * ENCODE DECODE INTEL double negative value
         ***********************************************************************
         */
        printf("\n\n\n");
        printf("===========================================================\n");
        printf("ENCODE DECODE INTEL double negative value\n");
        printf("===========================================================\n");

        memset(frame, 0, 8);
        dphy_value = -7.7979897;
        startbit = 0;
        len = 32;
        factor = 0.0000001;
        offset = 0;
        printf("\nstep 9.5\n\n");
        printf("value to be encoded: %.7lf\n", dphy_value);
        encode_double(frame, dphy_value, startbit, len, INTEL, factor, offset);
        display_one(frame, 8);
        printf("\ndecoded value:       %.7lf\n", 
                        decode_double(frame, startbit, len, INTEL, 
                                factor, offset));
        assert(cmp_double(decode_double(frame, startbit, len, INTEL, 
                                        factor, offset), dphy_value));

        /*
         ***********************************************************************
         * ENCODE DECODE INTEL unsigned int value
         ***********************************************************************
         */
        printf("\n\n\n");
        printf("===========================================================\n");
        printf("ENCODE DECODE INTEL unsigned int value\n");
        printf("===========================================================\n");

        memset(frame, 0, 8);
        uphy_value = 999999999;
        startbit = 0;
        len = 32;
        factor = 1;
        offset = 0;
        printf("\nstep 9.6\n\n");
        printf("value to be encoded: %ld\n", uphy_value);
        encode_uint64_t(frame, uphy_value, startbit, len, INTEL, 
                        factor, offset);
        display_one(frame, 8);
        printf("\ndecoded value:       %ld\n", 
                        decode_uint64_t(frame, startbit, len, INTEL, 
                                factor, offset));
        assert(uphy_value == decode_uint64_t(frame, startbit, len, INTEL, 
                                factor, offset));

        /*
         ***********************************************************************
         * ENCODE DECODE INTEL signed int negative value
         ***********************************************************************
         */
        printf("\n\n\n");
        printf("===========================================================\n");
        printf("ENCODE DECODE INTEL signed int negative value\n");
        printf("===========================================================\n");

        memset(frame, 0, 8);
        sphy_value = -1029384756;
        startbit = 0;
        len = 32;
        factor = 1;
        offset = 0;
        printf("\nstep 9.7\n\n");
        printf("value to be encoded: %ld\n", sphy_value);
        encode_int64_t(frame, sphy_value, startbit, len, INTEL, factor, offset);
        display_one(frame, 8);
        printf("\ndecoded value:       %ld\n", 
                        decode_int64_t(frame, startbit, len, INTEL, 
                                factor, offset));
        assert(sphy_value == decode_int64_t(frame, startbit, len, INTEL, 
                                factor, offset));

        /*
         ***********************************************************************
         * ENCODE DECODE MOTOROLA float negative value
         ***********************************************************************
         */
        printf("\n\n\n");
        printf("===========================================================\n");
        printf("ENCODE DECODE MOTOROLA float negative value\n");
        printf("===========================================================\n");

        memset(frame, 0, 8);
        fphy_value = -2938.345666;
        startbit = 7;
        len = 40;
        factor = 0.0000001;
        offset = 0;
        printf("\nstep 9.8\n\n");
        printf("value to be encoded: %.6f\n", fphy_value);
        encode_float(frame, fphy_value, startbit, len, MOTOROLA, 
                        factor, offset);
        display_one(frame, 8);
        printf("\ndecoded value:       %.6f\n", 
                        decode_float(frame, startbit, len, MOTOROLA, 
                                factor, offset));
        assert(cmp_float(decode_float(frame, startbit, len, MOTOROLA, 
                                        factor, offset), fphy_value));

        /*
         ***********************************************************************
         * ENCODE DECODE INTEL unsigned int value FDFRAME
         ***********************************************************************
         */
        printf("\n\n\n");
        printf("===========================================================\n");
        printf("ENCODE DECODE INTEL unsigned int value FDFRAME\n");
        printf("===========================================================\n");

        memset(frame_fd0, 0, 40); /* 40 bytes fdframe */
        uphy_value = 999999999;
        startbit = 288; /* start of byte */
        len = 32;
        factor = 1;
        offset = 0;
        printf("\nstep 9.9\n\n");
        printf("value to be encoded: %ld\n", uphy_value);
        encode_uint64_t(frame_fd0, uphy_value, startbit, len, INTEL, 
                        factor, offset);
        display_one(frame_fd0, 40);
        printf("\ndecoded value:       %ld\n", 
                        decode_uint64_t(frame_fd0, startbit, len, INTEL, 
                                factor, offset));
        assert(uphy_value == decode_uint64_t(frame_fd0, startbit, len, INTEL, 
                                factor, offset));

        /*
         ***********************************************************************
         * ENCODE DECODE MOTOROLA signed int value FDFRAME
         ***********************************************************************
         */
        printf("\n\n\n");
        printf("===========================================================\n");
        printf("ENCODE DECODE MOTOROLA signed int value FDFRAME\n");
        printf("===========================================================\n");

        memset(frame_fd1, 0, 56); /* 56 bytes fdframe */
        sphy_value = -7777;
        startbit = 431; /* start of byte */
        len = 16;
        factor = 1;
        offset = 0;
        printf("\nstep 10.0\n\n");
        printf("value to be encoded: %ld\n", sphy_value);
        encode_int64_t(frame_fd1, sphy_value, startbit, len, MOTOROLA, 
                        factor, offset);
        display_one(frame_fd1, 56);
        printf("\ndecoded value:       %ld\n", 
                        decode_int64_t(frame_fd1, startbit, len, MOTOROLA, 
                                factor, offset));
        assert(sphy_value == decode_int64_t(frame_fd1, startbit, len, MOTOROLA, 
                                factor, offset));

        /*
         ***********************************************************************
         * ENCODE DECODE INTEL signed negative int value FDFRAME
         ***********************************************************************
         */
        printf("\n\n\n");
        printf("===========================================================\n");
        printf("ENCODE DECODE INTEL signed negative int value FDFRAME\n");
        printf("===========================================================\n");

        memset(frame_fd2, 0, 48); /* 48 bytes fdframe */
        sphy_value = -1029384756;
        startbit = 184; /* start of byte */
        len = 32;
        factor = 1;
        offset = 0;
        printf("\nstep 10.1\n\n");
        printf("value to be encoded: %ld\n", sphy_value);
        encode_int64_t(frame_fd2, sphy_value, startbit, len, INTEL, 
                        factor, offset);
        display_one(frame_fd2, 48);
        printf("\ndecoded value:       %ld\n", 
                        decode_int64_t(frame_fd2, startbit, len, INTEL, 
                                factor, offset));
        assert(sphy_value == decode_int64_t(frame_fd2, startbit, len, INTEL, 
                                factor, offset));

        /*
         ***********************************************************************
         * ENCODE DECODE MOTOROLA float value FDFRAME
         ************************************************************************
         */
        printf("\n\n\n");
        printf("===========================================================\n");
        printf("ENCODE DECODE MOTOROLA float value FDFRAME\n");
        printf("===========================================================\n");

        memset(frame_fd3, 0, 64); /* 64 bytes fdframe */
        fphy_value = 8.49391;
        startbit = 383; /* start of byte */
        len = 32;
        factor = 0.0000001;
        offset = 0;
        printf("\nstep 10.2\n\n");
        printf("value to be encoded: %f\n", fphy_value);
        encode_float(frame_fd3, fphy_value, startbit, len, MOTOROLA, 
                        factor, offset);
        display_one(frame_fd3, 64);
        printf("\ndecoded value:       %f\n", 
                        decode_float(frame_fd3, startbit, len, MOTOROLA, 
                                factor, offset));
        assert(cmp_float(decode_float(frame_fd3, startbit, len, MOTOROLA, 
                                        factor, offset), fphy_value));

        /*
         ***********************************************************************
         * ENCODE DECODE INTEL double negative value FDFRAME
         ***********************************************************************
         */
        printf("\n\n\n");
        printf("===========================================================\n");
        printf("ENCODE DECODE INTEL double negative value FDFRAME\n");
        printf("===========================================================\n");

        memset(frame_fd4, 0, 24); /* 24 bytes fdframe */
        dphy_value = -7.7979897;
        startbit = 32; /* start of byte */
        len = 32;
        factor = 0.0000001;
        offset = 0;
        printf("\nstep 10.3\n\n");
        printf("value to be encoded: %.7lf\n", dphy_value);
        encode_double(frame_fd4, dphy_value, startbit, len, INTEL, 
                        factor, offset);
        display_one(frame_fd4, 24);
        printf("\ndecoded value:       %.7lf\n", 
                        decode_double(frame_fd4, startbit, len, INTEL, 
                                factor, offset));
        assert(cmp_double(decode_double(frame_fd4, startbit, len, INTEL, 
                                        factor, offset), dphy_value));

        return 0;
}
