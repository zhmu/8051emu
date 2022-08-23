This is my 8051 emulator; during a course at my first year of college, we had
to write code for this thing. The only downside is that we used antique versions
of the Keil software (in fact, we had the Windows 3.1 versions!) which were
a PAIN to use.

Therefore, I made my own emulator. As you can see, it can nicely run PAULMON2,
a very good free monitor). I've left it with this code so you can play with
this; mind you: serial input is not finished!

Anyway, I never really benchmarked this stuff but I reckon it's kind of fast.
I wrote this in a week during a holiday, so I never really optimized it. Still,
the processor core is only slightly about 1000 lines, which is kind of neat.

Feel free to (ab)use this in your own projects; a 4-CPU 8051 simulator would
be quite neat (and possible with this code :-) This code works fine on my
FreeBSD machine.

Enjoy!
