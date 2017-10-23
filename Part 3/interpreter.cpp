//*****************************************************************************
// purpose: CSE 4713 / 6713 Assignment 3
// assignment: Implementation of the supersub language interpreter
// name: Joshua Church
// netID: jqc10
//*****************************************************************************

#include <stdio.h>
#include <iostream>
#include <string>
#include <map>
#include <stdlib.h>

using namespace std;
#include "parseTree.h"
#include "interpreter.h"

// Recusively rename variables as long as they are not currently bound by
// slashdot operator
expressionP recursiveRenameVars(expressionP e, const string &name,
			     const string &toname) {
  switch(e->Type) {
  case Expression::VARIABLE:
    // If it is a variable and the name matches, do rename
    if(e->name == name)
      e->name = toname ;
  case Expression::COMBINATOR:
    // Ignore combinator variables
    return e ;
  case Expression::SLASHDOT:
    // If operator not reusing name, continue to rename right branch
    if(e->name != name) 
      e->updateRight(recursiveRenameVars(e->right,name,toname)) ;
    return e ;
  case Expression::APPLICATION:
    // rename both sides of application
    e->updateLeft(recursiveRenameVars(e->left,name,toname)) ;
    e->updateRight(recursiveRenameVars(e->right,name,toname)) ;
    return e ;
  default:
    throw "Unknown expression type in recursiveRenameVars()" ;
  }
}

// Recursively replace the given expression
// Example: $double --> \x. (x x)
expressionP recursiveReplaceExpression(expressionP e, const string &name, expressionP expr, int &count, bool &variableSubstitution) {
    switch(e->Type) {
        case Expression::VARIABLE:
            // If it is a variable and the name matches, return a deep copy of the expression after
            // removing any repeats of the expression (if needed).
            if(e->name == name) {
                if (variableSubstitution)
                    return canonicalRemoveRepeats(expr->Copy(), count);
                
                else {
                    variableSubstitution = true;
                    return expr->Copy();
                }
            }
        
        // If combinator found, just return the expression (as we do not need to modify anything).
        case Expression::COMBINATOR:
            return e ;
            
        // If the expressions current name is not equal to the passed in string name,
        // replace right side of the expression with the appropriate substitution
        case Expression::SLASHDOT:
            if(e->name != name)
                e->updateRight(recursiveReplaceExpression(e->right,name,expr, count, variableSubstitution)) ;
            return e ;
            
        // If an application is found, we must replace both the left and right side of the application
        // with the appropriate substitution.
        case Expression::APPLICATION:
            e->updateLeft(recursiveReplaceExpression(e->left,name,expr, count, variableSubstitution)) ;
            e->updateRight(recursiveReplaceExpression(e->right,name,expr, count, variableSubstitution)) ;
            return e ;
            
        default:
            throw "Unknown expression type in recursiveReplaceExpression()" ;
    }
}

// The following function checks to see if the current passed in expression
// contains an unbound variable.
bool hasUnboundVars(expressionP e) {
    expressionP temp = e->Copy();
    switch (temp->Type) {
        // Check if the current variable is within the alloted range.
        case Expression::VARIABLE: {
            const char front = e->name[0];
            return front >= 'A' && front <= 'Z';
        }
            
        // No checking is needed for the combinator.
        case Expression::COMBINATOR:
            return false;
            
        // If a slashdot is found, recursively call the right side of the expression.
        case Expression::SLASHDOT:
            return hasUnboundVars(e->right);
            
        // If an application is found, recursively check the left or right side
        // of the deep copied expression.
        case Expression::APPLICATION:
            return hasUnboundVars(temp->left) || hasUnboundVars(temp->right);
        
        // If no case is found, throw a flag warning of the error.
        default:
            cout << temp->String() << endl;
            throw "Unknown expression type in hasUnboundVars()";
    }
}

// Recursively mark all bound variables by prepending an underscore to the 
// variable name.
expressionP markBoundVariables(expressionP e) {
  switch(e->Type) {
  case Expression::VARIABLE:
  case Expression::COMBINATOR:
    return e ;
  case Expression::SLASHDOT:
    { // For slashdot operator we first rename the variable by adding an 
      // underscore to mark this as a bound variable
      string name = e->name ;
      string toname = "_"+name ;
      e->name = toname ;
      // Update all bound variables in right expression
      e->updateRight(recursiveRenameVars(e->right,name,toname)) ;
      // Now mark any other slashdot bound variables on right side
      e->updateRight(markBoundVariables(e->right)) ;
      return e ;
    }
  case Expression::APPLICATION:
    // mark both sides
    e->updateLeft(markBoundVariables(e->left)) ;
    e->updateRight(markBoundVariables(e->right)) ;
    return e ;
  default:
    throw "Unknown expression type in markBoundVariables()" ;
  }
}

