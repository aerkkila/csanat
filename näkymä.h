#ifndef __NAKYMA_H__
#define __NAKYMA_H__

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

/*
extern SDL_Window* ikk;
extern SDL_Renderer* rend;
extern SDL_Texture* pohja;

extern int ikk_x, ikk_y, ikk_w, ikk_h;
extern const SDL_Color etuvari;
extern const SDL_Color taustavari;
extern const SDL_Color oikeavari;
extern const SDL_Color virhevari;
extern const SDL_Color oikea_taustavari;
extern const SDL_Color virhe_taustavari;
extern const SDL_Color kohdistinvari;
extern nakyolio syoteol;
extern nakyolio kysymol;
extern nakyolio listaol;
extern nakyolio tietool;
*/

#define ASETA_VARI(c) SDL_SetRenderDrawColor( rend, (c).r, (c).g, (c).b, (c).a )
void paivita_sijainnit();
void paivita_ikkunan_koko();
void alusta_nakyma();
void tuhoa_nakyma();
void paivita_kuva(unsigned laitot);

#endif
