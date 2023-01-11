#ifndef S21_DECIMAL_H
#define S21_DECINAL_H

#include <limits.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

enum { LOW, MIDLE, HIGHE, SCALE };

typedef struct {
  unsigned int bits[4];
} s21_decimal;

void masCreat(unsigned int *arr, s21_decimal x);
int getBit(s21_decimal d, int i);
int setBit(s21_decimal *x, int index, int bit);
int getSign(s21_decimal x);
int setSign(s21_decimal *x, int sign);
int getScale(s21_decimal x);
int setScale(s21_decimal *x, int value);
void shift(s21_decimal *x, int sign);
int float_getScale(float value);
int binary_sum(s21_decimal value_1, s21_decimal value_2, s21_decimal *res);
void dop_Code(s21_decimal value, s21_decimal *dop_code, int first_index);
int first_bit(s21_decimal dec);
void binary_sub(s21_decimal value_1, s21_decimal value_2, s21_decimal *res);
void copy_decimal(s21_decimal src, s21_decimal *dst);
int scale_up(s21_decimal *value_smallScale, int bigger_scale,
             int smaller_scale);
int to_same_scale(s21_decimal *value_1, s21_decimal *value_2);
int is_zero(s21_decimal dec);
void sub_from_big(s21_decimal value_1, s21_decimal value_2, s21_decimal *result,
                  int sign_val1, int sign_val2);

// compare
int s21_is_less(s21_decimal value_1, s21_decimal value_2);
int s21_is_less_or_equal(s21_decimal value_1, s21_decimal value_2);
int s21_is_greater(s21_decimal value_1, s21_decimal value_2);
int s21_is_greater_or_equal(s21_decimal value_1, s21_decimal value_2);
int s21_is_equal(s21_decimal value_1, s21_decimal value_2);
int s21_is_not_equal(s21_decimal value_1, s21_decimal value_2);

// arithmetics
int s21_add(s21_decimal value_1, s21_decimal value_2, s21_decimal *result);
int s21_sub(s21_decimal value_1, s21_decimal value_2, s21_decimal *result);
int s21_mul(s21_decimal value_1, s21_decimal value_2, s21_decimal *res);

// convertors
int s21_from_float_to_decimal(float src, s21_decimal *dst);
int s21_from_decimal_to_float(s21_decimal src, float *dst);
int s21_from_int_to_decimal(int src, s21_decimal *dst);
int s21_from_decimal_to_int(s21_decimal src, int *dst);

#endif // S21_DECIMAL_H