// This procedure makes sure that all variables not marked with a leading
// underscore is changed to uppercase so unbound variables cannot capture
// bound ones
expressionP markUnboundVariables(expressionP e) {
  switch(e->Type) {
  case Expression::VARIABLE:
    // If variable first character is lower case, change to upper case
    if(e->name[0] >= 'a' && e->name[0] <= 'z') 
      e->name[0] = e->name[0] - 'a' + 'A' ;
  case Expression::COMBINATOR:
    return e ;
  case Expression::SLASHDOT:
    e->updateRight(markUnboundVariables(e->right)) ;
    return e ;
  case Expression::APPLICATION:
    e->updateLeft(markUnboundVariables(e->left)) ;
    e->updateRight(markUnboundVariables(e->right)) ;
    return e ;
  default:
    throw "Unknown expression type in markUnboundVariables()" ;
  }
}

// Convert integer count to a simple string for a variable name composed of
// lower case letters
string integerToName(int count) {
  char namelist[] = "xyzabcdefghijklmnopqrstuvw" ;
  string name ;
  while(count > 25) {
    name = namelist[count%26] + name ;
    count = count/26 ;
  }
  name = namelist[count] + name ;
  return name ;
}

// This replaces underscore named bound variables with a unique name
// based on the order that bound variables are encountered while searching
// the expression tree.
expressionP prettyRenameVariables(expressionP e, int &count) {
  switch(e->Type) {
  case Expression::VARIABLE:
  case Expression::COMBINATOR:
    return e ;
  case Expression::SLASHDOT:
    {
      int current_var = count ;
      count++ ;
      prettyRenameVariables(e->right,count) ;
      string name = e->name ;
      string newname = integerToName(current_var) ;
      e->name = newname ;
      e->updateRight(recursiveRenameVars(e->right,name,newname)) ;
    }
    return e ;
  case Expression::APPLICATION:
    e->updateLeft(prettyRenameVariables(e->left,count)) ;
    e->updateRight(prettyRenameVariables(e->right,count)) ;
    return e ;
  default:
    throw "Unknown expression type in prettyRenameVariables()" ;
  }
}

// Change all unbound variables to capitalized variables and 
// rename all bound variables based on count of the binding scope
// so that substititions can be performed without variable capture
expressionP canonicalRenameBoundVars(expressionP e, int &count) {
  e = markBoundVariables(e) ;
  e = markUnboundVariables(e) ;
  e = prettyRenameVariables(e,count) ;
  return e ;
}

// In the case of all repeats, change them to a different letter
expressionP canonicalRemoveRepeats(expressionP e, int &count) {
    e = markBoundVariables(e) ;
    e = prettyRenameVariables(e,count) ;
    return e ;
}


// Perform at most evalDepth steps of pre-order evaluations of input expression
// e.  If printLevel is nonzero, then print progress of evaluation.
expressionP evaluatePreOrder(expressionP e,
                             const expressionDictionary &dictionary,
                             int evalDepth,
                             int printLevel) {
    
    int count = 0;
    int currentDepth = 0;
    bool foundSubstitution = false;
    e = canonicalRemoveRepeats(e, count);

    // If the printLevel flag is set to true, print it.
    if (printLevel != 0)
        cout << "evaluatePreOrder with expression: " << e->String() << endl;

    // Loop until the maximum allowed steps have not been exceeded. (or until completion)
    while (currentDepth < evalDepth) {
        foundSubstitution = false;
        e = preOrderSubstitutionSearch(e, dictionary, count, foundSubstitution);
        
        // If we do not fina any more substitutions, we need to end the loop.
        if (foundSubstitution == false) {
            break;
        }
        
        // If the printLevel flag is set to true, print each step of pre-order
        if (printLevel > 0)
            cout << currentDepth << "--" << e->String() << endl;

        currentDepth++;
        
        // If the maximum allowed steps have exceeded, flag it.
        if (currentDepth == evalDepth)
            cout << "maximum number of steps exceeded!" << endl;

    }
    
    // Reset values and return
    count = 0;
    return canonicalRemoveRepeats(e, count);

}

