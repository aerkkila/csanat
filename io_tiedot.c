#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdint.h>
#include "menetelmiä.h"
#include "asetelma.h"

/*Tiedostossa ".tiedotCsanat" on tiedot kaikista sanoista:
  int32_t id, uint32_t[127] [1.bitti: osattiinko, ajankohta]

  Tiedostossa ".sanatCsanat on %i id, \036, sana, \036, käännös, \n

  Molemmissa oletetaan, että id:t ovat 0,1,2,3,4,...
  eli sinänsä id:tä ei tarvitsisi edes kirjoittaa näkyviin*/

#define TIETOPIT 512
#define TIETOPIT4 128
#define METAPIT 24

void tallenna_uusia_sanoja();
void tallenna_vanhoja_sanoja();
void tallenna();
void avaa_sanoja(int);
static int* valitse_sanat(int, FILE*, int yht);
static void siirry_sanan_kohtaan(int, FILE*, int pit, int sanoja);
static void _siirry_jarjestuksessa(int id, int siirto, FILE* f);
void* kantalukulajittele64(uint64_t* taul, int pituus);

const char* tiedot = ".tiedotCsanat";
const char* sanat = ".sanatCsanat";

void tallenna() {
  tallenna_uusia_sanoja();
  tallenna_vanhoja_sanoja();
}

