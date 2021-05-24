#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <strlista.h>
#include "asetelma.h"

#define knto(a) (!strcmp(knnot->str, a))
#define STREND(a) (a)[strlen(a)-1]

extern char* tmpc;

void komento(char* suote) {
  strlista* knnot = _strpilko_sanoiksi(suote);
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
      viestiol.lista = _strpoista_kaikki(_yalkuun(viestiol.lista));
      while(fgets(tmpc, maxpit_suote, p) > 0) {
	if(STREND(tmpc) == '\n')
	  STREND(tmpc) = '\0';
	viestiol.lista = _strlisaa_kopioiden(viestiol.lista, tmpc);
      }
      viestiol.lista = _yalkuun(viestiol.lista);
      pclose(p);
    }
    knnot = _strpoista1(knnot, 1);
  }
}
