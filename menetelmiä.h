#ifndef __menetelmia_h__
#define __menetelmia_h__
#include "lista.h"

#define META_KIERROKSIA 2
#define META_OSAAMISIA 3
#define META_OSAAMISET 4
#define NYT_OLEVA(l) ((l)->taul+(l)->sij)
#define VIIMEINEN(l) ((l)->taul[(l)->pit-1])
#define TOISEKSI_VIIM (kysynnat->taul[kysynnat->pit-2])
#define KIERROKSIA_SANALLA ((int*)( NYT_OLEVA(snsto)[2] )+META_KIERROKSIA)
#define OSAAMISKERRAT      ((int*)( NYT_OLEVA(snsto)[2] )+META_OSAAMISIA)
#define SANAN_OSAAMISET ((long long unsigned*)((int*)( NYT_OLEVA(snsto)[2] )+META_OSAAMISET))

#define aseta_vari(v) SDL_SetRenderDrawColor(rend, v.r, v.g, v.b, v.a)

int avaa_tiedosto(const char*);
lista* pilko_sanoiksi(const char* restrict str);
void tee_tiedot();
int laske_osatut();
void sekoita();
void edellinen_osatuksi();
#endif
