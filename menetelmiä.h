#ifndef __menetelmia_h__
#define __menetelmia_h__
#include <stdint.h>
#include "lista.h"
#include "grafiikka.h"

extern char* tmpc;

/*
#define META_ID 0
#define META_TIEDOSTONRO 1
#define META_KIERROKSIA 2
#define META_OSAAMISIA 3
#define META_LISTA 4
#define SANAMETA(i,meta_id) ((int*)(snsto->taul[i])+(meta_id))*/
typedef struct {
  int id;
  int tiedostonro;
  int kierroksia;
  int osaamisia;
  clista* lista; //tähän tulee (lista(uint32), jolla on kierroksilta)(hetki | osattu?<<31)
} sanameta_s;
#define NYT_OLEVA(l) ((l)->taul+(l)->sij)
#define VIIMEINEN(l) ((l)->taul+(l)->pit-1)
#define TOISEKSI_VIIM (kysynnat->taul[kysynnat->pit-2])
#define SANAMETA(i,jasen) ( ((sanameta_s*)(snsto->taul[i]))->jasen )
#define METATIEDOT        (*(sanameta_s*)NYT_OLEVA(snsto)[2])
#define ID_TASSA         (METATIEDOT.id)
#define TIEDOSTO_TASSA   (METATIEDOT.tiedostonro)
#define KIERROKSIA_TASSA (METATIEDOT.kierroksia)
#define OSAAMISIA_TASSA  (METATIEDOT.osaamisia)
#define OSLISTA_TASSA    (METATIEDOT.lista)

#define FOR_LISTA(l,i) for((l)->sij=0; (l)->sij<(l)->pit; (l)->sij+=i)
#define VAIHDA(a, b, tyyppi) do {	\
    tyyppi it205ox49fi = a;		\
    a = b;				\
    b = it205ox49fi;			\
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
#define VIESTIKSI_PERAAN(...) do {		\
    sprintf(tmpc, __VA_ARGS__);			\
    viestiksi_peraan(tmpc);			\
  } while(0)

#define aseta_vari(v) SDL_SetRenderDrawColor(rend, v.r, v.g, v.b, v.a)
#define laita(jotain) (laitot |= (1u << jotain ## _enum))

#ifndef __laitot_enum__
#define __laitot_enum__
enum laitot_enum {
  kysymys_enum,
  suote_enum,
  kaunti_enum,
  tiedot_enum,
  viesti_enum,
  laitot_enum_pituus //lukuarvo kertoo pituuden
};
#endif

int lue_tiedosto(const char*);
void tallenna();
void avaa_sanoja(int kpl);
lista* pilko_sanoiksi(const char* restrict str);
void tee_tiedot();
void sekoita();
void edellinen_osatuksi();
void viestiksi(const char* restrict s); //määritelty käyttöliittymä.c:ssä
int edellinen_kohta(const char* restrict suote, int* id);
int seuraava_kohta(const char* restrict suote, int* id);
int xsijainti(tekstiolio_s* o, int p);
void viestiksi(const char* restrict);
void viestiksi_peraan(const char* restrict);
void uusi_kierros();
void osaamaton();
#endif
