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
  Type* coeffs;

public:
  Polynomial(int d = DEGREE): deg(std::max(0,d)), coeffs(new Type[deg+1]){}
  Polynomial(const Polynomial<Type>& ck);//copying constructor
  Polynomial<Type>& operator=(const Polynomial<Type>& ck);//copying =
  ~Polynomial(){ delete[] coeffs; }

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
Type Polynomial<Type>::value(int x)
{
  Type ret = 0;
  for(int i = 0; i <= DEGREE; ++i)
  {
    ret += coeffs[i] * pow(x, i);
  }
  return ret;
}

template <class Type>
Polynomial<Type>::Polynomial(const Polynomial<Type>& ck)//copying constructor
{
  this->deg = ck.deg;
  this->coeffs = new Type[deg+1];
  for(int i = 0; i <= deg; ++i)
  {
    this->coeffs[i] = ck.coeffs[i];
  }
}

template <class Type>
Polynomial<Type>& Polynomial<Type>::operator=(const Polynomial<Type>& ck)//copying =
{
  deg = ck.deg;
  for(int i = 0; i <= deg; ++i)
  {
    coeffs[i] = ck.coeffs[i];
  }
  return *this;
}

#endif
