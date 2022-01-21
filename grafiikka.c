#include <stdlib.h>
#include <stdio.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include "asetelma.h"
#include "lista.h"
#include "grafiikka.h"
#include "csanat.h" //tarviiko

float skaala = 1.0;
extern SDL_Texture* tausta;
extern SDL_Renderer* rend;

static inline int __attribute__((always_inline)) leveys(TTF_Font* f, char c) {
  int lev;
  TTF_GlyphMetrics(f, c, NULL, NULL, NULL, NULL, &lev);
  return lev;
}
static inline void __attribute__((always_inline)) putsaa(tekstiolio_s* o) {
  SDL_RenderFillRect(rend, &o->toteutuma);
}

#define CASE(a) case a ## _enum
void paivita(unsigned laitot) {
  if(!laitot) {
    SDL_RenderCopy(rend,tausta,NULL,NULL);
    SDL_RenderPresent(rend);
    return;
  }
  aseta_vari(taustavari);
  SDL_SetRenderTarget(rend,tausta);
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
      aseta_vari(kohdistinvari);
      kohdistinsij.x = xsijainti(&suoteol, strlen(suoteol.teksti)-kohdistin);
      SDL_RenderFillRect(rend, &kohdistinsij);
      break;
    CASE(kaunti):
      laita_kaunti();
      break;
    CASE(tiedot):
      tee_tiedot();
      tiedotol.sij.x = kauntiol.toteutuma.x + kauntiol.toteutuma.w + 4*leveys(viestiol.font, ' ');
      tiedotol.sij.w = ikkuna_w - tiedotol.sij.x;
      laita_tekstilista(&tiedotol);
      break;
    CASE(viesti):
      viestiol.sij.x = kauntiol.toteutuma.x + kauntiol.toteutuma.w + 4*leveys(viestiol.font, ' ');
      viestiol.sij.y = tiedotol.toteutuma.y + tiedotol.toteutuma.h + TTF_FontLineSkip(viestiol.font);
      viestiol.sij.h = ikkuna_h - viestiol.sij.y;
      viestiol.sij.w = ikkuna_w - viestiol.sij.x;
      laita_tekstilista(&viestiol);
      break;
    }
  }
  SDL_SetRenderTarget(rend,NULL);
  SDL_RenderCopy(rend,tausta,NULL,NULL);
  laitot = 0;
  SDL_RenderPresent(rend);
}
#undef CASE

void laita_teksti_ttf(tekstiolio_s *o) {
  if(!o->teksti || !strcmp(o->teksti, "")) {
    o->toteutuma = (SDL_Rect){0,0,0,0};
    return;
  }
  SDL_Surface *pinta = TTF_RenderUTF8_Shaded(o->font, o->teksti, o->vari, tekstin_taustavari);
  if(!pinta) {
    fprintf(stderr, "Virhe tekstin luomisessa: %s\n", TTF_GetError());
    return;
  }
  SDL_Texture *ttuuri =  SDL_CreateTextureFromSurface(rend, pinta);
  if(!ttuuri)
    fprintf(stderr, "Virhe tekstuurin luomisessa: %s\n", SDL_GetError());

  /*tulostetaan vain se osa lopusta, joka mahtuu kuvaan*/
  o->toteutuma = (SDL_Rect){o->sij.x*skaala,				\
			    o->sij.y*skaala,				\
			    (pinta->w < o->sij.w)? pinta->w : o->sij.w, \
			    (pinta->h < o->sij.h)? pinta->h : o->sij.h};
  o->toteutuma.w *= skaala;
  o->toteutuma.h *= skaala;
  
  SDL_Rect osa = {(pinta->w < o->sij.w)? 0 : pinta->w - o->toteutuma.w, \
		  (pinta->h < o->sij.h)? 0 : pinta->h - o->toteutuma.h, \
		  pinta->w,						\
		  pinta->h};
  
  SDL_RenderCopy(rend, ttuuri, &osa, &o->toteutuma);
  fflush(stdout);
  SDL_FreeSurface(pinta);
  SDL_DestroyTexture(ttuuri);
}

