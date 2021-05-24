#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <SDL2/SDL.h>
#include <strlista.h>
#include <tekstigraf.h>
#include "asetelma.h"

void paivita();
void laita_suote();
void komento(char* suote);
void pyyhi(char* suote);

unsigned laitot;
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
	laita_suote();
	break;
      case SDL_KEYDOWN:
	switch(tapaht.key.keysym.sym) {
	case SDLK_RETURN:
	case SDLK_KP_ENTER:
	  komento(suote);
	  suote[0] = '\0';
	  laita_suote();
	  break;
	case SDLK_BACKSPACE:
	  pyyhi(suote);
	  laita_suote();
	  break;
	}
	break; //keydown
      } //tapaht.type
    } //pollEvent
    paivita();
    SDL_Delay(1);
  } //while 1
}

#define SUOTE 0x01

inline void __attribute__((always_inline)) laita_suote() {
  laitot |= SUOTE;
}

#define PYYHI(olio) SDL_RenderFillRect(rend, olio.toteutuma)

inline void __attribute__((always_inline)) paivita() {
  if(!laitot)
    return;
  SDL_SetRenderDrawColor(rend, 0,0,0,255);
  if(laitot & SUOTE)
    PYYHI(suoteol);
  if(laitot & SUOTE) {
    laita_teksti_ttf(&suoteol, rend);
    laitot &= ~SUOTE;
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
