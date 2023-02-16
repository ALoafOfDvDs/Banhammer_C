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
    // printf("current line: %s\n",parser->current_line);
    free(parser);
    return NULL;
  }
  // printf("current line: %s",parser->current_line);

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

// static char * my_strdup(char * word) {
//     char *copy;
//     int i = 0;
//     while(word[i] != '\0') {
//         i += 1;
//     }
//     i += 1; // for the null terminator
//     // copy = (char *)malloc(i);
//     copy = (char *)calloc(i, sizeof(char));

//     int j = 0;
//     while ((copy[j] = word[j]) != '\0') {
//         j += 1;
//     }
//     return copy;
// }

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
    // printf("got back to here\n");
    if (p->current_line[p->line_offset] == '\n') {
      // printf("got here\n");
      // fgets(p->current_line, MAX_PARSER_LINE_LENGTH +1, p->f);
      if (fgets(p->current_line, MAX_PARSER_LINE_LENGTH + 1, p->f) == NULL) {
        return false;
      }
      p->line_offset = 0;
      // printf("offset: %u\n", p->line_offset);
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

  // // printf("got to parser here\n");
  // // printf("current line inside nextword: %s", p->current_line);
  // while(p->current_line[0] == '\n') { // THIS NEEDS TO BE CHANGED
  //     // printf("got here\n");
  //     if(fgets(p->current_line, MAX_PARSER_LINE_LENGTH +1, p->f) == NULL) {
  //         // printf("calls fgets\n");
  //         p->current_line[0]='\0';
  //         break;
  //     }
  //     // printf("got to parser here1\n");
  // }

  // int delimit_check = 0;
  // while(!isalnum(p->current_line[delimit_check]) &&
  // p->current_line[delimit_check] != '-' && p->current_line[delimit_check] !=
  // '\'') {
  //     delimit_check += 1;
  // }

  // // printf("current line: %d\n", p->current_line[0]=='\n');
  // //i get an error that the parameter word is set but not used. Generally it
  // is passed in unitialized or NULL if(p == NULL || p->current_line[0] ==
  // '\0') {
  //     // printf("returning false\n");
  //     return false;
  // }

  // // char * var = word;
  // // word = var;

  // char * line = my_strdup(p->current_line);

  // // the below while loop should loop until it finds a delimiting character
  // int index = 0;
  // while(isalnum(line[index]) || line[index] == '-' || line[index] == '\'') {
  //     index += 1;
  // }
  // // printf("index: %d\n", index);

  // int length_of_line = 0;
  // while(p->current_line[length_of_line] != '\0') {
  //     length_of_line += 1;
  // }
  // length_of_line += 1;

  // char next_word[index + 1];
  // for (int i = 0; i < index; i += 1) {
  //     next_word[i] = tolower(line[i]);
  // }
  // next_word[index] = '\0';
  // for (int i = 0; i < index + 1; i += 1) {
  //     word[i] = next_word[i];
  // }
  // // printf("what I get from the line: %s\n", next_word);
  // // printf("char at index: -%c-\n", p->current_line[index]);

  // // to write this loop we need to know a couple things
  // // index is the point on the current line in the parser, and it should be
  // the location for some delimiting character
  // // the above line about the index means that i do not want the char at the
  // index in the remaining bit of the line, I want to start with the next valid
  // word character
  // // length of line is the entire length of the parser, INCLUDING the final
  // NULL terminator int secondary_iterator = 0; if (p->current_line[index] !=
  // '\0') {
  //     // printf("character at currentline is not null terminator\n");
  //     for (int i = index + 1; i < length_of_line; i += 1){
  //         p->current_line[secondary_iterator] = p->current_line[i];
  //         secondary_iterator += 1;
  //     }
  // }
  // else {
  //     p->current_line[0] = '\0';
  // }
  // // printf("currentline after adjustment: %s\n", p->current_line);

  // // next_word[index] = '\0';
  // // for (int i = 0; i < index + 1; i += 1) {
  // //     word[i] = next_word[i];
  // // }
  // // word = next_word;
  // // printf("what i should return %s\n", next_word);
  // // printf("\ni think I am returning %s (in parser.c)\n", word);
  // if(line[index] == '\n') {
  //     // if our terminating character was a newline, then we need to tell the
  //     parser to go to the next line
  //     // printf("should get here on the second go around\n");
  //     if (fgets(p->current_line, MAX_PARSER_LINE_LENGTH + 1, p->f) == NULL) {
  //         // in this case we are completely done and need to be fully done
  //         p->current_line[0] = '\0';
  //         free(line);
  //         // printf("returning true line 105\n");
  //         return true;
  //     }
  //     // otherwise, we have another line to go onto, which we should get
  //     before we return. This should theoretically be okay since we are
  //     copying values, and not pointers
  //     // as such, we can move what our main string is, and not have it affect
  //     the return value

  //     // ACTUALLY BY CALLING fgets IN THE IF STATEMENT THIS UPDATE HAS
  //     ALREADY HAPPENED. THE POINT THOUGH, IS THAT WE MAY WANT TO SET A
  //     VARIABLE HERE TO SAY THAT WE ARE DONE
  //     // printf("no more lines to read\n");
  // }
  // free(line);
  // // printf("returning true line 115\n");
  // return true;
}
