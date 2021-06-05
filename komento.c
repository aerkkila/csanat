#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <strlista.h>
#include "asetelma.h"

#define knto(a) (!strcmp(knnot->str, a))
#define STREND(a) (a)[strlen(a)-1]

int avaa(const char* nimi);
void sekoita();
void seuraava_osaamaton();
void osaamaton();

extern char* tmpc;

void komento(const char* restrict suote) {
  strlista* knnot = _strpilko_sanoiksi(suote);
  if(!knnot)
    knnot = _strlisaa_kopioiden(knnot, "");
  while(knnot) {
    if(knto("cd") && knnot->seur) {
      knnot = _strpoista1(knnot, 1);
      chdir(knnot->str);
      
    } else if(knto("cd") || knto("cd;")) {
      chdir(kotihak);
      
    } else if(knto("ls") || knto("ls;")) {
      strcpy(tmpc, "/bin/ls");
      if(knto("ls") && knnot->seur) {
	int jatka = 1;
	while(knnot->seur && jatka) {
	  if(STREND(knnot->seur->str) == ';')
	    jatka = 0;
	  sprintf(tmpc+strlen(tmpc), " %s", knnot->seur->str);
	  knnot = _strpoista1(knnot, 1);
	}
      }
      FILE *p = popen(tmpc, "r");
      while(fgets(tmpc, maxpit_suote, p) > 0) {
	if(STREND(tmpc) == '\n')
	  STREND(tmpc) = '\0';
	viestiol.lista = _strlisaa_kopioiden(viestiol.lista, tmpc);
      }
      viestiol.lista = _yalkuun(viestiol.lista);
      pclose(p);

    } else if(knto("osaamisrajaksi") && knnot->seur) {
      if(sscanf(knnot->seur->str, "%u", &osaamisraja))
	knnot = _strpoista1(knnot,1);
      else
	goto VERTAILU;

    } else {
    VERTAILU:
      if(kysymysol.teksti) {
	/*verrataan käännökseen ja laitetaan uusi sana ja poistutaan*/
	kysytytol.lista = _strlisaa_kopioiden_taakse(kysytytol.lista, kysymysol.teksti);
	META.kierroksia++;
	if(!strcmp(kaan->str, suote)) {
	  apuvari = suoteol.vari;
	  suoteol.vari = oikeavari;
	  META.osattu++;
	} else {
	  strcpy(suoteol.teksti, kaan->str);
	  apuvari = suoteol.vari;
	  suoteol.vari = virhevari;
	}
	suoteviesti = 1;
	seuraava_osaamaton();
	knnot = _strpoista_kaikki(knnot);
	return;
      }
      
      /*lista on lopussa*/
      if(!strlen(knnot->str)) {
	SKM(_yalkuun);
	sekoita();
	osaamaton();
	goto JATKA;
      }
      if(avaa(knnot->str)) {
	sprintf(tmpc, "Ei avattu tiedostoa \"%s\"", knnot->str);
	viestiol.lista = _strlisaa_kopioiden(viestiol.lista, tmpc);
	goto JATKA;
      }
      /*tiedosto avattiin*/
      sekoita();
      SKM(_yalkuun);
      osaamaton();
      goto JATKA;
    }
  JATKA:
    kysytytol.lista = _yjatka_taakse(kysytytol.lista);
    knnot = _strpoista1(knnot, 1);
  }
}

inline void __attribute__((always_inline)) seuraava_osaamaton() {
  do {
    if(!sana->seur) {
      kysymysol.teksti = NULL;
      return;
    }
    SKMLIIKU(seur);
  } while(META.osattu >= osaamisraja);
  kysymysol.teksti = sana->str;
}

inline void __attribute__((always_inline)) osaamaton() {
  while(META.osattu >= osaamisraja) {
    if(!sana->seur) {
      kysymysol.teksti = NULL;
      return;
    }
    SKMLIIKU(seur);
  }
  kysymysol.teksti = sana->str;
}
