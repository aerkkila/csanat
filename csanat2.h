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

/*järjestyksellä on väliä*/
enum laitot_enum {
  kysymys_enum,
  syote_enum,
  historia_enum,
  tieto_enum,
  laitot_enum_pituus,
};

typedef uint_fast64_t uaika_t;

#endif
