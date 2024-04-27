#include"matrix.h"
#include"base.h"
#include<stdlib.h>
#include<assert.h>

#define GetMatrixValue(matrix,row_index,column_index) (\
{\
    const unsigned int index = row_index*matrix->columns+column_index;\
    matrix->store[index];\
}\
)

#define SetMatrixValue(matrix,row_index,column_index,value)\
{\
    const unsigned int index = row_index*matrix->columns+column_index;\
    matrix->store[index] = value;\
}

#if MATRIX_STORETYPE==FLOATINGTYPE
void MatrixCreate(struct matrix* new_matrix,const _IN_ unsigned int row,const _IN_ unsigned int column,_IN_ double* data_array){
    if(!new_matrix)
        new_matrix = (struct matrix*)malloc(sizeof(struct matrix));
    new_matrix->columns = column;
    new_matrix->rows = row;
    unsigned int index,row_index,column_index;
    for(row_index=0;row_index<row;row_index++)
        for(column_index=0;column_index<column;column_index++)
        {
            index = row_index*column+column_index;
            new_matrix->store[index] = data_array[index];
        }
}
#else
void MatrixCreate(struct matrix* new_matrix,const _IN_ unsigned int row,const _IN_ unsigned int column,_IN_ int* data_array){
  if(!new_matrix)
        new_matrix = (struct matrix*)malloc(sizeof(struct matrix));
    new_matrix->columns = column;
    new_matrix->rows = row;
    unsigned int index,row_index,column_index;
    for(row_index=0;row_index<row;row_index++)
        for(column_index=0;column_index<column;column_index++)
        {
            index = row_index*column+column_index;
            new_matrix->store[index] = data_array[index];
        }
}
#endif

#define MatrixClear(matrix)\
{\
    unsigned int index,row_index,column_index;\
    for(row_index=0;row_index<matrix->rows;row_index++)\
        for(column_index=0;column_index<matrix->columns;column_index++)\
        {\
            index = row_index*matrix->columns+column_index;\
            matrix->store[index] = 0;\
        }\
}

void UnitMatrixCreate(struct matrix* new_matrix,const _IN_ unsigned int dimension){
    if(!new_matrix)
        new_matrix = (struct matrix*)malloc(sizeof(struct matrix));
    new_matrix->columns = dimension;
    new_matrix->rows = dimension;
    unsigned int index,level;
    MatrixClear(new_matrix);
    for(level=0;level<dimension;level++)
    {
        index = level*dimension+level;
        new_matrix->store[index] = 1;
    }
}

void MatrixAdd(_IN_ struct matrix* input_matrix1,struct matrix* input_matrix2,struct matrix* output_matrix){
    assert(input_matrix1->rows=input_matrix2->rows);
    assert(input_matrix1->columns=input_matrix2->columns);
    output_matrix->rows = input_matrix1->rows;
    output_matrix->columns = input_matrix1->rows;
    unsigned int index,row_index,column_index;
    #if MATRIX_STORETYPE==FLOATINGTYPE
    double value1,value2;
    #else
    int value1,value2;
    #endif
    for(row_index=0;row_index<input_matrix1->rows;row_index++)
        for(column_index=0;column_index<input_matrix1->columns;column_index++)
        {
            index = row_index*input_matrix1->columns+column_index;
            value1 = GetMatrixValue(input_matrix1,row_index,column_index);
            value2 = GetMatrixValue(input_matrix2,row_index,column_index);
            output_matrix->store[index] = value1+value2;
        }    
}

void MatrixSubtraction(_IN_ struct matrix* input_matrix1,struct matrix* input_matrix2,struct matrix* output_matrix){
    assert(input_matrix1->rows=input_matrix2->rows);
    assert(input_matrix1->columns=input_matrix2->columns);
    output_matrix->rows = input_matrix1->rows;
    output_matrix->columns = input_matrix1->rows;
    unsigned int index,row_index,column_index;
    #if MATRIX_STORETYPE==FLOATINGTYPE
    double value1,value2;
    #else
    int value1,value2;
    #endif
    for(row_index=0;row_index<input_matrix1->rows;row_index++)
        for(column_index=0;column_index<input_matrix1->columns;column_index++)
        {
            index = row_index*input_matrix1->columns+column_index;
            value1 = GetMatrixValue(input_matrix1,row_index,column_index);
            value2 = GetMatrixValue(input_matrix2,row_index,column_index);
            output_matrix->store[index] = value1+value2;
        }    
}

