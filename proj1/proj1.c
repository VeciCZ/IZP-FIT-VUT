/*
 * File:          proj1.c
 * Date:          05. 11. 2017
 * Author:        Dominik Vecera, xvecer23@stud.fit.vutbr.cz
 * Project:       Working with text
 * Description:   The program emulates the algorithm of simulating a navigation's virtual keyboard.
 *                The input argument is compared to a database of available cities and a matching city is displayed, if found,
 *                and enabled following characters which lead to a result are displayed, if there are any.
 */

#define BUFSIZE 101

#include <stdio.h>
#include <stdbool.h>

// Function which returns the length of a string.
int string_length(char *str)
{
  int i = 0;
  while (str[i] != '\0')
    i++;
  return i;
}

// Function which finds out if there is something wrong with the input and if yes, informs the user.
void errors(int argc, char* argv[])
{
  if (argc > 2)
    fprintf(stderr, "Multiple arguments were set. Only the first argument will be taken into consideration.\n");
  else if (argc > 1){
    if ((string_length(argv[1])) > BUFSIZE-1){ // If the input argument is longer than 100 characters, truncate the rest.
      fprintf(stderr, "The input argument is too long, characters after the 100th character truncated.\n");
      argv[1][BUFSIZE-1] = '\0';
    }
  }
}

// Function which compares two letters based on their ASCII values and returns boolean value of the result.
bool letter_compare(char ltr_1, char ltr_2)
{
  if ((ltr_1 == ltr_2) || (ltr_1 == (ltr_2 - ' ')) || (ltr_1 == (ltr_2 + ' ')))
    return 1;
  else
    return 0;
}

// Function which saves a character into an array as an available following character (based on ASCII codes).
void save_char(int i, char chars[], char city[])
{
  char found_char;

  if (city[i] >= 'a' && city[i] <= 'z'){ // if the character is a small letter a-z (ASCII 97-122)
    found_char = city[i] - ' ';
    chars[found_char - 'A'] = found_char;
  }
  else if (city[i] >= 'A' && city[i] <= 'Z'){ // if the character is a capital letter A-Z (ASCII 65-90)
    found_char = city[i];
    chars[found_char - 'A'] = found_char;
  }
  else if (city[i] == ' '){ // if the character is a space (ASCII 32)
    found_char = city[i];
    chars[found_char - 6] = found_char;
  }
  else if (city[i] == '-'){ // if the character is an en dash (ASCII 45)
    found_char = city[i];
    chars[found_char - 18] = found_char;
  }
}

// Function which finds out whether there are any available following characters or not.
bool find_print_chars(char chars[], int enabled_cities)
{
  bool char_available = false;
  int i = 0;

  for (i = 0; i < 28; i++){
    if (chars[i] != 0){
      char_available = true;
      break;
    }
  }

  /* If there are available characters and multiple options, print all available chars, and if there is
     only one available option, return a positive value so that it can be printed later. */
  if (char_available && (enabled_cities > 1)){
    printf("Enable: ");
    for (i = 0; i < 28; i++){
      printf("%c", chars[i]);
    }
    printf("\n");
    return 1;
  }
  else if (char_available && (enabled_cities == 1)){
    return 1;
  }
  else
    return 0;
}

int main(int argc, char* argv[])
{
  // Declaring or initializing the necessary variables.
  bool found_city = false;
  int i, enabled_cities = 0;
  char chars[28] = {0}, city[BUFSIZE], result[BUFSIZE]; // result - for storing the unique found result

  if (argc > 1){ // If an argument was set, search for matching results.
    int len = string_length(argv[1]);

      errors(argc, argv); // Input error detecting function

    // Load city names one by one from file to stdin.
    while (fgets(city, BUFSIZE, stdin) != NULL){
      bool equal = true;
      i = 0;

      /* Continue the cycle if the results match and there are still some characters
         in the input argument and the city name. */
      while (city[i] != '\n' && argv[1][i] != '\0' && i < len && equal == true){
        if (!(letter_compare(city[i], argv[1][i])))
          equal = false;

        i++;
      }

      /* If the results match, decide whether a city is found or there are some
         characters that can be enabled. */
      if (equal){
        if (city[i] == '\n'){
          printf("Found: %s", city);
          found_city = true;
        }
        else if (city[i] == '\0'){ // For the last line in the city database file.
          printf("Found: %s\n", city);
          found_city = true;
        }
        else {
          save_char(i, chars, city);

          // If the current city is the first available option, store its name in case it is the only one.
          if (enabled_cities == 0){
            for (int j = 0; city[j] != '\0'; j++)
              result[j] = city[j];
          }
          enabled_cities++;
        }
      }
    }
    if (enabled_cities == 1){
      printf("Found: %s", result);
      found_city = true;
    }

    // If no cities or available characters are found.
    if (!(find_print_chars(chars, enabled_cities)) && !(found_city))
      printf("Not found\n");

  }
  else { // If no argument was set, search for available characters that lead to a result.
    while(fgets(city, BUFSIZE, stdin)){
      i = 0;
      save_char(i, chars, city);
      enabled_cities++;
    }
    find_print_chars(chars, enabled_cities);
  }

  return 0;
}
