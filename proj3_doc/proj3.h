/**
 * @file    proj3.h
 * @brief   Project: Simple cluster analysis (3rd project IZP 2017)
 * @author  Dominik Vecera <xvecer23@stud.fit.vutbr.cz>
 * @date    15. 12. 2017
 * @version 1.0
 * @see     https://en.wikipedia.org/wiki/Cluster_analysis
 */

/**
 * @brief Structure representing an object.
 */
struct obj_t {
    /** Numeric identificator of an object. */
    int id;

    /** x coordinate of an object. */
    float x;

    /** y coordinate of an object. */
    float y;
};

/**
 * @brief Structure representing a cluster of objects.
 */
struct cluster_t {
    /** Number of objects in a cluster. */
    int size;

    /**
     * Capacity of a cluster (maximum number of objects, for which there
     *    is available space in the array).
     */
    int capacity;

    /** Pointer to an array of clusters. */
    struct obj_t *obj;
};

/**
 * @defgroup cluster Functions for cluster manipulation
 * @{
 */

/**
 * @brief Initializes cluster 'c' and allocates the necessary memory.
 *
 * @param c Pointer to cluster to be initialized.
 * @param cap Required capacity of the initialized cluster.
 *
 * @pre c != NULL
 * @pre Cluster capacity 'cap' >= 0.
 *
 * @post The cluster will be initialized with allocated memory.
 */
void init_cluster(struct cluster_t *c, int cap);

/**
 * @brief Clears all of the cluster's objects and initializes an empty cluster.
 *
 * @param c Pointer to cluster to be cleared.
 *
 * @pre c != NULL
 *
 * @post Allocated memory for all of the 'c' cluster's objects will be freed.
 */
void clear_cluster(struct cluster_t *c);

/// Chunk of cluster objects. Value recommended for reallocation.
extern const int CLUSTER_CHUNK;

/**
 * @brief Changes 'c' cluster's capacity to 'new_cap'.
 *
 * @param c Pointer to cluster to be resized.
 * @param new_cap New capacity of cluster.
 *
 * @pre c != NULL
 * @pre Capacity of cluster 'c' >= 0.
 * @pre New capacity 'new_cap' >= 0.
 *
 * @post Capacity of 'c' will be changed to 'new_cap',
 *          if the memory reallocation goes fine.
 *
 * @return Cluster 'c' with new capacity, NULL in case of error.
 */
struct cluster_t *resize_cluster(struct cluster_t *c, int new_cap);

/**
 * @brief Appends object 'obj' at the end of cluster 'c'.
 *
 * @param c Pointer to cluster to which the object is appended.
 * @param obj Object to be appended to the end of the cluster.
 *
 * @pre c != NULL
 *
 * @post Object 'obj' will be added to the end of cluster 'c'.
 */
void append_cluster(struct cluster_t *c, struct obj_t obj);

/**
 * @brief Merges clusters 'c2' and 'c1'.
 *
 * @param c1 Pointer to cluster, to which objects of 'c2' will be added.
 * @param c2 Pointer to cluster, from which objects will be added to cluster 'c1'.
 *
 * @pre 'c1' and 'c2' both != NULL.
 *
 * @post Objects from cluster 'c2' will be added to cluster 'c1'.
 * @post Objects in newly expanded cluster 'c1' will be sorted ascendingly
 *          according to their ID.
 */
void merge_clusters(struct cluster_t *c1, struct cluster_t *c2);

/**
 * @brief Sorts object in cluster ascendingly according to their ID.
 *
 * @param c Pointer to cluster to be sorted ascendingly by ID.
 *
 * @pre c != NULL
 *
 * @post Cluster 'c' will be sorted ascendingly by ID.
 */
void sort_cluster(struct cluster_t *c);

/**
 * @brief Prints cluster 'c' to stdout.
 *
 * @param c Pointer to cluster to be printed.
 *
 * @pre c != NULL
 *
 * @post Objects of cluster 'c' will be printed to stdout.
 */
void print_cluster(struct cluster_t *c);

/**
 * @}
 */

/**
 * @defgroup cluster_array Functions for cluster array manipulation
 * @{
 */

/**
 * @brief Removes cluster from cluster array 'carr'.
 *
 * @param carr Pointer to array of clusters.
 * @param narr Number of clusters in array.
 * @param idx Index of cluster to be removed from array.
 *
 * @pre 'narr' and 'idx' are both >= 0.
 * @pre idx < narr
 *
 * @post Array with index 'idx' will be removed from cluster array 'carr'.
 *
 * @post Help for program usage is printed to stdout.
 *
 * @return New count of clusters in cluster array.
 */
int remove_cluster(struct cluster_t *carr, int narr, int idx);

/**
 * @brief Counts Euclidean distance between two objects.
 *
 * @param o1 Pointer to 1st object
 * @param o2 Pointer to 2nd object
 *
 * @pre 'o1' and 'o2' both != NULL.
 *
 * @return Euclidean distance between objects 'o1' and 'o2'.
 *
 * @see https://en.wikipedia.org/wiki/Euclidean_distance
 */
