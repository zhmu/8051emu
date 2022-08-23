/*
 * 8051 Emulator - memory.cc (Processor External Memory)
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
#include <string.h>
#include "memory.h"

/*
 * MEMORY::MEMORY()
 *
 * This will initialize the processor's memory.
 *
 */
MEMORY::MEMORY() {
    // zero it all out
    bzero (&data, MEMORY_SIZE);
}

/*
 * MEMORY::set_byte (unsigned int offs, unsigned char b)
 *
 * This will change the byte at location [offs] to [b].
 *
 */
void
MEMORY::set_byte (unsigned int offs, unsigned char b) {
    data[offs] = b;
}

/*
 * MEMORY::get_byte (unsigned int offs)
 *
 * This will return the byte at location [offs].
 *
 */
unsigned char
MEMORY::get_byte (unsigned int offs) {
    return data[offs];
}

/*
 * MEMORY::get_memory()
 *
 * This will return the memory buffer itself.
 *
 */
unsigned char*
MEMORY::get_memory() {
    return data;
}
