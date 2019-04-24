/********************************************************************/
/*                                                                  */
/*  File: proj2.c                                                   */
/*  Date: 19. 11. 2017                                              */
/*  Author: Dominik Vecera, xvecer23@stud.fit.vutbr.cz              */
/*  Project: Iterative computations                                 */
/*                                                                  */
/********************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define MAX_ITER 10 // number of necessary iterations for accurately computing cfrac_tan
#define TAN_MIN_ITER 0
#define TAN_MAX_ITER 14 // number of iterations when comparing tan should be between 0 and 14

// Function which counts tan of an angle in radians using Taylor series.
double taylor_tan(double x, unsigned int n)
{
  // num - numerators, denom - denominators, both the first 13 of Taylor series for tan(x)
  long long num[13] = {1, 1, 2, 17, 62, 1382, 21844, 929569, 6404582, 443861162,
    18888466084, 113927491862, 58870668456604};
  long long denom[13] = {1, 3, 15, 315, 2835, 155925, 6081075, 638512875, 10854718875,
    1856156927625, 194896477400625, 49308808782358125, 3698160658676859375};
  double result = x; // for storing the result
  double xpow = x; // for multiplying by x^2 during each iteration
  for (unsigned int i = 1; i < n; i++){
    xpow *= (x*x);
    result += ((num[i] * xpow) / denom[i]);
  }

  return result;
}

// Function which counts tan of an angle in radians using continued fraction computation.
double cfrac_tan(double x, unsigned int n)
{
  double cf = INFINITY;
  double a = n * 2 + 1;

  for (unsigned int k = n+1; k > 0; k--)
  {
    cf = 1.0 / ((a/x) - cf);
    a -= 2;
  }

  return cf;
}

// Function which prints the manual for program usage.
void print_help()
{
  printf(
  "How to use the program:\n\n"
  "Compare the accuracies of different methods of computing tan:\n"
  "-------------------------------------------------------------\n"
  "--tan A N M\n"
  "A = angle in radians\n"
  "N, M - in which iterations the results are to be compared, 0 < N <= M < 14\n\n"
  "Output: I M T TE C CE\n"
  "I  = iteration number\n"
  "M  = result from Math.h library\n"
  "T  = result computed with Taylor series\n"
  "TE = absolute error between Math.h and Taylor series computations\n"
  "C  = result computed with continued fractions\n"
  "CE = absolute error between Math.h and continued fraction computations\n\n"
  "Count length and height using continued fraction tan computation:\n"
  "-------------------------------------------------------------------\n"
  "[-c X] -m A [B]\n"
  "A, B = angles in radians (B - optional), both in interval (0 ; 1.4>\n"
  "X = height of meter (optional, in interval (0 ; 100>, implicit value = 1.5 m)\n");
}

// Function which converts a string to an integer number.
int str_to_int(char *s)
{
  char *endptr;
  int num = strtol(s, &endptr, 0);
  if (endptr[0] == '\0' && !isinf(num) && !isnan(num))
    return num;
  else
    return 0;
}

// Function which converts a string to a floating-point number (type double).
double str_to_dbl(char *s)
{
  char *endptr;
  double num = strtod(s, &endptr);
  if (endptr[0] == '\0' && !isinf(num) && !isnan(num))
    return num;
  else
    return 0;
}

// Enumeration for each possible situation of input arguments.
typedef enum {
    GENERAL_ERROR, // general error for when arguments are set the wrong way
    HELP,          // show help
    TAN,           // count tan
    NO_ARGS,       // when no arguments were set
    DIST_A,        // measure distance - set are: angle alpha
    DIST_H_AB,     // measure distance and height - set are: angles alpha and beta
    DIST_CA,       // measure distance - set are: angle alpha, height c
    DIST_H_CAB     // measure distance and height - set are: angles alpha and beta, height c
} argoptions;

// Function which finds out if the set angle is within a correct range.
int correct_angle(double angle)
{
  if (angle > 0 && angle <= 1.4) // angle in radians must be in interval (0;1.4>
    return 1;
  else
    return 0;
}

// Function which finds out if the set height is within a correct range.
int correct_height(double height)
{
  if (height > 0 && height <= 100) // height in meters must be in interval (0;100>
    return 1;
  else
    return 0;
}

// Function which iteratively counts tan of an angle with various methods.
int count_tan(double tg, unsigned int n, unsigned int m)
{
  double M = tan(tg);

  for (;n <= m; n++){
    double T = taylor_tan(tg, n);
    double TE = fabs(T-M);
    double C = cfrac_tan(tg, n);
    double CE = fabs(C-M);
    printf("%d %e %e %e %e %e\n", n, M, T, TE, C, CE);
  }
  return 0;
}

// Function which counts distance when an angle is set in radians.
int distance_a(double angle)
{
  printf("%.10e\n", 1.5 / cfrac_tan(angle, MAX_ITER));
  return 1;
}

// Function which counts distance and height when two angles are set in radians.
int distance_height_ab(double angle_a, double angle_b)
{
  double d = 1.5 / cfrac_tan(angle_a, MAX_ITER);
  printf("%.10e\n", d);
  printf("%.10e\n", 1.5 + cfrac_tan(angle_b, MAX_ITER) * d);
  return 1;
}

// Function which counts distance when a height and an angle in radians is set.
int distance_ca(double height, double angle_a)
{
  double d = height / cfrac_tan(angle_a, MAX_ITER);
  printf("%.10e\n", d);
  return 1;
}

// Function which counts distance and height when a height and two angles in radians are set.
int distance_height_cab(double height, double angle_a, double angle_b)
{
  double d = height / cfrac_tan(angle_a, MAX_ITER);
  printf("%.10e\n", d);
  printf("%.10e\n", height + cfrac_tan(angle_b, MAX_ITER) * d);
  return 1;
}

// Function which finds out how to proceed based on the set arguments
argoptions check_args(int argc, char* argv[])
{
  if (argc > 1){
    if ((argc == 2) && strcmp(argv[1], "--help") == 0)
      return HELP;
    else if (argc == 5 && strcmp(argv[1], "--tan") == 0 && str_to_dbl(argv[2])
             && str_to_int(argv[3]) > TAN_MIN_ITER && str_to_int(argv[3]) < TAN_MAX_ITER
             && str_to_int(argv[4]) > TAN_MIN_ITER && str_to_int(argv[4]) < TAN_MAX_ITER
             && str_to_int(argv[3]) <= str_to_int(argv[4]))
      return TAN;
    else if (argc == 3 && strcmp(argv[1], "-m") == 0
             && correct_angle(str_to_dbl(argv[2])))
      return DIST_A;
    else if (argc == 4 && strcmp(argv[1], "-m") == 0
             && correct_angle(str_to_dbl(argv[2])) && correct_angle(str_to_dbl(argv[3])))
      return DIST_H_AB;
    else if (argc == 5 && strcmp(argv[1], "-c") == 0 && correct_height(str_to_dbl(argv[2]))
             && strcmp(argv[3], "-m") == 0 && correct_angle(str_to_dbl(argv[4])))
      return DIST_CA;
    else if (argc == 6 && strcmp(argv[1], "-c") == 0 && correct_height(str_to_dbl(argv[2]))
             && strcmp(argv[3], "-m") == 0 && correct_angle(str_to_dbl(argv[4]))
             && correct_angle(str_to_dbl(argv[5])))
      return DIST_H_CAB;
    else
      return GENERAL_ERROR;
  }
  else
    return NO_ARGS;
}

int main(int argc, char* argv[])
{
  // Chooses appropriate behavior of the program based on the input arguments.
  switch(check_args(argc, argv)){
    case NO_ARGS:
      fprintf(stderr, "No argument was set.\n"
                      "Use argument --help for program function explanation.\n");
      return EXIT_FAILURE;
    case HELP:
      print_help();
      return EXIT_SUCCESS;
    case TAN:
      count_tan(str_to_dbl(argv[2]), str_to_int(argv[3]), str_to_int(argv[4]));
      return EXIT_SUCCESS;
    case DIST_A:
      distance_a(str_to_dbl(argv[2]));
      return EXIT_SUCCESS;
    case DIST_H_AB:
      distance_height_ab(str_to_dbl(argv[2]), str_to_dbl(argv[3]));
      return EXIT_SUCCESS;
    case DIST_CA:
      distance_ca(str_to_dbl(argv[2]), str_to_dbl(argv[4]));
      return EXIT_SUCCESS;
    case DIST_H_CAB:
      distance_height_cab(str_to_dbl(argv[2]), str_to_dbl(argv[4]), str_to_dbl(argv[5]));
      return EXIT_SUCCESS;
    case GENERAL_ERROR:
      fprintf(stderr, "Something is wrong with the input - use argument --help for help.\n");
      return EXIT_FAILURE;
    default:
      return EXIT_SUCCESS;
  }
}
