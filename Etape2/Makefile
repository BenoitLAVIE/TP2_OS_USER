all : Clientudp Servudp

Clientudp : Clientudp.c creme.h
	cc -Wall -Werror -o Clientudp Clientudp.c
Serverudp_etape2 : Servudp.c
	cc -Wall -Werror -o Servudp Servudp.c
clean :
	rm -f Clientudp Servudp