int laita_tekstilista(tekstiolio_s *o) {
  lista* l = o->lista;
  if(!l) {
    o->toteutuma.w = 0;
    o->toteutuma.h = 0;
    return 0;
  }
  int rvali = TTF_FontLineSkip(o->font);
  int mahtuu = o->sij.h / rvali;
  int yht = l->pit - o->rullaus;
  int leveystot = 0;
  
  /*laitetaan niin monta jäsentä kuin mahtuu*/
  if(o->lopusta)
    o->alku = (yht > mahtuu)*(yht - mahtuu); //jos erotus on negatiivinen, kerrotaan 0:lla
  else
    o->alku = -o->rullaus;
  int oy = o->sij.y;
  int raja = (mahtuu < yht)? mahtuu : yht;
  for(int i=0; i<raja; i++) {
    o->teksti = *LISTALLA(l,char**,LISTA_ALUSTA,i+o->alku);
    laita_teksti_ttf(o);
    if(o->toteutuma.w > leveystot)
      leveystot = o->toteutuma.w;
    (o->sij.y) += rvali;
  }
  o->toteutuma.x = o->sij.x;
  o->toteutuma.y = oy;
  o->toteutuma.w = leveystot;
  o->toteutuma.h = o->sij.y - oy;
  o->sij.y = oy;
  return raja;
}


/*On vähän niin kuin laita_tekstilista, mutta kysynta_s-tietueelle, missä vastaukset laitetaan viereen.
  Lisäksi tämä laitetaan lopusta alkuun.*/
void laita_kaunti() {
  lista* l = kauntiol.lista;
  if(l->pit == 0)
    return;
  tekstiolio_s* o = &kauntiol;
  int rvali = TTF_FontLineSkip(o->font);
  int vali = leveys(o->font, ' ')*4;
  int mahtuu = o->sij.h / rvali;
  int yht = l->pit - o->rullaus;
  int leveystot = 0;

  if(suoteol.toteutuma.h)
    o->sij.y = suoteol.toteutuma.h + suoteol.toteutuma.y + rvali;
  int oy = o->sij.y;
  o->alku = yht-1;
  int alaraja = (yht-mahtuu)*(mahtuu<yht); //0, paitsi jos erotus ≥ 0

  /*kysynnät*/
  SDL_Color ovari = o->vari;
  o->vari = (SDL_Color){0,0,0,0};
  for(int i=o->alku; i>=alaraja; i--) {
    kysynta_s* kys = LISTALLA(l,kysynta_s*,LISTA_ALUSTA,i);
    if((o->teksti = kys->kysym)) {
      if(*kys->hetki>>31) //sana osattiin
	tekstin_taustavari = oikea_taustavari;
      else
	tekstin_taustavari = virhe_taustavari;
      laita_teksti_ttf(o);
      if(o->toteutuma.w > leveystot)
	leveystot = o->toteutuma.w;
    }
    (o->sij.y) += rvali;
  }
  tekstin_taustavari = taustavari;
  o->vari = ovari;

  int leveystot0 = leveystot;
  o->sij.y = oy;
  o->sij.x += leveystot + vali;
  
  /*syötteet*/
  for(int i=o->alku; i>=alaraja+1; i--) {
    o->teksti = LISTALLA(l,kysynta_s*,LISTA_ALUSTA,i)->suote;
    laita_teksti_ttf(o);
    if(o->toteutuma.w > leveystot)
      leveystot = o->toteutuma.w;
    (o->sij.y) += rvali;
  }

  o->sij.x = 0;
  o->sij.y = oy;
  
  o->toteutuma.x = o->sij.x;
  o->toteutuma.y = o->sij.y;
  o->toteutuma.w = leveystot0 + vali + leveystot;
  o->toteutuma.h = rvali * (o->alku*2-alaraja+2)/2;
}
