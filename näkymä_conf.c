int ikk_x=300, ikk_y=0, ikk_w=600, ikk_h=400;

SDL_Color const varit[] = {
  [ETUV]        = {255,255,255,255},
  [TAUSTV]      = {0,  0,  0  ,0  },
  [VAARAV]      = {255,20, 20 ,255},
  [OIKEAV]      = {20, 255,20 ,255},
  [VAARATAUSTV] = {255,80, 50 ,255},
  [OIKEATAUSTV] = {20, 255,60 ,255},
  [KOHDISTINV]  = {255,255,255,255},
};

static SDL_Rect kohdistin_r = {.w = 3};
nakyolio syoteol = { .fkoko=60, .etuvari=varit+ETUV, .takavari=varit+TAUSTV };
nakyolio kysymol = { .fkoko=60, .etuvari=varit+ETUV, .takavari=varit+TAUSTV };
nakyolio histrol = { .fkoko=20, .etuvari=varit+ETUV, .takavari=varit+TAUSTV };
nakyolio tietool = { .fkoko=17, .etuvari=varit+ETUV, .takavari=varit+TAUSTV };
const char* ohjelman_nimi = "csanat";
static const char* fonttied = "MAKE_LIITÄ_MONOFONTTI";
