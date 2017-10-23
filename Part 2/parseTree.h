#ifndef PARSETREE_H
#define PARSETREE_H

#include <map>
#include <vector>
#include <string>

// ***************************************************************************
// Definition of the class that describes the parse tree for expressions
typedef class Expression *expressionP ; 
class Expression {
public:
  // Node Type Definition
  enum Etype {VARIABLE, COMBINATOR, SLASHDOT, APPLICATION} Type ;
  // Name of variable, combinator variable, or shashdot varname
  string name ;
  // left side of APPLICATION type
  expressionP left ;
  // right side of APPLICATION type or for SLASHDOT operator
  expressionP right ;
  
  // Expression Constructors
  Expression(Expression::Etype T): Type(T) {
    left = 0; right = 0 ;
  }
  Expression(Expression::Etype T, string N): Type(T),name(N) {
    left = 0 ; right = 0 ;
  }
  Expression(Expression::Etype T, string N,expressionP A): Type(T),name(N) {
    left = 0 ; right = A ;
  }

  Expression(Expression::Etype T, expressionP A1, expressionP A2):Type(T) {
    left = A1, right = A2 ;
  }
  // Destructor releases memory of children nodes if allocated
  ~Expression() {
    if(left != 0)
      delete left ;
    if(right != 0)
      delete right ;
  }
  // Update left expression,
  // if update changes expression, then delete old expression
  expressionP updateLeft(expressionP p) {
    if(p != left && left != 0)
      delete left ;
    left = p ;
    return p ;
  }
  // update right expression with memory management
  expressionP updateRight(expressionP p) {
    if(p != right && right != 0)
      delete right ;
    right = p ;
    return p ;
  }
  // return a (deep) copy of the expression
  expressionP Copy() const {
    switch(Type) {
    case Expression::VARIABLE:
    case Expression::COMBINATOR:
      return new Expression(Type,name) ;
    case Expression::SLASHDOT:
      return new Expression(Type,name,right->Copy()) ;
    case Expression::APPLICATION:
      return new Expression(Type,left->Copy(),right->Copy()) ;
      break ;
    default:
      cerr << "Unknown expression type in Expression::Copy()" << endl ;
    }
  }
  // convert the expression into a string form
  string String() const {
    switch(Type) {
    case Expression::VARIABLE:
      return name ;
    case Expression::COMBINATOR:
      return "$" + name ;
    case Expression::SLASHDOT:
      return string("{\\")+ name+"." + right->String() +"}" ;
    case Expression::APPLICATION:
      return string("(")+left->String()+" "+right->String() +")" ;
    }
    return string("<{Err}>") ;
  }

} ;

// ***************************************************************************
// Convienince routines to make construction of expression elements more
// obvious.


// create a variable expression
inline expressionP makeVariable(string name) {
  return new Expression(Expression::VARIABLE,name) ;
}

// Create a combinator variable expression (e.g. a dollar variable)
inline expressionP makeCombinator(string name) {
  return new Expression(Expression::COMBINATOR,name) ;
}

// Create an expression for the \name.<E> operation
inline expressionP makeSlashDot(string name, expressionP argument) {
  return new Expression(Expression::SLASHDOT,name,argument) ;
}
// Create an expression for the application operation
inline expressionP makeApplication(expressionP left, expressionP right) {
  return new Expression(Expression::APPLICATION,left,right) ;
}

// ***************************************************************************
// An expression dictionary class to mantain a database of named expressions
class expressionDictionary {
  std::map<std::string,expressionP> dictionary ;
 public:
  // Return how many items ar ein the dictionary
  int size() const { return dictionary.size() ; }
  // Delete dictionary content
  ~expressionDictionary() {
    std::map<std::string,expressionP>::iterator mi ;
    for(mi=dictionary.begin();mi!=dictionary.end();++mi) {
      delete mi->second ;
      mi->second = 0 ;
    }
  }

  // Return all names of variables in the database
  std::vector<std::string> nameList() const {
    std::vector<std::string> list ;
    std::map<std::string,expressionP>::const_iterator mi ;
    for(mi=dictionary.begin();mi!=dictionary.end();++mi) {
      list.push_back(mi->first) ;
    }
    return list ;
  }

  // Print the database out to the stream provided
  std::ostream &Print(std::ostream &s) const {
    std::map<std::string,expressionP>::const_iterator mi ;
    for(mi=dictionary.begin();mi!=dictionary.end();++mi) {
      s << mi->first << ": " << mi->second->String() << endl ;
    }
    return s ;
  }
  
  // Check to see if an exprssion with the given name exists in the
  // database
  bool hasItem(const std::string &name) const {
    return dictionary.find(name) != dictionary.end() ;
  }

  // Inserting an expression into the dictionary transfers ownership
  // of the expression to the dictionary
  void insertItem(const std::string &name, expressionP e) {
    if(hasItem(name)) {
      std::map<std::string,expressionP>::iterator mi ;
      mi=dictionary.find(name) ;
      delete mi->second ;
      mi->second = e ;
    } else
      dictionary.insert(std::make_pair(name,e)) ;
  }
  // Get a copy of an expression from the database
  expressionP getItem(const std::string &name) const {
    if(hasItem(name)) {
      std::map<std::string,expressionP>::const_iterator mi ;
      mi=dictionary.find(name) ;
      return mi->second->Copy() ;
    }
    return 0 ;
  }
  // Get a string representation of an expression in the database
  std::string getItemString(const std::string &name) const {
    if(hasItem(name)) {
      std::map<std::string,expressionP>::const_iterator mi ;
      mi=dictionary.find(name) ;
      return mi->second->String() ;
    }
    return std::string("<ERR>") ;
  }
} ;
    
    
#endif
