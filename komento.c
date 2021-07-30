#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include "lista.h"
#include "asetelma.h"

#define knto(a) (!strcmp(*NYT_OLEVA(knnot), a))
#define STREND(a) (a)[strlen(a)-1]
#define EI_LOPUSSA(lista) (lista->sij+1 < lista->pit)
#define SEURAAVA(lista)  (lista->taul[lista->sij+1])
#define VIIMEINEN(l) ((l)->taul[(l)->pit-1])
#define TOISEKSI_VIIM (kysynnat->taul[kysynnat->pit-2])

int avaa_tiedosto(const char*);
void sekoita();
void osaamaton();
lista* pilko_sanoiksi(const char* restrict str);

extern char* tmpc;

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
	strcpy(TOISEKSI_VIIM, kysymysol.teksti); //nullin jälkeen tulee osaattiinko
	VIIMEINEN(kysynnat) = strdup(suote);
	*SANAN_OSAAMISET <<= 1;
	if(!strcmp(NYT_OLEVA(snsto)[1], suote)) {
	  apuvari = suoteol.vari;
	  suoteol.vari = oikeavari;
	  *SANAN_OSAAMISET += 1;
	  (*OSAAMISKERRAT)++;
	  TOISEKSI_VIIM[strlen(TOISEKSI_VIIM)+1] = 0x01;
	} else {
	  strcpy(suoteol.teksti, *(NYT_OLEVA(snsto)+1));
	  apuvari = suoteol.vari;
	  suoteol.vari = virhevari;
	  TOISEKSI_VIIM[strlen(TOISEKSI_VIIM)+1] = 0x00;
	}
	(*KIERROKSIA_SANALLA)++;
	suoteviesti = 1;
	snsto->sij+=3;
	osaamaton();
        knnot = tuhoa_lista(knnot);
	return;
      }
      
      /*sanasto on lopussa*/
      if(!strlen(*NYT_OLEVA(knnot))) {
	snsto->sij = 0;
	sekoita();
	osaamaton();
        knnot->sij++;
	continue;
      }
      if(avaa_tiedosto(*NYT_OLEVA(knnot)) < 0) {
	fprintf(stderr, "Ei avattu tiedostoa \"%s\"\n", *NYT_OLEVA(knnot));
	sprintf(tmpc, "Ei avattu tiedostoa \"%s\"", *NYT_OLEVA(knnot));
	if(viestiol.lista)
	  tuhoa_lista(viestiol.lista);
	viestiol.lista = alusta_lista(1);
	viestiol.lista->taul[0] = strdup(tmpc);
	viestiol.lista->pit = 1;
	knnot->sij++;
	continue;
      }
      /*tiedosto avattiin*/
      sekoita();
      osaamaton();
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

lista* pilko_sanoiksi(const char* restrict str) {
  const char* osoit = str;
  lista* r = alusta_lista(2);
  while(sscanf(osoit, "%s", tmpc) == 1) {
    while((unsigned char)osoit[0] <= 0x20)
      osoit++; //välien yli ensin
    osoit += strlen(tmpc);
    /*välien yli*/
    jatka_listaa(r, 1);
    VIIMEINEN(r) = strdup(tmpc);
  }
  if(r->pit == 0) { //myös tyhjästä syötteestä tehdään lista
    jatka_listaa(r, 1);
    VIIMEINEN(r) = strdup("");
  }
  return r;
}

void edellinen_osatuksi() {
  if(snsto->sij < 3)
    return;
  snsto->sij -= 3;
  if(!(*SANAN_OSAAMISET & 0x01)) {
    (*OSAAMISKERRAT)++;
    *SANAN_OSAAMISET += 1;
  }
  snsto->sij += 3;
  TOISEKSI_VIIM[strlen(TOISEKSI_VIIM)+1] = 0x01;
}
