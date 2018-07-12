#include "../include/libwecan.h"
#include <assert.h>

void init_frames(uint8_t *expected_frame, uint8_t *frame, uint8_t dlc);

void init_single_frame(uint8_t *frame, uint8_t dlc);

int test_equality(uint8_t *expected_frame, uint8_t *frame, uint8_t dlc);

void display(uint8_t *expected_frame, uint8_t *frame, uint8_t dlc);

void display_single(uint8_t *frame, uint8_t dlc);

int main(void)
{
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
        uint8_t startbit = 7;
        uint8_t length   = 8;
        uint64_t expected_value_unsigned = 255;
        uint8_t frame[8] = {0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
        uint64_t value_unsigned = extract(frame, startbit, length, UNSIGNED, MOTOROLA);
        printf("step 1.1\n");
        assert(expected_value_unsigned == value_unsigned);
        printf("expected_value_unsigned: %ld\n", expected_value_unsigned);
        printf("current_value:           %ld\n", value_unsigned);

        /* step 1.2 all signal on 1 byte, signed */
        startbit  = 15;
        length    = 8;
        int64_t expected_value_signed = -3;
        frame[1] = 0xFD;
        int64_t value_signed = (int64_t)extract(frame, startbit, length, SIGNED, MOTOROLA);
        printf("\nstep 1.2\n");
        assert(expected_value_signed == value_signed);
        printf("expected_value_signed:   %ld\n", expected_value_signed);
        printf("current_value:           %ld\n", value_signed);

        /* step 1.3 (lsb middle of byte), unsigned */
        startbit  = 27;
        length    = 3;
        expected_value_unsigned = 7;
        frame[3] = 0x0E;
        value_unsigned = extract(frame, startbit, length, UNSIGNED, MOTOROLA);
        printf("\nstep 1.3\n");
        assert(expected_value_unsigned == value_unsigned);
        printf("expected_value_unsigned:   %ld\n", expected_value_unsigned);
        printf("current_value:             %ld\n", value_unsigned);

        /*
         * step x.x (lsb middle of byte), signed , signed signals always encoded on
         * start of byte 
         */

        /* step 1.4 (lsb start of byte), unsigned */
        startbit  = 21;
        length    = 6;
        expected_value_unsigned = 63;
        frame[2] = 0x3F;
        value_unsigned = extract(frame, startbit, length, UNSIGNED, MOTOROLA);
        printf("\nstep 1.4\n");
        assert(expected_value_unsigned == value_unsigned);
        printf("expected_value_unsigned:   %ld\n", expected_value_unsigned);
        printf("current_value:             %ld\n", value_unsigned);

        /* step 1.5 (lsb start of byte), signed */
        startbit  = 35;
        length    = 4;
        expected_value_signed = -5;
        frame[4] = 0x0B;
        value_signed = (int64_t)extract(frame, startbit, length, SIGNED, MOTOROLA);
        printf("\nstep 1.5\n");
        assert(expected_value_signed == value_signed);
        printf("expected_value_signed:     %ld\n", expected_value_signed);
        printf("current_value:             %ld\n", value_signed);

        /* step 2 (2 bytes) */
        /* step 2.1 (all signal on 2 bytes), unsigned */
        startbit  = 55;
        length    = 16;
        expected_value_unsigned = 52651;
        frame[6] = 0xCD;
        frame[7] = 0xAB;
        value_unsigned = extract(frame, startbit, length, UNSIGNED, MOTOROLA);
        printf("\nstep 2.1\n");
        assert(expected_value_unsigned == value_unsigned);
        printf("expected_value_unsigned:     %ld\n", expected_value_unsigned);
        printf("current_value:               %ld\n", value_unsigned);

        /* step 2.2 (all signal on 2 bytes), signed */
        startbit  = 39;
        length    = 16;
        expected_value_signed = -9;
        frame[4] = 0xFF;
        frame[5] = 0xF7;
        value_signed = (int64_t)extract(frame, startbit, length, SIGNED, MOTOROLA);
        printf("\nstep 2.2\n");
        assert(expected_value_signed == value_signed);
        printf("expected_value_signed:       %ld\n", expected_value_signed);
        printf("current_value:               %ld\n", value_signed);

        /* step 2.3 (lsb middle of byte), unsigned */
        startbit  = 26;
        length    = 9;
        expected_value_unsigned = 511;
        frame[3] = 0x07;
        frame[4] = 0xFC;
        value_unsigned = extract(frame, startbit, length, UNSIGNED, MOTOROLA);
        printf("\nstep 2.3\n");
        assert(expected_value_unsigned == value_unsigned);
        printf("expected_value_unsigned:       %ld\n", expected_value_unsigned);
        printf("current_value:                 %ld\n", value_unsigned);

        /*
         * step x.x (lsb middle of byte), signed , signed signals always encoded 
         * on start of byte 
         */

        /* step 2.4 (lsb start of byte), unsigned */
        startbit  = 29;
        length    = 14;
        expected_value_unsigned = 16383;
        frame[3] = 0x3F;
        frame[4] = 0xFF;
        value_unsigned = extract(frame, startbit, length, UNSIGNED, MOTOROLA);
        printf("\nstep 2.4\n");
        assert(expected_value_unsigned == value_unsigned);
        printf("expected_value_unsigned:       %ld\n", expected_value_unsigned);
        printf("current_value:                 %ld\n", value_unsigned);

        /* step 2.5 (lsb start of byte), signed */
        startbit  = 18;
        length    = 11;
        expected_value_signed = -789;
        frame[2] = 0x04;
        frame[3] = 0xEB;
        value_signed = (int64_t)extract(frame, startbit, length, SIGNED, MOTOROLA);
        printf("\nstep 2.5\n");
        assert(expected_value_signed == value_signed);
        printf("expected_value_signed:         %ld\n", expected_value_signed);
        printf("current_value:                 %ld\n", value_signed);

        /* step 2.6 (signal on 7 bytes, lsb start of byte), unsigned */
        startbit  = 7;
        length    = 56;
        expected_value_unsigned = 72057594037927935;
        frame[0] = 0xFF;
        frame[1] = 0xFF;
        frame[2] = 0xFF;
        frame[3] = 0xFF;
        frame[4] = 0xFF;
        frame[5] = 0xFF;
        frame[6] = 0xFF;
        value_unsigned = extract(frame, startbit, length, UNSIGNED, MOTOROLA);
        printf("\nstep 2.6\n");
        assert(expected_value_unsigned == value_unsigned);
        printf("expected_value_unsigned:         %ld\n", expected_value_unsigned);
        printf("current_value:                   %ld\n", value_unsigned);

        /* step 2.7 (signal on 4 bytes, lsb start of byte), signed */
        startbit  = 39;
        length    = 32;
        expected_value_signed = -2345634;
        frame[4] = 0xFF;
        frame[5] = 0xDC;
        frame[6] = 0x35;
        frame[7] = 0x5E;
        value_signed = (int64_t)extract(frame, startbit, length, SIGNED, MOTOROLA);
        printf("\nstep 2.7\n");
        assert(expected_value_signed == value_signed);
        printf("expected_value_signed:         %ld\n", expected_value_signed);
        printf("current_value:                 %ld\n", value_signed);

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
        length   = 8;
        expected_value_unsigned = 255;
        frame[0] = 0xFF;
        value_unsigned = extract(frame, startbit, length, UNSIGNED, INTEL);
        printf("\nstep 3.1\n");
        assert(expected_value_unsigned == value_unsigned);
        printf("expected_value_unsigned:         %ld\n", expected_value_unsigned);
        printf("current_value:                   %ld\n", value_unsigned);

        /* step 3.2 all signal on 1 byte, signed */
        startbit  = 40;
        length    = 8;
        expected_value_signed = -33;
        frame[5] = 0xDF;
        value_signed = (int64_t)extract(frame, startbit, length, SIGNED, INTEL);
        printf("\nstep 3.2\n");
        assert(expected_value_signed == value_signed);
        printf("expected_value_signed:         %ld\n", expected_value_signed);
        printf("current_value:                 %ld\n", value_signed);

        /* step 3.3 (lsb middle of byte), unsigned */
        startbit  = 17;
        length    = 7;
        expected_value_unsigned = 47;
        frame[2] = 0x5E;
        value_unsigned = extract(frame, startbit, length, UNSIGNED, INTEL);
        printf("\nstep 3.3\n");
        assert(expected_value_unsigned == value_unsigned);
        printf("expected_value_unsigned:         %ld\n", expected_value_unsigned);
        printf("current_value:                   %ld\n", value_unsigned);

        /*
         * step x.x (lsb middle of byte), signed , signed signals always encoded 
         * on start of byte 
         */

        /* step 3.4 (lsb start of byte), unsigned */
        startbit  = 48;
        length    = 7;
        expected_value_unsigned = 118;
        frame[6] = 0x76;
        value_unsigned = extract(frame, startbit, length, UNSIGNED, INTEL);
        printf("\nstep 3.4\n");
        assert(expected_value_unsigned == value_unsigned);
        printf("expected_value_unsigned:         %ld\n", expected_value_unsigned);
        printf("current_value:                   %ld\n", value_unsigned);

        /* step 3.5 (lsb start of byte), signed */
        startbit  = 32;
        length    = 8;
        expected_value_signed = -45;
        frame[4] = 0xD3;
        value_signed = (int64_t)extract(frame, startbit, length, SIGNED, INTEL);
        printf("\nstep 3.5\n");
        assert(expected_value_signed == value_signed);
        printf("expected_value_signed:         %ld\n", expected_value_signed);
        printf("current_value:                 %ld\n", value_signed);

        /* step 4 (2 bytes) */
        /* step 4.1 (all signal on 2 bytes), unsigned */
        startbit  = 24;
        length    = 16;
        expected_value_unsigned = 53754;
        frame[3] = 0xFA;
        frame[4] = 0xD1;
        value_unsigned = extract(frame, startbit, length, UNSIGNED, INTEL);
        printf("\nstep 4.1\n");
        assert(expected_value_unsigned == value_unsigned);
        printf("expected_value_unsigned:         %ld\n", expected_value_unsigned);
        printf("current_value:                   %ld\n", value_unsigned);

        /* step 4.2 (all signal on 2 bytes), signed */
        startbit  = 48;
        length    = 16;
        expected_value_signed = -999;
        frame[6] = 0x19;
        frame[7] = 0xFC;
        value_signed = (int64_t)extract(frame, startbit, length, SIGNED, INTEL);
        printf("\nstep 4.2\n");
        assert(expected_value_signed == value_signed);
        printf("expected_value_signed:         %ld\n", expected_value_signed);
        printf("current_value:                 %ld\n", value_signed);

        /* step 4.3 (lsb middle of byte), unsigned */
        startbit  = 2;
        length    = 12;
        expected_value_unsigned = 3387;
        frame[0] = 0xEC;
        frame[1] = 0x34;
        value_unsigned = extract(frame, startbit, length, UNSIGNED, INTEL);
        printf("\nstep 4.3\n");
        assert(expected_value_unsigned == value_unsigned);
        printf("expected_value_unsigned:         %ld\n", expected_value_unsigned);
        printf("current_value:                   %ld\n", value_unsigned);

        /* 
         * step x.x (lsb middle of byte), signed , signed signals always encoded 
         * on start of byte 
         */

        /* step 4.4 (lsb start of byte), unsigned */
        startbit  = 16;
        length    = 11;
        expected_value_unsigned = 885;
        frame[2] = 0x75;
        frame[3] = 0x03;
        value_unsigned = extract(frame, startbit, length, UNSIGNED, INTEL);
        printf("\nstep 4.4\n");
        assert(expected_value_unsigned == value_unsigned);
        printf("expected_value_unsigned:         %ld\n", expected_value_unsigned);
        printf("current_value:                   %ld\n", value_unsigned);

        /* step 4.5 (lsb start of byte), signed */
        startbit  = 40;
        length    = 16;
        expected_value_signed = -6666;
        frame[5] = 0xF6;
        frame[6] = 0xE5;
        value_signed = (int64_t)extract(frame, startbit, length, SIGNED, INTEL);
        printf("\nstep 4.5\n");
        assert(expected_value_signed == value_signed);
        printf("expected_value_signed:         %ld\n", expected_value_signed);
        printf("current_value:                 %ld\n", value_signed);

        /* step 4.6 (signal on 7 bytes, lsb start of byte), unsigned */
        startbit  = 0;
        length    = 56;
        expected_value_unsigned = 48413335211474859;
        frame[0] = 0xAB;
        frame[1] = 0xFF;
        frame[2] = 0xAB;
        frame[3] = 0xFF;
        frame[4] = 0xAB;
        frame[5] = 0xFF;
        frame[6] = 0xAB;
        value_unsigned = extract(frame, startbit, length, UNSIGNED, INTEL);
        printf("\nstep 4.6\n");
        assert(expected_value_unsigned == value_unsigned);
        printf("expected_value_unsigned:         %ld\n", expected_value_unsigned);
        printf("current_value:                   %ld\n", value_unsigned);

        /* step 4.7 (signal on 4 bytes, lsb start of byte), signed */
        startbit  = 0;
        length    = 32;
        expected_value_signed = -1666666;
        frame[0] = 0x96;
        frame[1] = 0x91;
        frame[2] = 0xE6;
        frame[3] = 0xFF;
        value_signed = (int64_t)extract(frame, startbit, length, SIGNED, INTEL);

        printf("\nstep 4.7\n");
        assert(expected_value_signed == value_signed);
        printf("expected_value_signed:         %ld\n", expected_value_signed);
        printf("current_value:                 %ld\n", value_signed);

        /*
         ***********************************************************************
         * INSERT MOTOROLA
         ***********************************************************************
         */
        printf("\n\n\n");
        printf("===========================================================\n");
        printf("INSERT MOTOROLA\n");
        printf("===========================================================\n");

        uint8_t expected_frame[8];
        init_frames(expected_frame, frame, 8);

        /* step 5 (1 byte) 
         * step 5.1 all signal on 1 byte, unsigned */
        uint64_t value_one_byte_unsigned = 6;
        startbit = 31;
        length   = 8;
        expected_frame[3] = 0x06;
        insert(frame, startbit, length, value_one_byte_unsigned, MOTOROLA);
        printf("\nstep 5.1");
        assert(test_equality(expected_frame, frame, 8));
        display(expected_frame, frame, 8);

        /* step 5.2 all signal on 1 byte, signed */
        int64_t value_one_byte_signed = -15;
        startbit = 31;
        length   = 8;
        init_frames(expected_frame, frame, 8);
        expected_frame[3] = 0xF1;
        insert(frame, startbit, length, value_one_byte_signed, MOTOROLA);
        printf("\nstep 5.2");
        assert(test_equality(expected_frame, frame, 8));
        display(expected_frame, frame, 8);

        /* step 5.3 (lsb middle of byte), unsigned */
        value_one_byte_unsigned = 63;
        startbit  = 7;
        length    = 6;
        init_frames(expected_frame, frame, 8);
        expected_frame[0] = 0xFC;
        insert(frame, startbit, length, value_one_byte_unsigned, MOTOROLA);
        printf("\nstep 5.3");
        assert(test_equality(expected_frame, frame, 8));
        display(expected_frame, frame, 8);

        /*
         * step x.x (lsb middle of byte), signed , signed signals always encoded 
         * on start of byte 
         */

        /* step 5.4 (lsb start of byte), unsigned */
        value_one_byte_unsigned = 113;
        startbit  = 47;
        length    = 8;
        init_frames(expected_frame, frame, 8);
        expected_frame[5] = 0x71;
        insert(frame, startbit, length, value_one_byte_unsigned, MOTOROLA);
        printf("\nstep 5.4");
        assert(test_equality(expected_frame, frame, 8));
        display(expected_frame, frame, 8);

        /* step 5.5 (lsb start of byte), signed */
        value_one_byte_signed = -113;
        startbit  = 23;
        length    = 8;
        init_frames(expected_frame, frame, 8);
        expected_frame[2] = 0x8F;
        insert(frame, startbit, length, value_one_byte_signed, MOTOROLA);
        printf("\nstep 5.5");
        assert(test_equality(expected_frame, frame, 8));
        display(expected_frame, frame, 8);

        /* step 6 (2 bytes) */
        /* step 6.1 (all signal on 2 bytes), unsigned */
        uint64_t value_two_bytes_unsigned = 30126;
        startbit = 55;
        length   = 16;
        init_frames(expected_frame, frame, 8);
        expected_frame[6] = 0x75;
        expected_frame[7] = 0xAE;
        insert(frame, startbit, length, value_two_bytes_unsigned, MOTOROLA);
        printf("\nstep 6.1");
        assert(test_equality(expected_frame, frame, 8));
        display(expected_frame, frame, 8);

        /* step 6.2 (all signal on 2 bytes), signed */
        int64_t value_two_bytes_signed = -59595;
        startbit = 39;
        length   = 16;
        init_frames(expected_frame, frame, 8);
        expected_frame[4] = 0x17;
        expected_frame[5] = 0x35;
        insert(frame, startbit, length, value_two_bytes_signed, MOTOROLA);
        printf("\nstep 6.2");
        assert(test_equality(expected_frame, frame, 8));
        display(expected_frame, frame, 8);

        /* step 6.3 (lsb middle of byte), unsigned */
        value_two_bytes_unsigned = 189;
        startbit = 21;
        length   = 9;
        init_frames(expected_frame, frame, 8);
        expected_frame[2] = 0x17;
        expected_frame[3] = 0xA0;
        insert(frame, startbit, length, value_two_bytes_unsigned, MOTOROLA);
        printf("\nstep 6.3");
        assert(test_equality(expected_frame, frame, 8));
        display(expected_frame, frame, 8);

        /*
         * step x.x (lsb middle of byte), signed , signed signals always encoded 
         * on start of byte 
         */

        /* step 6.4 (lsb start of byte), unsigned */
        value_two_bytes_unsigned = 1390;
        startbit = 34;
        length   = 11;
        init_frames(expected_frame, frame, 8);
        expected_frame[4] = 0x05;
        expected_frame[5] = 0x6E;
        insert(frame, startbit, length, value_two_bytes_unsigned, MOTOROLA);
        printf("\nstep 6.4");
        assert(test_equality(expected_frame, frame, 8));
        display(expected_frame, frame, 8);

        /* step 6.5 (lsb start of byte), signed */
        value_two_bytes_signed = -24244;
        startbit = 7;
        length   = 16;
        init_frames(expected_frame, frame, 8);
        expected_frame[0] = 0xA1;
        expected_frame[1] = 0x4C;
        insert(frame, startbit, length, value_two_bytes_signed, MOTOROLA);
        printf("\nstep 6.5");
        assert(test_equality(expected_frame, frame, 8));
        display(expected_frame, frame, 8);

        /* step 6.6 (signal on 7 bytes, lsb start of byte), unsigned */
        uint64_t value_seven_bytes_unsigned = 48413335211474859;
        startbit = 7;
        length   = 56;
        init_frames(expected_frame, frame, 8);
        expected_frame[0] = 0xAB;
        expected_frame[1] = 0xFF;
        expected_frame[2] = 0xAB;
        expected_frame[3] = 0xFF;
        expected_frame[4] = 0xAB;
        expected_frame[5] = 0xFF;
        expected_frame[6] = 0xAB;
        insert(frame, startbit, length, value_seven_bytes_unsigned, MOTOROLA);
        printf("\nstep 6.6");
        assert(test_equality(expected_frame, frame, 8));
        display(expected_frame, frame, 8);

        /* step 6.7 (signal on 4 bytes, lsb start of byte), signed */
        int64_t value_four_bytes_signed = -489;
        startbit = 39;
        length   = 32;
        init_frames(expected_frame, frame, 8);
        expected_frame[4] = 0xFF;
        expected_frame[5] = 0xFF;
        expected_frame[6] = 0xFE;
        expected_frame[7] = 0x17;
        insert(frame, startbit, length, value_four_bytes_signed, MOTOROLA);
        printf("\nstep 6.7");
        assert(test_equality(expected_frame, frame, 8));
        display(expected_frame, frame, 8);

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
        value_one_byte_unsigned = 240;
        startbit = 16;
        length   = 8;
        init_frames(expected_frame, frame, 8);
        expected_frame[2] = 0xF0;
        insert(frame, startbit, length, value_one_byte_unsigned, INTEL);
        printf("\nstep 7.1");
        assert(test_equality(expected_frame, frame, 8));
        display(expected_frame, frame, 8);

        /* step 7.2 all signal on 1 byte, signed */
        value_one_byte_signed = -202;
        startbit = 32;
        length   = 8;
        init_frames(expected_frame, frame, 8);
        expected_frame[4] = 0x36;
        insert(frame, startbit, length, value_one_byte_signed, INTEL);
        printf("\nstep 7.2");
        assert(test_equality(expected_frame, frame, 8));
        display(expected_frame, frame, 8);

        /* step 7.3 (lsb middle of byte), unsigned */
        value_one_byte_unsigned = 7;
        startbit  = 29;
        length    = 3;
        init_frames(expected_frame, frame, 8);
        expected_frame[3] = 0xE0;
        insert(frame, startbit, length, value_one_byte_unsigned, INTEL);
        printf("\nstep 7.3");
        assert(test_equality(expected_frame, frame, 8));
        display(expected_frame, frame, 8);

        /*
         * step x.x (lsb middle of byte), signed , signed signals always encoded 
         * on start of byte 
         */

        /* step 7.4 (lsb start of byte), unsigned */
        value_one_byte_unsigned = 23;
        startbit  = 56;
        length    = 5;
        init_frames(expected_frame, frame, 8);
        expected_frame[7] = 0x17;
        insert(frame, startbit, length, value_one_byte_unsigned, INTEL);
        printf("\nstep 7.4");
        assert(test_equality(expected_frame, frame, 8));
        display(expected_frame, frame, 8);

        /* step 7.5 (lsb start of byte), signed */
        value_one_byte_signed = -199;
        startbit  = 40;
        length    = 8;
        init_frames(expected_frame, frame, 8);
        expected_frame[5] = 0x39;
        insert(frame, startbit, length, value_one_byte_signed, INTEL);
        printf("\nstep 7.5");
        assert(test_equality(expected_frame, frame, 8));
        display(expected_frame, frame, 8);

        /* step 8 (2 bytes) */
        /* step 8.1 (all signal on 2 bytes), unsigned */
        value_two_bytes_unsigned = 52077;
        startbit = 16;
        length   = 16;
        init_frames(expected_frame, frame, 8);
        expected_frame[2] = 0x6D;
        expected_frame[3] = 0xCB;
        insert(frame, startbit, length, value_two_bytes_unsigned, INTEL);
        printf("\nstep 8.1");
        assert(test_equality(expected_frame, frame, 8));
        display(expected_frame, frame, 8);

        /* step 8.2 (all signal on 2 bytes), signed */
        value_two_bytes_signed = -48666;
        startbit = 32;
        length   = 16;
        init_frames(expected_frame, frame, 8);
        expected_frame[4] = 0xE6;
        expected_frame[5] = 0x41;
        insert(frame, startbit, length, value_two_bytes_signed, INTEL);
        printf("\nstep 8.2");
        assert(test_equality(expected_frame, frame, 8));
        display(expected_frame, frame, 8);

        /* step 8.3 (lsb middle of byte), unsigned */
        value_two_bytes_unsigned = 1707;
        startbit = 44;
        length   = 11;
        init_frames(expected_frame, frame, 8);
        expected_frame[5] = 0xB0;
        expected_frame[6] = 0x6A;
        insert(frame, startbit, length, value_two_bytes_unsigned, INTEL);
        printf("\nstep 8.3");
        assert(test_equality(expected_frame, frame, 8));
        display(expected_frame, frame, 8);

        /*
         * step x.x (lsb middle of byte), signed , signed signals always encoded 
         * on start of byte 
         */

        /* step 8.4 (lsb start of byte), unsigned */
        value_two_bytes_unsigned = 1023;
        startbit = 8;
        length   = 10;
        init_frames(expected_frame, frame, 8);
        expected_frame[1] = 0xFF;
        expected_frame[2] = 0x03;
        insert(frame, startbit, length, value_two_bytes_unsigned, INTEL);
        printf("\nstep 8.4");
        assert(test_equality(expected_frame, frame, 8));
        display(expected_frame, frame, 8);

        /* step 8.5 (lsb start of byte), signed */
        value_two_bytes_signed = -59821;
        startbit = 48;
        length   = 16;
        init_frames(expected_frame, frame, 8);
        expected_frame[6] = 0x53;
        expected_frame[7] = 0x16;
        insert(frame, startbit, length, value_two_bytes_signed, INTEL);
        printf("\nstep 8.5");
        assert(test_equality(expected_frame, frame, 8));
        display(expected_frame, frame, 8);

        /* step 8.6 (signal on 7 bytes, lsb start of byte), unsigned */
        value_seven_bytes_unsigned = 48413335211474859;
        startbit = 0;
        length   = 56;
        init_frames(expected_frame, frame, 8);
        expected_frame[0] = 0xAB;
        expected_frame[1] = 0xFF;
        expected_frame[2] = 0xAB;
        expected_frame[3] = 0xFF;
        expected_frame[4] = 0xAB;
        expected_frame[5] = 0xFF;
        expected_frame[6] = 0xAB;
        insert(frame, startbit, length, value_seven_bytes_unsigned, INTEL);
        printf("\nstep 8.6");
        assert(test_equality(expected_frame, frame, 8));
        display(expected_frame, frame, 8);

        /* step 8.7 (signal on 4 bytes, lsb start of byte), signed */
        value_four_bytes_signed = -1339;
        startbit = 7;
        length   = 32;
        init_frames(expected_frame, frame, 8);
        expected_frame[0] = 0xFF;
        expected_frame[1] = 0xFF;
        expected_frame[2] = 0xFA;
        expected_frame[3] = 0xC5;
        insert(frame, startbit, length, value_four_bytes_signed, MOTOROLA);
        printf("\nstep 8.7");
        assert(test_equality(expected_frame, frame, 8));
        display(expected_frame, frame, 8);

        /*
         ***********************************************************************
         * ENCODE DECODE MOTOROLA 32 bits double float value
         ***********************************************************************
         */
        printf("\n\n\n");
        printf("===========================================================\n");
        printf("ENCODE DECODE MOTOROLA 32 bits double value\n");
        printf("===========================================================\n");
        double physical_value = 66.66666;
        init_single_frame(frame, 8);
        startbit = 7;
        length = 32;
        double factor = 0.0000001; /* 1E-007 */
        double offset = 0;
        printf("\nstep 9.1\n\n");
        printf("value to be encoded: %.5lf\n", physical_value);
        encode_double(frame, physical_value, startbit, length, MOTOROLA, factor, offset);
        display_single(frame, 8);
        printf("\ndecoded value:       %.5lf\n", 
                        decode_double(frame, startbit, length, MOTOROLA, factor, offset));
        assert(physical_value == decode_double(frame, startbit, length, MOTOROLA, factor, offset));

        /*
         ***********************************************************************
         * ENCODE DECODE MOTOROLA 32 bits double float negative value
         ***********************************************************************
         */
        printf("\n\n\n");
        printf("===========================================================\n");
        printf("ENCODE DECODE MOTOROLA 32 bits double negative value\n");
        printf("===========================================================\n");
        physical_value = -50.6164129;
        init_single_frame(frame, 8);
        startbit = 7;
        length = 32;
        factor = 0.0000001; /* 1E-007 */
        offset = 0;
        printf("\nstep 9.2\n\n");
        printf("value to be encoded: %.7lf\n", physical_value);
        encode_double(frame, physical_value, startbit, length, MOTOROLA, factor, offset);
        display_single(frame, 8);
        printf("\ndecoded value:       %.7lf\n", 
                        decode_double(frame, startbit, length, MOTOROLA, factor, offset));
        assert(physical_value == decode_double(frame, startbit, length, MOTOROLA, factor, offset));

        /*
         ***********************************************************************
         * ENCODE DECODE MOTOROLA 32 bits uint64_t value
         ***********************************************************************
         */
        printf("\n\n\n");
        printf("===========================================================\n");
        printf("ENCODE DECODE MOTOROLA 32 bits uint64_t value\n");
        printf("===========================================================\n");
        uint64_t uphysical_value = 666666666;
        init_single_frame(frame, 8);
        startbit = 7;
        length = 32;
        factor = 1;
        offset = 0;
        printf("\nstep 9.3\n\n");
        printf("value to be encoded: %ld\n", uphysical_value);
        encode_uint64_t(frame, uphysical_value, startbit, length, MOTOROLA, factor, offset);
        display_single(frame, 8);
        printf("\ndecoded value:       %ld\n", 
                        decode_uint64_t(frame, startbit, length, MOTOROLA, factor, offset));
        assert(uphysical_value == decode_uint64_t(frame, startbit, length, MOTOROLA, factor, offset));

        /*
         ***********************************************************************
         * ENCODE DECODE INTEL 32 bits double float value
         ***********************************************************************
         */
        printf("\n\n\n");
        printf("===========================================================\n");
        printf("ENCODE DECODE INTEL 32 bits double value\n");
        printf("===========================================================\n");
        physical_value = 8.4939123;
        init_single_frame(frame, 8);
        startbit = 0;
        length = 32;
        factor = 0.0000001; /* 1E-007 */
        offset = 0;
        printf("\nstep 9.4\n\n");
        printf("value to be encoded: %.7lf\n", physical_value);
        encode_double(frame, physical_value, startbit, length, INTEL, factor, offset);
        display_single(frame, 8);
        printf("\ndecoded value:       %.7lf\n",
                        decode_double(frame, startbit, length, INTEL, factor, offset));
        assert(physical_value == 
                        decode_double(frame, startbit, length, INTEL, factor, offset));

        /*
         ***********************************************************************
         * ENCODE DECODE INTEL 32 bits double float negative value
         ***********************************************************************
         */
        printf("\n\n\n");
        printf("===========================================================\n");
        printf("ENCODE DECODE INTEL 32 bits double negative value\n");
        printf("===========================================================\n");
        physical_value = -7.7979897;
        init_single_frame(frame, 8);
        startbit = 0;
        length = 32;
        factor = 0.0000001; /* 1E-007 */
        offset = 0;
        printf("\nstep 9.5\n\n");
        printf("value to be encoded: %.7lf\n", physical_value);
        encode_double(frame, physical_value, startbit, length, INTEL, factor, offset);
        display_single(frame, 8);
        printf("\ndecoded value:       %.7lf\n", 
                        decode_double(frame, startbit, length, INTEL, factor, offset));
        assert(physical_value == 
                        decode_double(frame, startbit, length, INTEL, factor, offset));

        /*
         ***********************************************************************
         * ENCODE DECODE INTEL 32 bits uint64_t value
         ***********************************************************************
         */
        printf("\n\n\n");
        printf("===========================================================\n");
        printf("ENCODE DECODE INTEL 32 bits uint64_t value\n");
        printf("===========================================================\n");
        uphysical_value = 999999999;
        init_single_frame(frame, 8);
        startbit = 0;
        length = 32;
        factor = 1;
        offset = 0;
        printf("\nstep 9.6\n\n");
        printf("value to be encoded: %ld\n", uphysical_value);
        encode_uint64_t(frame, uphysical_value, startbit, length, INTEL, factor, offset);
        display_single(frame, 8);
        printf("\ndecoded value:       %ld\n", 
                        decode_uint64_t(frame, startbit, length, INTEL, factor, offset));
        assert(uphysical_value == decode_uint64_t(frame, startbit, length, INTEL, factor, offset));

        /*
         ***********************************************************************
         * ENCODE DECODE INTEL int64_t negative value
         ***********************************************************************
         */
        printf("\n\n\n");
        printf("===========================================================\n");
        printf("ENCODE DECODE INTEL int64_t negative value\n");
        printf("===========================================================\n");
        int64_t sphysical_value = -1029384756;
        init_single_frame(frame, 8);
        startbit = 0;
        length = 32;
        factor = 1;
        offset = 0;
        printf("\nstep 9.7\n\n");
        printf("value to be encoded: %ld\n", sphysical_value);
        encode_int64_t(frame, sphysical_value, startbit, length, INTEL, factor, offset);
        display_single(frame, 8);
        printf("\ndecoded value:       %ld\n", 
                        decode_int64_t(frame, startbit, length, INTEL, factor, offset));
        assert(sphysical_value == decode_int64_t(frame, startbit, length, INTEL, factor, offset));

        /*
         ***********************************************************************
         * ENCODE DECODE MOTOROLA float negative value
         ***********************************************************************
         */
        printf("\n\n\n");
        printf("===========================================================\n");
        printf("ENCODE DECODE MOTOROLA float negative value\n");
        printf("===========================================================\n");
        float fphysical_value = -2938.345666;
        init_single_frame(frame, 8);
        startbit = 7;
        length = 40;
        factor = 0.0000001;
        offset = 0;
        printf("\nstep 9.8\n\n");
        printf("value to be encoded: %.6f\n", fphysical_value);
        encode_float(frame, fphysical_value, startbit, length, MOTOROLA, factor, offset);
        display_single(frame, 8);
        printf("\ndecoded value:       %.6f\n", 
                        decode_float(frame, startbit, length, MOTOROLA, factor, offset));
        assert(fphysical_value == decode_float(frame, startbit, length, MOTOROLA, factor, offset));

        return 0;
}

void init_frames(uint8_t *expected_frame, uint8_t *frame, uint8_t dlc)
{
        int i;
        for (i = 0; i < dlc; i++) {
                expected_frame[i] = 0x00;
                frame[i] = 0x00;
        }
}

void init_single_frame(uint8_t *frame, uint8_t dlc)
{
        int i;
        for (i = 0; i < dlc; i++) {
                frame[i] = 0x00;
        }
}

int test_equality(uint8_t *expected_frame, uint8_t *frame, uint8_t dlc)
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

void display(uint8_t *expected_frame, uint8_t *frame, uint8_t dlc)
{
        printf("\n");

        printf("expected frame: ");
        int i;
        for (i = 0; i < dlc; i++) 
                printf("%02x ", expected_frame[i]);

        printf("\n");

        printf("frame:          ");
        for (i = 0; i < dlc; i++) 
                printf("%02x ", frame[i]);

        printf("\n");
}

void display_single(uint8_t *frame, uint8_t dlc)
{
        printf("\n");

        printf("encoded frame: ");
        int i;
        for (i = 0; i < dlc; i++) 
                printf("%02x ", frame[i]);

        printf("\n");
}
