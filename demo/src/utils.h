#ifndef __UTILS_H__
#define __UTILS_H__

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include "refmem.h"

//Prints question. clear input-buffer. Will only return a uppercase char followed by 1 more digits.
// Example: "A25", "B1" "C374652365475"
// \param question: a string for displaying valid user inputs
char *ask_question_shelf(char *question);

// Prints a string and return user input if, and only if,
// the inputed string is a floating point decimal.
// \param question: a string for displaying valid user inputs
double ask_question_float(char *question);

// Prints a string and return user input if, and only if,
// the inputed string only contains digits.
int ask_question_int(char *question);

// Prints a string and return user input if, and only if,
// the inputed string is non-empty.
// \param question: a string for displaying valid user inputs
char *ask_question_string(char *question);

// Prints a string and return user input if, and only if,
// the inputed string is a single letter.
// \param question: a string for displaying valid user inputs
char ask_question_char(char *question);

char ask_question_menu(char *question, char *menu_choices);

#endif
