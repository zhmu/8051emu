/*
 * 8051 Emulator - d8051.cc (Processor Disassembly)
 * (c) 2002 Rink Springer
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRENTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
 * USA.
 */
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "d8051.h"
#include "p8051.h"

/* D8051_direct[] is the table of direct memory locations */
struct D8051_DIRECT D8051_direct[] = {
	{ P8051_REG_R0,			"R0" },
	{ P8051_REG_R1,			"R1" },
	{ P8051_REG_R2,			"R2" },
	{ P8051_REG_R3,			"R3" },
	{ P8051_REG_R4,			"R4" },
	{ P8051_REG_R5,			"R5" },
	{ P8051_REG_R6,			"R6" },
	{ P8051_REG_R7,			"R7" },
	{ P8051_REG_P0,			"P0" },
	{ P8051_REG_SP,			"SP" },
	{ P8051_REG_DPL,		"DPL" },
	{ P8051_REG_DPH,		"DPH" },
	{ P8051_REG_PCON,		"PCON" },
	{ P8051_REG_TCON,		"TCON" },
	{ P8051_REG_TMOD,		"TMOD" },
	{ P8051_REG_TL0,		"TL0" },
	{ P8051_REG_TH0,		"TH0" },
	{ P8051_REG_TL1,		"TL1" },
	{ P8051_REG_TH1,		"TH1" },
	{ P8051_REG_P1,			"P1" },
	{ P8051_REG_SCON,		"SCON" },
	{ P8051_REG_SBUF,		"SBUF" },
	{ P8051_REG_P2,			"P2" },
	{ P8051_REG_IEN0,		"IEN0" },
	{ P8051_REG_IP0,		"IP0" },
	{ P8051_REG_P3,			"P3" },
	{ P8051_REG_IEN1,		"IEN1" },
	{ P8051_REG_IP1,		"IP1" },
	{ P8051_REG_IRCON,		"IRCON" },
	{ P8051_REG_CCEN,		"CCEN" },
	{ P8051_REG_CCL1,		"CCL1" },
	{ P8051_REG_CCH1,		"CCH1" },
	{ P8051_REG_CCL2,		"CCL2" },
	{ P8051_REG_CCH2,		"CCH2" },
	{ P8051_REG_CCL3,		"CCL3" },
	{ P8051_REG_CCH3,		"CCH3" },
	{ P8051_REG_T2CON,		"T2CON" },
	{ P8051_REG_CRCH,		"CRCH" },
	{ P8051_REG_CRCL,		"CRCL" },
	{ P8051_REG_TL2,		"TL2" },
	{ P8051_REG_TH2,		"TH2" },
	{ P8051_REG_PSW,		"PSW" },
	{ P8051_REG_ADCON,		"ADCON" },
	{ P8051_REG_ADDAT,		"ADDAT" },
	{ P8051_REG_DAPR,		"DAPR" },
	{ P8051_REG_P6,			"P6" },
	{ P8051_REG_ACC,		"ACC" },
	{ P8051_REG_P4,			"P4" },
	{ P8051_REG_B,			"B" },
	{ P8051_REG_P5,			"P5" },
	{ 0,				NULL }
    };

