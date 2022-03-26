tdstot=csanat.c lista.c
otsakk=lista.h modkeys.h toiminta.h

csanat: ${tdstot} näkymä.c näkymä_conf.c näkymä_conf1.c csanat.h ${otsakk}
	gcc -Wall -o $@ ${tdstot} -lSDL2 -lSDL2_ttf -gdwarf-2 -g3

näkymä_conf1.c: näkymä_conf.c
	./configure.sh $@