void MatrixMultiplication(_IN_ struct matrix* input_matrix1,struct matrix* input_matrix2,struct matrix* output_matrix){
    assert(input_matrix1->columns=input_matrix2->rows);
    output_matrix->rows = input_matrix1->rows;
    output_matrix->columns = input_matrix2->columns;
    MatrixClear(output_matrix);
    unsigned int index,row_index=0,column_index=0;
    unsigned int current_count;
    const unsigned int total_counts = input_matrix1->columns;
    #if MATRIX_STORETYPE==FLOATINGTYPE
    double value1,value2,temp;
    #else
    int value1,value2,temp;
    #endif
    while(row_index<output_matrix->rows)      
    {    
        while(column_index<output_matrix->columns)
        {
            temp = 0;
            for(current_count=0;current_count<total_counts;current_count++)
            {
                value1 = GetMatrixValue(input_matrix1,row_index,current_count);
                value2 = GetMatrixValue(input_matrix2,current_count,column_index);
                temp = temp+value1*value2;
            }
            index = row_index*output_matrix->columns+column_index;
            output_matrix->store[index] = temp;
            column_index++;    
        }
        row_index++;
        column_index = 0;
    }    
}

void GetChildMatrix(_IN_ struct matrix* input_matrix,const _IN_ unsigned int start_row,_IN_ const unsigned int end_row,_IN_ const unsigned int start_column,_IN_ const unsigned int end_column,\
                    _OUT_ struct matrix* output_matrix){
    assert(end_row<input_matrix->rows);
    assert(end_column<input_matrix->columns);
    output_matrix->rows = end_row-start_row+1;
    output_matrix->columns = end_column-start_column+1;
    unsigned int row_index,column_index,index;
    #if MATRIX_STORETYPE==FLOATINGTYPE
    double temp_value;
    #else
    int temp_value;
    #endif
    for(row_index=0;row_index<output_matrix->rows;row_index++)\
        for(column_index=0;column_index<output_matrix->columns;column_index++)
        {
            temp_value = GetMatrixValue(input_matrix,row_index+start_row,column_index+start_column);
            SetMatrixValue(output_matrix,row_index,column_index,temp_value);
        }
}

void MatrixScale(struct matrix* obj_matrix,const double scale){
    unsigned int index,row_index,column_index;
    #if MATRIX_STORETYPE==FLOATINGTYPE
    double value;
    #else
    int value;
    #endif
    for(row_index=0;row_index<obj_matrix->rows;row_index++)
        for(column_index=0;column_index<obj_matrix->columns;column_index++)
        {
            index = row_index*obj_matrix->columns+column_index;
            obj_matrix->store[index] = obj_matrix->store[index]*scale;
        }  
}

void MatrixTranspose(_IN_ struct matrix* input_matrix,_OUT_ struct matrix* output_matrix){
    assert(input_matrix);
    assert(output_matrix);
    output_matrix->columns = input_matrix->rows;
    output_matrix->rows = input_matrix->columns;
    unsigned int row_index,column_index;
    #if MATRIX_STORETYPE==FLOATINGTYPE
    double value;
    #else
    int value;
    #endif
    for(row_index=0;row_index<input_matrix->rows;row_index++)
        for(column_index=0;column_index<input_matrix->columns;column_index++)
        {
            value = GetMatrixValue(input_matrix,row_index,column_index);
            SetMatrixValue(output_matrix,column_index,row_index,value);
        }
}

