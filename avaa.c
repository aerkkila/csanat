#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "lista.h"
#include "asetelma.h"

#define STRPAATE(a) ((a)[strlen(a)-1])
extern char* tmpc;

int avaa_tiedosto(const char* nimi) {
  static int tiedostonro = 0;
  
  FILE *f = fopen(nimi, "r");
  if (!f)
    return -1;

  int sanoja = 0;
  
  while (!feof(f)) {
    //rivinvaihto lopussa ja tyhjät rivit välissä ohitetaan
    if (fgetc(f) == '\n' || feof(f))
      continue;
    jatka_listaa(snsto, 3);
    fseek(f,-1,SEEK_CUR);
    if(!fgets(tmpc, maxpit_suote, f)) //sana
      break;
    STRPAATE(tmpc) = 0; //rivinvaihto pois lopusta
    snsto->taul[snsto->pit-3] = strdup(tmpc);
    
    if(!fgets(tmpc, maxpit_suote, f)) //käännös
      fprintf(stderr, "Virhe: Ei luettu käännöstä\n");
    if(STRPAATE(tmpc) == '\n')
      STRPAATE(tmpc) = 0;
    snsto->taul[snsto->pit-2] = strdup(tmpc);

    /*3:nteen tulee:
      (int)parin numero; (int)tiedoston numero; (int)montako kierrosta; (int)osaamisten määrä;
      (uint64)bittimaski osaamisista, viimeisin kierros vähiten merkitsevänä*/
    snsto->taul[snsto->pit-1] = calloc(24, 1);
    *(int*)(snsto->taul[snsto->pit-1]+0) = sanoja++;
    *(int*)(snsto->taul[snsto->pit-1]+4) = tiedostonro;
    /*loput ovat alussa nollia*/
  }
  fclose(f);
  return sanoja;
}

#define VAIHDA(a, b) do {			\
    char* apu = a;				\
    a = b;					\
    b = apu;					\
  } while(0)

void sekoita() {
  srand((unsigned)time(NULL));
  for(int jaljella=snsto->pit/3; jaljella>1; jaljella--) {
    int sij = rand() % jaljella;
    /*vaihdetaan sijainti ja viimeinen keskenään
      kolme peräkkäistä osoitinta kuuluvat aina yhteen*/
    for(int i=0; i<3; i++)
      VAIHDA(snsto->taul[jaljella*3-3+i], snsto->taul[sij*3+i]);
  }
}
