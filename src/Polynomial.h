#ifndef POLYNOMIAL_H
#define POLYNOMIAL_H

#include <stdio.h>
#include <vector>
#include <math.h>

#define DEGREE 2

template <class Type>
class Polynomial
{
  int deg;
  std::vector<Type> coeffs;

public:
  Polynomial(int d = DEGREE);
  ~Polynomial(){}//std::cout<<"Polynomial destructor"<<std::endl;}

  Type & operator[](int i);
  int degree() {return deg; }
  Type value(int x);
};


template <class Type>
Type & Polynomial<Type>::operator[](int i)
{
  if(i < 0 || i > deg)
    throw "Requested index out of memory";

  return coeffs[i];
}

template <class Type>
Polynomial<Type>::Polynomial(int d)
{
  deg = std::max(0, d);
  for(int i = 0; i <= deg; ++i)
    coeffs.push_back(0);
}

template <class Type>
Type Polynomial<Type>::value(int x)
{
  Type ret = 0;
  for(int i = 0; i <= DEGREE; ++i)
  {
    ret += coeffs[i] * pow(x, i);
  }
  return ret;
}

#endif
