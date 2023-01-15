#include "s21_decimal.h"

int s21_add(s21_decimal value_1, s21_decimal value_2, s21_decimal *result) {
  int res_code = 0, sign_val1 = 0, sign_val2 = 0, res_scale = 0;
  memset(result, 0, sizeof(*result));
  sign_val1 = getSign(value_1);
  sign_val2 = getSign(value_2);
  s21_decimal_long value_1_long = {0};
  s21_decimal_long value_2_long = {0};
  s21_decimal_long result_long = {0};
  from_decimal_to_long(value_1, &value_1_long);
  from_decimal_to_long(value_2, &value_2_long);
  from_decimal_to_long(*result, &result_long);

  if (is_zero(value_1_long))
    copy_decimal_long(value_2_long, &result_long);
  else if (is_zero(value_2_long))
    copy_decimal_long(value_1_long, &result_long);
  else {
    if (getScale_long(value_1_long) != getScale_long(value_2_long))
      res_scale = to_same_scale(&value_1_long, &value_2_long);
    else
      res_scale = getScale_long(value_1_long);
    if (sign_val1 == sign_val2) { // оба знака одинаковы
      res_code = binary_sum(value_1_long, value_2_long,
                            &result_long); // если переполнение - код 1
      if (sign_val1) {
        setSign_long(&result_long, sign_val1);
        if (res_code)
          res_code = 2; // если переполнение - код 2
      }
    } else { // знаки разные - вычитаем из большего
      sub_from_big(value_1, value_2, result, sign_val1, sign_val2);
    }
    setScale_long(&result_long, res_scale);
  }
  if (is_zero(result_long))
    setSign_long(&result_long, 0);

  return res_code;
}