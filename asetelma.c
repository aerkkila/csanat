#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <stdlib.h>
#include "lista.h"
#include "grafiikka.h"
#include "asetelma.h"
#include "csanat.h"

const int ikkuna_x0 = 150;
const int ikkuna_y0 = 200;
const int ikkuna_w0 = 600;
const int ikkuna_h0 = 400;
const char* ohjelman_nimi = "csanat";
const char* aloituskomentotiedosto = "aloituskomennot.txt";

int ikkuna_x;
int ikkuna_y;
int ikkuna_w;
int ikkuna_h;

const unsigned uniaika = 10;
const unsigned maxpit_suote = 2048;

const SDL_Color taustavari       = {0,  0,  0,  255};
const SDL_Color oikeavari        = {20, 255,20, 255};
const SDL_Color virhevari        = {255,20, 20, 255};
const SDL_Color oikea_taustavari = {20, 255,60, 255};
const SDL_Color virhe_taustavari = {255,80, 50, 255};
const SDL_Color kohdistinvari    = {255,255,255,255};
SDL_Color tekstin_taustavari; // = taustavari
SDL_Color apuvari;
int suoteviesti = 0;

static const char* paafontti = "MAKE_LIITÄ_SERIFFONTTI";

#define VALKOINEN {255,255,255,255}
tekstiolio_s suoteol =   {.vari=VALKOINEN,.fonttikoko=60};
tekstiolio_s kysymysol = {.vari=VALKOINEN};
tekstiolio_s kauntiol =  {.vari=VALKOINEN,.fonttikoko=20};
tekstiolio_s tiedotol =  {.vari=VALKOINEN,.fonttikoko=20,.sij={.x=200}};
tekstiolio_s viestiol =  {.vari=VALKOINEN,.fonttikoko=17,.sij={.x=320},.lopusta=1};

lista* snsto;
lista* kysynnat;
lista* tiedostot;
int osaamisraja = 1;
int kohdistin = 0;
SDL_Rect kohdistinsij;
int alussa = 0;
int edellinen_sij = -1;

static void avaa_fontti(tekstiolio_s* olio) {
  if(!olio->fonttied)
    olio->fonttied = paafontti;
  olio->font = TTF_OpenFont(olio->fonttied, olio->fonttikoko);
  if(!olio->font) {
    fprintf(stderr, "Ei avattu fonttia \"%s\"\n%s\n", olio->fonttied, TTF_GetError());
    exit(1);
  }
}

void asetelma() {
  snsto     = alusta_lista(11,snsto_s);
  kysynnat  = alusta_lista(11,kysynta_s);
  tiedostot = alusta_lista(1,char**);
  
  avaa_fontti(&suoteol);
  avaa_fontti(&kauntiol);
  avaa_fontti(&viestiol);
  avaa_fontti(&tiedotol);
  kysymysol.font = suoteol.font;

  #define RIVEJA 3 //tiedoissa on kolme riviä
  suoteol.teksti = calloc(maxpit_suote, 1);
  kauntiol.lista = kysynnat;
  tiedotol.lista = alusta_lista(RIVEJA,char**);
  jatka_listaa(tiedotol.lista, RIVEJA);
  for(int i=0; i<RIVEJA; i++)
    *LISTALLA(tiedotol.lista,char**,i) = malloc(32);
  #undef RIVEJA
  
  tekstin_taustavari = taustavari;
}

void aseta_sijainnit() {
  suoteol.sij.y   = TTF_FontLineSkip(kysymysol.font);
  kauntiol.sij.y  = suoteol.sij.y + TTF_FontLineSkip(suoteol.font);
  tiedotol.sij.y  = kauntiol.sij.y;
  
  suoteol.sij.w   = ikkuna_w - suoteol.sij.x;
  kysymysol.sij.w = ikkuna_w - kysymysol.sij.x;
  viestiol.sij.w  = ikkuna_w - viestiol.sij.x;
  tiedotol.sij.w  = ikkuna_w - tiedotol.sij.x;
  kauntiol.sij.w  = ikkuna_w - kauntiol.sij.x;
  
  kysymysol.sij.h = ikkuna_h - TTF_FontLineSkip(suoteol.font);
  suoteol.sij.h   = ikkuna_h - kysymysol.toteutuma.h;
  viestiol.sij.h  = ikkuna_h;
  kauntiol.sij.h  = ikkuna_h - kauntiol.sij.y;
  tiedotol.sij.h  = ikkuna_h;
}

void saada_kohdistin() {
  kohdistinsij.y = suoteol.sij.y;
  kohdistinsij.h = TTF_FontLineSkip(suoteol.font);
  TTF_GlyphMetrics(suoteol.font, ' ', NULL, NULL, NULL, NULL, &kohdistinsij.w);
  kohdistinsij.w /= 10;
  if(!kohdistinsij.w)
    kohdistinsij.w = 1;
}
