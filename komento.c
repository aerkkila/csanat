#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "lista.h"
#include "asetelma.h"

#define NYT_OLEVA(l) ((l)->taul+(l)->sij)
#define knto(a) (!strcmp(*NYT_OLEVA(knnot), a))
#define STREND(a) (a)[strlen(a)-1]
#define EI_LOPUSSA(lista) (lista->sij+1 < lista->pit)
#define SEURAAVA(lista)  (lista->taul[lista->sij+1])
#define VIIMEINEN(l) ((l)->taul[(l)->pit-1])

#define KIERROKSIA_SANALLA ((int*)( NYT_OLEVA(snsto)[2] )+2)
#define OSAAMISKERRAT (KIERROKSIA_SANALLA+1)
#define SANAN_OSAAMISET ((long long unsigned*)(KIERROKSIA_SANALLA+2))

int avaa_tiedosto(const char*);
void sekoita();
void seuraava_osaamaton();
void osaamaton();
lista* pilko_sanoiksi(const char* restrict str);

extern char* tmpc;

void komento(const char* restrict suote) {
  lista* knnot = pilko_sanoiksi(suote);
  while(knnot) {
    if(knnot->sij == knnot->pit)
      break;
    if(knto("cd") && EI_LOPUSSA(knnot)) {
      knnot->sij++;
      chdir(*NYT_OLEVA(knnot));
      
    } else if(knto("cd") || knto("cd;")) {
      chdir(kotihak);
      
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
	(&VIIMEINEN(kysynnat))[-1] = strdup(kysymysol.teksti);
	VIIMEINEN(kysynnat) = strdup(suote);
	*SANAN_OSAAMISET <<= 1;
	if(!strcmp(*NYT_OLEVA(snsto), suote)) {
	  apuvari = suoteol.vari;
	  suoteol.vari = oikeavari;
	  *SANAN_OSAAMISET += 1;
	  (*OSAAMISKERRAT)++;
	} else {
	  strcpy(suoteol.teksti, *(NYT_OLEVA(snsto)+1));
	  apuvari = suoteol.vari;
	  suoteol.vari = virhevari;
	}
	(*KIERROKSIA_SANALLA)++;
	suoteviesti = 1;
	seuraava_osaamaton();
        knnot = tuhoa_lista(knnot);
	return;
      }
      
      /*lista on lopussa*/
      if(!strlen(*NYT_OLEVA(knnot))) {
	snsto->sij = 0;
	sekoita();
	osaamaton();
        knnot->sij++;
	continue;
      }
      if(avaa_tiedosto(*NYT_OLEVA(knnot)) < 0) {
	sprintf(tmpc, "Ei avattu tiedostoa \"%s\"", *NYT_OLEVA(knnot));
	if(viestiol.lista)
	  tuhoa_lista(viestiol.lista);
	viestiol.lista = alusta_lista(1);
	viestiol.lista->taul[0] = strdup(tmpc);
	knnot->sij++;
	continue;
      }
      /*tiedosto avattiin*/
      sekoita();
      osaamaton();
      knnot->sij++;
    }
  }
  if(knnot)
    knnot = tuhoa_lista(knnot);
}

inline void __attribute__((always_inline)) seuraava_osaamaton() {
  do {
    snsto->sij++;
    if(snsto->sij == snsto->pit) {
      kysymysol.teksti = NULL;
      return;
    }
  } while(*OSAAMISKERRAT >= osaamisraja);
  kysymysol.teksti = *NYT_OLEVA(snsto);
}

inline void __attribute__((always_inline)) osaamaton() {
  while(*OSAAMISKERRAT >= osaamisraja) {
    if(snsto->sij == snsto->pit) {
      kysymysol.teksti = NULL;
      return;
    }
    snsto->sij++;
  }
  kysymysol.teksti = *NYT_OLEVA(snsto);
}

lista* pilko_sanoiksi(const char* restrict str) {
  const char* osoit = str;
  lista* r = alusta_lista(2);
  while(sscanf(osoit, "%s", tmpc) == 1) {
    osoit += strlen(tmpc);
    jatka_listaa(r, 1);
    VIIMEINEN(r) = strdup(tmpc);
  }
  return r;
}