// Recursive pre-order search for APPLICATION node to substitute and perform substitution.
expressionP preOrderSubstitutionSearch(expressionP e, const expressionDictionary &dictionary, int &count, bool &found) {
    
    switch(e->Type) {
            
        // If we find a variable or a combinator, we just return the expression.
        case Expression::VARIABLE:
            return e;
            
        case Expression::COMBINATOR:
            return e;
        
        // If a slashdot is found, recursively update the right side of the expression
        case Expression::SLASHDOT:
            e->updateRight(preOrderSubstitutionSearch(e->right, dictionary, count, found));
            return e;
        
        case Expression::APPLICATION:
            switch (e->left->Type) {
                    
                // If we get a variable, we do not need to handle it here.
                case Expression::VARIABLE:
                    break;
                
                // If a combinator is found, we need to check if it is currently stored in the
                // combinatorDictionary.
                case Expression::COMBINATOR: {
                    found = true;
                    expressionP comb = dictionary.getItem(e->left->name);
                    
                    // If we cannot find the combinator name in the dictionary, then
                    // throw a flag.
                    if (comb == 0)
                        throw "Not found in the dictionary";
                    
                    // If found in dictionary, remove the repeats and update the left side
                    // of the expression.
                    e->updateLeft(canonicalRemoveRepeats(comb, count));
                    
                    return e;
                }
                
                case Expression::SLASHDOT: {
                    found = true;
                    
                    // Boolean to determine if we have a variable substitution
                    bool variableSubstitution = false;
                    
                    // Make a copy of the right side of the slash dot and replace the expression appropriately
                    return recursiveReplaceExpression(e->left->right->Copy(), e->left->name, e->right->Copy(), count, variableSubstitution);
                }
                case Expression::APPLICATION:
                    break;
                    
                default:
                    throw "Unknown expression type in preOrderSubstitutionSearch Switch Application";
            }
            
            // Check the current node
            if (found)
                return e;
            
            // If nothing found, check left node
            e->updateLeft(preOrderSubstitutionSearch(e->left, dictionary, count, found));
            
            // Check the current node
            if (found)
                return e;
            
            // If nothing found on left node, check right node
            e->updateRight(preOrderSubstitutionSearch(e->right, dictionary, count, found));
            return e;

        default:
            throw "Unknown expression type in preOrderSubstitutionSearch Switch Type";
    }
}


// Perform at most evalDepth steps of post-order evaluations of input expression
// e.  If printLevel is nonzero, then print progress of evaluation.
expressionP evaluatePostOrder(expressionP e,
                             const expressionDictionary &dictionary,
                             int evalDepth,
                             int printLevel) {
    
    int count = 0;
    int currentDepth = 0;
    bool foundSubstitution = false;
    e = canonicalRemoveRepeats(e, count);
    
    if (printLevel != 0) {
        cout << "evaluatePostOrder with expression: " << e->String() << endl;
    }

    // Loop until the maximum allowed steps have not been exceeded. (or until completion)
    while (currentDepth < evalDepth) {
        foundSubstitution = false;
        e = postOrderSubstitutionSearch(e, dictionary, count, foundSubstitution);
        
        if (foundSubstitution == false) {
            break;
        }
        
        if (printLevel > 0) {
            cout << currentDepth << "--" << e->String() << endl;
        }
        
        currentDepth++;
        
        if (currentDepth == evalDepth) {
            cout << "maximum number of steps exceeded!" << endl;
        }
    }
    
    
    // Reset values
    count = 0;
    return canonicalRemoveRepeats(e, count);
    
}

// Recursive pre-order search for APPLICATION node to substitute and perform substitution.
expressionP postOrderSubstitutionSearch(expressionP e, const expressionDictionary &dictionary, int &count, bool &found) {
    
    switch(e->Type) {
        case Expression::VARIABLE:
            return e;
            
        case Expression::COMBINATOR:
            return e;
            
        case Expression::SLASHDOT:
            e->updateRight(postOrderSubstitutionSearch(e->right, dictionary, count, found));
            return e;
            
        // In post-order, we must check the left node, then right node, then top node.
        case Expression::APPLICATION:

            // Check the left node
            e->updateLeft(postOrderSubstitutionSearch(e->left, dictionary, count, found));

            if (found)
                return e;
            
            // If nothing found on left node, check right node
            e->updateRight(postOrderSubstitutionSearch(e->right, dictionary, count, found));
            if (found)
                return e;
            
            // If nothing found, check the top node.
            switch (e->left->Type) {
                case Expression::VARIABLE:
                    break;
                    
                case Expression::COMBINATOR: {
                    found = true;
                    expressionP comb = dictionary.getItem(e->left->name);
                    if (comb == 0)
                        throw "Not found in the dictionary";
                    
                    e->updateLeft(canonicalRemoveRepeats(comb, count));
                    
                    return e;
                }
                case Expression::SLASHDOT: {
                    found = true;
                    bool variableSubstitution = false;
                    
                    // Make a copy of the right side of the slash dot and replace the expression appropriately
                    return recursiveReplaceExpression(e->left->right->Copy(), e->left->name, e->right->Copy(), count, variableSubstitution);
                }
                case Expression::APPLICATION:
                    break;
                    
                default:
                    throw "Unknown expression type in preOrderSubstitutionSearch Switch Application";
            }
            
            return e;
            
        default:
            throw "Unknown expression type in preOrderSubstitutionSearch Switch Type";
    }
}


