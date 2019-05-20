/*************************************************************************
 *                                                                       *
 *  This Source Code Form is subject to the terms of the Mozilla Public  *
 *  License, v. 2.0. If a copy of the MPL was not distributed with this  *
 *  file, You can obtain one at http://mozilla.org/MPL/2.0/.             *
 *                                                                       *
 *************************************************************************/

#include <iostream>
#include <fstream>
#include <string>

using namespace std;


const char EOFCHAR = -1;

                   //Input System class which returns chars from a given file
class InputSystem {
protected:
  ifstream fin;
  string line;
  int pos;

                //Get the next line from the file (return -1 if it is the end of file) 
  int reset () {
    getline(fin, line);
    if (fin.eof() == true) {
      return -1;
    }
    pos = 0;
    return 0;
  }

public:
                                //Opens the file and preps the system buffer
  InputSystem (string & fname) {
    fin.open(fname.c_str());
    if (fin.fail() == true) {
      cerr << "Error opening file '" << fname << "'\n";
      exit(1);
    }
    int r = reset();
    if (r < 0) {        //Exit if it is an empty file
      exit(0);
    }
  }

                //Return the next character from the buffer (refill it if necessary)
  char next () {
    if (pos >= line.length()) {
      int r = reset();
      if (r < 0) {
        return EOFCHAR;
      }
    }
    pos++;
    return line[pos - 1];
  }

                //Moves attention back in the buffer if possible
  void back () {
    if (pos > 0) {
      pos--;
    }
  }

                //Prints an error message with the contents of the line
  void error (string message, int indicators = 1) {  //Indicators dictates how many carats to display
    cout << line << " at pos " << pos - indicators << endl;
    for (int i = 0; i < pos - indicators; i++) {
      cout << " ";
    }
    for (int i = 0; i < indicators; i++) {
      cout << "^";
    }
    cout << endl << message << endl;
  }
  
                //Closes the file
  ~InputSystem () {
    fin.close();
  }
};

//Different types of tokens found in C++
enum token_type {
  Lvar, Lnum, Loperator, Lreserved, LEOF, Lopar, Lcpar, Locur, Lccur, Lscol, Lcol, Lcomstart, Lcomend
};

//Token object, contains the other info that'll be needed by the parser
struct Token {
  token_type type;
  string name;
  int value, size;  //Size is the number of spaces the token occupies, for error printing

  Token ()
  { }

  Token (const token_type t, const string n, const int v, const int s) {
    type = t;
    name = n;
    value = v;
    size = s;
  }
};

                       //Lexical Analyzer - takes characters from the internal Input System and groups them into tokens
class LexicalAnalyzer {
protected:
  InputSystem * IS;
  bool lex;            //Bool to handle whether the next function should return the previous token or get a new one

public:
  LexicalAnalyzer (string & fname) {
    IS = new InputSystem(fname);
    lex = true;
  }

