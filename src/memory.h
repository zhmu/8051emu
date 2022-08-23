/*
 * 8051 Emulator - memory.h (Processor Memory)
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
#ifndef __MEMORY_H__
#define __MEMORY_H__

/* MEMORY_SIZE is the size of the processor's external RAM, in bytes */
#define MEMORY_SIZE		65535

/*
 * MEMORY is the 8051's external memory.
 */
class MEMORY {
public:
    MEMORY();

    void	set_byte (unsigned int, unsigned char);
    unsigned char get_byte (unsigned int);
    unsigned char* get_memory();

private:
    unsigned char data[MEMORY_SIZE];
};

#endif
