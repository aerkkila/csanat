tdstot=csanat.c asetelma1.c komento.c lista.c grafiikka.c io_tiedot.c
otsakkeet=asetelma.h grafiikka.h lista.h csanat.h
krjstot=-lSDL2 -lSDL2_ttf

all: csanat2

csanat: ${tdstot} ${otsakkeet}
	gcc -gdwarf-2 -g3 -Wall -o $@ ${tdstot} ${krjstot} -Og

csanat2: csanat2.c lista.c näkymä.c näkymä_conf.c näkymä_conf1.c csanat2.h lista.h modkeys.h näkymä.h toiminta.h
	gcc -Wall -o $@ csanat2.c lista.c näkymä.c -lSDL2 -lSDL2_ttf -g

näkymä_conf1.c: näkymä_conf.c
	./configure.sh $@

asetelma1.c: asetelma.c configure.sh
	./configure.sh $@
