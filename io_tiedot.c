#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdint.h>
#include "menetelmiä.h"
#include "asetelma.h"

/*Tiedostossa ".tiedotCsanat" on tiedot kaikista sanoista:
  int32_t id, uint32_t[127] [1.bitti: osattiinko, ajankohta]

  Tiedostossa ".sanatCsanat on %i id, \036, sana, \036, käännös, \n*/

#define TIETOPIT 512
#define TIETOPIT4 128

void tallenna_uusia_sanoja();
void tallenna_vanhoja_sanoja();
void tallenna();

void tallenna() {
  tallenna_uusia_sanoja();
  tallenna_vanhoja_sanoja();
}

void tallenna_uusia_sanoja() {
  FILE *f = fopen(".tiedotCsanat", "a+");
  int id=0;
  if(!fseek(f,-TIETOPIT,SEEK_END)) {
    if(fread(&id, 4, 1, f) != 1) {
      TEE("Lukeminen epäonnistui eikä siten tallennettu");
      fclose(f);
      return;
    }
    id++;
  }
  FILE *sanatied = fopen(".sanatCsanat", "a");
  fseek(f,0,SEEK_END);
  int32_t kirjoite[TIETOPIT4];
  uint32_t hetki = time(NULL);
  int sij0 = snsto->sij;
  char** apuc;
  FOR_LISTA(snsto,3) {
    if(*ID_SANALLA >= 0)
      continue;
    memset(kirjoite, 0, TIETOPIT);
    *ID_SANALLA = id; //asetetaan nämä tunnisteet myös aukiolevaan sanastoon
    kirjoite[0] = id;
    for(int j=0; j<*KIERROKSIA_SANALLA; j++)
      kirjoite[j+1] = hetki | (*SANAN_OSAAMISET >> j & 1) << 31; //suurimpaan bittiin osaaminen
    fwrite(kirjoite, TIETOPIT, 1, f);
    /*sanan ja käännöksen kirjoittaminen*/
    apuc = NYT_OLEVA(snsto);
    fprintf(sanatied, "%i\036%s\036%s\n", id, apuc[0], apuc[1]);
    id++;
  }
  fclose(f);
  fclose(sanatied);
  snsto->sij = sij0;
}

void tallenna_vanhoja_sanoja() {
  FILE *f = fopen(".tiedotCsanat", "r+");
  if(!f) {
    TEE("Ei avattu tiedostoa .tiedotCsanat eikä siten tallennettu");
    return;
  }
  int sij0 = snsto->sij;
  int idtarkistus;
  uint32_t kirjoite[TIETOPIT4-1]; //id:tä ei kirjoiteta uudestaan
  uint32_t hetki = time(NULL);
  FOR_LISTA(snsto,3) {
    if(*ID_SANALLA < 0)
      continue;
    if(fseek(f, *ID_SANALLA*TIETOPIT, SEEK_SET)) {
      TEE("Sanaa %i ei löytynyt eikä tallennettu. (%s)", snsto->sij/3, *NYT_OLEVA(snsto));
      continue;
    }
    if(fread(&idtarkistus, 4, 1, f) != 1) {
      TEE("Sanan %i id:tä ei luettu eikä tallennettu. (%s)", snsto->sij/3, *NYT_OLEVA(snsto));
      continue;
    }
    if(idtarkistus != *ID_SANALLA) {
      TEE("Id ei täsmää sanalla %i: %i, mutta luettiin %i.", snsto->sij/3, *ID_SANALLA, idtarkistus);
      continue;
    }
    int maara = 0;
    do
      fread(kirjoite, 4, 1, f);
    while(*kirjoite && ++maara < TIETOPIT4-1); //ohitetaan tallennetut kohdat
    if(maara + *KIERROKSIA_SANALLA >= TIETOPIT4) {
      TEE("Sanan \"%s\" tiedot ovat täynnä", *NYT_OLEVA(snsto));
      continue;
    }
    fseek(f, -4, SEEK_CUR); //viimeisin oli tyhjä, joten kirjoitetaan sen päälle
    *kirjoite = 0;
    for(int j=0; j<*KIERROKSIA_SANALLA; j++)
      kirjoite[j] = hetki | (*SANAN_OSAAMISET >> j & 1) << 31; //suurimpaan bittiin osaaminen
    fwrite(kirjoite, TIETOPIT-4-maara, 1, f);
  }
  fclose(f);
  snsto->sij = sij0;
}
