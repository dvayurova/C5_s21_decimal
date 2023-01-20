#include <check.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "s21_decimal.h"

START_TEST(add_Test) {
  s21_decimal zero = {0};
  s21_decimal negative_zero = {0};
  negative_zero.bits[3] = 0b10000000000000000000000000000000;
  s21_decimal big_num_positive = {0};
  big_num_positive.bits[0] = UINT_MAX;
  big_num_positive.bits[1] = UINT_MAX;
  big_num_positive.bits[2] = UINT_MAX;
  big_num_positive.bits[3] = 0;
  s21_decimal big_num_negative = {0};
  big_num_negative.bits[0] = UINT_MAX;
  big_num_negative.bits[1] = UINT_MAX;
  big_num_negative.bits[2] = UINT_MAX;
  big_num_negative.bits[3] = 0b10000000000000000000000000000000;
  s21_decimal long_num_positive = {0};
  big_num_positive.bits[0] = UINT_MAX;
  big_num_positive.bits[1] = UINT_MAX;
  big_num_positive.bits[2] = UINT_MAX;
  big_num_positive.bits[3] = 0b00000000000111000000000000000000; // scale= 28
  s21_decimal long_num_negative = {0};
  big_num_negative.bits[0] = UINT_MAX;
  big_num_negative.bits[1] = UINT_MAX;
  big_num_negative.bits[2] = UINT_MAX;
  big_num_negative.bits[3] = 0b10000000000111000000000000000000; // scale = 28
  s21_decimal one = {0};
  one.bits[0] = 1;
  s21_decimal ten = {0};
  ten.bits[0] = 10;
  s21_decimal other_ten = {0};
  other_ten.bits[0] = 10000000;
  other_ten.bits[3] = 0b00000000000001100000000000000000; // scale = 6
  s21_decimal other_ten_neg = {0};
  other_ten_neg.bits[0] = 10000000;
  other_ten_neg.bits[3] = 0b10000000000001100000000000000000; // scale = 6
  int random_pos_int = 2345;
  int random_neg_int = -2345;
  float random_pos_float = 6545.45456;
  float random_pos_float2 = 22.98236;
  float random_neg_float = -6545.45456;
  float random_neg_float2 = -65.12345;

  s21_decimal result = {0};
  s21_decimal tmp1 = {0};
  s21_decimal tmp2 = {0};
  int res = 0;
  float res_float = 0.0;

  s21_from_int_to_decimal(random_pos_int, &tmp1);
  s21_add(tmp1, ten, &result);
  s21_from_decimal_to_int(result, &res);
  ck_assert_int_eq(res, random_pos_int + 10);

  memset(&result, 0, sizeof(result));
  memset(&tmp1, 0, sizeof(tmp1));
  s21_add(zero, negative_zero, &result);
  res = 0;
  s21_from_decimal_to_int(result, &res);
  ck_assert_int_eq(res, 0);

  memset(&result, 0, sizeof(result));
  res = 0;
  s21_from_int_to_decimal(random_pos_int, &tmp1);
  s21_add(zero, tmp1, &result);
  s21_from_decimal_to_int(result, &res);
  ck_assert_int_eq(res, random_pos_int);

  memset(&result, 0, sizeof(result));
  memset(&tmp1, 0, sizeof(tmp1));
  res = 0;
  s21_from_int_to_decimal(random_neg_int, &tmp1);
  s21_add(zero, tmp1, &result);
  s21_from_decimal_to_int(result, &res);
  ck_assert_int_eq(res, random_neg_int);

  memset(&result, 0, sizeof(result));
  memset(&tmp1, 0, sizeof(tmp1));
  memset(&tmp2, 0, sizeof(tmp2));
  res = 0;
  s21_from_int_to_decimal(random_pos_int, &tmp1);
  s21_from_int_to_decimal(random_neg_int, &tmp2);
  s21_add(tmp1, tmp2, &result);
  s21_from_decimal_to_int(result, &res);
  ck_assert_int_eq(res, random_pos_int + random_neg_int);

  memset(&result, 0, sizeof(result));
  res = 0;
  s21_add(one, other_ten, &result);
  s21_from_decimal_to_int(result, &res);
  ck_assert_int_eq(res, 11);

  memset(&result, 0, sizeof(result));
  res = 0;
  s21_add(ten, other_ten, &result);
  s21_from_decimal_to_int(result, &res);
  ck_assert_int_eq(res, 20);

  memset(&result, 0, sizeof(result));
  res = 0;
  s21_add(ten, other_ten_neg, &result);
  s21_from_decimal_to_int(result, &res);
  ck_assert_int_eq(res, 0);

  memset(&tmp1, 0, sizeof(tmp1));
  memset(&tmp2, 0, sizeof(tmp2));
  memset(&result, 0, sizeof(result));
  s21_from_float_to_decimal(random_pos_float, &tmp1);
  s21_from_float_to_decimal(random_pos_float2, &tmp2);
  s21_add(tmp1, tmp2, &result);
  s21_from_decimal_to_float(result, &res_float);
  ck_assert_float_eq_tol(res_float, random_pos_float + random_pos_float2,
                         1e-02);

  memset(&tmp1, 0, sizeof(tmp1));
  memset(&tmp2, 0, sizeof(tmp2));
  memset(&result, 0, sizeof(result));
  s21_from_float_to_decimal(random_neg_float, &tmp1);
  s21_from_float_to_decimal(random_neg_float2, &tmp2);
  s21_add(tmp1, tmp2, &result);
  s21_from_decimal_to_float(result, &res_float);
  ck_assert_float_eq_tol(res_float, random_neg_float + random_neg_float2,
                         1e-02);

  memset(&result, 0, sizeof(result));
  s21_add(big_num_positive, zero, &result);
  ck_assert_int_eq(result.bits[0], big_num_positive.bits[0]);
  ck_assert_int_eq(result.bits[1], big_num_positive.bits[1]);
  ck_assert_int_eq(result.bits[2], big_num_positive.bits[2]);
  ck_assert_int_eq(result.bits[3], big_num_positive.bits[3]);

  memset(&result, 0, sizeof(result));
  s21_add(big_num_positive, big_num_negative, &result);
  s21_from_decimal_to_int(result, &res);
  ck_assert_int_eq(res, 0);

  memset(&result, 0, sizeof(result));
  s21_add(long_num_positive, long_num_negative, &result);
  s21_from_decimal_to_int(result, &res);
  ck_assert_int_eq(res, 0);
  // memset(&result, 0, sizeof(result));
  // memset(&tmp1, 0, sizeof(tmp1));
  // s21_add(long_num_positive, big_num_negative, &result);
}
END_TEST

