/*
 * 8051 Emulator - p8051.h (Processor Disassembler)
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
#ifndef __D8051_H__
#define __D8051_H__

// D8051_DIRECT is a 8051 direct memory location description
typedef struct D8051_DIRECT {
    unsigned char	addr;
    char*		name;
};

// D8051_BIT is a 8051 bit location description
typedef struct D8051_BIT {
    unsigned char	addr;
    char*		name;
};

// D8051_OPCODE is a 8051 instruction

// D8051_OPCODE is a 8051 instruction
typedef struct D8051_OPCODE {
    char*	name;
    char*	arg;
};

/*
 * D8051 is a 8051 disassembler.
 */
class D8051 {
public:
    int disasm (unsigned int, char*, int, unsigned char*, int);

private:
    D8051_DIRECT* lookup_direct (unsigned char);
    D8051_BIT*	  lookup_bit (unsigned char);
};
#endif
