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

class InputSystem {
protected:
  ifstream fin;
  string line;
  int pos;

  int reset () {
    getline(fin, line);
    if (fin.eof() == true) {
      return -1;
    }
    pos = 0;
    return 0;
  }

public:
  InputSystem (string fname) {
    fin.open(fname.c_str());
    if (fin.fail() == true) {
      cerr << "Error opening file '" << fname << "'\n";
      exit(1);
    }
    int r = reset();
    if (r < 0) then {
      exit(0);
    }
  }

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

  void back () {
    if (pos > 0) {
      pos--;
    }
  }

  void error (string message, int indicators = 1) {
    cout << line << " at pos " << pos - indicators << endl;
    for (int i = 0; i < pos - indicators; i++) {
      cout << " ";
    }
    for (int i = 0; i < indicators; i++) {
      cout << "^";
    }
    cout << endl << message << endl;
  }
  
  ~InputSystem () {
    fin.close();
  }
};

int main () {

  return 0;
}
