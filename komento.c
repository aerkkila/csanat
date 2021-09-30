#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include "lista.h"
#include "asetelma.h"
#include "menetelmiä.h"

#define knto(a) (!strcmp(*NYT_OLEVA(knnot), #a))
#define STREND(a) (a)[strlen(a)-1]
#define EI_LOPUSSA(lista) (lista->sij+1 < lista->pit)
#define SEURAAVA(lista)  (lista->taul[lista->sij+1])

extern char* tmpc;
static void avaa(lista*);

void komento(const char* restrict suote) {
  lista* knnot = pilko_sanoiksi(suote);
  int snsto_eiole_lopussa = snsto->sij < snsto->pit; // tämä talteen, koska sijainti muuttuu avattaessa
  while(knnot->sij < knnot->pit) {
    
    if(knto(cd) && EI_LOPUSSA(knnot)) {                           //cd
      knnot->sij++;
      if(chdir(*NYT_OLEVA(knnot)))
	TEE("Virhe: %s", strerror(errno));
    } else if(knto(cd) || knto(cd;)) {
      if(chdir(kotihak))
	TEE("Virhe: %s", strerror(errno));
      
    } else if(knto(ls) || knto(ls;)) {                            //ls
      strcpy(tmpc, "/bin/ls");
      if(knto(ls) && EI_LOPUSSA(knnot)) {
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

    } else if(knto(.osaamisrajaksi) && EI_LOPUSSA(knnot)) {       //osaamisrajaksi
      if(sscanf(SEURAAVA(knnot), "%u", &osaamisraja))
	knnot->sij++;
      else
	break;
      
    } else if(knto(.tyhjennä) || knto(thj)) {                     //tyhjennä
      tuhoa_lista(snsto);
      snsto = alusta_lista(11*3);
      kysymysol.teksti = NULL;

    } else if(knto(.uudesti)) {                                   //uudesti
      for(snsto->sij=0; snsto->sij<snsto->pit; snsto->sij+=3) {
	*OSAAMISKERRAT = 0;
	*KIERROKSIA_SANALLA = 0;
	*SANAN_OSAAMISET = 0;
      }
      uusi_kierros();

    } else if(knto(.korjaa) && EI_LOPUSSA(knnot)) {               //korjaa
      int taakse;
      knnot->sij++;
      if(sscanf(*NYT_OLEVA(knnot), "%i", &taakse) != 1 || !(EI_LOPUSSA(knnot))) {
	viestiksi("Käyttö: .korjaa n_taakse korjaus");
	break;
      }
      knnot->sij++;
      int sij = snsto->sij-taakse/2*3 + taakse%2;
      if(!(0 <= sij && sij < snsto->pit)) {
	VIESTIKSI("Virheellinen sijainti (%i)", sij);
	break;
      }
      sprintf(tmpc, "Korjattiin paikallisesti \"%s\" --> ", snsto->taul[sij]);
      free(snsto->taul[sij]);
      snsto->taul[sij] = strdup(*NYT_OLEVA(knnot));
      sprintf(tmpc+strlen(tmpc), "\"%s\"", snsto->taul[sij]);
      viestiksi(tmpc);

    } else if(knto(.avaa)) {                                      //avaa
      if(knnot->sij == knnot->pit-1) {
	viestiksi("Käyttö .avaa tiedoston_nimi");
	break;
      }
      knnot->sij++;
      avaa(knnot);

    } else if(knto(.käännä)) {                                    //käännä
      for(int i=0; i<snsto->pit; i+=3)
	VAIHDA(snsto->taul[i], snsto->taul[i+1], char*);

    } else if(knto(.tallenna)) {                                  //tallenna
      tallenna();
      
    } else {
      if(snsto_eiole_lopussa) {
	/*verrataan käännökseen, laitetaan uusi sana ja poistutaan*/
	jatka_listaa(kysynnat, 2);
	TOISEKSI_VIIM = malloc(strlen(kysymysol.teksti) + 2);
	strcpy(TOISEKSI_VIIM, kysymysol.teksti); //nollatavun jälkeen tulee osaattiinko
	*VIIMEINEN(kysynnat) = strdup(suote);
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
        knnot->sij++;
	uusi_kierros();
	continue;
      }
      avaa(knnot);
    }
    knnot->sij++;
  }
  knnot = tuhoa_lista(knnot);
}

static void avaa(lista* knnot) {
  int _avaa = avaa_tiedosto(*NYT_OLEVA(knnot));
  if(_avaa < 0) {
    TEE("Ei avattu tiedostoa \"%s\"", *NYT_OLEVA(knnot));
    return;
  } else if(_avaa == 0)
    TEE("Varoitus: tiedosto \"%s\" avattiin mutta yhtään sanaa ei luettu", *NYT_OLEVA(knnot));    
  /*tiedosto avattiin*/
  sekoita();
  osaamaton();
  edellinen_sij = -1;
}