/* D8051_bit[] is the table of memory bit locations */
struct D8051_BIT D8051_bit[] = {
	{ 0xD7,				"CY" },		/* PSW */
	{ 0xD6,				"AC" },
	{ 0xD5,				"F0" },
	{ 0xD4,				"RS1" },
	{ 0xD3,				"RS0" },
	{ 0xD2,				"OV" },
	{ 0xD1,				"F1" },
	{ 0xD0,				"P" },
	{ 0x9F,				"SM0" },	/* SCON */
	{ 0x9E,				"SM1" },
	{ 0x9D,				"SM2" },
	{ 0x9C,				"REN" },
	{ 0x9B,				"TB8" },
	{ 0x9A,				"RB8" },
	{ 0x99,				"TI" },
	{ 0x98,				"RI" },
	{ 0xDF,				"BD" },		/* ADCON */
	{ 0xDE,				"CLK" },
	{ 0xDD,				"ADEX" },
	{ 0xDC,				"BSY" },
	{ 0xDB,				"ADM" },
	{ 0xDA,				"MX2" },
	{ 0xD9,				"MX1" },
	{ 0xD8,				"MX0" },
	{ 0x8F,				"TF1" },	/* TCON */
	{ 0x8E,				"TR1" },
	{ 0x8D,				"TF0" },
	{ 0x8C,				"TR0" },
	{ 0x8B,				"IE1" },
	{ 0x8A,				"IT1" },
	{ 0x89,				"IE0" },
	{ 0x88,				"IT0" },
	{ 0xCF,				"T2PS" },	/* T2CON */
	{ 0xCE,				"I3FR" },
	{ 0xCD,				"I2FR" },
	{ 0xCC,				"I2R1" },
	{ 0xCB,				"I2R0" },
	{ 0xCA,				"T2CM" },
	{ 0xC9,				"T2I1" },
	{ 0xC8,				"T2I0" },
	{ 0xAF,				"EAL" },	/* IEN0 */
	{ 0xAE,				"WDT" },
	{ 0xAD,				"ET2" },
	{ 0xAC,				"ES" },
	{ 0xAB,				"ET1" },
	{ 0xAA,				"EX1" },
	{ 0xA9,				"ET0" },
	{ 0xA8,				"EX0" },
	{ 0xBF,				"EXEN2" },	/* IEN1 */
	{ 0xBE,				"SWDT" },
	{ 0xBD,				"EX6" },
	{ 0xBC,				"EX5" },
	{ 0xBB,				"EX4" },
	{ 0xBA,				"EX3" },
	{ 0xB9,				"EX2" },
	{ 0xB8,				"EADC" },
	{ 0xC7,				"EXF2" },	/* IRCON */
	{ 0xC6,				"TF2" },
	{ 0xC5,				"IEX6" },
	{ 0xC4,				"IEX5" },
	{ 0xC3,				"IEX4" },
	{ 0xC2,				"IEX3" },
	{ 0xC1,				"IEX2" },
	{ 0xC0,				"IADC" },
    };