START_TEST(sub_Test) {
  s21_decimal zero = {0};
  s21_decimal negative_zero = {0};
  negative_zero.bits[3] = 0b10000000000000000000000000000000;
  s21_decimal big_num_positive = {0};
  big_num_positive.bits[0] = UINT_MAX;
  big_num_positive.bits[1] = UINT_MAX;
  big_num_positive.bits[2] = UINT_MAX;
  big_num_positive.bits[3] = 0;
  s21_decimal big_num_negative = {0};
  big_num_negative.bits[0] = UINT_MAX;
  big_num_negative.bits[1] = UINT_MAX;
  big_num_negative.bits[2] = UINT_MAX;
  big_num_negative.bits[3] = 0b10000000000000000000000000000000;
  s21_decimal long_num_positive = {0};
  big_num_positive.bits[0] = UINT_MAX;
  big_num_positive.bits[1] = UINT_MAX;
  big_num_positive.bits[2] = UINT_MAX;
  big_num_positive.bits[3] = 0b00000000000111000000000000000000; // scale 28
  s21_decimal long_num_negative = {0};
  big_num_negative.bits[0] = UINT_MAX;
  big_num_negative.bits[1] = UINT_MAX;
  big_num_negative.bits[2] = UINT_MAX;
  big_num_negative.bits[3] = 0b10000000000111000000000000000000; // scale = 28
  s21_decimal one = {0};
  one.bits[0] = 1;
  s21_decimal ten = {0};
  ten.bits[0] = 10;
  s21_decimal other_ten = {0};
  other_ten.bits[0] = 10000000;
  other_ten.bits[3] = 0b00000000000001100000000000000000; // scale = 6
  s21_decimal other_ten_neg = {0};
  other_ten_neg.bits[0] = 10000000;
  other_ten_neg.bits[3] = 0b10000000000001100000000000000000; // scale = 6
  int random_pos_int = 2345;
  int random_neg_int = -2345;
  float random_pos_float = 6545.45456;
  float random_pos_float2 = 22.98236;
  float random_neg_float = -6545.45456;
  float random_neg_float2 = -65.12345;

  s21_decimal result = {0};
  s21_decimal tmp1 = {0};
  s21_decimal tmp2 = {0};
  int res = 0;
  float res_float = 0.0;

  s21_from_int_to_decimal(random_pos_int, &tmp1);
  s21_sub(tmp1, ten, &result);
  s21_from_decimal_to_int(result, &res);
  ck_assert_int_eq(res, random_pos_int - 10);

  memset(&result, 0, sizeof(result));
  memset(&tmp1, 0, sizeof(tmp1));
  s21_sub(zero, negative_zero, &result);
  res = 0;
  s21_from_decimal_to_int(result, &res);
  ck_assert_int_eq(res, 0);

  memset(&result, 0, sizeof(result));
  res = 0;
  s21_from_int_to_decimal(random_pos_int, &tmp1);
  s21_sub(zero, tmp1, &result);
  s21_from_decimal_to_int(result, &res);
  ck_assert_int_eq(res, 0 - random_pos_int);

  memset(&result, 0, sizeof(result));
  memset(&tmp1, 0, sizeof(tmp1));
  res = 0;
  s21_from_int_to_decimal(random_neg_int, &tmp1);
  s21_sub(zero, tmp1, &result);
  s21_from_decimal_to_int(result, &res);
  ck_assert_int_eq(res, 0 - random_neg_int);

  memset(&result, 0, sizeof(result));
  memset(&tmp1, 0, sizeof(tmp1));
  res = 0;
  s21_from_int_to_decimal(random_pos_int, &tmp1);
  s21_from_int_to_decimal(random_neg_int, &tmp2);
  s21_sub(tmp1, tmp2, &result);
  s21_from_decimal_to_int(result, &res);
  ck_assert_int_eq(res, random_pos_int - random_neg_int);

  memset(&result, 0, sizeof(result));
  res = 0;
  s21_sub(one, other_ten, &result);
  s21_from_decimal_to_int(result, &res);
  ck_assert_int_eq(res, -9);

  memset(&result, 0, sizeof(result));
  res = 0;
  s21_sub(ten, other_ten, &result);
  s21_from_decimal_to_int(result, &res);
  ck_assert_int_eq(res, 0);

  memset(&result, 0, sizeof(result));
  res = 0;
  s21_sub(ten, other_ten_neg, &result);
  s21_from_decimal_to_int(result, &res);
  ck_assert_int_eq(res, 20);

  memset(&tmp1, 0, sizeof(tmp1));
  memset(&tmp2, 0, sizeof(tmp2));
  memset(&result, 0, sizeof(result));
  s21_from_float_to_decimal(random_pos_float, &tmp1);
  s21_from_float_to_decimal(random_pos_float2, &tmp2);
  s21_sub(tmp1, tmp2, &result);
  s21_from_decimal_to_float(result, &res_float);
  ck_assert_float_eq_tol(res_float, random_pos_float - random_pos_float2,
                         1e-02);

  memset(&tmp1, 0, sizeof(tmp1));
  memset(&tmp2, 0, sizeof(tmp2));
  memset(&result, 0, sizeof(result));
  s21_from_float_to_decimal(random_neg_float, &tmp1);
  s21_from_float_to_decimal(random_neg_float2, &tmp2);
  s21_sub(tmp1, tmp2, &result);
  s21_from_decimal_to_float(result, &res_float);
  ck_assert_float_eq_tol(res_float, random_neg_float - random_neg_float2,
                         1e-02);

  memset(&result, 0, sizeof(result));
  s21_sub(big_num_positive, zero, &result);
  ck_assert_int_eq(result.bits[0], big_num_positive.bits[0]);
  ck_assert_int_eq(result.bits[1], big_num_positive.bits[1]);
  ck_assert_int_eq(result.bits[2], big_num_positive.bits[2]);
  ck_assert_int_eq(result.bits[3], big_num_positive.bits[3]);

  ck_assert_int_eq(1, s21_sub(big_num_positive, big_num_negative, &result));

  memset(&result, 0, sizeof(result));
  s21_sub(long_num_positive, long_num_negative, &result);
  s21_from_decimal_to_int(result, &res);
  ck_assert_int_eq(res, 0);
}
END_TEST

int main(void) {
  Suite *suite =
      suite_create("S21_TEST"); // создание наборов тестов и с именем S21_TEST
  SRunner *srunner = srunner_create(suite);

  TCase *s21add_test =
      tcase_create("s21add"); // создание тестового примера с именем s21add
  suite_add_tcase(suite, s21add_test); // добавление в тестовый набор
  tcase_add_test(s21add_test,
                 add_Test); // добавление тестовой функции в тестовый пример

  TCase *s21sub_test = tcase_create("s21sub");
  suite_add_tcase(suite, s21sub_test);
  tcase_add_test(s21sub_test, sub_Test);

  srunner_run_all(srunner, CK_VERBOSE);
  int number_failed = srunner_ntests_failed(srunner);
  srunner_free(srunner);

  return number_failed == 0 ? EXIT_SUCCESS : EXIT_FAILURE;
}
