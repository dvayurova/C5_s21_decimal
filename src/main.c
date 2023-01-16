#include "s21_decimal.h"

int main() {
  // float num1 = 1.0;
  // float num2 = 1.0;
  s21_decimal dec = {0};
  s21_decimal val2 = {0};
  // s21_from_float_to_decimal(num1, &dec);
  // s21_from_float_to_decimal(num2, &val2);

  long_decimal value_1 = {0};
  value_1.long_bits[0] = 10;
  long_decimal result1 = {0};
  value_1.long_bits[0] = 4;
  divide(value_1, &result1);
  // printf("\n  s21_greater_or_equal10= %d \n",
  // s21_greater_or_equal10(value_1));
  dec.bits[0] = UINT_MAX;
  dec.bits[1] = UINT_MAX;
  dec.bits[2] = UINT_MAX;
  dec.bits[3] = 0b00000000000001100000000000000000;
  // printf("\n dec: \n");
  // for (int i = 95; i >= 0; i--) {
  //   printf("%d", getBit(dec, i));
  // }
  // printf("\ndec.bits[3]:\n");
  // for (int i = 127; i >= 96; i--) {
  //   printf("%d", getBit(dec, i));
  // }

  // printf("\n sclae val1 =%d", getScale(dec));
  // printf("\n sign val1 =%d", getSign(dec));

  // int sign = getSign(dec) ? -1 : 1;
  // printf("\n VALUE1=%f\n", sign * (dec.bits[0] * pow(10, -1 *
  // getScale(dec)))); float two = -3.2;

  // s21_from_int_to_decimal(two, &val2);

  val2.bits[0] = 100000;
  val2.bits[1] = 0;
  val2.bits[2] = 0;
  val2.bits[3] = 0b00000000000011100000000000000000;

  // printf("\n is less = %d", s21_is_less(dec, val2));
  // 0b00000000000001100000000000000000 = 6
  // printf("\n val2: \n");
  // for (int i = 95; i >= 0; i--) {
  //   printf("%d", getBit(val2, i));
  // }
  // printf("\nval2.bits[3]:\n");
  // for (int i = 127; i >= 96; i--) {
  //   printf("%d", getBit(val2, i));
  // }
  // int sign_2 = getSign(val2) ? -1 : 1;
  // printf("\n sclae val2 =%d", getScale(val2));
  // printf("\n sign val2 =%d", getSign(val2));
  // printf("\n VALUE2=%f\n",
  //        sign_2 * (val2.bits[0] * pow(10, -1 * getScale(val2))));

  // s21_decimal res = {0};
  // s21_add(dec, val2, &res);
  // printf("\n res.bits[0]:\n");
  // for (int i = 31; i >= 0; i--) {
  //   printf("%d", getBit(res, i));
  // }
  // printf("\n res.bits[1]:\n");
  // for (int i = 63; i >= 32; i--) {
  //   printf("%d", getBit(res, i));
  // }
  // printf("\n res.bits[2]:\n");
  // for (int i = 95; i >= 64; i--) {
  //   printf("%d", getBit(res, i));
  // }
  // printf("\n res.bits[3]:\n");
  // for (int i = 127; i >= 96; i--) {
  //   printf("%d", getBit(res, i));
  // }
  // // printf("\n s21_add code result = %d\n", s21_add(dec, val2, &res));
  // printf("\n res:\n");
  // for (int i = 95; i >= 0; i--) {
  //   printf("%d", getBit(res, i));
  // }
  // printf("\n res.bits[3]:\n");
  // for (int i = 127; i >= 96; i--) {
  //   printf("%d", getBit(res, i));
  // }
  // float res_float = 0.0;
  // s21_from_decimal_to_float(res, &res_float);
  // printf("\n sclae res =%d", getScale(res));
  printf("\n  s21_is_less_or_equal = %d\n", s21_is_less_or_equal(dec, val2));
  // float result = 0;
  // s21_from_decimal_to_float(res, &result);
  // printf("\nresult = %f\n", result);

  // sign = getSign(res) ? -1 : 1;
  // printf("\n sclae res =%d", getScale(res));
  // printf("\n sign res =%d\n", getSign(res));
  // printf("\n SUMM=%f\n", sign * (res.bits[0] * pow(10, -1 *
  // getScale(res))));

  // s21_decimal c = {{0, 0, 0, ~(INT_MAX)}};
  // s21_decimal d = {{0, 0, 0, ~(INT_MAX)}};
  // s21_decimal etalon = {{0, 0, 0, ~(INT_MAX)}};
  // printf("\n C:\n");
  // for (int i = 95; i >= 0; i--) {
  //   printf("%d", getBit(c, i));
  // }
  // printf("\n D:\n");
  // for (int i = 95; i >= 0; i--) {
  //   printf("%d", getBit(d, i));
  // }
  // s21_decimal ress = {{0, 0, 0, 0}};
  // s21_decimal *p_res = &ress;

  // int add = s21_add(c, d, p_res);
  // printf("\nADD = %d\n", add);

  // printf("\n EQUAL = %d\nEtalon:\n", equal);
  // for (int i = 95; i >= 0; i--) {
  //   printf("%d", getBit(etalon, i));
  // }
  // printf("\n RESS:\n");
  // for (int i = 95; i >= 0; i--) {
  //   printf("%d", getBit(ress, i));
  // }
  return 0;
}
