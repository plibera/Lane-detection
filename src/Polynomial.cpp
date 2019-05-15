#include "Polynomial.h"


template <typename Type>
Type & Polynomial<Type>::operator[](int i)
{
  if(i < 0 || i > deg)
    return 0;//throw...

  return coeffs[i];
}


template<typename Type>
const Type & Polynomial<Type>::operator[](int i) const
{
  if(i < 0 || i > deg)
    return 0;//throw...

  return coeffs[i];
}
