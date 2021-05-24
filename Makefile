tdstot=main.c käyttöliittymä.c asetelma.c komento.c
headerit=asetelma.h
incdir=-I/home/antterkk/c/include
libdir=-L/home/antterkk/c/kirjastot
krjstot=-lSDL2 -lSDL2_ttf -llistat -ltekstigraf

sanat: ${tdstot} ${headerit}
	gcc -gdwarf-2 -g3 -Wall -o sanat ${tdstot} ${incdir} ${libdir} ${krjstot}
