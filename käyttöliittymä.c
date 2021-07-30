#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <SDL2/SDL.h>
#include "lista.h"
#include "asetelma.h"
#include "menetelmiä.h"

void paivita();
void komento(const char* restrict suote);
void pyyhi(char* suote);

enum laitot_enum {
  kysymys_enum,
  suote_enum,
  kaunti_enum,
  tiedot_enum,
  viesti_enum,
  laitot_enum_pituus //lukuarvo kertoo pituuden
};

const unsigned kaikkilaitot =  0xffff;

unsigned laitot = 0;
extern SDL_Renderer* rend;
extern char* tmpc;

void kaunnista() {
  SDL_Event tapaht;
  SDL_StartTextInput();
  char* const suote = suoteol.teksti;
  char vaihto = 0;
  if(strlen(tmpc)) {
    strcpy(suote, tmpc);
    goto ENTER;
  }
  /*Mielestäni goto on helpommin luettava kuin while(1), jos ehdoton toistolause on pitkä*/
 TOISTOLAUSE:
  while(SDL_PollEvent(&tapaht)) {
    switch(tapaht.type) {
    case SDL_QUIT:
      return;
    case SDL_TEXTINPUT:
      if(suoteviesti) {
	suote[0] = '\0';
	suoteol.vari = apuvari;
	suoteviesti = 0;
      }
      strcat(suote, tapaht.text.text);
      laita(suote);
      break;
    case SDL_KEYDOWN:
      switch(tapaht.key.keysym.sym) {
      case SDLK_LSHIFT:
      case SDLK_RSHIFT:
	vaihto = 1;
	break;
      case SDLK_RETURN:
      case SDLK_KP_ENTER:
	if(vaihto) {
	  edellinen_osatuksi();
	  laita(kaunti);
	  tee_tiedot();
	  laita(tiedot);
	  break;
	}
	if(suoteviesti) {
	  suote[0] = '\0';
	  suoteol.vari = apuvari;
	  suoteviesti = 0;
	}
      ENTER:
	if(viestiol.lista)
	  viestiol.lista = tuhoa_lista(viestiol.lista);
	komento(suote);
	if(!suoteviesti)
	  suote[0] = '\0';
	laitot = kaikkilaitot;
	break;
      case SDLK_BACKSPACE:
	pyyhi(suote);
	laita(suote);
	break;
      case SDLK_ESCAPE:
	snsto->sij = snsto->pit;
	kysymysol.teksti = NULL;
	laita(kysymys);
	break;
      case SDLK_PAUSE:
	if(vaihto)
	  asm("int $0x03"); //jäljityspisteansa (breakpoint debuggerille)
	break;
      }
      break; //keydown
    case SDL_KEYUP:
      switch(tapaht.key.keysym.sym) {
      case SDLK_LSHIFT:
      case SDLK_RSHIFT:
	vaihto = 0;
	break;
      }
      break; //keyup 
    case SDL_WINDOWEVENT:
      switch(tapaht.window.event) {
      case SDL_WINDOWEVENT_RESIZED:
	ikkuna_w = tapaht.window.data1;
	ikkuna_h = tapaht.window.data1;
	laitot = kaikkilaitot;
	break;
      }
      break;
    } //tapaht.type
  } //pollEvent
  paivita();
  SDL_Delay(uniaika);
  goto TOISTOLAUSE;
}

inline int __attribute__((always_inline)) leveys(TTF_Font* f, char c) {
  int lev;
  TTF_GlyphMetrics(f, c, NULL, NULL, NULL, NULL, &lev);
  return lev;
}

inline void __attribute__((always_inline)) putsaa(tekstiolio_s* o) {
  aseta_vari(taustavari);
  SDL_RenderFillRect(rend, &o->toteutuma);
}

#define CASE(a) case a ## _enum

inline void __attribute__((always_inline)) paivita() {
  if(!laitot)
    return;
  /*putsataan kaikki kerralla ennen laittamista*/
  for(int i=0; i<laitot_enum_pituus; i++) {
    if( !((laitot >> i) & 0x01) )
      continue;
    switch(i) {
    CASE(kysymys):
      putsaa(&kysymysol);
      break;
    CASE(suote):
      putsaa(&suoteol);
      break;
    CASE(kaunti):
      putsaa(&kauntiol);
      break;
    CASE(tiedot):
      putsaa(&tiedotol);
      break;
    CASE(viesti):
      putsaa(&viestiol);
      break;
    }
  }
  for(int i=0; i<laitot_enum_pituus; i++) {
    if( !((laitot >> i) & 0x01) )
      continue;
    switch(i) {
    CASE(kysymys):
      laita_teksti_ttf(&kysymysol);
      break;
    CASE(suote):
      laita_teksti_ttf(&suoteol);
      break;
    CASE(kaunti):
      laita_kaunti();
      break;
    CASE(tiedot):
      tee_tiedot();
      tiedotol.sij.x = kauntiol.toteutuma.x + kauntiol.toteutuma.w + 4*leveys(viestiol.font, ' ');
      laita_tekstilista(&tiedotol);
      break;
    CASE(viesti):
      viestiol.sij.x = kauntiol.toteutuma.x + kauntiol.toteutuma.w + tiedotol.toteutuma.w + 8*leveys(viestiol.font, ' ');
      laita_tekstilista(&viestiol);
      break;
    }
  }
  laitot = 0;
  SDL_RenderPresent(rend);
}
#undef CASE

/*Kerralla pois koko utf8-merkki, joka voi sisältää monta tavua.*/
inline void __attribute__((always_inline)) pyyhi(char* suote) {
  int jatka = 1;
  int pit = strlen(suote);
  while(jatka && pit--) {
    jatka = (suote[pit] & 0xc0) == 0x80; //alkaako 10:lla
    suote[pit] = '\0';
  }
}

void viestiksi(const char* restrict s) {
  if(viestiol.lista)
    tuhoa_lista(viestiol.lista);
  viestiol.lista = alusta_lista(1);
  viestiol.lista->taul[0] = strdup(s);
  viestiol.lista->pit = 1;
  laita(viesti);
}
