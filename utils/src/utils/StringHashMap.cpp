
// See copyright notice in file Copyright in the root directory of this archive.

#include "StringHashMap.h"

// template <bool caseSensitive>
// size_t
// HashString<caseSensitive>::operator()( const string &s ) const {
//   hash<const char *> H;
//   string toHash;
//   if( caseSensitive ){
//     toHash = s;
//   }
//   else{
//     toHash = upperCase(s);
//   }
//   return H( toHash.c_str() );
// }

// template <bool caseSensitive>
// bool 
// HashEqual<caseSensitive>::operator()( const string &s1, const string &s2 ) const{
//   if( caseSensitive ){
//     return s1 == s2;
//   }
//   else{
//     return stringCaseCompare(s1, s2);
//   }
// }
