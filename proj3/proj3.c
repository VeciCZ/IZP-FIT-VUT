/**
 * Kostra programu pro 3. projekt IZP 2017/18
 *
 * Jednoducha shlukova analyza
 * Unweighted pair-group average
 * https://is.muni.cz/th/172767/fi_b/5739129/web/web/usrov.html
 */
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h> // sqrtf
#include <limits.h> // INT_MAX
#include <string.h>

/*****************************************************************
 * Ladici makra. Vypnout jejich efekt lze definici makra
 * NDEBUG, napr.:
 *   a) pri prekladu argumentem prekladaci -DNDEBUG
 *   b) v souboru (na radek pred #include <assert.h>
 *      #define NDEBUG
 */
#ifdef NDEBUG
#define debug(s)
#define dfmt(s, ...)
#define dint(i)
#define dfloat(f)
#else

// vypise ladici retezec
#define debug(s) printf("- %s\n", s)

// vypise formatovany ladici vystup - pouziti podobne jako printf
#define dfmt(s, ...) printf(" - "__FILE__":%u: "s"\n",__LINE__,__VA_ARGS__)

// vypise ladici informaci o promenne - pouziti dint(identifikator_promenne)
#define dint(i) printf(" - " __FILE__ ":%u: " #i " = %d\n", __LINE__, i)

// vypise ladici informaci o promenne typu float - pouziti
// dfloat(identifikator_promenne)
#define dfloat(f) printf(" - " __FILE__ ":%u: " #f " = %g\n", __LINE__, f)

#endif

/****************************************************************
 * Deklarace maker pouzivanych v programu.
 */

#define DEFAULT_CLUSTER_COUNT 1 // pokud neni zadan, vychozi pocet shluku = 1

/* Maximalni delka retezce s poctem objektu na prvnim radku vstupniho souboru
   ve tvaru count=N, kde N je pocet objektu specifikovanych v danem souboru.
   'count=' zabira 6 znaku plus nulovy byte na konci, tudiz na cislo samotne
   zbyde 9 znaku - lze tedy mit ve vstupnim souboru mene nez miliardu objektu.
*/
#define MAX_OBJECT_COUNT_LINE_LENGTH 16

#define MIN_COORDINATE 0
#define MAX_COORDINATE 1000 // souradnice X a Y musi byt mezi 0 a 1000 vcetne

/*****************************************************************
 * Definice typu pro globalni promennou potrebnou k urceni
 * pozadovane metody shlukovani.
 */

typedef enum {
  AVG, //specifikuje metodu "Unweighted pair-group average" (vychozi)
  MIN, //speficikuje metodu nejblizsiho souseda
  MAX  //speficikuje metodu nejvzdalenejsiho souseda
} caseoptions;
caseoptions premium_case = AVG;

/*****************************************************************
 * Deklarace potrebnych datovych typu:
 *
 * TYTO DEKLARACE NEMENTE
 *
 *   struct obj_t - struktura objektu: identifikator a souradnice
 *   struct cluster_t - shluk objektu:
 *      pocet objektu ve shluku,
 *      kapacita shluku (pocet objektu, pro ktere je rezervovano
 *          misto v poli),
 *      ukazatel na pole shluku.
 */

struct obj_t {
    int id;
    float x;
    float y;
};

struct cluster_t {
    int size;
    int capacity;
    struct obj_t *obj;
};

/*****************************************************************
 * Deklarace potrebnych funkci.
 *
 * PROTOTYPY FUNKCI NEMENTE
 *
 * IMPLEMENTUJTE POUZE FUNKCE NA MISTECH OZNACENYCH 'TODO'
 *
 */

/*
 Funkce vypisujici libovolne chybove hlaseni na chybovy vystup (stderr).
*/
void print_error(char *str){
    fprintf(stderr, "%s", str);
}

