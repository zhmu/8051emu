/*
 * 8051 Emulator - p8051.cc (Processor Emulation)
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
#include "p8051.h"
#include "d8051.h"

/*
 * P8051::P8051(MEMORY* mem,D8051* d)
 *
 * This will initialize a plain 8051 processor with memory [mem] and debugger
 * [d].
 *
 */
P8051::P8051(MEMORY* mem, D8051* d) {
    // activate the memory and debugger
    memory = mem; debugger = d;

    // reset the system
    reset();
}

/*
 * P8051::reset()
 *
 * This will reset the 8051 processor.
 *
 */
void
P8051::reset() {
    // reset all ports
    P0 = 0xFF; P1 = 0xFF; P2 = 0xFF; P3 = 0xFF; P4 = 0xFF; P5 = 0xFF; P6 = 0xFF;

    // reset the CCx registers
    CCL1 = CCH1 = CCL2 = CCH2 = CCL3 = CCH3 = CCEN = 0x00;

    // reset the xCON registers
    TCON = T2CON = ADCON = PCON = 0;

    // reset the timers
    TL0 = TH0 = TL1 = TH1 = TL2 = TH2 = 0;

    // reset the serial stuff
    SCON = 0;

    // reset the registers
    SP = 0x07; ACC = 0; B = 0; reg_base = 0;

    // reset everything else
    DPTRH = DPTRL = 0; TMOD = 0; IEN0 = 0; IEN1 = 0; IP0 = 0; IP1 = 0; PSW = 0;

    // finally, the Program Counter
    PC = 0;
}

/*
 * P8051::load (char* fname)
 *
 * This will try to load Intel HEX file [fname] into the 8051's memory. It
 * will return 0 on success or -1 on failure.
 *
 */
int
P8051::load (char* fname) {
    FILE* f;
    char line[512];
    unsigned int len;
    unsigned int addr;
    unsigned int type;
    unsigned int data;
    char* ptr;

    // try to open the file
    if ((f = fopen (fname, "rt")) == NULL) {
	// this failed. complain
	return -1;
    }

    // handle it line by line
    while (fgets (line, sizeof (line), f)) {
	// kill the newline
	line[strlen (line) - 1] = 0;

	// does it start with a colon ?
	if (*line == ':') {
	    // yes. fetch the size
	    if (sscanf (line, ":%02x%04x%02x", &len, &addr, &type)) {
		//printf ("[(%02x) %02x @ %04x]\n", type, len, addr);

		// add all data bytes as needed
		ptr = line + 9;
		while (len--) {
		    if (sscanf (ptr, "%02x", &data)) {
			// this worked.
			memory->set_byte (addr, data);
			addr++; ptr += 2;
		    } else {
			// invalid file
			fclose (f);
			return -1;
		    }
		}
	    }
	}
    }

    // close the file, all went ok
    fclose (f);
    return 0;
}

/*
 * P8051::disasm (unsigned int offs, char* buf, int buflen)
 *
 * This will disassemble the instructions at offset [offs], and put the
 * result in [buf], using [buflen] bytes at most. It will return the
 * number of bytes read at [offs], which is always 1, 2 or 3.
 *
 */
int
P8051::disasm (unsigned int offs, char* buf, int buflen) {
    return debugger->disasm (offs, buf, buflen, memory->get_memory() + offs, MEMORY_SIZE);
}

/*
 * P8051::get_indirect (unsigned char addr)
 *
 * This will return (addr), used for things like (R0) etc.
 *
 */
unsigned char
P8051::get_indirect (unsigned char addr) {
    return int_ram[addr];
}

/*
 * P8051::proc_write (unsigned char addr)
 *
 * This will be called when the processor wrote to an address. The address
 * written to is in [addr].
 *
 */
void
P8051::proc_write (unsigned char addr) {
    // PSW?
    if (addr == P8051_REG_PSW) {
	// yes. make sure the register banks are all-right
	reg_base = ((int_ram[addr] >> 3) & 3) << 3;
    }
    
    // SBUF
    if (addr == 0x99) {
//	printf ("SBUF [%c]\n", int_ram[addr]);
	fprintf (stderr, "%c", int_ram[addr]); fflush (stderr);
	SCON |= 2;
    }
}

/*
 * P8051::set_[xxx](int x)
 *
 * This will set the [xxx] flag on if [x] is non-zero or off is [x] is zero.
 *
 */