/* D8051_opcodetab[] is the table of opcodes and their arguments */
struct D8051_OPCODE D8051_opcodetab[256] = {
	{ "NOP",	NULL },				/* 00 */
	{ "AJMP",	"%a" },
	{ "LJMP",	"%A" },
	{ "RR",		"A" },
	{ "INC",	"A" },
	{ "INC",	"%d" },
	{ "INC",	"@R0" },
	{ "INC",	"@R1" },
	{ "INC",	"R0" },
	{ "INC",	"R1" },
	{ "INC",	"R2" },
	{ "INC",	"R3" },
	{ "INC",	"R4" },
	{ "INC",	"R5" },
	{ "INC",	"R6" },
	{ "INC",	"R7" },
	{ "JBC",	"%b, %r" },			/* 10 */
	{ "ACALL",	"%a" },
	{ "LCALL",	"%A" },
	{ "RRC",	"A" },
	{ "DEC",	"A" },
	{ "DEC",	"%d" },
	{ "DEC",	"@R0" },
	{ "DEC",	"@R1" },
	{ "DEC",	"R0" },
	{ "DEC",	"R1" },
	{ "DEC",	"R2" },
	{ "DEC",	"R3" },
	{ "DEC",	"R4" },
	{ "DEC",	"R5" },
	{ "DEC",	"R6" },
	{ "DEC",	"R7" },
	{ "JB",		"%b, %r" },			/* 20 */
	{ "AJMP",	"%a" },
	{ "RET",	NULL },
	{ "RL",		"A" },
	{ "ADD",	"A, %D" },
	{ "ADD",	"A, %d" },
	{ "ADD",	"A, @R0" },
	{ "ADD",	"A, @R1" },
	{ "ADD",	"A, R0" },
	{ "ADD",	"A, R1" },
	{ "ADD",	"A, R2" },
	{ "ADD",	"A, R3" },
	{ "ADD",	"A, R4" },
	{ "ADD",	"A, R5" },
	{ "ADD",	"A, R6" },
	{ "ADD",	"A, R7" },
	{ "JNB",	"%b, %r" },			/* 30 */
	{ "ACALL",	"%a" },
	{ "RETI",	NULL },
	{ "RLC",	"A" },
	{ "ADDC",	"A, %D" },
	{ "ADDC",	"A, %d" },
	{ "ADDC",	"A, @R0" },
	{ "ADDC",	"A, @R1" },
	{ "ADC",	"A, R0" },
	{ "ADC",	"A, R1" },
	{ "ADC",	"A, R2" },
	{ "ADC",	"A, R3" },
	{ "ADC",	"A, R4" },
	{ "ADC",	"A, R5" },
	{ "ADC",	"A, R6" },
	{ "ADC",	"A, R7" },
	{ "JC",		"%r" },				/* 40 */
	{ "AJMP",	"%a" },
	{ "ORL",	"%d, A" },
	{ "ORL",	"%d, %D" },
	{ "ORL",	"A, %D" },
	{ "ORL",	"A, %d" },
	{ "ORL",	"A, @R0" },
	{ "ORL",	"A, @R1" },
	{ "ORL",	"A, R0" },
	{ "ORL",	"A, R1" },
	{ "ORL",	"A, R2" },
	{ "ORL",	"A, R3" },
	{ "ORL",	"A, R4" },
	{ "ORL",	"A, R5" },
	{ "ORL",	"A, R6" },
	{ "ORL",	"A, R7" },
	{ "JNC",	"%r" },				/* 50 */
	{ "ACALL",	"%a" },
	{ "ANL",	"%d, A" },
	{ "ANL",	"%d, %D" },
	{ "ANL",	"A, %D" },
	{ "ANL",	"A, %d" },
	{ "ANL",	"A, @R0" },
	{ "ANL",	"A, @R1" },
	{ "ANL",	"A, R0" },
	{ "ANL",	"A, R1" },
	{ "ANL",	"A, R2" },
	{ "ANL",	"A, R3" },
	{ "ANL",	"A, R4" },
	{ "ANL",	"A, R5" },
	{ "ANL",	"A, R6" },
	{ "ANL",	"A, R7" },
	{ "JZ",		"%r" },				/* 60 */
	{ "AJMP",	"%a" },
	{ "XRL",	"%d, A" },
	{ "XRL",	"%d, %D" },
	{ "XRL",	"A, %D" },
	{ "XRL",	"A, %d" },
	{ "XRL",	"A, @R0" },
	{ "XRL",	"A, @R1" },
	{ "XRL",	"A, R0" },
	{ "XRL",	"A, R1" },
	{ "XRL",	"A, R2" },
	{ "XRL",	"A, R3" },
	{ "XRL",	"A, R4" },
	{ "XRL",	"A, R5" },
	{ "XRL",	"A, R6" },
	{ "XRL",	"A, R7" },
	{ "JNZ",	"%r" },				/* 70 */
	{ "ACALL",	"%a" },
	{ "ORL",	"C, %b" },
	{ "JMP",	"@A + DPTR" },
	{ "MOV",	"A, %D" },
	{ "MOV",	"%d, %D" },
	{ "MOV",	"@R0, %D" },
	{ "MOV",	"@R1, %D" },
	{ "MOV",	"R0, %D" },
	{ "MOV",	"R1, %D" },
	{ "MOV",	"R2, %D" },
	{ "MOV",	"R3, %D" },
	{ "MOV",	"R4, %D" },
	{ "MOV",	"R5, %D" },
	{ "MOV",	"R6, %D" },
	{ "MOV",	"R7, %D" },
	{ "SJMP",	"%r" },				/* 80 */
	{ "AJMP",	"%a" },
	{ "ANL",	"C, %b" },
	{ "MOVC",	"A, @A + PC" },
	{ "DIV",	"AB" },
	{ "MOV",	"%d, %d" },
	{ "MOV",	"%d, @R0" },
	{ "MOV",	"%d, @R1" },
	{ "MOV",	"%d, R0" },
	{ "MOV",	"%d, R1" },
	{ "MOV",	"%d, R2" },
	{ "MOV",	"%d, R3" },
	{ "MOV",	"%d, R4" },
	{ "MOV",	"%d, R5" },
	{ "MOV",	"%d, R6" },
	{ "MOV",	"%d, R7" },
	{ "MOV",	"DPTR, #x" },			/* 90 */
	{ "ACALL",	"%a" },
	{ "MOV",	"%b, C" },
	{ "MOVC",	"A, @A + DPTR" },
	{ "SUBB",	"A, %D" },
	{ "SUBB",	"A, %d" },
	{ "SUBB",	"A, @R0" },
	{ "SUBB",	"A, @R1" },
	{ "SUBB",	"A, R0" },
	{ "SUBB",	"A, R1" },
	{ "SUBB",	"A, R2" },
	{ "SUBB",	"A, R3" },
	{ "SUBB",	"A, R4" },
	{ "SUBB",	"A, R5" },
	{ "SUBB",	"A, R6" },
	{ "SUBB",	"A, R7" },
	{ "ORL",	"C, /%b" },			/* A0 */
	{ "AJMP",	"%a" },
	{ "MOV",	"C, %b" },
	{ "INC",	"DPTR" },
	{ "MUL",	"AB" },
	{ "DBG",	NULL },
	{ "MOV",	"@R0, %d" },
	{ "MOV",	"@R1, %d" },
	{ "MOV",	"R0, %d" },
	{ "MOV",	"R1, %d" },
	{ "MOV",	"R2, %d" },
	{ "MOV",	"R3, %d" },
	{ "MOV",	"R4, %d" },
	{ "MOV",	"R5, %d" },
	{ "MOV",	"R6, %d" },
	{ "MOV",	"R7, %d" },
	{ "ANL",	"C, /%b" },			/* B0 */
	{ "ACALL",	"%a" },
	{ "CPL",	"%b" },
	{ "CPL",	"C" },
	{ "CJNE",	"A, %D, %r" },
	{ "CJNE",	"A, %d, %r" },
	{ "CJNE",	"@R0, %D, %r" },
	{ "CJNE",	"@R1, %D, %r" },
	{ "CJNE",	"R0, %D, %r" },
	{ "CJNE",	"R1, %D, %r" },
	{ "CJNE",	"R2, %D, %r" },
	{ "CJNE",	"R3, %D, %r" },
	{ "CJNE",	"R4, %D, %r" },
	{ "CJNE",	"R5, %D, %r" },
	{ "CJNE",	"R6, %D, %r" },
	{ "CJNE",	"R7, %D, %r" },
	{ "PUSH",	"%d" },				/* C0 */
	{ "AJMP",	"%a" },
	{ "CLR",	"%b" },
	{ "CLR",	"C" },
	{ "SWAP",	"A" },
	{ "XCH",	"A, %d" },
	{ "XCH",	"A, @R0" },
	{ "XCH",	"A, @R1" },
	{ "XCH",	"A, R0" },
	{ "XCH",	"A, R1" },
	{ "XCH",	"A, R2" },
	{ "XCH",	"A, R3" },
	{ "XCH",	"A, R4" },
	{ "XCH",	"A, R5" },
	{ "XCH",	"A, R6" },
	{ "XCH",	"A, R7" },
	{ "POP",	"%d" },				/* D0 */
	{ "ACALL",	"%a" },
	{ "SETB",	"%b" },
	{ "SETB",	"C" },
	{ "DA",		NULL },
	{ "DJNZ",	"%d, %r" },
	{ "XCHD",	"A, @R0" },
	{ "XCHD",	"A, @R1" },
	{ "DJNZ",	"R0, %r" },
	{ "DJNZ",	"R1, %r" },
	{ "DJNZ",	"R2, %r" },
	{ "DJNZ",	"R3, %r" },
	{ "DJNZ",	"R4, %r" },
	{ "DJNZ",	"R5, %r" },
	{ "DJNZ",	"R6, %r" },
	{ "DJNZ",	"R7, %r" },
	{ "MOVX",	"A, @DPTR" },			/* E0 */
	{ "AJMP",	"%a" },
	{ "MOVX",	"A, @R0" },
	{ "MOVX",	"A, @R1" },
	{ "CLR",	"A" },
	{ "MOV",	"A, %d" },
	{ "MOV",	"A, @R0" },
	{ "MOV",	"A, @R1" },
	{ "MOV",	"A, R0" },
	{ "MOV",	"A, R1" },
	{ "MOV",	"A, R2" },
	{ "MOV",	"A, R3" },
	{ "MOV",	"A, R4" },
	{ "MOV",	"A, R5" },
	{ "MOV",	"A, R6" },
	{ "MOV",	"A, R7" },
	{ "MOVX",	"@DPTR, A" },			/* F0 */
	{ "ACALL",	"%a" },
	{ "MOVX",	"@R0, A" },
	{ "MOVX",	"@R1, A" },
	{ "CPL",	"A" },
	{ "MOV",	"%d, A" },
	{ "MOV",	"@R0, A" },
	{ "MOV",	"@R1, A" },
	{ "MOV",	"R0, A" },
	{ "MOV",	"R1, A" },
	{ "MOV",	"R2, A" },
	{ "MOV",	"R3, A" },
	{ "MOV",	"R4, A" },
	{ "MOV",	"R5, A" },
	{ "MOV",	"R6, A" },
	{ "MOV",	"R7, A" }
    };

