#ifndef S21_DECIMAL_H
#define S21_DECIMAL_H

#include <limits.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

enum { LOW, MIDLE, HIGHE, SCALE };

typedef struct {
  unsigned int bits[4];
} s21_decimal;

#define MAX_VALUE 79228162514264337593543950335.0F
#define MIN_VALUE -79228162514264337593543950335.0F

typedef struct {
  unsigned int long_bits[7];
} long_decimal;

int getBit(s21_decimal d, int i);
int setBit(s21_decimal *x, int index, int bit);
int long_getBit(long_decimal x, int index);
int long_setBit(long_decimal *x, int index, int bit);
int getSign(s21_decimal d);
int setSign(s21_decimal *x, int sign);
int getSign_long(long_decimal d);
int setSign_long(long_decimal *x, int sign);
int getScale_long(long_decimal x);
int setScale_long(long_decimal *d, int scale);
int getScale(s21_decimal x);
int setScale(s21_decimal *d, int scale);
int first_bit(s21_decimal dec);
int first_bit_long(long_decimal dec);
int binary_sum(long_decimal value_1, long_decimal value_2, long_decimal *res);
void dop_Code(long_decimal value, long_decimal *dop_code, int first_index);
void binary_sub(long_decimal value_1, long_decimal value_2, long_decimal *res);
void copy_decimal_long(long_decimal src, long_decimal *dst);
void from_decimal_to_long(s21_decimal dec, long_decimal *dec_long);
void from_long_to_dec(long_decimal src, s21_decimal *dst);
int scale_up(long_decimal *value_smallScale, int bigger_scale,
             int smaller_scale);
int to_same_scale(long_decimal *value_1, long_decimal *value_2);
int is_zero(long_decimal dec);
void masCreat(unsigned int *arr, s21_decimal x);
int float_getScale(float value);
void shift(s21_decimal *x, int sign);
void long_shift(long_decimal *x, int sign);
void sub_from_big(long_decimal value_1_long, long_decimal value_2_long,
                  long_decimal *result_long, int sign_val1, int sign_val2);
int from_long_to_decimal(long_decimal val1, s21_decimal *result);
long_decimal div_div(long_decimal val1);
int s21_greater_or_equal_long(long_decimal value_1_long,
                              long_decimal value_2_long);

// arithmetics
int s21_add(s21_decimal value_1, s21_decimal value_2, s21_decimal *result);
int s21_sub(s21_decimal value_1, s21_decimal value_2, s21_decimal *result);
int s21_mul(s21_decimal value_1, s21_decimal value_2, s21_decimal *res);

// compare
int s21_is_equal(s21_decimal value_1, s21_decimal value_2);
int s21_is_less(s21_decimal value_1, s21_decimal value_2);
int s21_is_less_or_equal(s21_decimal value_1, s21_decimal value_2);
int s21_is_greater(s21_decimal value_1, s21_decimal value_2);
int s21_is_greater_or_equal(s21_decimal value_1, s21_decimal value_2);
int s21_is_not_equal(s21_decimal value_1, s21_decimal value_2);

// convertors
int s21_from_float_to_decimal(float src, s21_decimal *dst);
int s21_from_decimal_to_float(s21_decimal src, float *dst);
int s21_from_int_to_decimal(int src, s21_decimal *dst);
int s21_from_decimal_to_int(s21_decimal src, int *dst);

#endif // S21_DECIMAL_H