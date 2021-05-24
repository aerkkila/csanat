#include <tekstigraf.h>
#include <SDL2/SDL_pixels.h>

extern const int ikkuna_x0;
extern const int ikkuna_y0;
extern const int ikkuna_w0;
extern const int ikkuna_h0;
extern const char ohjelman_nimi[];

extern int ikkuna_x;
extern int ikkuna_y;
extern int ikkuna_w;
extern int ikkuna_h;

extern SDL_Color tv;

extern const unsigned uniaika;
extern const unsigned maxpit_suote;

extern const char kotihak[];

extern tekstiolio_s suoteol;
extern tekstiolio_s annetutol;
extern tekstiolio_s viestiol;

void asetelma();
void tuhoa_asetelma();