/*
 * D8051::lookup_direct (unsigned char addr)
 *
 * This will try to look up direct memory location [addr]. It will return a
 * pointer to the D8051_DIRECT record associated with it or NULL it does not
 * exist.
 *
 */
D8051_DIRECT*
D8051::lookup_direct (unsigned char addr) {
    D8051_DIRECT* dir = (D8051_DIRECT*)&D8051_direct;

    // scan them all
    while (dir->name) {
	// match?
	if (dir->addr == addr)
	    // yes. return the record
	    return dir;

	// no, next
	dir++;
    }

    // no match
    return NULL;
}

/*
 * D8051::lookup_bit (unsigned char addr)
 *
 * This will try to look up bit location [addr]. It will return a pointer to the
 * D8051_BIT record associated with it or NULL it does not exist.
 *
 */
D8051_BIT*
D8051::lookup_bit (unsigned char addr) {
    D8051_BIT* bit = (D8051_BIT*)&D8051_bit;

    // scan them all
    while (bit->name) {
	// match?
	if (bit->addr == addr)
	    // yes. return the record
	    return bit;

	// no, next
	bit++;
    }

    // no match
    return NULL;
}

/*
 * D8051::disasm (unsigned int addr, char* buf, int buflen, char* data, int datalen)
 *
 * This will disassemble a single instruction, and return that into [buf],
 * using [buflen] bytes or less. It will fetch the instruction and the
 * operands from [data], using [datalen] bytes at most. This will return the
 * number of bytes read, which is always 1, 2 or 3. The current address is
 * assumed to be [addr].
 *
 */
