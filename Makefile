tdstot=main.c käyttöliittymä.c asetelma.c komento.c avaa.c lista.c grafiikka.c
otsakkeet=asetelma.h grafiikka.h lista.h
krjstot=-lSDL2 -lSDL2_ttf

sanat: ${tdstot} ${otsakkeet}
	gcc -gdwarf-2 -g3 -Wall -o sanat ${tdstot} ${incdir} ${libdir} ${krjstot} -O1
