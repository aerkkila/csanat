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
void ennen_komentoa();

const unsigned kaikkilaitot =  0xffff;

unsigned laitot = 0xffff;
extern SDL_Renderer* rend;
extern char* tmpc;
static char* apusuote = NULL; //syöte tallennetaan tähän, kun nuolilla selataan aiempia syötteitä

void kaunnista() {
  SDL_Event tapaht;
  char* const suote = suoteol.teksti;
  SDL_StartTextInput();
  char vaihto = 0;
  if(strlen(tmpc)) { //alussa komentoriviargumentit
    strcpy(suote, tmpc); //pilko_sanoiksi käyttää tmpc-muuttujaa
    komento(suote);
    suote[0] = '\0';
  }
  /*Mielestäni goto TOISTOLAUSE on tässä selkeämpi kuin while(1)*/
 TOISTOLAUSE:
  while(SDL_PollEvent(&tapaht)) {
    switch(tapaht.type) {
    case SDL_QUIT:
      if(apusuote) {free(apusuote); apusuote=NULL;}
      return;
    case SDL_TEXTINPUT:
      if(suoteviesti) {
	suote[0] = '\0';
	suoteol.vari = apuvari;
	suoteviesti = 0;
      }
      if(!kohdistin)
	strcat(suote, tapaht.text.text);
      else {
	char* s = suote+strlen(suote)-kohdistin;
	strcpy(tmpc, s); //loppuosa talteen
	strcpy(s, tapaht.text.text); //syötetty teksti
	strcat(suote, tmpc); //loppuosa perään
      }
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
	ennen_komentoa();
	komento(suote);
	if(!suoteviesti)
	  suote[0] = '\0';
	laitot = kaikkilaitot;
	break;
      case SDLK_BACKSPACE:
	pyyhi(suote);
	laita(suote);
	break;
      case SDLK_DELETE:
	if(seuraava_kohta(suote, &kohdistin))
	  pyyhi(suote);
	laita(suote);
	break;
      case SDLK_ESCAPE:
	snsto->sij = snsto->pit;
	tee_tiedot();
	laita(tiedot);
	kysymysol.teksti = NULL;
	laita(kysymys);
	break;
      case SDLK_DOWN:
	if(suoteviesti) {
	  suoteviesti = 0;
	  suoteol.vari = apuvari;
	  laita(suote);
	}
	if(!kysynnat->pit)
	  break;
	if(!apusuote) { //ensimmäinen painallus
	  if(!(apusuote = strdup(suote)))
	    printf("Varoitus: apusyötettä ei alustettu\n");
	  kysynnat->sij = kysynnat->pit-1; //kysynnat->sij on tavallisesti määrittelemätön
	} else if(kysynnat->sij < 3)
	  break;
	else
	  kysynnat->sij -= 2;
	strcpy(suote, *NYT_OLEVA(kysynnat));
	laita(suote);
	break;
      case SDLK_UP:
	if(suoteviesti) {
	  suoteviesti = 0;
	  suoteol.vari = apuvari;
	  laita(suote);
	}
	if(!apusuote) //ollaan jo uudessa syötteessä
	  break;
	if(kysynnat->sij == kysynnat->pit-1) { //tullaan uuteen syötteeseen
	  strcpy(suote, apusuote);
	  free(apusuote);
	  apusuote = NULL;
	  laita(suote);
	  break;
	}
	kysynnat->sij += 2;
	if(kysynnat->sij >= kysynnat->pit)
	  printf("Varoitus: kysyntöjen sijainti on %i ja pituus on %i\n", kysynnat->sij, kysynnat->pit);
	strcpy(suote, *NYT_OLEVA(kysynnat));
	laita(suote);
	break;
      case SDLK_LEFT:
	edellinen_kohta(suote, &kohdistin);
	laita(suote);
	break;
      case SDLK_RIGHT:
	seuraava_kohta(suote, &kohdistin);
	laita(suote);
	break;
      case SDLK_HOME:
	kohdistin = strlen(suote);
	laita(suote);
	break;
      case SDLK_END:
	kohdistin = 0;
	laita(suote);
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
    } //switch tapaht.type
  } //while pollEvent
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
  SDL_RenderFillRect(rend, &o->toteutuma);
}

#define CASE(a) case a ## _enum

inline void __attribute__((always_inline)) paivita() {
  if(!laitot)
    return;
  aseta_vari(taustavari);
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
      SDL_RenderFillRect(rend, &kohdistinsij);
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
      if(kohdistin) {
	aseta_vari(kohdistinvari);
	kohdistinsij.x = xsijainti(&suoteol, strlen(suoteol.teksti)-kohdistin);
	SDL_RenderFillRect(rend, &kohdistinsij);
      }
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

inline void __attribute__((always_inline)) pyyhi(char* suote) {
  int pit = strlen(suote);
  int id0 = kohdistin;
  strcpy(tmpc, suote+pit-kohdistin);
  edellinen_kohta(suote, &kohdistin);
  strcpy(suote+pit-kohdistin, tmpc);
  kohdistin = id0;
}

void ennen_komentoa() {
  kohdistin = 0;
  if(apusuote) {
    free(apusuote);
    apusuote = NULL;
  }
  if(suoteviesti) {
    suoteol.teksti[0] = '\0';
    suoteol.vari = apuvari;
    suoteviesti = 0;
  }
  if(viestiol.lista)
    viestiol.lista = tuhoa_lista(viestiol.lista);
}