/*
 Funkce tisknouci napovedu k zadani argumentu.
*/
void print_help()
{
    printf("-----------------------------\n"
         "Program se spousti nasledovne:\n"
         "./proj3 SOUBOR [N]\n"
         "SOUBOR je jmeno souboru obsahujiciho vstupni data.\n"
         "N je volitelny argument definujici cilovy pocet shluku, N > 0.\n"
         "Vychozi hodnota (pokud neni zadan argument N) je 1.\n\n"
         "Volitelne lze take urcit metodu shlukovani.\n"
         "V tom pripade se program spousti takto:\n"
         "./proj3 SOUBOR N [METHOD]\n"
         "kde jsou prvni 2 zadane argumenty stejne jako v predchozim pripade,\n"
         "ale argument N je povinny. METHOD je volitelny argument urcujici\n"
         "pozadovanou metodu shlukovani, ktery muze mit tyto hodnoty:\n"
         "--avg - metoda \"Unweighted pair-group average\" (vychozi),\n"
         "--min - metoda nejblizsiho souseda,\n"
         "--max - metoda nejvzdalenejsiho souseda.\n");
}

/*
 Funkce tisknouci napovedu ke vstupnimu souboru s objekty.
*/
void print_file_help()
{
    printf("---------------------------------------------\n"
           "Vstupni data jsou ulozena v textovem souboru.\n"
           "Prvni radek souboru je vyhrazen pro pocet objektu v souboru\n"
           "a ma format: \"count=N\", kde N je pocet objektu v souboru.\n"
           "Nasleduje na kazdem radku definice jednoho objektu.\n"
           "Pocet radku souboru odpovida nejmene poctu objektu + 1 (1. radek).\n"
           "Dalsi radky jsou ignorovany. Radek definujici objekt je formatu:\n"
           "OBJID X Y\n"
           "kde OBJID je v ramci souboru jednoznacny celociselny identifikator,\n"
           "X a Y jsou souradnice objektu take cela cisla.\n"
           "Plati 0 <= X <= 1000, 0 <= Y <= 1000.\n");
}

/*
 Inicializace shluku 'c'. Alokuje pamet pro cap objektu (kapacitu).
 Ukazatel NULL u pole objektu znamena kapacitu 0.
*/
void init_cluster(struct cluster_t *c, int cap)
{
    assert(c != NULL);
    assert(cap >= 0);

    c->size = 0;
    if (cap > 0){
      if ((c->obj = malloc(sizeof(struct obj_t) * cap)) != NULL){
        c->capacity = cap;
        return;
      }
      else
        print_error("Nezdarila se alokace pameti.\n");
    }

    c->capacity = 0;
    c->obj = NULL;
}

/*
 Odstraneni vsech objektu shluku a inicializace na prazdny shluk.
 */
void clear_cluster(struct cluster_t *c)
{
    free(c->obj);
    init_cluster(c, 0);
}

/// Chunk of cluster objects. Value recommended for reallocation.
const int CLUSTER_CHUNK = 10;

/*
 Zmena kapacity shluku 'c' na kapacitu 'new_cap'.
 */
struct cluster_t *resize_cluster(struct cluster_t *c, int new_cap)
{
    // TUTO FUNKCI NEMENTE
    assert(c);
    assert(c->capacity >= 0);
    assert(new_cap >= 0);

    if (c->capacity >= new_cap)
        return c;

    size_t size = sizeof(struct obj_t) * new_cap;

    void *arr = realloc(c->obj, size);
    if (arr == NULL)
        return NULL;

    c->obj = (struct obj_t*)arr;
    c->capacity = new_cap;
    return c;
}

/*
 Prida objekt 'obj' na konec shluku 'c'. Rozsiri shluk, pokud se do nej objekt
 nevejde.
 */
void append_cluster(struct cluster_t *c, struct obj_t obj)
{
    if (c->capacity <= c->size){
      if (resize_cluster(c, c->capacity + CLUSTER_CHUNK) == NULL){
        print_error("Chyba zvetseni shluku.\n");
        return;
      }
    }

    c->obj[c->size++] = obj;
}

/*
 Seradi objekty ve shluku 'c' vzestupne podle jejich identifikacniho cisla.
 */
void sort_cluster(struct cluster_t *c);

/*
 Do shluku 'c1' prida objekty 'c2'. Shluk 'c1' bude v pripade nutnosti rozsiren.
 Objekty ve shluku 'c1' budou serazeny vzestupne podle identifikacniho cisla.
 Shluk 'c2' bude nezmenen.
 */
void merge_clusters(struct cluster_t *c1, struct cluster_t *c2)
{
    assert(c1 != NULL);
    assert(c2 != NULL);

    for (int i = 0; i < c2->size; i++)
      append_cluster(c1, c2->obj[i]);

    sort_cluster(c1);
}

/**********************************************************************/
/* Prace s polem shluku */