bool MatrixInversion(_IN_ struct matrix* input_matrix,_OUT_ struct matrix* output_matrix){
    assert(input_matrix->columns=input_matrix->rows);
    output_matrix->rows = input_matrix->rows;
    output_matrix->columns = input_matrix->columns;
    struct matrix augmented_matrix;
    augmented_matrix.rows = input_matrix->rows;
    augmented_matrix.columns = 2*input_matrix->columns;
    struct matrix* temp_matrix = &augmented_matrix;
    unsigned int index,row_index,column_index;
    #if MATRIX_STORETYPE==FLOATINGTYPE
    double temp_value,former_value;
    #else
    int temp_value,former_value;
    #endif
    for(row_index=0;row_index<input_matrix->rows;row_index++)
        for(column_index=0;column_index<input_matrix->columns;column_index++)
        {
            temp_value = GetMatrixValue(input_matrix,row_index,column_index);
            SetMatrixValue(temp_matrix,row_index,column_index,temp_value);
        }
    unsigned int level;
    for(level=0;level<input_matrix->rows;level++)
    {
        row_index = level;
        column_index = input_matrix->columns+level;
        SetMatrixValue(temp_matrix,row_index,column_index,1.0f);
    }

    bool exist = true;
    level = 0;
    unsigned int row_probe;
    struct matrix* temp_MatrixPointer = &augmented_matrix;
    while(level<augmented_matrix.rows)
    {
        temp_value = GetMatrixValue(temp_MatrixPointer,level,level);
        if(temp_value==0)
        {
            for(row_probe=level+1;row_probe<augmented_matrix.rows;row_probe++)
            {
                temp_value = GetMatrixValue(temp_MatrixPointer,row_probe,level);
                if(temp_value!=0.0f)        
                    break;    
            }
            if(row_probe==augmented_matrix.rows)
            {
                exist = false;
                break;
            }
            for(column_index=level;column_index<augmented_matrix.columns;column_index++)
            {
                temp_value = GetMatrixValue(temp_MatrixPointer,row_probe,column_index);
                former_value = GetMatrixValue(temp_MatrixPointer,level,column_index);
                SetMatrixValue(temp_MatrixPointer,level,column_index,(temp_value+former_value));
            }
        }
        double scale_value = GetMatrixValue(temp_MatrixPointer,level,level);
        for(column_index=level;column_index<augmented_matrix.columns;column_index++)
        {
            temp_value = GetMatrixValue(temp_MatrixPointer,level,column_index);
            temp_value = temp_value/scale_value;
            SetMatrixValue(temp_MatrixPointer,level,column_index,temp_value);  
        }        
        for(row_index=level+1;row_index<augmented_matrix.rows;row_index++)
        {   
            scale_value = GetMatrixValue(temp_MatrixPointer,row_index,level);
            for(column_index=level;column_index<augmented_matrix.columns;column_index++)
            {
                former_value = GetMatrixValue(temp_MatrixPointer,row_index,column_index);
                temp_value = GetMatrixValue(temp_MatrixPointer,level,column_index)*scale_value-former_value;
                SetMatrixValue(temp_MatrixPointer,row_index,column_index,temp_value);
            }
        }
        level++;
    }
    if(exist==false)
    {
        MatrixClear(output_matrix);
        return false;
    }
    const unsigned int shift_index = input_matrix->columns; 
    for(row_index=0;row_index<output_matrix->rows;row_index++)
        for(column_index=0;column_index<output_matrix->columns;column_index++)
        {
            temp_value = GetMatrixValue(temp_MatrixPointer,row_index,shift_index+column_index);
            SetMatrixValue(output_matrix,row_index,column_index,temp_value);
        }
    return true;    
}

void MatrixPower(_IN_ struct matrix* input_matrix,_OUT_ struct matrix* output_matrix,_IN_ const unsigned int power){

}


void MatrixPrint(_IN_ struct matrix* input_matrix){
    unsigned int index,row_index,column_index;
    for(row_index=0;row_index<input_matrix->rows;row_index++)
    {
        for(column_index=0;column_index<input_matrix->columns;column_index++)
        {
            index = row_index*input_matrix->columns+column_index;
            #if MATRIX_STORETYPE==FLOATINGTYPE
            printf("%-7.3f\t",input_matrix->store[index]);
            #else
            printf("%-7d\t",input_matrix->store[index]);
            #endif
        }
        printf("\n");
    }    
}


#if MATRIX_STORETYPE==FLOATINGTYPE
void MatrixTrace(double* return_value,_IN_ struct matrix* input_matrix){
    assert(input_matrix->rows=input_matrix->columns);
    unsigned int index,level=0;
    double result = 0.0f;
    while(level<input_matrix->rows)
    {
        result += GetMatrixValue(input_matrix,level,level);
        level++;
    }
    *return_value = result;
}

