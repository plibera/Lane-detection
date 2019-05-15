#ifndef POLYNOMIAL_H
#define POLYNOMIAL_H

#define DEGREE 2

template <class Type>
class Polynomial
{
  int deg;
  Type *coeffs;

public:
  Polynomial(int d = DEGREE): deg(std::max(0, d)), coeffs(new Type[deg+1]) {}
  ~Polynomial() { delete[] coeffs; }

  Type & operator[](int i);
  int degree() {return deg; }
};


template <class Type>
Type & Polynomial<Type>::operator[](int i)
{
  if(i < 0 || i > deg)
    return coeffs[0];//throw...

  return coeffs[i];
}


#endif
