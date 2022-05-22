int ikk_x=SDL_WINDOWPOS_UNDEFINED, ikk_y=SDL_WINDOWPOS_UNDEFINED, ikk_w=800, ikk_h=400;

/*muotoa [v/o]_*[1/2] olevat nimet tarkoittavat väärin/oikein_*tekstin/taustan väri*/
#define _TAUSTA 0,0,0,255
SDL_Color const varit[] = {
  [ETUV]       = {255,255,255,255},
  [TAUSTV]     = { _TAUSTA       },
  [V_SYOTE1]   = {255,20, 20, 255},
  [V_SYOTE2]   = { _TAUSTA       },
  [O_SYOTE1]   = {20, 255,20, 255},
  [O_SYOTE2]   = { _TAUSTA       },
  [V_HIST1]    = {0,  0,  0,  255},
  [V_HIST2]    = {255,80, 50, 255},
  [O_HIST1]    = {0,  0,  0,  255},
  [O_HIST2]    = {20, 255,60, 255},
  [KOHDISTINV] = {255,255,255,255},
};
#undef _TAUSTA

static SDL_Rect kohdistin_r = {.w = 3};
nakyolio syoteol = { .fkoko=60, .etuvari=varit+ETUV, .takavari=varit+TAUSTV };
nakyolio kysymol = { .fkoko=60, .etuvari=varit+ETUV, .takavari=varit+TAUSTV };
nakyolio histrol = { .fkoko=20, .etuvari=varit+ETUV, .takavari=varit+TAUSTV };
nakyolio tietool = { .fkoko=17, .etuvari=varit+ETUV, .takavari=varit+TAUSTV };
const char* ohjelman_nimi = "csanat";
static const char* fonttied = "MAKE_LIITÄ_MONOFONTTI";