/*
 Odstrani shluk z pole shluku 'carr'. Pole shluku obsahuje 'narr' polozek
 (shluku). Shluk pro odstraneni se nachazi na indexu 'idx'. Funkce vraci novy
 pocet shluku v poli.
*/
int remove_cluster(struct cluster_t *carr, int narr, int idx)
{
    assert(idx < narr);
    assert(narr > 0);

    clear_cluster(&carr[idx]);

    for (int i = idx; i < narr-1; i++)
      carr[i] = carr[i+1];

    return (narr-1);
}

/*
 Odstrani pole shluku.
*/
void clear_all_clusters(struct cluster_t *carr, int narr)
{
    for (int i = 0; i < narr; i++){
      clear_cluster(&carr[i]);
    }

    free(carr);
    carr = NULL;
}

/*
 Pocita Euklidovskou vzdalenost mezi dvema objekty.
 */
float obj_distance(struct obj_t *o1, struct obj_t *o2)
{
    assert(o1 != NULL);
    assert(o2 != NULL);

    float x = (o1->x - o2->x);
    x *= x;
    float y = (o1->y - o2->y);
    y *= y;

    return sqrtf(x + y);
}

/*
 Pocita vzdalenost dvou shluku.
*/
float cluster_distance(struct cluster_t *c1, struct cluster_t *c2)
{
    assert(c1 != NULL);
    assert(c1->size > 0);
    assert(c2 != NULL);
    assert(c2->size > 0);

    float cluster_dist = 0.0, temp_dist = 0.0;

    switch(premium_case){
      case MIN:

        cluster_dist = INT_MAX;

        for (int i = 0; i < c1->size; i++) {
          for (int j = 0; j < c2->size; j++) {
            temp_dist = obj_distance(&c1->obj[i], &c2->obj[j]);

            if (temp_dist < cluster_dist)
              cluster_dist = temp_dist;
          }
        }
        break;

      case MAX:

        for (int i = 0; i < c1->size; i++) {
          for (int j = 0; j < c2->size; j++) {
            temp_dist = obj_distance(&c1->obj[i], &c2->obj[j]);

            if (temp_dist > cluster_dist)
              cluster_dist = temp_dist;
          }
        }
        break;

      case AVG:
      default:

        for (int i = 0; i < c1->size; i++)
          for (int j = 0; j < c2->size; j++)
            temp_dist += obj_distance(&c1->obj[i], &c2->obj[j]);

        cluster_dist = temp_dist/(c1->size*c2->size);
    }

    return cluster_dist;
}

/*
 Funkce najde dva nejblizsi shluky. V poli shluku 'carr' o velikosti 'narr'
 hleda dva nejblizsi shluky. Nalezene shluky identifikuje jejich indexy v poli
 'carr'. Funkce nalezene shluky (indexy do pole 'carr') uklada do pameti na
 adresu 'c1' resp. 'c2'.
*/
void find_neighbours(struct cluster_t *carr, int narr, int *c1, int *c2)
{
    assert(narr > 0);

    // pokud pole shluku obsahuje pouze 1 shluk, nelze funkci provest
    if (narr == 1){
        *c1 = 0;
        *c2 = 0;
        return;
    }

    float temp_dist, dist_min = -1;

    for (int i = 0; i < narr; i++){
      for (int j = i + 1; j < narr; j++){

        temp_dist = cluster_distance(&carr[i], &carr[j]);

        if (temp_dist < dist_min || dist_min == -1){
          *c1 = i;
          *c2 = j;
          dist_min = temp_dist;
        }
      }
    }
}

// pomocna funkce pro razeni shluku
static int obj_sort_compar(const void *a, const void *b)
{
    // TUTO FUNKCI NEMENTE
    const struct obj_t *o1 = (const struct obj_t *)a;
    const struct obj_t *o2 = (const struct obj_t *)b;
    if (o1->id < o2->id) return -1;
    if (o1->id > o2->id) return 1;
    return 0;
}

/*
 Razeni objektu ve shluku vzestupne podle jejich identifikatoru.
*/
void sort_cluster(struct cluster_t *c)
{
    // TUTO FUNKCI NEMENTE
    qsort(c->obj, c->size, sizeof(struct obj_t), &obj_sort_compar);
}