float obj_distance(struct obj_t *o1, struct obj_t *o2);

/**
 * @brief Counts distance of two clusters.
 *
 * @param c1 Pointer to 1st cluster
 * @param c2 Pointer to 2nd cluster
 *
 * @pre 'c1' and 'c2' both != NULL.
 * @pre Size of both 'c1' and 'c2' >= 0.
 *
 * @return Distance of clusters 'c1' and 'c2'.
 */
float cluster_distance(struct cluster_t *c1, struct cluster_t *c2);

/**
 * @brief Finds two nearest clusters in cluster array 'carr'.
 *
 * @param carr Pointer to array of clusters.
 * @param narr Number of clusters in array.
 * @param c1 Pointer to index of the first nearest found cluster.
 * @param c2 Pointer to index of the second nearest found cluster.
 *
 * @pre narr >= 0
 *
 * @post Indexes of two nearest clusters will be stored in 'c1' and 'c2'.
 */
void find_neighbours(struct cluster_t *carr, int narr, int *c1, int *c2);

/**
 * @brief Loads objects from a file and creates clusters for them.
 *
 * @param filename Name of file from which the objects will be loaded.
 * @param arr Pointer to array of clusters loaded from file 'filename'.
 *
 * @pre File 'filename' must exist and the program must be able to read from it.
 * @pre Data in file 'filename' must have correct form.
 *
 * @post For each object from the data file a cluster will be created,
 *          all of these clusters will be stored in the cluster array
 *          'arr', which will have an allocated memory for all the clusters
 *          from the input file, if nothing goes wrong during allocation.
 *
 * @return Count of loaded clusters. In case of error - count * (-1).
 */
int load_clusters(char *filename, struct cluster_t **arr);

/**
 * @brief Prints array of clusters to stdout.
 *
 * @param carr Pointer to first item of array of clusters to be printed.
 * @param narr Number of clusters from cluster array to be printed.
 *
 * @post 'narr' objects of cluster array 'carr' will be printed to stdout.
 */
void print_clusters(struct cluster_t *carr, int narr);

/**
 * @brief Removes all clusters from the cluster array.
 *
 * @param carr Pointer to array of clusters to be removed.
 * @param narr Number of clusters in array of clusters.
 *
 * @post Individual clusters will be cleared and memory space allocated for
 *          the cluster array will be freed.
 */
void clear_all_clusters(struct cluster_t *carr, int narr);

/**
 * @brief Reduces number of clusters to desired quantity.
 *
 * @param clusters Pointer to array of clusters to be reduced.
 * @param size Original size of cluster array (count of clusters in it).
 * @param final_size Desired final size of cluster array.
 *
 * @pre final_size < size
 *
 * @post Count of clusters will be reduced to desired count.
 *
 * @return New size of cluster array.
 */
int clustering(struct cluster_t *clusters, int size, int final_size);

/**
 * @}
 */

/**
 * @defgroup custom_functions Custom functions used in the program
 * @{
 */

/**
 * @brief Evaluates input arguments with which the program was run.
 *
 * @param argc Count of program arguments.
 * @param argv Array of program arguments as strings.
 *
 * @post The program's behavior will be decided based on the input arguments,
 *          if they were set correctly.
 *
 * @return Desired final count of clusters.
 */
int arg_check(const int argc, const char *argv[]);

/**
 * @brief Gets expected count of objects in file from its first line.
 *
 * @param fr Pointer to open file with object definitions.
 *
 * @pre File must be open and the program must be able to read from it.
 *
 * @return Count of objects read from first line of file.
 */
int get_object_count_from_first_line(FILE *fr);

/**
 * @brief Informs user about wrong object coordinate in the input file.
 *
 * @param line Number of line which contains an invalid coordinate.
 * @param filename Name of file from which the program reads.
 * @param c Character X/Y depending on which coordinate on the line is invalid.
 * @param fr Pointer to open file.
 *
 * @post An error message will be displayed and the program will end.
 */
void invalid_coordinate(int line, const char *filename, char c, FILE *fr);

/**
 * @brief Converts string to integer number.
 *
 * @param s String to be converted to a number.
 *
 * @post Input string is converted to a number, if it is valid.
 *
 * @return Input string as an integer number.
 */
int str_to_int(const char *s);

/**
 * @brief Prints error mesage given as parameter to error output.
 *
 * @param str String containing error message to be printed.
 *
 * @post Error message is printed to stderr.
 */
void print_error(char *str);

/**
 * @brief Prints help for program usage.
 *
 * @post Help for program usage is printed to stdout.
 */
void print_help();

/**
 * @brief Prints help for how the object file should look.
 *
 * @post Help for object file is printed to stdout.
 */
void print_file_help();

/**
 * @}
 */