void tallenna_uusia_sanoja() {
  FILE *f = fopen(tiedot, "a+");
  int id=0;
  if(!fseek(f,-TIETOPIT,SEEK_END)) {
    if(fread(&id, 4, 1, f) != 1) {
      TEE("Lukeminen epäonnistui eikä siten tallennettu");
      fclose(f);
      return;
    }
    id++;
  }
  FILE *sanatied = fopen(sanat, "a");
  fseek(f,0,SEEK_END);
  int32_t kirjoite[TIETOPIT4] = {0};
  int sij0 = snsto->sij;
  char** apuc;
  FOR_LISTA(snsto,3) {
    if(*ID_SANALLA >= 0) //vanha sana
      continue;
    memset(kirjoite, 0, TIETOPIT);
    *ID_SANALLA = id; //asetetaan nämä tunnisteet myös aukiolevaan sanastoon
    kirjoite[0] = id;
    fwrite(kirjoite, TIETOPIT, 1, f); //laitetaan vain tunniste, ei aikoja tässä funktiossa
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
  FILE *f = fopen(tiedot, "r+");
  if(!f) {
    TEE("Ei avattu tiedostoa %s eikä siten tallennettu", tiedot);
    return;
  }
  int sij0 = snsto->sij;
  int idtarkistus;
  uint32_t kirjoite[TIETOPIT4-1]; //id:tä ei kirjoiteta uudestaan
  uint32_t hetki = time(NULL);
  FOR_LISTA(snsto,3) {
    if(*ID_SANALLA < 0) //uusi sana
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
    while(*kirjoite && ++maara < TIETOPIT4-1); //ohitetaan tallennetut kohdat ja tarkistetaan mahtuminen
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

void avaa_sanoja(int kpl) {
  FILE *f = fopen(tiedot, "r");
  if(!f) {
    TEE("Ei avattu tiedostoa %s", tiedot);
    return;
  }
  int32_t maara; //onko sanoja tarpeeksi
  fseek(f, 0, SEEK_END);
  maara = ftell(f) / TIETOPIT;
  if(maara < kpl)
    kpl = maara;

  int* idt = valitse_sanat(kpl, f, maara);
  f = freopen(sanat, "r", f);
  fseek(f, 0, SEEK_END);
  int pit = ftell(f);
  for(int i=0; i<kpl; i++) {
    siirry_sanan_kohtaan(idt[i], f, pit, maara);
    int j=0;
    char c;
    while((c = fgetc(f)) != '\036') //luetaan sana
      tmpc[j++] = c;
    tmpc[j] = 0;
    listalle_kopioiden(snsto, tmpc);
    j=0;
    while((c = fgetc(f)) != '\n') //luetaan käännös
      tmpc[j++] = c;
    tmpc[j] = 0;
    listalle_kopioiden(snsto, tmpc);
    jatka_listaa(snsto, 1);
    *VIIMEINEN(snsto) = calloc(METAPIT,1);
    *(int32_t*)*VIIMEINEN(snsto) = idt[i];
  }
  fclose(f);
  free(idt);
}

/*Tämä on toistaiseksi hyvin yksinkertainen:
  Avataan sen mukaan, minkä osaamisesta on kauimmin.
  Samanaikaisista valitaan satunnaisesti, jos niitä on liikaa.*/
static int* valitse_sanat(int kpl, FILE* f, int yht) {
  const uint64_t alkunolla = 0x00000000ffffffff;
  uint64_t* lis = malloc(yht*8);
  for(int i=0; i<yht; i++) {
    fseek(f, i*TIETOPIT+4, SEEK_SET);
    long unsigned luentalong;
    unsigned luenta;
    do
      fread(&luenta, 4, 1, f);
    while(luenta);
    fseek(f, -8, SEEK_CUR); //viimeisen ajan paikka: taakse nolla ja aika eli 2*4
    fread(&luenta, 4, 1, f);
    luentalong = luenta;
    lis[i] = luentalong << 32;
    lis[i] += i;
  }
  /*Listalla on nyt aina (int32 aika, int32 id), missä ajan merkitsevin bitti on osaaminen (0/1).
    Tämä voidaan lajitella uint64:nä jolloin aika ja id pysyvät peräkkäin,
    kaikki osaamattomat tulevat ensin ja osatut sen jälkeen sen mukaan, mistä on kauimmin.*/
  lis = kantalukulajittele64((uint64_t*)lis, yht);
  /*tarvittaessa arvotaan, jos on samaan aikaan saatuja, ei valita vain pienimmän id:n mukaan*/
  int alkukohta=0;
  for(int i=1; i<kpl; i++)
    if(lis[i]>>32 != lis[i-1]>>32)
      alkukohta = i;
  /*ei-arvottavat*/
  int* valinnat = malloc(kpl*sizeof(int));
  for(int i=0; i<alkukohta; i++)
    valinnat[i] = lis[i] & alkunolla; //val <- id[i]
  /*arvottavat*/
  int pituus=1;
  for(; lis[(alkukohta+pituus)]>>32 == lis[alkukohta]>>32 && alkukohta+pituus<yht; pituus++); //montako samanaikaista on
  int arvonta[pituus];
  for(int i=0; i<pituus; i++)
    arvonta[i] = i;
  srand((unsigned)( time(NULL)+(intptr_t)f ));
  for(int i=0; i<kpl-alkukohta; i++) {//sekoitetaan vain tarvittava määrä
    int j = rand()%(pituus-i); //VAIHDA-makron argumenttina otettaisiin kahdesti rand()
    VAIHDA(arvonta[i], arvonta[j], int);
  }
  for(int i=0; i<kpl-alkukohta; i++)
    valinnat[i+alkukohta] = lis[arvonta[i]+alkukohta] & alkunolla;
  free(lis);
  return valinnat;
}

static void siirry_sanan_kohtaan(int id, FILE* f, int pit, int sanoja) {
  if(id < 12) {
    rewind(f);
    _siirry_jarjestuksessa(id, id, f);
    return;
  }
  int idluenta;
  int yhden_pit = pit/sanoja;
  int alkukohta=0, loppukohta=pit;
  fseek(f, (id-6)*yhden_pit, SEEK_SET);
 SILMUKKA:;
  int kohta = ftell(f);
  fscanf(f, "%*[^\n]\n");
  if(fscanf(f, "%i\036", &idluenta) != 1)
    TEE("Virhe id:n %i lukemisessa", id);
  if(idluenta == id)
    return;
  if(idluenta < id) {
    if(id-idluenta < 12) {
      _siirry_jarjestuksessa(id, id-idluenta, f);
      return;
    }
    alkukohta = ftell(f);
  } else
    loppukohta = kohta;
  fseek(f, (loppukohta-alkukohta)/2, SEEK_SET);
  goto SILMUKKA;  
}

static void _siirry_jarjestuksessa(int id, int siirto, FILE* f) {
  int idluenta;
  for(int i=0; i<siirto; i++)
    fscanf(f, "%*[^\n]\n");
  if(fscanf(f, "%i\036", &idluenta) != 1)
    TEE("Virhe sanan (id=%i) id:n lukemisessa", id);
  if(idluenta != id)
    TEE("Luettiin väärä id: tavoite = %i, luettiin %i", id, idluenta);
  return;
}

/*tämä saa alkaa 4:stä, koska id-osuuksia ei tarvitse lajitella*/
void* kantalukulajittele64(uint64_t* taul, int pit) {
  uint64_t* apu = malloc(pit*8);
  unsigned siirto = 4*8;
  int lasku[256];
  for(int j=4; j<8; j++, siirto+=8) {
    memset(lasku, 0, 256*sizeof(int));
    for(int i=0; i<pit; i++)
      lasku[taul[i]>>siirto & 0xff]++;
    for(int i=1; i<256; i++)
      lasku[i] += lasku[i-1];
    for(int i=pit-1; i>=0; i--)
      apu[--lasku[taul[i]>>siirto & 0xff]] = taul[i];
    VAIHDA(taul, apu, void*);
  }
  free(apu);
  return taul;
}
