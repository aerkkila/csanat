#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <SDL2/SDL.h>
#include <strlista.h>
#include <tekstigraf.h>
#include "asetelma.h"

void paivita();
void komento(const char* restrict suote);
void pyyhi(char* suote);

enum laitot_enum {
  kysymys_enum,
  suote_enum,
  kysynta_enum,
  annetut_enum,
  viesti_enum,
  laitot_enum_pituus //lukuarvo kertoo pituuden
};

const unsigned kaikkilaitot =  0xffff;
#define laita(jotain) (laitot |= (1u << jotain ## _enum))

unsigned laitot;
strlista* annetut=NULL;
extern SDL_Renderer* rend;

void kaunnista() {
  /*testi*/
  kysyntaol.lista = _yalkuun(_strlistaksi("joo;ei;1;2;54алудв", ";"));
  kysyntaol.lista = _yloppuun(_yjatka_taakse(kysyntaol.lista));
  kysymysol.teksti = kysyntaol.lista->str;
  SDL_Event tapaht;
  SDL_StartTextInput();
  laitot=kaikkilaitot;
  char* const suote = suoteol.teksti;
  while(1) {
    while(SDL_PollEvent(&tapaht)) {
      switch(tapaht.type) {
      case SDL_QUIT:
	_strpoista_kaikki(_yalkuun(kysyntaol.lista));
	return;
      case SDL_TEXTINPUT:
	strcat(suote, tapaht.text.text);
	laita(suote);
	break;
      case SDL_KEYDOWN:
	switch(tapaht.key.keysym.sym) {
	case SDLK_RETURN:
	case SDLK_KP_ENTER:
	  _strlisaa_kopioiden(annetutol.lista, suote);
	  komento(suote);
	  suote[0] = '\0';
	  laitot = kaikkilaitot;
	  break;
	case SDLK_BACKSPACE:
	  pyyhi(suote);
	  laita(suote);
	  break;
	}
	break; //keydown
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

void laita_ttuurit(int n, ...) {
  tekstiolio_s* ol;
  ylista* tex;
  ylista* osa;
  ylista* tot;
  va_list ap;
  va_start(ap, n);
  for(int i=0; i<n; i++) {
    ol = va_arg(ap, tekstiolio_s*);
    tex = ol->ttuurit;
    osa = ol->osat;
    tot = ol->totmat;
    while(osa) {
      SDL_RenderCopy(rend, tex->p, osa->p, tot->p);
      tex=tex->edel;
      osa=osa->edel;
      tot=tot->edel;
    }
  }
  va_end(ap);
}

#define CASE(a) case a ## _enum

inline void __attribute__((always_inline)) paivita() {
  if(!laitot)
    return;
  SDL_SetRenderDrawColor(rend, tv.r, tv.g, tv.b, tv.a);
  SDL_RenderClear(rend);
  for(int i=0; i<laitot_enum_pituus; i++) {
    if( !((laitot >> i) & 0x01) )
      continue;
    switch(i) {
    CASE(kysymys):
      Poista_ttuurit(kysymys);
      laita_teksti_ttf(&kysymysol, rend);
      break;
    CASE(suote):
      Poista_ttuurit(suote);
      suoteol.sij->y = kysymysol.toteutuma->y+kysymysol.toteutuma->h;
      laita_teksti_ttf(&suoteol, rend);
      break;
    CASE(kysynta):
      Poista_ttuurit(kysynta);
      laita_alle(&suoteol, 0, kysyntaol.lista->seur, &kysyntaol, rend);
      break;
    CASE(annetut):
      Poista_ttuurit(annetut);
      int vali;
      TTF_GlyphMetrics(kysyntaol.font, ' ', NULL, NULL, NULL, NULL, &vali);
      annetutol.sij->y = kysyntaol.toteutuma->y;
      laita_oikealle(&kysyntaol, 4*vali, annetutol.lista->seur, annetutol.lopusta, &annetutol, rend);
      break;
    CASE(viesti):
      Poista_ttuurit(viesti);
      laita_tekstilista(viestiol.lista, viestiol.lopusta, &viestiol, rend);
      break;
    }
  }
  laitot = 0;
  laita_ttuurit(laitot_enum_pituus, &kysymysol, &suoteol, &kysyntaol, &annetutol, &viestiol);
  SDL_RenderPresent(rend);
}
#undef CASE

/*Koko utf8-merkki pois kerralla. Voi sisältää monta tavua.*/
inline void __attribute__((always_inline)) pyyhi(char* suote) {
  int jatka = 1;
  int pit = strlen(suote);
  while(jatka && pit--) {
    jatka = ((suote[pit] & 0xc0) == 0x80)? 1 : 0; //alkaako 10:lla: 0xc0 poimii, 0x80 vertaa
    suote[pit] = '\0';
  }
}