int
D8051::disasm (unsigned int addr, char* buf, int buflen, unsigned char* data, int datalen) {
    unsigned char opcode = *data++;
    struct D8051_OPCODE op = D8051_opcodetab[opcode];
    struct D8051_DIRECT* dir;
    struct D8051_BIT* bit;
    int count = 1;
    unsigned int i;
    char* ptr;

    // go
    snprintf (buf, buflen, "%s", op.name);

    // any arguments?
    if (op.arg == NULL) {
	// nope. all set
	return count;
    }

    // append spaces
    while (strlen (buf) != 6)
        snprintf (buf, buflen, "%s ", buf);

    // handle the arguments
    ptr = op.arg;
    while (*ptr) {
	// special char?
	if (*ptr == '%') {
	    // yes. handle it
	    ptr++;
	    switch (*ptr++) {
		case 'a': // 11-bit address
			  i = (unsigned int)(*data++);
			  snprintf (buf, buflen, "%s0x%x", buf, i);
			  count++;
			  break;
		case 'A': // 16-bit address
			  i = (unsigned int)(*data << 8) | (unsigned int)(*(data + 1));
			  snprintf (buf, buflen, "%s0x%x", buf, i);
			  count += 2; data += 2;
			  break;
		case 'd': // direct
			  i = (unsigned int)(*data++);

			  // try to look it up
			  dir = lookup_direct (i);
			  if (dir != NULL) {
			      snprintf (buf, buflen, "%s%s", buf, dir->name);
			  } else {
			      snprintf (buf, buflen, "%s0x%x", buf, i);
			  }
			  count++;
			  break;
		case 'D': // data
			  i = (unsigned int)(*data++);
			  snprintf (buf, buflen, "%s#0x%x", buf, i);
			  count++;
			  break;
		case 'b': // bit
			  i = (unsigned int)(*data++);

			  // try to look it up
			  bit = lookup_bit (i);
			  if (bit != NULL) {
			      snprintf (buf, buflen, "%s%s", buf, bit->name);
			  } else {
			      snprintf (buf, buflen, "%s0x%x", buf, i);
			  }
			  count++;
			  break;
		case 'r': // relative
			  i = (unsigned int)(*data++);
			  if (i > 0x7f) {
			      snprintf (buf, buflen, "%s0x%x", buf, addr - (0xfe - i));
			  } else {
			      snprintf (buf, buflen, "%s0x%x", buf, addr + i);
			  }
			  count++;
			  break;
		case 'x': // data16
			  i = (unsigned int)(*data << 8) | (unsigned int)(*(data + 1));
			  snprintf (buf, buflen, "%s#0x%x", buf, i);
			  count += 2; data += 2;
			  break;
	    }
	} else {
	    // just append the character
	    snprintf (buf, buflen, "%s%c", buf, *ptr++);
	}
    } 

    return count;
}
