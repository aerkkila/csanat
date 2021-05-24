#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <SDL2/SDL.h>
#include <strlista.h>
#include <tekstigraf.h>
#include "asetelma.h"

void paivita();
void laita_suote();
void laita_annetut();
void laita_viesti();
void komento(char* suote);
void pyyhi(char* suote);

#define SUOTE   0x01
#define ANNETUT 0x02
#define VIESTI  0x04
#define KAIKKIL 0x07

#define laita(joku) laitot |= joku

unsigned laitot;
strlista* annetut=NULL;
extern SDL_Renderer* rend;

void kaunnista() {
  SDL_Event tapaht;
  SDL_StartTextInput();
  laitot=0x00;
  char* const suote = suoteol.teksti;
  while(1) {
    while(SDL_PollEvent(&tapaht)) {
      switch(tapaht.type) {
      case SDL_QUIT:
	return;
      case SDL_TEXTINPUT:
	strcat(suote, tapaht.text.text);
	laita(SUOTE);
	break;
      case SDL_KEYDOWN:
	switch(tapaht.key.keysym.sym) {
	case SDLK_RETURN:
	case SDLK_KP_ENTER:
	  _strlisaa_kopioiden(annetutol.lista, suote);
	  komento(suote);
	  suote[0] = '\0';
	  laita(KAIKKIL);
	  break;
	case SDLK_BACKSPACE:
	  pyyhi(suote);
	  laita(SUOTE);
	  break;
	}
	break; //keydown
      } //tapaht.type
    } //pollEvent
    paivita();
    SDL_Delay(1);
  } //while 1
}

#define PYYHI(olio) SDL_RenderFillRect(rend, olio.toteutuma)

inline void __attribute__((always_inline)) paivita() {
  if(!laitot)
    return;
  SDL_SetRenderDrawColor(rend, tv.r, tv.g, tv.b, tv.a);
  if(laitot & SUOTE)
    PYYHI(suoteol);
  if(laitot & ANNETUT)
    PYYHI(annetutol);
  if(laitot & VIESTI)
    PYYHI(viestiol);
  if(laitot & SUOTE) {
    laita_teksti_ttf(&suoteol, rend);
    laitot &= ~SUOTE;
  }
  if(laitot & ANNETUT) {
    laita_alle(&suoteol, 0, annetutol.lista->seur, &annetutol, rend);
    laitot &= ~ANNETUT;
  }
  if(laitot & VIESTI) {
    laita_tekstilista(viestiol.lista, viestiol.lopusta, &viestiol, rend);
    laitot &= ~VIESTI;
  }
  SDL_RenderPresent(rend);
}

/*Koko utf8-merkki pois kerralla. Voi sisältää monta tavua.*/
inline void __attribute__((always_inline)) pyyhi(char* suote) {
  int jatka = 1;
  int pit = strlen(suote);
  while(jatka && pit--) {
    jatka = ((suote[pit] & 0xc0) == 0x80)? 1 : 0; //alkaako 10:lla: 0xc0 poimii, 0x80 vertaa
    suote[pit] = '\0';
  }
}
