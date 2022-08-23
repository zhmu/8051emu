/*
 * 8051 Emulator - main.cc ("The Glue that Binds")
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
#include "p8051.h"
#include "d8051.h"
#include "memory.h"

P8051* p;
D8051* d;
MEMORY* m;

int debugging = 0;

/*
 * main(int argc, char** argv)
 *
 * This will launch the emulator.
 *
 */
int
main(int argc, char** argv) {
    char bla[512];

    m = new MEMORY();
    d = new D8051 ();
    p = new P8051 (m, d);

    if (argc != 2) {
	printf ("Usuage: 8051emu [file.hex]\n");
	return 1;
    }

    if (p->load (argv[1]) < 0) {
	printf ("unable to load '%s'\n", argv[1]);
	return 1;
    }

    /* memory dump */
    /*printf ("XDATA dump\n");
    for (i = 0xB5B; i < 0xB5F; i++) {
	printf ("%02x ", m->get_byte (i));
    }
    printf ("\n");*/

    /*pos = 0x860; l = 0;
    while (++l < 25) {
	i = p->disasm (pos, bla, 512);
	printf ("0x%04x: ", pos);

	for (j = pos; j < (pos + i); j++)
	    printf ("%02x ", (unsigned int)*((unsigned char*)m->get_memory() + j));
	j = 4 - i;
	while (j-- > 0)
	    printf ("   ");

	printf ("%s\n", bla);
	pos += i;
    }*/

    while (1) {
        p->go();

	if (debugging) {

	printf (">> A = 0x%02x B = 0x%02x SP = 0x%02x PC = 0x%04x DPTR = 0x%04x\n",
		p->get_acc(), p->get_b(), p->get_sp(), p->get_pc(), p->get_DPTR());
	printf (">> R0 = 0x%02x, R1 = 0x%02x, R2 = 0x%02x, R3 = 0x%02x, R4 = 0x%02x, R5 = 0x%02x, R6 = 0x%02x, R7 = 0x%02x\n", p->get_R0(), p->get_R1(), p->get_R2(), p->get_R3(), p->get_R4(), p->get_R5(), p->get_R6(), p->get_R7());
	fgets (bla, sizeof (bla), stdin);
	}
    }

    delete m; delete p; delete d;

    // bye bye
    return EXIT_SUCCESS;
}
