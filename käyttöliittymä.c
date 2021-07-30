#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <SDL2/SDL.h>
#include "lista.h"
#include "asetelma.h"

void paivita();
void komento(const char* restrict suote);
void edellinen_osatuksi();
void pyyhi(char* suote);
void seis() {;};

enum laitot_enum {
  kysymys_enum,
  suote_enum,
  kaunti_enum,
  viesti_enum,
  laitot_enum_pituus //lukuarvo kertoo pituuden
};

const unsigned kaikkilaitot =  0xffff;
#define laita(jotain) (laitot |= (1u << jotain ## _enum))

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
  while(1) {
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
	  seis();
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
  } //while 1
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
  for(int i=0; i<laitot_enum_pituus; i++) {
    if( !((laitot >> i) & 0x01) )
      continue;
    switch(i) {
    CASE(kysymys):
      putsaa(&kysymysol);
      laita_teksti_ttf(&kysymysol);
      break;
    CASE(suote):
      putsaa(&suoteol);
      suoteol.sij.y = kysymysol.toteutuma.y+kysymysol.toteutuma.h;
      laita_teksti_ttf(&suoteol);
      break;
    CASE(kaunti):
      putsaa(&kauntiol);
      laita_kaunti();
      break;
    CASE(viesti):
      putsaa(&viestiol);
      viestiol.sij.y = suoteol.toteutuma.y + suoteol.toteutuma.h;
      viestiol.sij.x = kauntiol.toteutuma.x + kauntiol.toteutuma.w + 4*leveys(viestiol.font, ' ');
      laita_tekstilista(viestiol.lista, viestiol.lopusta, &viestiol);
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
