/*
 * 8051 Emulator - p8051.h (Processor Emulation)
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
#ifndef __P8051_H__
#define __P8051_H__

#include "d8051.h"
#include "memory.h"

/* BIT_MASK(x) will calculate a bitmask for us */
#define BIT_MASK(x) (1 << (x & 0x07))

/* P8051_RAM_SIZE is the size of the processor's internal RAM, in bytes */
#define P8051_RAM_SIZE		256

/* P8051_REG_xxx are the memory register addresses */
#define P8051_REG_R0		0x00			/* reg 0 */
#define P8051_REG_R1		0x01			/* reg 1 */
#define P8051_REG_R2		0x02			/* reg 2 */
#define P8051_REG_R3		0x03			/* reg 3 */
#define P8051_REG_R4		0x04			/* reg 4 */
#define P8051_REG_R5		0x05			/* reg 5 */
#define P8051_REG_R6		0x06			/* reg 6 */
#define P8051_REG_R7		0x07			/* reg 7 */
#define P8051_REG_P0		0x80			/* port 0 */
#define P8051_REG_SP		0x81			/* stack pointer */
#define P8051_REG_DPL		0x82			/* data ptr, low */
#define P8051_REG_DPH		0x83			/* data ptr, hi */
#define P8051_REG_PCON		0x87			/* power control */
#define P8051_REG_TCON		0x88			/* timer control */
#define P8051_REG_TMOD		0x89			/* timer mode */
#define P8051_REG_TL0		0x8A			/* timer 0 low byte */
#define P8051_REG_TL1		0x8B			/* timer 0 low byte */
#define P8051_REG_TH0		0x8C			/* timer 0 low byte */
#define P8051_REG_TH1		0x8D			/* timer 0 low byte */
#define P8051_REG_P1		0x90			/* port 1 */
#define P8051_REG_SCON		0x98			/* serial control */
#define P8051_REG_SBUF		0x98			/* serial buffer */
#define P8051_REG_P2		0xA0			/* port 2 */
#define P8051_REG_IEN0		0xA8			/* int enable 0 */
#define P8051_REG_IP0		0xA9			/* int priority 0 */
#define P8051_REG_P3		0xB0			/* port 3 */
#define P8051_REG_IEN1		0xB8			/* int enable 1 */
#define P8051_REG_IP1		0xB9			/* int priority 1 */
#define P8051_REG_IRCON		0xC0			/* int req control */
#define P8051_REG_CCEN		0xC1			/* cc enable reg */
#define P8051_REG_CCL1		0xC2			/* cc reg 1 low */
#define P8051_REG_CCH1		0xC3			/* cc reg 1 hi */
#define P8051_REG_CCL2		0xC4			/* cc reg 2 low */
#define P8051_REG_CCH2		0xC5			/* cc reg 2 hi */
#define P8051_REG_CCL3		0xC6			/* cc reg 3 low */
#define P8051_REG_CCH3		0xC7			/* cc reg 3 hi */
#define P8051_REG_T2CON		0xC8			/* timer 2 control */
#define P8051_REG_CRCH		0xCA			/* cc reg low */
#define P8051_REG_CRCL		0xCB			/* cc reg hi */
#define P8051_REG_TL2		0xCC			/* timer 2 low */
#define P8051_REG_TH2		0xCD			/* timer 2 hi */
#define P8051_REG_PSW		0xD0			/* prog status word */
#define P8051_REG_ADCON		0xD8			/* a/d control reg */
#define P8051_REG_ADDAT		0xD9			/* a/d data reg */
#define P8051_REG_DAPR		0xDA			/* d/a program reg */
#define P8051_REG_P6		0xDB			/* port 6 */
#define P8051_REG_ACC		0xE0			/* accumulator */
#define P8051_REG_P4		0xE8			/* port 4 */
#define P8051_REG_B		0xF0			/* b */
#define P8051_REG_P5		0xF8			/* port 5 */

