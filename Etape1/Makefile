all : clientudp Servudp

clientudp : clientudp.c
	cc -Wall -Werror -o clientudp clientudp.c
Servudp : Servudp.c
	cc -Wall -Werror -o Servudp Servudp.c

clean :
	rm -f clientudp Servudp