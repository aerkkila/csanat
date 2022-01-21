#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <time.h>
#include "lista.h"
#include "asetelma.h"
#include "csanat.h"

#define TAMA_KOMENTO (*LISTALLA(knnot,char**,LISTA_ALUSTA,knnot->sij))
#define knto(a) (!strcmp(TAMA_KOMENTO, #a))
#define EI_LOPUSSA(lista) (lista->sij+1 < lista->pit)

extern char* tmpc;
static void avaa(lista*);
static void lue(lista*);
void tallenna();
void avaa_sanoja(int);

void komento(const char* restrict suote) {
  lista* knnot = pilko_sanoiksi(suote);
  int snsto_eiole_lopussa = snsto->sij < snsto->pit; // tämä talteen, koska sijainti muuttuu avattaessa
  while(knnot->sij < knnot->pit) {
    
    if(knto(cd) && EI_LOPUSSA(knnot)) {                           //cd
      knnot->sij++;
      if(chdir(TAMA_KOMENTO))
	TEE("Virhe: %s", strerror(errno));
    } else if(knto(cd) || knto(cd;)) {
      if(chdir(getenv("HOME")))
	TEE("Virhe: %s", strerror(errno));
      
    } else if(knto(ls) || knto(ls;)) {                            //ls
      strcpy(tmpc, "/bin/ls");
      if(knto(ls) && EI_LOPUSSA(knnot)) {
	int jatka = 1;
	do {
	  knnot->sij++;
	  if(*STRPAATE(TAMA_KOMENTO) == ';')
	    jatka = 0;
	  sprintf(tmpc+strlen(tmpc), " %s", TAMA_KOMENTO);
	} while(EI_LOPUSSA(knnot) && jatka);
      }
      FILE *p = popen(tmpc, "r");
      if(viestiol.lista)
	tuhoa_lista2(viestiol.lista);
      viestiol.lista = alusta_lista(16,char**);
      while(fgets(tmpc, maxpit_suote, p) > 0) {
	if(*STRPAATE(tmpc) == '\n')
	  *STRPAATE(tmpc) = '\0';
	jatka_listaa(viestiol.lista, 1);
	*LISTALLA(viestiol.lista,char**,LISTA_LOPUSTA,-1) = strdup(tmpc);
	viestiol.lista->sij++;
      }
      pclose(p);

    } else if(knto(.osaamisrajaksi) && EI_LOPUSSA(knnot)) {       //osaamisrajaksi
      if(sscanf(*LISTALLA(knnot,char**,LISTA_ALUSTA,knnot->sij+1), "%u", &osaamisraja))
	knnot->sij++;
      else
	break;
      
    } else if(knto(.tyhjennä) || knto(thj)) {                     //tyhjennä
      for(int i=0; i<snsto->pit; i++)
	tuhoa_lista(LISTALLA(snsto,snsto_s*,LISTA_ALUSTA,i)->meta.hetket);
      tuhoa_lista(snsto);
      snsto = alusta_lista(11,snsto_s);
      kysymysol.teksti = NULL;

    } else if(knto(.uudesti)) {                                   //uudesti
      for(snsto->sij=0; snsto->sij<snsto->pit; snsto->sij+=3) {
	LISTALLA(snsto,snsto_s*,LISTA_ALUSTA,snsto->sij)->meta.kierroksia = 0;
	LISTALLA(snsto,snsto_s*,LISTA_ALUSTA,snsto->sij)->meta.osaamisia = 0;
	LISTALLA(snsto,snsto_s*,LISTA_ALUSTA,snsto->sij)->meta.hetket->pit=0;
      }
      uusi_kierros();

    } else if(knto(.korjaa) && EI_LOPUSSA(knnot)) {               //korjaa
      int taakse,montako;
      char ab;
      knnot->sij++;
      montako = sscanf(TAMA_KOMENTO, "%i%[ab]", &taakse, &ab);
      if(montako == 1) {
	knnot->sij++;
	montako = sscanf(TAMA_KOMENTO, "%[ab]", &ab);
      }
      if( montako < 1 || !(EI_LOPUSSA(knnot)) ) {
	viestiksi("Käyttö: .korjaa n_taakse a/b(=sana/käännös) korjaus");
	break;
      }
      knnot->sij++;
      int sij = snsto->sij-taakse;
      if(!(0 <= sij && sij < snsto->pit)) {
	VIESTIKSI("Virheellinen sijainti (%i) väliltä [0,%i[", sij, snsto->pit);
	break;
      }
      char** paikka = LISTALLA(snsto,char**,LISTA_ALUSTA,sij);
      sprintf(tmpc, "Korjattiin paikallisesti \"%s\" --> ", *paikka);
      free(*paikka);
      *paikka = strdup(TAMA_KOMENTO);
      sprintf(tmpc+strlen(tmpc), "\"%s\"", *paikka);
      viestiksi(tmpc);

    } else if(knto(.lue)) {                                       //lue
      if(knnot->sij == knnot->pit-1) {
	viestiksi("Käyttö .lue tiedoston_nimi");
	break;
      }
      knnot->sij++;
      lue(knnot);

    } else if(knto(.avaa)) {                                      //avaa
      if(knnot->sij == knnot->pit-1) {
	viestiksi("Käyttö .avaa n_kpl");
	break;
      }
      knnot->sij++;
      avaa(knnot);

    } else if(knto(.käännä)) {                                    //käännä
      for(int i=0; i<snsto->pit; i++) {
	snsto_s* sns = LISTALLA(snsto,snsto_s*,LISTA_ALUSTA,i);
	VAIHDA(sns->sana, sns->kaan, char*);
      }

    } else if(knto(.tallenna)) {                                  //tallenna
      tallenna();
      
    } else {
      if(snsto_eiole_lopussa) {
	/*verrataan käännökseen, laitetaan uusi sana ja poistutaan*/
	jatka_listaa(kysynnat, 1);
	LISTALLA(kysynnat,kysynta_s*,LISTA_LOPUSTA,-1)->kysym = strdup(kysymysol.teksti);
	LISTALLA(kysynnat,kysynta_s*,LISTA_LOPUSTA,-1)->suote = strdup(suote);
	lista* osaam = LISTALLA(snsto,snsto_s*,LISTA_ALUSTA,snsto->sij)->meta.hetket;
	jatka_listaa(osaam,1);
	*LISTALLA(osaam,uint32_t*,LISTA_LOPUSTA,-1) = time(NULL);
	LISTALLA(kysynnat,kysynta_s*,LISTA_LOPUSTA,-1)->hetki = LISTALLA(osaam,uint32_t*,LISTA_LOPUSTA,-1);
	LISTALLA(snsto,snsto_s*,LISTA_ALUSTA,snsto->sij)->meta.kierroksia++;
	if( !strcmp(LISTALLA(snsto,snsto_s*,LISTA_ALUSTA,snsto->sij)->kaan, suote) ) { //osattiin
	  apuvari = suoteol.vari;
	  suoteol.vari = oikeavari;
	  *LISTALLA(osaam,uint32_t*,LISTA_LOPUSTA,-1) |= 1<<31;
	  LISTALLA(snsto,snsto_s*,LISTA_ALUSTA,snsto->sij)->meta.osaamisia++;
	} else {
	  strcpy(suoteol.teksti, LISTALLA(snsto,snsto_s*,LISTA_ALUSTA,snsto->sij)->kaan);
	  apuvari = suoteol.vari;
	  suoteol.vari = virhevari;
	  *LISTALLA(osaam,uint32_t*,LISTA_LOPUSTA,-1) <<= 1;
	  *LISTALLA(osaam,uint32_t*,LISTA_LOPUSTA,-1) >>= 1;
	}
	suoteviesti = 1;
	edellinen_sij = snsto->sij;
	snsto->sij++;
	osaamaton();
        knnot = tuhoa_lista2(knnot);
	return;
      }
      
      /*sanasto on lopussa*/
      if(!strlen(TAMA_KOMENTO) && snsto->pit) {
        knnot->sij++;
	uusi_kierros();
	continue;
      }
      lue(knnot);
    }
    knnot->sij++;
  }
  knnot = tuhoa_lista2(knnot);
}

static void avaa(lista* knnot) {
  int kpl;
  if(!sscanf(TAMA_KOMENTO, "%i", &kpl)) {
    VIESTIKSI("Ei luettu sanojen määrää, argumentti = %s", TAMA_KOMENTO);
    jatka_listaa(viestiol.lista,1);
    *LISTALLA(viestiol.lista,char**,LISTA_LOPUSTA,-1) = strdup("Tiedosto avataan komennolla \".lue\"");
    return;
  }
  avaa_sanoja(kpl);
  sekoita();
  osaamaton();
  edellinen_sij = -1;
}

static void lue(lista* knnot) {
  int _lue = lue_tiedosto(TAMA_KOMENTO);
  if(_lue < 0) {
    TEE("Ei luettu tiedostoa \"%s\"", TAMA_KOMENTO);
    return;
  } else if(_lue == 0)
    TEE("Varoitus: tiedosto \"%s\" avattiin mutta yhtään sanaa ei luettu", TAMA_KOMENTO);    
  /*tiedosto avattiin*/
  sekoita();
  osaamaton();
  edellinen_sij = -1;
}
