#ifndef MATRIX_H
#define MATRIX_H
#include<stdbool.h>

#define _IN_
#define _OUT_

#define MAXMATRIXSIZE 4096
struct matrix{
    unsigned int rows;
    unsigned int columns;
    #if MATRIX_STORETYPE==FLOATINGTYPE
    double store[MAXMATRIXSIZE];
    #else
    int store[MAXMATRIXSIZE];
    #endif
};

#if MATRIX_STORETYPE==FLOATINGTYPE
extern void MatrixCreate(struct matrix*,const _IN_ unsigned int,const _IN_ unsigned int,_IN_ double*);
#else
extern void MatrixCreate(struct matrix*,const _IN_ unsigned int,const _IN_ unsigned int,_IN_ int*);
#endif

extern void UnitMatrixCreate(struct matrix*,const _IN_ unsigned int);
extern void MatrixAdd(_IN_ struct matrix*,struct matrix*,struct matrix*);
extern void MatrixSubtraction(_IN_ struct matrix*,struct matrix*,struct matrix*);
extern void MatrixMultiplication(_IN_ struct matrix*,struct matrix*,struct matrix*);

extern void GetChildMatrix(_IN_ struct matrix*,const _IN_ unsigned int,_IN_ const unsigned int,_IN_ const unsigned int,_IN_ const unsigned int,_OUT_ struct matrix*);
extern void MatrixScale(struct matrix*,const double);
extern void MatrixTranspose(_IN_ struct matrix*,_OUT_ struct matrix*);
extern bool MatrixInversion(_IN_ struct matrix*,_OUT_ struct matrix*);
extern void MatrixPower(_IN_ struct matrix*,_OUT_ struct matrix*,_IN_ const unsigned int);
extern void MatrixPrint(_IN_ struct matrix*);


#if MATRIX_STORETYPE==FLOATINGTYPE
extern void MatrixTrace(double*,_IN_ struct matrix*);
extern void DeterminantValue(double*,_IN_ struct matrix*); 
#else
extern void MatrixTrace(int*,_IN_ struct matrix*);
extern void DeterminantValue(int*,_IN_ struct matrix*);
#endif

#endif