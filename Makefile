tdstot=csanat.c asetelma1.c komento.c lista.c grafiikka.c io_tiedot.c
otsakkeet=asetelma.h grafiikka.h lista.h csanat.h
krjstot=-lSDL2 -lSDL2_ttf

all: csanat2

csanat: ${tdstot} ${otsakkeet}
	gcc -gdwarf-2 -g3 -Wall -o $@ ${tdstot} ${krjstot} -Og

asetelma1.c: asetelma.c configure.sh
	./configure.sh $@

tdstot2=csanat2.c lista.c
otsakk2=lista.h modkeys.h toiminta.h

csanat2: ${tdstot2} näkymä.c näkymä_conf.c näkymä_conf1.c csanat2.h ${otsakk2}
	gcc -Wall -o $@ ${tdstot2} -lSDL2 -lSDL2_ttf -gdwarf-2 -g3

näkymä_conf1.c: näkymä_conf.c
	./configure.sh $@
