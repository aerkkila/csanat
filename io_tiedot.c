#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <time.h>
#include "csanat.h"
#include "asetelma.h"

/*Tämä on pahasti kesken. Ajatus on kerätä harjoitusdataa sanojen osaamisista,
  jotta voidaan koneoppimista käyttäen valita ohjelmaan avattavaksi ne sanat,
  joita luultavimmin ei osata. Luulisin tekeväni tämän valmiiksi keväällä 2022.
  Tämä tiedosto on kirjoitettu ennen kuin olin opiskellut koneoppimista lainkaan.

  Tässä tiedostossa on sanojen osaamistietojen tallentaminen ja lukeminen.
  Kaikki, mikä liittyy tiedostoihin .tiedotCsanat tai .sanatCsanat on täällä.

  Tiedostossa ".tiedotCsanat" on tiedot kaikista sanoista:
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
  tallenna_vanhoja_sanoja();
  tallenna_uusia_sanoja();
}

void tallenna_uusia_sanoja() {
  FILE *f = fopen(tiedot, "a+");
  int32_t id=0;
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
  int sij0 = snsto->sij;
  FOR_LISTA(snsto) {
    snsto_s* sns = LISTALLA(snsto,snsto_s*,snsto->sij);
    if(sns->meta.id >= 0) //vanha sana
      continue;
    sns->meta.id = id; //asetetaan luodut tunnisteet myös aukiolevaan sanastoon
    fwrite(&id, 4, 1, f);
    int pit = sns->meta.hetket->pit;
    if(pit > TIETOPIT4) //tuskin tapahtuu
      pit = TIETOPIT4;
    fwrite(sns->meta.hetket->taul, 4, pit, f);
    for(int i=0; i<(TIETOPIT4-pit)*4; i++)
      fputc(0,f);
    /*sanan ja käännöksen kirjoittaminen*/
    fprintf(sanatied, "%i\036%s\036%s\n", id, sns->sana, sns->kaan);
    id++;
  }
  fclose(f);
  fclose(sanatied);
  snsto->sij = sij0;
}

void tallenna_vanhoja_sanoja() {
  FILE *f = fopen(tiedot, "a+");
  if(!f) {
    VIESTIKSI("Ei avattu tiedostoa %s eikä siten tallennettu", tiedot);
    return;
  }
  int testi;
  for(int i=0; i<snsto->pit; i++) {
    snsto_s* sns = LISTALLA(snsto,snsto_s*,i);
    if(sns->meta.id < 0) //uusi sana
      continue;
    if(fseek(f, sns->meta.id*TIETOPIT, SEEK_SET)) {
      TEE("Sanaa %i ei löytynyt eikä tallennettu. (%s | %s)", i, sns->sana, sns->kaan);
      continue;
    }
    if(fread(&testi, 4, 1, f) != 1) {
      TEE("Sanan %i id:tä ei luettu eikä tallennettu. (%s | %s)", i, sns->sana, sns->kaan);
      continue;
    }
    if(testi != sns->meta.id) {
      TEE("Id ei täsmää sanalla %i: %i, mutta luettiin %i.", i, sns->meta.id, testi);
      continue;
    }
    int maara = 1; //0. on tunniste
    do
      fread(&testi, 4, 1, f);
    while(testi && ++maara < TIETOPIT4); //ohitetaan tallennetut kohdat (kirjoite ≠ 0) ja tarkistetaan mahtuminen
    if(maara + sns->meta.kierroksia >= TIETOPIT4) {
      TEE("Sanan \"%s\" | \"%s\" tiedot ovat täynnä", sns->sana, sns->kaan);
      continue;
    }
    fseek(f, -4, SEEK_CUR); //viimeisin oli tyhjä, joten kirjoitetaan sen päälle
    fwrite(sns->meta.hetket->taul, TIETOPIT4-maara, 4, f);
  }
  fclose(f);
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
  if(maara < kpl || kpl < 0)
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
    listalle_kopioiden_mjon(snsto, tmpc);
    j=0;
    while((c = fgetc(f)) != '\n') //luetaan käännös
      tmpc[j++] = c;
    tmpc[j] = 0;
    listalle_kopioiden_mjon(snsto, tmpc);
    jatka_listaa(snsto, 1);
    LISTALLA_LOPUSTA(snsto,snsto_s*,-1)->meta.id = idt[i];
  }
  fclose(f);
  free(idt);
}

/*Tämä on toistaiseksi yksinkertainen:
  Avataan viime kerran perusteella:
  ei-osatut ensin, sitten kauimmin sitten olleet
  Samanaikaisista valitaan ne, joissa on ollut virheitä
  ja sitten satunnaisesti, jos niitä on liikaa.*/
static int* valitse_sanat(int kpl, FILE* f, int yht) {
  const uint64_t alkunolla = 0x00000000ffffffff;
  const int aikasiirto = 14; //noin 4,5 tunnin sisään (2^14 s) pidetään samanaikaisina
  long unsigned luentalong;
  uint64_t* lis = malloc(yht*8);
  for(int i=0; i<yht; i++) {
    fseek(f, i*TIETOPIT+4, SEEK_SET);
    unsigned viimeaika=0, luenta;
    int pituus=1;
    do {
      fread(&luenta, 4, 1, f);
      if(luenta<<1>>(aikasiirto+1) == viimeaika)
	pituus++;
      else {
	viimeaika = luenta>>aikasiirto;
	pituus=1;
      }
    } while(luenta);
    fseek(f, -4-4*pituus, SEEK_CUR); //viimeisen ajan paikka: taakse nolla ja luettavat
    luentalong = 0xff;
    for(int j=0; j<pituus; j++) {
      fread(&luenta, 4, 1, f);
      luentalong -= !(luenta>>31); //montako meni väärin
    }
    luentalong |= luenta >> aikasiirto << aikasiirto;
    lis[i] = luentalong << 32;
    lis[i] += i;
  }
  /*Listalla on nyt aina (int32 aika,virheet; int32 id), missä ajan merkitsevin bitti on osaaminen (0/1).
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
