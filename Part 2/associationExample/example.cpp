//*****************************************************************************
// purpose: CSE 4713 / 6713 Assignment 3 Reference Solution
//*****************************************************************************
#include <stdio.h>
#include <iostream>
#include <string>
#include <map>
#include <stdlib.h>

using namespace std;

#include "parseTree.h"


int main() {
  // Create tree with left association
  expressionP left = makeApplication(makeApplication(makeVariable("a"),
						     makeVariable("b")),
				     makeVariable("c")) ;
  expressionP right =makeApplication(makeVariable("a"),
				     makeApplication(makeVariable("b"),
						     makeVariable("c"))) ;

  cout << "left = " << left->String() << endl ;
  cout << "right = " << right->String() << endl ;
  // Example of substitution rule

  expressionP substitute = makeSlashDot("x",makeApplication(makeVariable("x"),
							    makeVariable("x"))) ;
  expressionP subapply = makeApplication(substitute,makeVariable("repeat")) ;
  cout << "substitute rule=" << subapply->String() << endl ;
}