void DeterminantValue(double* return_value,_IN_ struct matrix* input_matrix)
{
    assert(input_matrix->rows=input_matrix->columns);
    double result = 1;
    unsigned int level=0,probe,scale,row_index,column_index;
    struct matrix diagonal_matrix;
    struct matrix* temp_matrix = &diagonal_matrix;
    diagonal_matrix.rows = input_matrix->rows;
    diagonal_matrix.columns = input_matrix->columns;
    double temp_value = 0;
    for(row_index=0;row_index<diagonal_matrix.rows;row_index++)
        for(column_index=0;column_index<diagonal_matrix.columns;column_index++)
        {
            temp_value = GetMatrixValue(input_matrix,row_index,column_index);
            SetMatrixValue(temp_matrix,row_index,column_index,temp_value);
        }
    int base_value;    
    level = 0;
    while(level<diagonal_matrix.rows-1)
    {
        base_value = GetMatrixValue(temp_matrix,level,level);
        for(probe=level+1;probe<diagonal_matrix.rows;probe++)
        {
            temp_value = GetMatrixValue(temp_matrix,probe,level);
            if(temp_value==0)
                continue;
            scale = temp_value/base_value;
            for(column_index=level;column_index<diagonal_matrix.columns;column_index++)\
            {
                base_value = scale*GetMatrixValue(temp_matrix,level,column_index);
                temp_value = GetMatrixValue(temp_matrix,probe,column_index)-base_value;
                SetMatrixValue(temp_matrix,probe,column_index,temp_value);
            }    
        }
        level++;
    }
    level = 0;
    while(level<diagonal_matrix.rows-1)
    {
        for(probe=level+1;probe<diagonal_matrix.rows;probe++)
        {
            temp_value = GetMatrixValue(temp_matrix,level,probe);
            base_value = GetMatrixValue(temp_matrix,probe,probe);
            if(base_value==0)
                return 0;
            scale = temp_value/base_value;
            for(column_index=probe;column_index<diagonal_matrix.columns;column_index++)
            {
                base_value = GetMatrixValue(temp_matrix,probe,column_index);
                temp_value = GetMatrixValue(temp_matrix,level,column_index)-scale*base_value;
                SetMatrixValue(temp_matrix,level,column_index,temp_value);
            }
        }
        level++; 
    }
    for(level=0;level<diagonal_matrix.rows;level++)
        result *= GetMatrixValue(temp_matrix,level,level);
    *return_value = result;        
}

#else
void MatrixTrace(int* return_value,_IN_ struct matrix* input_matrix){
    assert(input_matrix->rows=input_matrix->columns);
    unsigned int index,level=0;
    int result = 0;
    while(level<input_matrix->rows)
    {
        result += GetMatrixValue(input_matrix,level,level);
        level++;
    }
    *return_value = result;
}

void DeterminantValue(int* return_value,_IN_ struct matrix* input_matrix){
    assert(input_matrix->rows=input_matrix->columns);
    int result = 1;
    unsigned int level=0,probe,scale,row_index,column_index;
    struct matrix diagonal_matrix;
    struct matrix* temp_matrix = &diagonal_matrix;
    diagonal_matrix.rows = input_matrix->rows;
    diagonal_matrix.columns = input_matrix->columns;
    int temp_value = 0;
    for(row_index=0;row_index<diagonal_matrix.rows;row_index++)
        for(column_index=0;column_index<diagonal_matrix.columns;column_index++)
        {
            temp_value = GetMatrixValue(input_matrix,row_index,column_index);
            SetMatrixValue(temp_matrix,row_index,column_index,temp_value);
        }
    int base_value;    
    level = 0;
    while(level<diagonal_matrix.rows-1)
    {
        base_value = GetMatrixValue(temp_matrix,level,level);
        for(probe=level+1;probe<diagonal_matrix.rows;probe++)
        {
            temp_value = GetMatrixValue(temp_matrix,probe,level);
            if(temp_value==0)
                continue;
            scale = temp_value/base_value;
            for(column_index=level;column_index<diagonal_matrix.columns;column_index++)\
            {
                base_value = scale*GetMatrixValue(temp_matrix,level,column_index);
                temp_value = GetMatrixValue(temp_matrix,probe,column_index)-base_value;
                SetMatrixValue(temp_matrix,probe,column_index,temp_value);
            }    
        }
        level++;
    }
    level = 0;
    while(level<diagonal_matrix.rows-1)
    {
        for(probe=level+1;probe<diagonal_matrix.rows;probe++)
        {
            temp_value = GetMatrixValue(temp_matrix,level,probe);
            base_value = GetMatrixValue(temp_matrix,probe,probe);
            if(base_value==0)
            {
                *return_value = 0;
                return;
            }
            scale = temp_value/base_value;
            for(column_index=probe;column_index<diagonal_matrix.columns;column_index++)
            {
                base_value = GetMatrixValue(temp_matrix,probe,column_index);
                temp_value = GetMatrixValue(temp_matrix,level,column_index)-scale*base_value;
                SetMatrixValue(temp_matrix,level,column_index,temp_value);
            }
        }
        level++; 
    }
    for(level=0;level<diagonal_matrix.rows;level++)
        result *= GetMatrixValue(temp_matrix,level,level);
    *return_value = result;    
}
#endif