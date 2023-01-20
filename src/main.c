#include "s21_decimal.h"

int main() {

  s21_decimal val1 = {0};
  s21_decimal val2 = {0};
  // float num1 = 12.9;
  float num2 = -1.01448;
  // s21_from_float_to_decimal(num1, &val1);
  s21_from_float_to_decimal(num2, &val2);
  val1.bits[0] = UINT_MAX;
  val1.bits[1] = UINT_MAX;
  val1.bits[2] = UINT_MAX;
  val1.bits[3] = 0b10000000000000000000000000000000;
  printf("\n val1: \n");
  for (int i = 95; i >= 0; i--) {
    printf("%d", getBit(val1, i));
  }
  printf("\n val1.bits[3]:\n");
  for (int i = 127; i >= 96; i--) {
    printf("%d", getBit(val1, i));
  }
  printf("\n sclae val1 =%d", getScale(val1));
  // val2.bits[0] = UINT_MAX;
  // val2.bits[1] = UINT_MAX;
  // val2.bits[2] = UINT_MAX;
  // val2.bits[3] = 0b10000000000000000000000000000000;
  // 0b00000000000001100000000000000000 = 6
  printf("\n val2: \n");
  for (int i = 95; i >= 0; i--) {
    printf("%d", getBit(val2, i));
  }
  printf("\nval2.bits[3]:\n");
  for (int i = 127; i >= 96; i--) {
    printf("%d", getBit(val2, i));
  }
  printf("\n sclae val2 =%d", getScale(val2));
  s21_decimal res = {0};
  printf("\n add = %d", s21_add(val1, val2, &res));
  printf("\n ===res===: \n");
  for (int i = 95; i >= 0; i--) {
    printf("%d", getBit(res, i));
  }
  printf("\n res.bits[3]:\n");
  for (int i = 127; i >= 96; i--) {
    printf("%d", getBit(res, i));
  }
  // printf("\n sclae res =%d", getScale(res));
  // float resf = 0.0;
  // s21_from_decimal_to_float(res, &resf);
  // printf("\n float res = %f", resf);
  // return 0;
  // int res_int = 0;
  // s21_from_decimal_to_int(res, &res_int);
  // printf("\n res_int = %d", res_int);

  s21_decimal long_num_positive = {0};
  long_num_positive.bits[0] = UINT_MAX;
  long_num_positive.bits[1] = UINT_MAX;
  long_num_positive.bits[2] = UINT_MAX;
  long_num_positive.bits[3] = 0b00000000000111000000000000000000; // scale= 28
  s21_decimal long_num_negative = {0};
  long_num_negative.bits[0] = UINT_MAX;
  long_num_negative.bits[1] = UINT_MAX;
  long_num_negative.bits[2] = UINT_MAX;
  long_num_negative.bits[3] = 0b10000000000111000000000000000000; // scale = 28
  printf("\n s21_is_not_equal = %d",
         s21_is_not_equal(long_num_negative, long_num_positive));
}
