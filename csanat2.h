#ifndef __CSANAT2_H__
#define __CSANAT2_H__

#include "lista.h"
#include "stdint.h"
extern lista snsto;
extern lista historia[];
extern lista tietolis;
extern lista tiedostot;
extern char syotetxt[];
extern char kysymtxt[];
extern int kohdistin;

/*järjestyksellä on väliä*/
enum laitot_enum {
  kysymys_enum,
  syote_enum,
  historia_enum,
  tieto_enum,
  laitot_enum_pituus,
};

#define ETUV        0
#define TAUSTV      1
#define VAARAV      2
#define OIKEAV      3
#define VAARATAUSTV 4
#define OIKEATAUSTV 5
#define KOHDISTINV  6

typedef int_fast64_t aika_t;

#endif