/*
 Tisk shluku 'c' na stdout.
*/
void print_cluster(struct cluster_t *c)
{
    // TUTO FUNKCI NEMENTE
    for (int i = 0; i < c->size; i++)
    {
        if (i) putchar(' ');
        printf("%d[%g,%g]", c->obj[i].id, c->obj[i].x, c->obj[i].y);
    }
    putchar('\n');
}

/*
 Funkce, ktera prevadi retezec na cele cislo (integer).
*/
int str_to_int(const char *s)
{
    char *endptr;
    int num = strtol(s, &endptr, 0);

    if (endptr[0] == '\0')
      return num;
    else
      return 0;
}

/*
 Funkce informujici uzivatele o spatne souradnici ve vstupnim souboru
 a provadejici prislusne ukoncovaci akce.
*/
void invalid_coordinate(int line, const char *filename, char c, FILE *fr)
{
    fprintf(stderr, "Na radku %d v souboru \"%s\" je neplatna "
                "souradnice %c. X i Y musi byt v rozmezi 0-1000 vcetne.\n",
                line, filename, c);
    print_file_help();
    fclose(fr);
}

/*
 Funkce zjistujici pocet objektu v souboru na zaklade udaje na prvnim radku.
*/
int get_object_count_from_first_line(FILE *fr)
{
    char first_line[MAX_OBJECT_COUNT_LINE_LENGTH], *count_str;

    fscanf(fr, "%s", first_line);

    if (strncmp(first_line, "count=", 6) != 0){
      print_error("Prvni radek souboru neni v pozadovanem formatu \"count=N\".\n");
      fclose(fr);
      return -1;
    }

    char c;
    if ((c = getc(fr)) != '\n' && c != EOF && c != '\r'){
      print_error("V souboru se vyskytl nevalidni radek.\n");
      print_file_help();
      fclose(fr);
      return -1;
    }

    count_str = strchr(first_line, '=');

    int object_count = str_to_int(++count_str);
    if (object_count <= 0){
      print_error("Pocet objektu v souboru musi byt vetsi nez 0.\n");
      fclose(fr);
      return -1;
    }

    return object_count;
}


/*
 Ze souboru 'filename' nacte objekty. Pro kazdy objekt vytvori shluk a ulozi
 jej do pole shluku. Alokuje prostor pro pole vsech shluku a ukazatel na prvni
 polozku pole (ukazatel na prvni shluk v alokovanem poli) ulozi do pameti,
 kam se odkazuje parametr 'arr'. Funkce vraci pocet nactenych objektu (shluku).
 V pripade nejake chyby uklada do pameti, kam se odkazuje 'arr', hodnotu NULL.
*/
int load_clusters(const char *filename, struct cluster_t **arr)
{
    assert(arr != NULL);

    *arr = NULL;

    FILE *fr;
    if ((fr = fopen(filename, "r")) == NULL){
      print_error("Nelze otevrit zadany soubor. Zkontrolujte spravnost nazvu.\n");
      print_help();
      return 0;
    }

    int id, loaded_count = 0;
    float x, y;

    int specified_count;
    if ((specified_count = get_object_count_from_first_line(fr)) == -1){
      return 0;
    }

    if ((*arr = malloc(specified_count * sizeof(struct cluster_t))) == NULL){
      print_error("Alokace pameti se nezdarila.\n");
      fclose(fr);
      return 0;
    }

    int ids[specified_count];
    struct obj_t temp_obj;
    int f;
    // postupne nacitam objekty ze souboru po jednom radku
    while (((f = fscanf(fr, "%9d %4f %4f", &id, &x, &y)) == 3)
           && loaded_count < specified_count) {

      char c;
      if ((c = getc(fr)) != '\n' && c != EOF && c != '\r'){
        print_error("V souboru se vyskytl nevalidni radek.\n");
        print_file_help();
        fclose(fr);
        // pri chybe vracim pocet nactenych shluku *(-1), aby byla poznat chyba
        return -loaded_count;
      }

      if (x < MIN_COORDINATE || x > MAX_COORDINATE){
        invalid_coordinate(loaded_count+2, filename, 'X', fr);
        return -loaded_count;
      }
      else if (y < MIN_COORDINATE || y > MAX_COORDINATE){
        invalid_coordinate(loaded_count+2, filename, 'Y', fr);
        return -loaded_count;
      }

      temp_obj.id = id;
      temp_obj.x = x;
      temp_obj.y = y;

      ids[loaded_count] = id;

      for (int i = 0; i < loaded_count; i++){
        if (ids[i] == id){
          print_error("V souboru byly nalezeny 2 shluky s duplicitnimi ID.\n");
          fclose(fr);
          return -loaded_count;
        }
      }

      // inicializuje prazdny shluk v poli shluku a da do nej dany objekt
      init_cluster(&(*arr)[loaded_count], 0);
      append_cluster(&(*arr)[loaded_count], temp_obj);

      loaded_count++;
    }

    if (specified_count != loaded_count){
      print_error("Pocet objektu specifikovany na zacatku souboru\n "
                  "neodpovida skutecnemu poctu nactenych objektu.\n"
                  "V souboru se nejspise nekde nachazi nevalidni udaje\n"
                  "a proto nemohl byt korektne cely nacten.\n");
      print_file_help();
      fclose(fr);
      return -loaded_count;
    }

    fclose(fr);
    return loaded_count;
}