void P8051::set_carry (int x) { if (x) { PSW |= 0x80; } else { PSW &= ~0x80; } }
void P8051::set_overflow (int x) { if (x) { PSW |= 4; } else { PSW &= ~4; } }
void P8051::set_auxcarry (int x) { if (x) { PSW |= 0x40; } else { PSW &= ~0x40; } }
void P8051::set_parity (int x) { if (x) { PSW |= 1; } else { PSW &= ~1; } }

/*
 * P8051::get_[xxx]()
 *
 * This will retrieve the status of the [xxx] flag or register.
 *
 */
int P8051::get_carry() { return (PSW & 0x80); }
int P8051::get_overflow() { return (PSW & 4); }
int P8051::get_auxcarry() { return (PSW & 0x40); }
int P8051::get_parity() { return (PSW & 1); }
unsigned char P8051::get_acc() { return ACC; }
unsigned char P8051::get_b() { return B; }
unsigned char P8051::get_sp() { return SP; }
unsigned int P8051::get_pc() { return PC; }
unsigned char P8051::get_R0() { return R0; }
unsigned char P8051::get_R1() { return R1; }
unsigned char P8051::get_R2() { return R2; }
unsigned char P8051::get_R3() { return R3; }
unsigned char P8051::get_R4() { return R4; }
unsigned char P8051::get_R5() { return R5; }
unsigned char P8051::get_R6() { return R6; }
unsigned char P8051::get_R7() { return R7; }
unsigned int  P8051::get_DPTR() { return (DPTRH << 8) | DPTRL; }

/*
 * P8051::set_[xxx]([yyy] [zzz])
 *
 * This will set the [xxx] register to [zzz].
 * 
 */
void P8051::set_DPTR(unsigned int c) { DPTRH = (c >> 8); DPTRL = (c & 0xff); }

/*
 * P8051::push (unsigned char b)
 *
 * This will push byte [b] on the stack.
 *
 */
void P8051::push (unsigned char b) { SP++; int_ram[SP] = b; }

/*
 * P8051::pop()
 *
 * This will pop a byte from the stack and return it.
 *
 */
unsigned char P8051::pop() { return int_ram[SP--]; }

/*
 * P8051::add (unsigned char dest)
 *
 * This will handle the ADD A, [dest] operations, and set the flags as needed.
 *
 */
void
P8051::add (unsigned char dest) {
    set_carry (((unsigned int)ACC + (unsigned int)dest) > 255);
    set_auxcarry (((ACC & 0x0F) + (dest & 0x0F)) & 0xF0);
    set_overflow (((ACC & 0x7F) + (dest & 0x7F)) & 0x80);
    ACC = ACC + dest;
}

/*
 * P8051::addc (unsigned char dest)
 *
 * This will handle the ADDC A, [dest] operations, and set the flags as needed.
 *
 */
void
P8051::addc (unsigned char dest) {
    int c = (get_carry() ? 1 : 0);

    set_carry (((unsigned int)ACC + (unsigned int)dest + (unsigned int)c) > 255);
    set_auxcarry (((ACC & 0x0F) + (dest & 0x0F) + c) & 0xF0);
    set_overflow (((ACC & 0x7F) + (dest & 0x7F) + c) & 0x80);
    ACC = ACC + dest;
}

/*
 * P8051::subb (unsigned char dest)
 *
 * This will handle the SUBB A, [dest] operations, and set the flags as needed.
 *
 */
void
P8051::subb (unsigned char dest) {
    unsigned char res = ACC - dest;

    if (get_carry())
	res--;

    set_carry (((unsigned int)ACC < (unsigned int)(dest + get_carry())));
    set_overflow ((ACC < 0x80 && dest > 0x7F) ||
		  (ACC > 0x7F && dest < 0x80));
    set_auxcarry ((ACC & 0x0F) < ((dest + get_carry()) & 0x0F) ||
		  get_carry() && ((dest & 0x0F) == 0x0F));
    
}

/*
 * P8051::jmp_rel (unsigned char offs)
 *
 * This will do a relative jump according to [offs].
 *
 */
void
P8051::jmp_rel (unsigned char offs) {
    if (offs > 0x7f) {
	PC -= (0x100 - offs);
    } else {
        PC += offs;
    }
}

/*
 * P8051::get_bit (unsigned char bitaddr)
 *
 * This will calculate and return the real address of bit [bitaddr].
 *
 */
unsigned char
P8051::get_bit (unsigned char bitaddr) {
    if (bitaddr < 0x80) {
	return (bitaddr / 8) + 32;
    }

    return bitaddr & 0xF8;
}

/*
 * P8051::set_input (unsigned char c)
 *
 * This will set [c] as Serial Input.
 *
 */
void
P8051::set_input (unsigned char c) {
    int_ram[0x99] = c;
    SCON |= 1;
}

