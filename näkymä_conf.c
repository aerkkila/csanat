int ikk_x=300, ikk_y=0, ikk_w=600, ikk_h=400;
const SDL_Color etuvari         = {255,255,255,255};
const SDL_Color taustavari      = {0,  0,  0,  0  };
const SDL_Color vo_vari[]       = { {20, 255,20, 255},
				    {255,20, 20, 255}, };
const SDL_Color vo_taustavari[] = { {20, 255,60, 255},
				    {255,80, 50, 255}, };
const SDL_Color kohdistinvari   = {255,255,255,255};
static SDL_Rect kohdistin_r = {.w = 3};
nakyolio syoteol = { .fkoko=60, .etuvari=&etuvari, .takavari=&taustavari };
nakyolio kysymol = { .fkoko=60, .etuvari=&etuvari, .takavari=&taustavari };
nakyolio histrol = { .fkoko=20, .etuvari=&etuvari, .takavari=&taustavari };
nakyolio tietool = { .fkoko=17, .etuvari=&etuvari, .takavari=&taustavari };
const char* ohjelman_nimi = "csanat";
static const char* fonttied = "MAKE_LIITÄ_MONOFONTTI";