/*
 Tisk pole shluku. Parametr 'carr' je ukazatel na prvni polozku (shluk).
 Tiskne se prvnich 'narr' shluku.
*/
void print_clusters(struct cluster_t *carr, int narr)
{
    printf("Clusters:\n");
    for (int i = 0; i < narr; i++)
    {
        printf("cluster %d: ", i);
        print_cluster(&carr[i]);
    }
}

/*
 Funkce shlukujici shluky, dokud neni jejich pocet dostatecne zredukovany.
*/
int clustering(struct cluster_t *clusters, int size, int final_size)
{
    if (final_size > size){
      print_error("Zadany pozadovany pocet shluku je vetsi nez puvodni pocet.\n");
      return -1;
    }

    int c1_orig_size, c1_index, c2_index;

    while (size > final_size) {
      find_neighbours(clusters, size, &c1_index, &c2_index);

      c1_orig_size = clusters[c1_index].size;

      merge_clusters(&clusters[c1_index], &clusters[c2_index]);

      if (clusters[c1_index].size != c1_orig_size + clusters[c2_index].size &&
          clusters[c2_index].size > 0) {
        print_error("Nezdarila se alokace pameti.\n");
        return -1;
      }

      size = remove_cluster(clusters, size, c2_index);
    }

    return size;
}

/*
 Funkce kontrolujici spravnost zadanych argumentu.
*/
int arg_check(const int argc, const char *argv[])
{
    int cluster_required_count;
    if (argc == 1){
      print_error("Nezadan zadny argument.\n");
      return -1;
    }
    else if (argc == 2) // pokud nebyl zadan cilovy pocet shluku
      cluster_required_count = DEFAULT_CLUSTER_COUNT;
    else if (argc == 3){
      if (!(cluster_required_count = str_to_int(argv[2]))
          || cluster_required_count <= 0){
        print_error("Nastaveny pocet shluku musi byt nenulove cislo.\n");
        return -1;
      }
    }
    else if (argc == 4){
      if (!(cluster_required_count = str_to_int(argv[2]))
          || cluster_required_count <= 0){
        print_error("Nastaveny pocet shluku musi byt nenulove cislo.\n");
        return -1;
      }

      if (strcmp(argv[3], "--avg") == 0)
        premium_case = AVG;
      else if (strcmp(argv[3], "--min") == 0)
        premium_case = MIN;
      else if (strcmp(argv[3], "--max") == 0)
        premium_case = MAX;
      else{
        print_error("Zadan neplatny argument metody shlukovani.\n");
        return -1;
      }
    }
    else {
      print_error("Zadan nadbytecny pocet argumentu.\n");
      return -1;
    }

    return cluster_required_count;
}

int main(const int argc, const char *argv[])
{
    int final_size;
    struct cluster_t *clusters = NULL;

    if ((final_size = arg_check(argc, argv)) == -1){
      print_help();
      return EXIT_FAILURE;
    }

    int loaded; // pocet nactenych objektu ze souboru
    if ((loaded = load_clusters(argv[1], &clusters)) <= 0){
      clear_all_clusters(clusters, -loaded);
      return EXIT_FAILURE;
    }

    if ((final_size = clustering(clusters, loaded, final_size)) == -1){
      clear_all_clusters(clusters, loaded);
      print_help();
      return EXIT_FAILURE;
    }

    print_clusters(clusters, final_size);
    clear_all_clusters(clusters, final_size);
    return EXIT_SUCCESS;
}