  Token * next () {
    static Token * T;
    if (lex == false) {   //Check if a new token should be given or not
      lex = true;
      return T;
    }
    char c = IS->next();
    while (c == ' ' || c == '\t' || c == '\n') {    //Skip leading spaces (TODO: check if newlines should be skipped)
      c = IS->next();
    }
    Token * temp = new Token;     //Temporary token made (don't lose the stored static token in case the file has errors)
    switch (c) {
    case ';':
      temp->type = Lscol;    //Semicolon type
      temp->size = 1;
      break;
    case ':':
      temp->type = Lcol;     //Colon type
      temp->size = 1;
      break;
    case '{':
      temp->type = Locur;    //Open curly brace type
      temp->size = 1;
      break;
    case '}':
      temp->type = Lccur;    //Closed curly brace type
      temp->size = 1;
      break;
    case '(':
      temp->type = Lopar;    //Open parenthesis type
      temp->size = 1;
      break;
    case ')':
      temp->type = Lcpar;    //Closed parenthesis type
      temp->size = 1;
      break;
    case '0': case '1': case '2': case '3': case '4': case '5': case '6': case '7': case '8': case '9':
      temp->type = Lnum;     //Integer handler (potentially unnnecessary if just using for formatting)
      temp->value = 0;
      temp->size = 0;
      while (c >= '0' && c <= '9') {
        temp->value = (temp->value * 10) + (c - '0');
        (temp->size)++;
        c = IS->next();
      }
      IS->back();                       //Put back the char that broke the while loop
      break;
    case 'A': case 'B': case 'C': case 'D': case 'E': case 'F': case 'G': case 'H': case 'I': case 'J': case 'K': case 'L': case 'M':
    case 'N': case 'O': case 'P': case 'Q': case 'R': case 'S': case 'T': case 'U': case 'V': case 'W': case 'X': case 'Y': case 'Z':
    case 'a': case 'b': case 'c': case 'd': case 'e': case 'f': case 'g': case 'h': case 'i': case 'j': case 'k': case 'l': case 'm':
    case 'n': case 'o': case 'p': case 'q': case 'r': case 's': case 't': case 'u': case 'v': case 'w': case 'x': case 'y': case 'z':
      temp->name = "";
      while (c >= 'a' && c <= 'z' || c >= 'A' && c <= 'Z' || c >= '0' && c <= '9' || c == '_') {
        temp->name += c;
        c = IS->next();
      }
      temp->size = (temp->name).length();
      IS->back();                       //Put back the char that broke the while loop
      if (temp->name == "bool" || temp->name == "break" || temp->name == "case" || temp->name == "char" || temp->name == "class" ||
          temp->name == "const" || temp->name == "continue" || temp->name == "default" || temp->name == "delete" || temp->name == "do" ||
          temp->name == "double" || temp->name == "else" || temp->name == "enum" || temp->name == "false" || temp->name == "float" ||
          temp->name == "for" || temp->name == "if" || temp->name == "int" || temp->name == "long" || temp->name == "namespace" ||
          temp->name == "new" || temp->name == "private" || temp->name == "protected" || temp->name == "public" || temp->name == "return" ||
          temp->name == "short" || temp->name == "signed" || temp->name == "sizeof" || temp->name == "static" || temp->name == "struct" ||
          temp->name == "switch" || temp->name == "template" || temp->name == "this" || temp->name == "true" || temp->name == "typedef" ||
          temp->name == "unsigned" || temp->name == "using" || temp->name == "virtual" || temp->name == "virtual" || temp->name == "void" ||
          temp->name == "while"
      ) {  //Reserved word recognition (TODO: see which of these are actually important to have in a formatting utility)
        temp->type = Lreserved;       //Also consider a hash table type thing so user-defined words can be inserted
      } else {
        temp->type = Lvar;
      }
      break;
    case EOFCHAR:
      temp->type = LEOF;     //EOF type
      temp->size = 1;
      break;
    case '~':
      temp->type = Loperator;  //Operator but used only for destructor syntax?
      temp->name = "";
      temp->name += c;
      temp->size = 1;
      break;
    case '=': case '!': case '^':   //All the below are operator cases (see if there's a way to consolidate everything)
      temp->type = Loperator;
      temp->name = "";
      temp->name += c;
      c = IS->next();
      if (c == '=') {      //==, !=, ^=
        temp->name += c;
      } else {
        IS->back();
      }
      temp->size = temp->name.length();
      break;
    case '+':
      temp->type = Loperator;
      temp->name = "";
      temp->name += c;
      c = IS->next();
      if (c == '=' || c == '+') {   //+=, ++
        temp->name += c;
      } else {
        IS->back();
      }
      temp->size = temp->name.length();
      break;
    case '-':
      temp->type = Loperator;
      temp->name = "";
      temp->name += c;
      c = IS->next();
      if (c == '=' || c == '-') {   //-=, --
        temp->name += c;
      } else {
        IS->back();
      }
      temp->size = temp->name.length();
      break;
    case '*':
      temp->type = Loperator;
      temp->name = "";
      temp->name += c;
      c = IS->next();
      if (c == '=') {          // *=
        temp->name += c;
      } else if (c == '/') {   // */
        temp->type = Lcomend;        //Comment end type
        temp->name += c;
      } else {
        IS->back();
      }
      temp->size = temp->name.length();
      break;
    case '/':
      temp->type = Loperator;
      temp->name = "";
      temp->name += c;
      c = IS->next();
      if (c == '=') {         // /=
        temp->name += c;
      } else if (c == '/' || c == '*') {   // //, /*
        temp->type = Lcomstart;      //See if it's necessary to have separate token types for comment start/end
        temp->name += c;
      } else {
        IS->back();
      }
      temp->size = temp->name.length();
      break;
    case '&':
      temp->type = Loperator;
      temp->name = "";
      temp->name += c;
      c = IS->next();
      if (c == '=' || c == '&') {   // &=, &&
        temp->name += c;
      } else {
        IS->back();
      }
      temp->size = temp->name.length();
      break;
    case '|':
      temp->type = Loperator;
      temp->name = "";
      temp->name += c;
      c = IS->next();
      if (c == '=' || c == '|') {   // |=, ||
        temp->name += c;
      } else {
        IS->back();
      }
      temp->size = temp->name.length();
      break;
    case '<':
      temp->type = Loperator;
      temp->name = "";
      temp->name += c;
      c = IS->next();
      if (c == '=') {        // <=
        temp->name += c;
      } else if (c == '<') { // << or <<=
        temp->name += c;
        c = IS->next();
        if (c == '=') {      // <<=
          temp->name += c;
        } else {
          IS->back();
        }
      } else {
        IS->back();
      }
      temp->size = temp->name.length();
      break;
    case '>':
      temp->type = Loperator;
      temp->name = "";
      temp->name += c;
      c = IS->next();
      if (c == '=') {        // >=
        temp->name += c;
      } else if (c == '>') { // >> or >>=
        temp->name += c;
        c = IS->next();
        if (c == '=') {      // >>=
          temp->name += c;
        } else {
          IS->back();
        }
      } else {
        IS->back();
      }
      temp->size = temp->name.length();
      break;
    default:
      IS->error("Improper character");
      delete temp;      //No type to return (but don't lose the previous token)
      return NULL;
    }
    delete T;      //If a valid token was found make it the previous token
    T = temp;
    return T;
  }

  void back () {   //Set lex to false so the next lexeme will be the previous returned
    lex = false;
  }

                   //Error printing message for lexemes (taking into account their size)
  void error (string message, Token * T) {
    IS->error(message, T->size);
  }

  ~LexicalAnalyzer () {
    delete IS;
  }
};

int main () {

  return 0;
}
