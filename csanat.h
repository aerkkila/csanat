#ifndef __menetelmia_h__
#define __menetelmia_h__

#include "lista.h"
#include "grafiikka.h"
extern char* tmpc;
extern SDL_Window *ikkuna;
extern SDL_Texture* tausta;
extern SDL_Renderer *rend;

#define STRPAATE(mjon) (mjon+strlen(mjon)-1)
#define VAIHDA(a, b, tyyppi) do {	\
    tyyppi MAKRO_APU_VAIHDA = a;	\
    a = b;				\
    b = MAKRO_APU_VAIHDA;		\
  } while(0)
#define TEE(...) do {		       	\
    sprintf(tmpc, __VA_ARGS__);		\
    puts(tmpc);				\
    viestiksi(tmpc);			\
  } while(0)
#define VIESTIKSI(...) do {		\
    sprintf(tmpc, __VA_ARGS__);		\
    viestiksi(tmpc);			\
  } while(0)
#define aseta_vari(v) SDL_SetRenderDrawColor(rend, v.r, v.g, v.b, v.a)
#define laita(jotain) (laitot |= (1u << jotain ## _enum)) //laitot kannattaa tarkistaa myöhemmin
#define FOR_LISTA(l) for((l)->sij=0; (l)->sij<(l)->pit; (l)->sij++)

typedef struct {
  int id;
  int tiedostonro;
  int kierroksia;
  int osaamisia;
  lista* hetket; //tähän tulee (lista(uint32), jolla on kierroksilta)(hetki | osattu?<<31)
} sanameta_s;

typedef struct {
  char* sana;
  char* kaan;
  sanameta_s meta;
} snsto_s;

typedef struct {
  char* kysym;
  char* suote;
  uint32_t *hetki; //tämä on viite sanameta-hetket-listalle sopivaan kohtaan
} kysynta_s;

enum laitot_enum {
  kysymys_enum,
  suote_enum,
  kaunti_enum,
  tiedot_enum,
  viesti_enum,
  laitot_enum_pituus //lukuarvo kertoo pituuden
};
int lue_tiedosto(const char* restrict);
void lopeta();
void tee_tiedot();
void edellinen_osatuksi();
lista* pilko_sanoiksi(const char* restrict);
void sekoita();
int edellinen_kohta(const char* restrict, int*);
int seuraava_kohta(const char* restrict, int*);
int xsijainti(tekstiolio_s*, int);
void viestiksi(const char* restrict);
void uusi_kierros();
void osaamaton();
#endif
