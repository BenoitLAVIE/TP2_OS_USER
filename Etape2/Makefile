all : Clientudp Servudp

Clientudp : Clientudp.c 
	cc -Wall -Werror -o Clientudp Clientudp.c
Servudp : Servudp.c
	cc -Wall -Werror -o Servudp Servudp.c
clean :
	rm -f Clientudp Servudp