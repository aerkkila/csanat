tdstot=csanat.c asetelma1.c komento.c lista.c grafiikka.c io_tiedot.c
otsakkeet=asetelma.h grafiikka.h lista.h csanat.h
krjstot=-lSDL2 -lSDL2_ttf

csanat: ${tdstot} ${otsakkeet}
	gcc -gdwarf-2 -g3 -Wall -o $@ ${tdstot} ${krjstot} -Og

asetelma1.c: asetelma.c configure.sh
	./configure.sh $@
