//clang-format off
#include "parser.h"

#include <ctype.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
//clang-format on

// #define MAX_PARSER_LINE_LENGTH 1000

struct Parser {
  FILE *f;
  char current_line[MAX_PARSER_LINE_LENGTH + 1];
  uint32_t line_offset;
};

Parser *parser_create(FILE *f) {
  Parser *parser = (Parser *)malloc(sizeof(Parser));
  parser->line_offset = 0;
  parser->f = f;
  if (fgets(parser->current_line, MAX_PARSER_LINE_LENGTH + 1, f) == NULL) {
    // This means EOF is the inital file so..... just end the entire thing?
    free(parser);
    return NULL;
  }

  // I DO NOT THINK I NEED TO INITIALIZE THE CURRENT LINE, OTHERWISE I WOULD
  // SKIP A LINE correction, I could use a Do-While, but if I do that then I
  // might mess up the parser, since I would then need to change to check that
  // the parser is not NULL
  return parser;
}

void parser_delete(Parser **p) {
  fclose((*p)->f);
  free(*p);
  *p = NULL;
}


bool next_word(Parser *p, char *word) {

  // There has been a significant change in how I did my next word program. This
  // is because despite my original method working quite well, it was extremely
  // difficult to account for the case in which we have 2 delimiting characters
  // right after each other on the same line. As such, given a now intimate
  // understanding of what all of the variables mean, I could rewrite the
  // function extremely efficiently (in my opinion)

  // while we are on a delimiting character, check if that character is a
  // newline or null terminator, and call fgets or return false respectively if
  // fgets returns NULL then there are no more new lines and we can again return
  // false otherwise, increment the line offset so that we can move onto a valid
  // char
  while (!isalnum(p->current_line[p->line_offset]) &&
         p->current_line[p->line_offset] != '-' &&
         p->current_line[p->line_offset] != '\'') {
    if (p->current_line[p->line_offset] == '\n') {
      if (fgets(p->current_line, MAX_PARSER_LINE_LENGTH + 1, p->f) == NULL) {
        return false;
      }
      p->line_offset = 0;
      continue;
    } else if (p->current_line[p->line_offset] == '\0') {
      return false;
    }
    p->line_offset += 1;
  }

  uint32_t starting_offset = p->line_offset;
  while (isalnum(p->current_line[p->line_offset]) ||
         p->current_line[p->line_offset] == '-' ||
         p->current_line[p->line_offset] == '\'') {
    p->line_offset += 1;
  }
  uint32_t difference = p->line_offset - starting_offset;
  char next_word[difference + 1]; // adding 1 to account for null terminator
  uint32_t index = 0;
  for (uint32_t i = starting_offset; i < p->line_offset; i += 1) {
    next_word[index] = tolower(p->current_line[i]);
    index += 1;
  }
  next_word[index] = '\0';
  for (uint32_t i = 0; i < index + 1; i += 1) {
    word[i] = next_word[i];
  }
  return true;
}