/*
 * P8051::go()
 *
 * This will execute a single 8051 instruction.
 *
 */
void
P8051::go() {
    unsigned char  opcode = *(memory->get_memory() + PC);
    unsigned char* data = (memory->get_memory() + PC + 1);
    char bla[512];
    unsigned char  tmp;
    unsigned char  tmp2;
    unsigned char  tmp3;
    unsigned int   tmpw;
    int		   cycl = 1;

    if (debugging) {
	disasm (PC, bla, sizeof (bla));
	printf ("0x%04x: %s\n", PC, bla);
    }
    PC++;

    // do it
    switch (opcode) {
	case 0x00: // NOP
		   break;
	case 0x01: // AJMP
	case 0x21: // AJMP
	case 0x41: // AJMP
	case 0x61: // AJMP
	case 0x81: // AJMP
	case 0xA1: // AJMP
	case 0xC1: // AJMP
	case 0xE1: // AJMP
		   cycl++; tmp = (opcode >> 5) & 0x07;
		   PC = (PC & 0xF800) | (tmp << 8) + *data;
		   break;
	case 0x02: // LJMP
		   PC = (unsigned int)((*data) << 8) |
			(unsigned int) (*(data + 1));
		   cycl++;
		   break;
	case 0x03: // RR A
		   tmp = ACC; ACC >>= 1;
		   if (tmp & 1) ACC |= 0x80;
		   break;
	case 0x04: // INC A
		   ACC++;
		   break;
	case 0x05: // INC direct
		   tmp = *data;
		   int_ram[tmp]++; PC++;
		   proc_write (tmp);
		   break;
	case 0x06: // INC @R0
	case 0x07: // INC @R1
		   tmp = get_indirect ((opcode == 0x06) ? R0 : R1);
		   int_ram[tmp]++;
		   proc_write (tmp);
		   break;
	case 0x08: // INC R0
	case 0x09: // INC R1
	case 0x0A: // INC R2
	case 0x0B: // INC R3
	case 0x0C: // INC R4
	case 0x0D: // INC R5
	case 0x0E: // INC R6
	case 0x0F: // INC R7
		   int_ram[reg_base + (opcode - 8)]++;
		   break;
	case 0x10: // JBC bit, rel
		   tmp = *data; data++; tmp2 = *data; PC += 2; cycl++;
		   tmp3 = get_bit (tmp);
		   if (int_ram[tmp3] & BIT_MASK (tmp)) {
		       int_ram[tmp3] &= ~BIT_MASK (tmp);
		       proc_write (tmp3);
		       jmp_rel (tmp2);
	 	   }
		   break;
	case 0x11: // ACALL addr11
	case 0x31: // ACALL addr11
	case 0x51: // ACALL addr11
	case 0x71: // ACALL addr11
	case 0x91: // ACALL addr11
	case 0xB1: // ACALL addr11
	case 0xD1: // ACALL addr11
	case 0xF1: // ACALL addr11
		   cycl++; PC++; tmp = (opcode >> 5) & 0x07;
		   push (PC & 0xff); push (PC >> 8);
		   PC = (PC & 0xF800) | (tmp << 8) + *data;
		   break;
	case 0x12: // LCALL
		   tmpw = ((*data) << 8) | (*(data + 1)); PC += 2; cycl++;
		   push (PC & 0xff); push (PC >> 8);
		   PC = tmpw;
		   break;
	case 0x13: // RRC A
		   tmp = ACC;
		   ACC >>= 1;
		   if (get_carry()) ACC |= 0x80;
		   set_carry (tmp & 1);
		   break;
	case 0x14: // DEC A
		   ACC--;
		   break;
	case 0x15: // DEC direct
		   tmp = *data;
		   int_ram[tmp]--; PC++;
		   proc_write (tmp);
		   break;
	case 0x16: // DEC @R0
	case 0x17: // DEC @R1
		   tmp = get_indirect ((opcode == 0x16) ? R0 : R1);
		   int_ram[tmp]--;
		   proc_write (tmp);
		   break;
	case 0x18: // DEC R0
	case 0x19: // DEC R1
	case 0x1A: // DEC R2
	case 0x1B: // DEC R3
	case 0x1C: // DEC R4
	case 0x1D: // DEC R5
	case 0x1E: // DEC R6
	case 0x1F: // DEC R7
		   int_ram[reg_base + (opcode - 0x18)]--;
		   break;
	case 0x20: // JB bit, rel
		   tmp = *data; data++; tmp2 = *data; PC += 2; cycl++;
		   tmp3 = get_bit (tmp);
		   if (int_ram[tmp3] & BIT_MASK (tmp))
		       jmp_rel (tmp2);
		   break;
	case 0x22: // RET
		   PC = (pop() << 8) | pop(); cycl++;
		   break;
	case 0x23: // RL A
		   tmp = ACC; ACC <<= 1;
		   if (tmp & 0x80) ACC |= 1;
		   break;
	case 0x24: // ADD A, #data
		   tmp = *data; PC++;
		   add (tmp);
		   break;
	case 0x25: // ADD A, direct
		   tmp = *data; PC++;
		   add (int_ram[tmp]);
		   break;
	case 0x26: // ADD A, @R0
	case 0x27: // ADD A, @R1
		   tmp = get_indirect ((opcode == 0x26) ? R0 : R1);
		   add (int_ram[tmp]);
		   break;
	case 0x28: // ADD A, R0
	case 0x29: // ADD A, R1
	case 0x2A: // ADD A, R2
	case 0x2B: // ADD A, R3
	case 0x2C: // ADD A, R4
	case 0x2D: // ADD A, R5
	case 0x2E: // ADD A, R6
	case 0x2F: // ADD A, R7
		   add (int_ram[reg_base + (opcode - 0x28)]);
		   break;
	case 0x30: // JNB bit, rel
		   tmp = *data; data++; tmp2 = *data; PC += 2; cycl++;
		   tmp3 = get_bit (tmp);
		   if (!(int_ram[tmp3] & BIT_MASK (tmp)))
		       jmp_rel (tmp2);
		   break;
	case 0x32: // RETI 
		   PC = (pop() << 8) | pop(); cycl++;
		   break;
	case 0x33: // RLC A
		   tmp = ACC;
		   ACC <<= 1;
		   if (get_carry()) ACC |= 1;
		   set_carry (tmp & 0x80);
		   break;
	case 0x34: // ADDC A, #data
		   tmp = *data; PC++;
		   addc (tmp);
		   break;
	case 0x35: // ADC A, direct
		   tmp = *data; PC++;
		   add (int_ram[tmp] + (get_carry()) ? 1 : 0);
		   break;
	case 0x36: // ADDC A, @R0
	case 0x37: // ADDC A, @R1
		   tmp = get_indirect ((opcode == 0x36) ? R0 : R1);
		   add (tmp);
		   break;
	case 0x38: // ADC A, R0
	case 0x39: // ADC A, R1
	case 0x3A: // ADC A, R2
	case 0x3B: // ADC A, R3
	case 0x3C: // ADC A, R4
	case 0x3D: // ADC A, R5
	case 0x3E: // ADC A, R6
	case 0x3F: // ADC A, R7
		   add (int_ram[reg_base + (opcode - 0x38)] + (get_carry()) ? 1 : 0);
		   break;
	case 0x40: // JC rel
		   tmp = *data; PC++; cycl++;
		   if (get_carry())
		       jmp_rel (tmp);
		   break;
	case 0x42: // ORL direct, A
		   tmp = *data; PC++;
		   int_ram[tmp] |= ACC;
		   proc_write (tmp);
		   break;
	case 0x43: // ORL direct, #data
		   tmp = *data++; tmp2 = *data; PC += 2;
		   int_ram[tmp] |= tmp2;
		   proc_write (tmp); cycl++;
		   break;
	case 0x44: // ORL A, #data
		   tmp = *data; PC++;
		   ACC |= tmp;
		   break;
	case 0x45: // ORL A, direct
		   tmp = *data; PC++;
		   ACC |= int_ram[tmp];
		   break;
	case 0x46: // ORL A, @R0
	case 0x47: // ORL A, @R1
		   tmp = get_indirect ((opcode == 0x46) ? R0 : R1);
		   ACC |= tmp;
		   break;
	case 0x48: // ORL A, R0
	case 0x49: // ORL A, R1
	case 0x4A: // ORL A, R2
	case 0x4B: // ORL A, R3
	case 0x4C: // ORL A, R4
	case 0x4D: // ORL A, R5
	case 0x4E: // ORL A, R6
	case 0x4F: // ORL A, R7
		   ACC |= int_ram[reg_base + (opcode - 0x48)];
		   break;
	case 0x50: // JNC rel
		   tmp = *data; PC++; cycl++;
		   if (!get_carry())
		       jmp_rel (tmp);
		   break;
	case 0x52: // ANL direct, A
		   tmp = *data; PC++;
		   int_ram[tmp] &= ACC;
		   proc_write (tmp);
		   break;
	case 0x53: // ANL direct, #data
		   tmp = *data++; tmp2 = *data; PC += 2;
		   int_ram[tmp] &= tmp2;
		   proc_write (tmp); cycl++;
		   break;
	case 0x54: // ANL A, #data
		   tmp = *data; PC++;
		   ACC &= tmp;
		   break;
	case 0x55: // ANL A, direct
		   tmp = *data; PC++;
		   ACC &= int_ram[tmp];
		   break;
	case 0x56: // ANL A, @R0
	case 0x57: // ANL A, @R1
		   tmp = get_indirect ((opcode == 0x56) ? R0 : R1);
		   ACC &= tmp;
		   break;
	case 0x58: // ANL A, R0
	case 0x59: // ANL A, R1
	case 0x5A: // ANL A, R2
	case 0x5B: // ANL A, R3
	case 0x5C: // ANL A, R4
	case 0x5D: // ANL A, R5
	case 0x5E: // ANL A, R6
	case 0x5F: // ANL A, R7
		   ACC &= int_ram[reg_base + (opcode - 0x58)];
		   break;
	case 0x60: // JZ rel
		   tmp = *data; PC++; cycl++;
		   if (!ACC)
		       jmp_rel (tmp);
		   break;
	case 0x62: // XRL direct, A
		   tmp = *data; PC++;
		   int_ram[tmp] ^= ACC;
		   proc_write (tmp);
		   break;
	case 0x63: // XRL direct, #data
		   tmp = *data++; tmp2 = *data; PC += 2; cycl++;
		   int_ram[tmp] ^= tmp2;
		   proc_write (tmp);
		   break;
	case 0x64: // XRL A, #data
		   tmp = *data; PC++;
		   ACC ^= tmp;
		   break;
	case 0x65: // XRL A, direct
		   tmp = *data; PC++;
		   ACC ^= int_ram[tmp];
		   proc_write (tmp);
		   break;
	case 0x66: // XRL A, @R0
	case 0x67: // XRL A, @R1
		   tmp = get_indirect ((opcode == 0x66) ? R0 : R1);
		   ACC &= tmp;
		   break;
	case 0x68: // XRL A, R0
	case 0x69: // XRL A, R1
	case 0x6A: // XRL A, R2
	case 0x6B: // XRL A, R3
	case 0x6C: // XRL A, R4
	case 0x6D: // XRL A, R5
	case 0x6E: // XRL A, R6
	case 0x6F: // XRL A, R7
		   ACC ^= int_ram[reg_base + (opcode - 0x68)];
		   break;
	case 0x70: // JNZ rel
		   tmp = *data; PC++; cycl++;
		   if (ACC)
		       jmp_rel (tmp);
		   break;
	case 0x72: // ORL C, bit
		   tmp = *data; PC++; cycl++;
		   tmp2 = get_bit (tmp);
		   set_carry (get_carry() | (int_ram[tmp2] & BIT_MASK (tmp)));
		   break;
	case 0x73: // JMP @A + DPTR
		   PC = (DPTRH << 8) + DPTRL + ACC; cycl++;
		   break;
	case 0x74: // MOV A, #data
		   ACC = *data; PC++;
		   break;
	case 0x75: // MOV direct, #data
		   tmp = *data; data++; tmp2 = *data; PC += 2; cycl++;
		   int_ram[tmp] = tmp2;
		   proc_write (tmp);
		   break;
	case 0x76: // MOV @R0, #data
	case 0x77: // MOV @R1, #data
		   tmp = get_indirect ((opcode == 0x66) ? R0 : R1);
		   tmp2 = *data; PC++;
		   int_ram[tmp] = tmp2;
		   proc_write (tmp);
		   break;
	case 0x78: // MOV R0, #data
	case 0x79: // MOV R1, #data
	case 0x7A: // MOV R2, #data
	case 0x7B: // MOV R3, #data
	case 0x7C: // MOV R4, #data
	case 0x7D: // MOV R5, #data
	case 0x7E: // MOV R6, #data
	case 0x7F: // MOV R7, #data
		   int_ram[reg_base + (opcode - 0x78)] = *data; PC++;
		   break;
	case 0x80: // SJMP rel
		   tmp = *data; PC++; cycl++;
		   jmp_rel (tmp);
		   break;
	case 0x82: // ANL C, bit
		   tmp = *data; PC++; cycl++;
		   tmp2 = get_bit (tmp);
		   set_carry (get_carry() & (int_ram[tmp2] & BIT_MASK (tmp)));
		   break;
	case 0x83: // MOVC A, @A + PC
		   ACC = memory->get_byte (ACC + PC);
		   cycl++;
		   break;
	case 0x84: // DIV AB
		   set_carry (0); cycl = 4;
		   if (B) {
			set_overflow (0);
			tmp = (ACC % B);
			ACC = (ACC / B);
			B = tmp;
		   } else {
			set_overflow (1);
		   }
		   break;
	case 0x85: // MOV direct, direct
		   tmp = *data; data++; tmp2 = *data; PC += 2; cycl++;
		   int_ram[tmp] = int_ram[tmp2];
		   proc_write (tmp);
		   break;
	case 0x86: // MOV direct, @R0
	case 0x87: // MOV direct, @R1
		   tmp = *data; PC++; cycl++;
		   tmp2 = get_indirect ((opcode == 0x86) ? R0 : R1);
		   int_ram[tmp] = tmp2;
		   proc_write (tmp);
		   break;
	case 0x88: // MOV direct, R0
	case 0x89: // MOV direct, R1
	case 0x8A: // MOV direct, R2
	case 0x8B: // MOV direct, R3
	case 0x8C: // MOV direct, R4
	case 0x8D: // MOV direct, R5
	case 0x8E: // MOV direct, R6
	case 0x8F: // MOV direct, R7
		   tmp = *data; PC++; cycl++;
		   int_ram[tmp] = int_ram[reg_base + (opcode - 0x88)];
		   proc_write (tmp);
		   break;
	case 0x90: // MOV DPTR, #data16
		   DPTRH = *data; data++; DPTRL = *data; PC += 2; cycl++;
		   break;
	case 0x92: // MOV bit, C
		   tmp = *data; PC++; cycl++;
		   tmp2 = get_bit (tmp);
		   if (get_carry()) {
		       int_ram[tmp2] |= BIT_MASK (tmp);
		   } else {
		       int_ram[tmp2] &= ~BIT_MASK (tmp);
		   }
		   proc_write (tmp2);
		   break;
	case 0x93: // MOVC A, @A + DPTR
		   ACC = memory->get_byte (ACC + (DPTRH << 8) + DPTRL);
		   cycl++;
		   break;
	case 0x94: // SUBB A, #data
		   tmp = *data; PC++;
		   subb (tmp);
		   break;
	case 0x95: // SUBB A, direct
		   tmp = *data; PC++;
		   subb (int_ram[tmp]);
		   break;
	case 0x96: // SUBB A, @R0
	case 0x97: // SUBB A, @R1
		   tmp = get_indirect ((opcode == 0x96) ? R0 : R1);
		   subb (int_ram[tmp]);
		   break;
	case 0x98: // SUBB A, R0
	case 0x99: // SUBB A, R1
	case 0x9A: // SUBB A, R2
	case 0x9B: // SUBB A, R3
	case 0x9C: // SUBB A, R4
	case 0x9D: // SUBB A, R5
	case 0x9E: // SUBB A, R6
	case 0x9F: // SUBB A, R7
		   subb (int_ram[reg_base + (opcode - 0x98)]);
		   break;
	case 0xA0: // ORL C, /bit
		   tmp = *data; PC++; cycl++;
		   tmp2 = get_bit (tmp);
		   set_carry ((get_carry() | (!(int_ram[tmp2] & BIT_MASK (tmp)))));
		   break;
	case 0xA2: // MOV C, bit
		   tmp = *data; PC++;
		   tmp2 = get_bit (tmp);
		   set_carry (int_ram[tmp2] & BIT_MASK (tmp));
		   break;
	case 0xA3: // INC DPTR
		   DPTRL++; cycl++;
		   if (DPTRL == 0) DPTRH++;
		   break;
	case 0xA4: // MUL AB
		   set_carry (0); cycl = 4;
		   tmpw = ACC * B;
		   ACC = (tmpw & 0xff); B = (tmpw >> 8);
	 	   set_overflow (B);
		   break;
	case 0xA5: // DBG (debugger command)
		   debugging = 1;
		   break;
	case 0xA6: // MOV @R0, direct
	case 0xA7: // MOV @R1, direct
		   tmp = ((opcode == 0xA6) ? R0 : R1);
		   tmp2 = *data; PC++; cycl++;
		   int_ram[tmp] = tmp2;
		   proc_write (tmp);
		   break;
	case 0xA8: // MOV R0, direct
	case 0xA9: // MOV R1, direct
	case 0xAA: // MOV R2, direct
	case 0xAB: // MOV R3, direct
	case 0xAC: // MOV R4, direct
	case 0xAD: // MOV R5, direct
	case 0xAE: // MOV R6, direct
	case 0xAF: // MOV R7, direct
		   tmp = *data; PC++; cycl++;
		   int_ram[reg_base + (opcode - 0xA8)] = tmp;
		   break;
	case 0xB0: // ANL C, /bit
		   tmp = *data; PC++; cycl++;
		   tmp2 = get_bit (tmp);
		   set_carry ((get_carry() & (!(int_ram[tmp2] & BIT_MASK (tmp)))));
		   break;
	case 0xB2: // CPL bit
		   tmp = *data; PC++;
		   tmp2 = get_bit (tmp);
		   if (int_ram[tmp2] & BIT_MASK (tmp)) {
		       int_ram[tmp2] &= ~BIT_MASK (tmp);
		   } else {
		       int_ram[tmp2] |= BIT_MASK (tmp);
		   }
		   proc_write (tmp2);
		   break;
	case 0xB3: // CPL C
		   set_carry (!get_carry());
		   break;
	case 0xB4: // CJNE A, #data, rel
	 	   tmp = *data; data++; tmp2 = *data; PC += 2; cycl++;
		   if (ACC != tmp)
		       jmp_rel (tmp2);
		   break;
	case 0xB5: // CJNE A, direct, rel
	 	   tmp = *data; data++; tmp2 = *data; PC += 2; cycl++;
		   if (ACC != int_ram[tmp])
		       jmp_rel (tmp2);
		   break;
	case 0xB6: // CJNE @R0, #data, rel
	case 0xB7: // CJNE @R1, #data, rel
		   tmp = get_indirect (int_ram[reg_base + (opcode - 0xB6)]);
		   tmp2 = *data; data++;
		   tmp3 = *data; PC += 2; cycl++;
		   if (tmp != tmp2)
		       jmp_rel (tmp3);
		   break;
	case 0xB8: // CJNE R0, #data, rel
	case 0xB9: // CJNE R1, #data, rel
	case 0xBA: // CJNE R2, #data, rel
	case 0xBB: // CJNE R3, #data, rel
	case 0xBC: // CJNE R4, #data, rel
	case 0xBD: // CJNE R5, #data, rel
	case 0xBE: // CJNE R6, #data, rel
	case 0xBF: // CJNE R7, #data, rel
		   tmp = int_ram[reg_base + (opcode - 0xB8)];
		   tmp2 = *data; data++;
		   tmp3 = *data; PC += 2; cycl++;
		   if (tmp != tmp2)
		       jmp_rel (tmp3);
		   break;
	case 0xC0: // PUSH direct
		   tmp = *data; PC++; cycl++;
		   push (int_ram[tmp]);
		   break;
	case 0xC2: // CLR bit
		   tmp = *data; PC++;
		   tmp2 = get_bit (tmp);
		   int_ram[tmp2] &= ~BIT_MASK (tmp);
		   proc_write (tmp2);
		   break;
	case 0xC3: // CLR C
		   set_carry (0);
		   break;
	case 0xC4: // SWAP A
		   tmp = ACC;
		   ACC = (tmp & 0xf) << 4 | (tmp >> 4);
		   break;
	case 0xC5: // XCH A, direct
		   tmp2 = ACC; tmp = *data; PC++;
		   ACC = int_ram[tmp];
		   int_ram[tmp] = tmp2;
		   proc_write (tmp);
		   break;
	case 0xC6: // XCH A, @R0
	case 0xC7: // XCH A, @R1
		   tmp2 = ACC; tmp = int_ram[reg_base + (opcode - 0xC6)];
		   ACC = int_ram[tmp];
		   int_ram[tmp] = tmp2;
		   proc_write (tmp);
		   break;
	case 0xC8: // XCH A, R0
	case 0xC9: // XCH A, R1
	case 0xCA: // XCH A, R2
	case 0xCB: // XCH A, R3
	case 0xCC: // XCH A, R4
	case 0xCD: // XCH A, R5
	case 0xCE: // XCH A, R6
	case 0xCF: // XCH A, R7
		   tmp2 = ACC; tmp = reg_base + (opcode - 0xC8);
		   ACC = int_ram[tmp];
		   int_ram[tmp] = tmp2;
		   break;
	case 0xD0: // POP direct
		   tmp = *data; PC++; cycl++;
		   int_ram[tmp] = pop();
		   proc_write (tmp);
		   break;
	case 0xD2: // SETB bit
		   tmp = *data; PC++;
		   tmp2 = get_bit (tmp);
		   int_ram[tmp2] |= BIT_MASK (tmp);
		   proc_write (tmp2);
		   break;
	case 0xD3: // SETB C
		   set_carry (1);
		   break;
	case 0xD4: // DA
		   if (((ACC & 0x0F) > 9) || (get_auxcarry())) {
			if (((unsigned int)ACC + (unsigned int)6) > 255)
			    set_carry (1);
			ACC += 0x06;
		   }
		   if (((ACC & 0xF0) > 0x90) || (get_auxcarry())) {
			if (((unsigned int)ACC + (unsigned int)0x60) > 255)
			    set_carry (1);
			ACC += 0x60;
	 	   }
		   break;
	case 0xD5: // DJNZ direct, rel
		   tmp = *data; data++; tmp2 = *data; PC += 2; cycl++;
		   int_ram[tmp]--;
		   if (int_ram[tmp])
		       jmp_rel (tmp2);
		   proc_write (tmp);
		   break;
	case 0xD6: // XCHD A, @R0
	case 0xD7: // XCHD A, @R1
		   tmp2 = ACC; tmp = int_ram[reg_base + (opcode - 0xC6)];
		   ACC = (ACC & 0xF0) | (int_ram[tmp] & 0x0F);
		   int_ram[tmp] = (int_ram[tmp] & 0xF0) | (tmp2 & 0x0F);
		   proc_write (tmp);
		   break;
	case 0xD8: // DJNZ R0, rel
	case 0xD9: // DJNZ R1, rel
	case 0xDA: // DJNZ R2, rel
	case 0xDB: // DJNZ R3, rel
	case 0xDC: // DJNZ R4, rel
	case 0xDD: // DJNZ R5, rel
	case 0xDE: // DJNZ R6, rel
	case 0xDF: // DJNZ R7, rel
		   tmp = reg_base + (opcode - 0xD8);
		   tmp2 = *data; PC++; cycl++;
		   int_ram[tmp]--;
		   if (int_ram[tmp])
		       jmp_rel (tmp2);
		   break;
	case 0xE0: // MOVX A, @DPTR
		   ACC = memory->get_byte ((DPTRH << 8) | DPTRL);
		   cycl++;
		   break;
	case 0xE2: // MOVX A, @R0
	case 0xE3: // MOVX A, @R1
		   tmp = get_indirect (int_ram[reg_base + (opcode - 0xE2)]);
		   ACC = memory->get_byte (tmp);
		   cycl++;
		   break;
	case 0xE4: // CLR A
		   ACC = 0;
		   break;
	case 0xE5: // MOV A, direct
		   ACC = *data; PC++;
		   break;
	case 0xE6: // MOV A, @R0
	case 0xE7: // MOV A, @R1
		   ACC = get_indirect ((opcode == 0xE6) ? R0 : R1);
		   break;
	case 0xE8: // MOV A, R0
	case 0xE9: // MOV A, R1
	case 0xEA: // MOV A, R2
	case 0xEB: // MOV A, R3
	case 0xEC: // MOV A, R4
	case 0xED: // MOV A, R5
	case 0xEE: // MOV A, R6
	case 0xEF: // MOV A, R7
		   ACC = int_ram[reg_base + (opcode - 0xE8)];
		   break;
	case 0xF0: // MOVX @DPTR, A
		   tmpw = (DPTRH << 8) | DPTRL;
		   memory->set_byte (tmpw, ACC); cycl++;
		   break;
	case 0xF2: // MOVX @R0, A
	case 0xF3: // MOVX @R1, A
		   tmp = get_indirect ((opcode == 0xF2) ? R0 : R1);
		   memory->set_byte (tmp, ACC); cycl++;
		   break;
	case 0xF4: // CPL A
		   ACC = ~ACC;
		   break;
	case 0xF5: // MOV direct, A
		   tmp = *data; PC++;
		   int_ram[tmp] = ACC;
		   proc_write (tmp);
		   break;
	case 0xF6: // MOV @R0, A
	case 0xF7: // MOV @R1, A
		   tmp = get_indirect ((opcode == 0xF6) ? R0 : R1);
		   int_ram[tmp] = ACC;
		   proc_write (tmp);
		   break;
	case 0xF8: // MOV R0, A
	case 0xF9: // MOV R1, A
	case 0xFA: // MOV R2, A
	case 0xFB: // MOV R3, A
	case 0xFC: // MOV R4, A
	case 0xFD: // MOV R5, A
	case 0xFE: // MOV R6, A
	case 0xFF: // MOV R7, A
		   int_ram[reg_base + (opcode - 0xF8)] = ACC;
		   break;
	  default: // ???
		   printf ("Unhandled opcode 0x%x, ignored\n", opcode);
    }

    // handle the Parity flag
    if (ACC & 1) {
	PSW |= 1;
    } else {
	PSW &= ~1;
    }
}
