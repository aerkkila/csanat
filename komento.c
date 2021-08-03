#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include "lista.h"
#include "asetelma.h"
#include "menetelmiä.h"

#define knto(a) (!strcmp(*NYT_OLEVA(knnot), a))
#define STREND(a) (a)[strlen(a)-1]
#define EI_LOPUSSA(lista) (lista->sij+1 < lista->pit)
#define SEURAAVA(lista)  (lista->taul[lista->sij+1])

void osaamaton();

extern char* tmpc;
int alussa = 0;
int edellinen_sij = -1;

void komento(const char* restrict suote) {
  lista* knnot = pilko_sanoiksi(suote);
  while(knnot->sij < knnot->pit) {
    if(knto("cd") && EI_LOPUSSA(knnot)) {
      knnot->sij++;
      if(chdir(*NYT_OLEVA(knnot)))
	fprintf(stderr, "Virhe: %s\n", strerror(errno));
      
    } else if(knto("cd") || knto("cd;")) {
      if(chdir(kotihak))
	fprintf(stderr, "Virhe: %s\n", strerror(errno));
      
    } else if(knto("ls") || knto("ls;")) {
      strcpy(tmpc, "/bin/ls");
      if(knto("ls") && EI_LOPUSSA(knnot)) {
	int jatka = 1;
	do {
	  knnot->sij++;
	  if(STREND(*NYT_OLEVA(knnot)) == ';')
	    jatka = 0;
	  sprintf(tmpc+strlen(tmpc), " %s", *NYT_OLEVA(knnot));
	} while(EI_LOPUSSA(knnot) && jatka);
      }
      FILE *p = popen(tmpc, "r");
      if(viestiol.lista)
	tuhoa_lista(viestiol.lista);
      viestiol.lista = alusta_lista(32);
      while(fgets(tmpc, maxpit_suote, p) > 0) {
	if(STREND(tmpc) == '\n')
	  STREND(tmpc) = '\0';
	jatka_listaa(viestiol.lista, 1);
	*NYT_OLEVA(viestiol.lista) = strdup(tmpc);
	viestiol.lista->sij++;
      }
      pclose(p);

    } else if(knto("osaamisrajaksi") && EI_LOPUSSA(knnot)) {
      if(sscanf(SEURAAVA(knnot), "%u", &osaamisraja))
	knnot->sij++;
      else
	goto VERTAILU;
      
    } else {
    VERTAILU:
      if(kysymysol.teksti) {
	/*verrataan käännökseen, laitetaan uusi sana ja poistutaan*/
	jatka_listaa(kysynnat, 2);
	TOISEKSI_VIIM = malloc(strlen(kysymysol.teksti) + 2);
	strcpy(TOISEKSI_VIIM, kysymysol.teksti); //nollatavun jälkeen tulee osaattiinko
	VIIMEINEN(kysynnat) = strdup(suote);
	*SANAN_OSAAMISET <<= 1;
	if(!strcmp(NYT_OLEVA(snsto)[1], suote)) { //osattiin
	  apuvari = suoteol.vari;
	  suoteol.vari = oikeavari;
	  *SANAN_OSAAMISET += 1;
	  (*OSAAMISKERRAT)++;
	  TOISEKSI_VIIM[strlen(TOISEKSI_VIIM)+1] = 0x01; //kysyntään merkintä tämän kierroksen osaamisesta
	} else {
	  strcpy(suoteol.teksti, *(NYT_OLEVA(snsto)+1));
	  apuvari = suoteol.vari;
	  suoteol.vari = virhevari;
	  TOISEKSI_VIIM[strlen(TOISEKSI_VIIM)+1] = 0x00;
	}
	(*KIERROKSIA_SANALLA)++;
	suoteviesti = 1;
	edellinen_sij = snsto->sij;
	snsto->sij+=3;
	osaamaton();
        knnot = tuhoa_lista(knnot);
	return;
      }
      
      /*sanasto on lopussa*/
      if(!strlen(*NYT_OLEVA(knnot))) {
	edellinen_sij = snsto->pit-3;
	snsto->sij = 0;
	sekoita();
	osaamaton();
	alussa = 1;
        knnot->sij++;
	continue;
      }
      int _avaa = avaa_tiedosto(*NYT_OLEVA(knnot));
      if(_avaa < 0) {
	fprintf(stderr, "Ei avattu tiedostoa \"%s\"\n", *NYT_OLEVA(knnot));
	sprintf(tmpc, "Ei avattu tiedostoa \"%s\"", *NYT_OLEVA(knnot));
	knnot->sij++;
	continue;
      } else if(_avaa == 0) {
	printf("Varoitus: tiedosto \"%s\" avattiin mutta yhtään sanaa ei luettu\n", *NYT_OLEVA(knnot));
      }
      /*tiedosto avattiin*/
      sekoita();
      osaamaton();
      edellinen_sij = -1;
      knnot->sij++;
    }
    knnot->sij++;
  }
  knnot = tuhoa_lista(knnot);
}

inline void __attribute__((always_inline)) osaamaton() {
  while(snsto->sij < snsto->pit) {
    if( *OSAAMISKERRAT < osaamisraja) {
      kysymysol.teksti = *NYT_OLEVA(snsto);
      return;
    }
    snsto->sij+=3;
  }
  kysymysol.teksti = NULL;
}
