all : Clientudp Servudp biceps3

Clientudp : Clientudp.c Creme.c creme.h
	cc -Wall -Werror -o Clientudp Clientudp.c Creme.c

Servudp : Servudp.c creme.h
	cc -Wall -Werror -o Servudp Servudp.c

biceps3 : biceps_1.c Creme.c creme.h
	cc -Wall -Werror -o biceps3 biceps_1.c Creme.c -lreadline

clean :
	rm -f Clientudp Servudp biceps3