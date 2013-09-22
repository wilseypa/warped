#ifndef TERMINATE_TOKEN_H
#define TERMINATE_TOKEN_H


/**
   This is the class defining the termination token that gets passed
   around.  Essentially, it has a color and a terminator.  

   The color can be WHITE, which means it's on the first cycle,
   RED, which means it's on it's second cycle, or BLACK.
*/

class TerminateToken {
public:
  TerminateToken( unsigned int terminator ) : myColor( WHITE ){}

  TokenColor getColor(){ return myColor; }
  

  enum TokenColor { BLACK, WHITE, RED };

private:
  TokenColor myColor;
  unsigned int myTerminator;
};

#endif
