#include "s21_decimal.h"

int main() {

  s21_decimal val1 = {0};
  s21_decimal val2 = {0};
  // float num1 = 12.9;
  // float num2 = 12.5;
  // s21_from_float_to_decimal(num1, &val1);
  // s21_from_float_to_decimal(num2, &val2);
  val1.bits[0] = UINT_MAX;
  val1.bits[1] = UINT_MAX;
  val1.bits[2] = UINT_MAX;
  val1.bits[3] = UINT_MAX;
  // printf("\n val1: \n");
  // for (int i = 95; i >= 0; i--) {
  //   printf("%d", getBit(val1, i));
  // }
  // printf("\n val1.bits[3]:\n");
  // for (int i = 127; i >= 96; i--) {
  //   printf("%d", getBit(val1, i));
  // }
  // printf("\n sclae val1 =%d", getScale(val1));
  val2.bits[0] = 1;
  val2.bits[1] = 0;
  val2.bits[2] = 0;
  val2.bits[3] = 0;
  // 0b00000000000001100000000000000000 = 6
  // printf("\n val2: \n");
  // for (int i = 95; i >= 0; i--) {
  //   printf("%d", getBit(val2, i));
  // }
  // printf("\nval2.bits[3]:\n");
  // for (int i = 127; i >= 96; i--) {
  //   printf("%d", getBit(val2, i));
  // }
  // printf("\n sclae val2 =%d", getScale(val2));
  s21_decimal res = {0};
  s21_sub(val1, val2, &res);
  // printf("\n ===res===: \n");
  // for (int i = 95; i >= 0; i--) {
  //   printf("%d", getBit(res, i));
  // }
  // printf("\n res.bits[3]:\n");
  // for (int i = 127; i >= 96; i--) {
  //   printf("%d", getBit(res, i));
  // }
  // printf("\n sclae res =%d", getScale(res));
  // float resf = 0.0;
  // s21_from_decimal_to_float(res, &resf);
  // printf("\n float res = %f", resf);
  // return 0;

  // long_decimal value1 = {0};
  // long_decimal value2 = {0};
  // value1.long_bits[0] = 2;
  // value1.long_bits[1] = 0;
  // value1.long_bits[2] = 0;
  // value1.long_bits[3] = 0;
  // value1.long_bits[4] = 0;
  // value1.long_bits[5] = 0;
  // value1.long_bits[6] = 0;

  // value2.long_bits[0] = 10;
  // value2.long_bits[1] = 0;
  // value2.long_bits[2] = 0;
  // value2.long_bits[3] = 0;
  // value2.long_bits[4] = 0;
  // value2.long_bits[5] = 0;
  // value2.long_bits[6] = 0;
  // printf("\n s21_greater_or_equal_long = %d \n",
  //        s21_greater_or_equal_long(value1, value2));

  // printf("\n s21_greater_or_equal10 = %d \n",
  // s21_greater_or_equal10(value1));
}
