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

#define ETUV       0
#define TAUSTV     1
#define V_SYOTE1   2
#define V_SYOTE2   3
#define O_SYOTE1   4
#define O_SYOTE2   5
#define V_HIST1    6
#define V_HIST2    7
#define O_HIST1    8
#define O_HIST2    9
#define KOHDISTINV 10

typedef uint_fast64_t aika_t;

#endif