/* below are convenience macros */
#define P0			int_ram[P8051_REG_P0]
#define P1			int_ram[P8051_REG_P1]
#define P2			int_ram[P8051_REG_P2]
#define P3			int_ram[P8051_REG_P3]
#define P4			int_ram[P8051_REG_P4]
#define P5			int_ram[P8051_REG_P5]
#define P6			int_ram[P8051_REG_P6]
#define ACC			int_ram[P8051_REG_ACC]
#define SP			int_ram[P8051_REG_SP]
#define CCL1			int_ram[P8051_REG_CCL1]
#define CCH1			int_ram[P8051_REG_CCH1]
#define CCL2			int_ram[P8051_REG_CCL2]
#define CCH2			int_ram[P8051_REG_CCH2]
#define CCL3			int_ram[P8051_REG_CCL3]
#define CCH3			int_ram[P8051_REG_CCH3]
#define CCEN			int_ram[P8051_REG_CCEN]
#define TCON			int_ram[P8051_REG_TCON]
#define T2CON			int_ram[P8051_REG_T2CON]
#define ADCON			int_ram[P8051_REG_ADCON]
#define PCON			int_ram[P8051_REG_PCON]
#define B			int_ram[P8051_REG_B]
#define PC			pc
#define DPTRH			int_ram[P8051_REG_DPH]
#define DPTRL			int_ram[P8051_REG_DPL]
#define TL0			int_ram[P8051_REG_TL0]
#define TH0			int_ram[P8051_REG_TH0]
#define TL1			int_ram[P8051_REG_TL1]
#define TH1			int_ram[P8051_REG_TH1]
#define TL2			int_ram[P8051_REG_TL2]
#define TH2			int_ram[P8051_REG_TH2]
#define SCON			int_ram[P8051_REG_SCON]
#define TMOD			int_ram[P8051_REG_TMOD]
#define IEN0 			int_ram[P8051_REG_IEN0]
#define IEN1 			int_ram[P8051_REG_IEN1]
#define IP0 			int_ram[P8051_REG_IP0]
#define IP1 			int_ram[P8051_REG_IP1]
#define PSW 			int_ram[P8051_REG_PSW]
#define R0 			int_ram[P8051_REG_R0 + reg_base]
#define R1 			int_ram[P8051_REG_R1 + reg_base]
#define R2 			int_ram[P8051_REG_R2 + reg_base]
#define R3 			int_ram[P8051_REG_R3 + reg_base]
#define R4 			int_ram[P8051_REG_R4 + reg_base]
#define R5 			int_ram[P8051_REG_R5 + reg_base]
#define R6 			int_ram[P8051_REG_R6 + reg_base]
#define R7 			int_ram[P8051_REG_R7 + reg_base]

/*
 * P8051 is a 8051 processor.
 */
class P8051 {
public:
    P8051(MEMORY*,D8051*);

    void    reset();
    int     load(char*);
    int	    disasm (unsigned int, char*, int);
    void    go();

    unsigned char  get_acc();
    unsigned char  get_b();
    unsigned char  get_sp();
    unsigned int   get_pc();
    unsigned char  get_R0();
    unsigned char  get_R1();
    unsigned char  get_R2();
    unsigned char  get_R3();
    unsigned char  get_R4();
    unsigned char  get_R5();
    unsigned char  get_R6();
    unsigned char  get_R7();
    unsigned int   get_DPTR();

    void	   set_input(unsigned char);

private:
    MEMORY*	  memory;
    D8051*	  debugger;
    unsigned char int_ram[P8051_RAM_SIZE];

    unsigned short reg_base;
    unsigned short pc;

    unsigned char  get_indirect (unsigned char);
    void	   proc_write (unsigned char);

    void	   set_carry(int);
    void	   set_auxcarry(int);
    void	   set_parity(int);
    void	   set_overflow(int);
    void	   set_DPTR(unsigned int);

    int		   get_carry();
    int		   get_auxcarry();
    int		   get_overflow();
    int		   get_parity();

    void	   push (unsigned char);
    unsigned char  pop ();

    void	   add (unsigned char);
    void	   addc (unsigned char);
    void	   subb (unsigned char);

    void	   jmp_rel (unsigned char);
    unsigned char  get_bit (unsigned char);
};

extern		   int debugging;
#endif
