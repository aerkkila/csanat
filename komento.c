#include <strlista.h>

void komento(char* suote) {
  strlista* knnot = _strpilko_sanoiksi(suote);
  while(knnot)
    knnot = _strpoista1(knnot, 1);
}
