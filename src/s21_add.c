#include "s21_decimal.h"

int s21_add(s21_decimal value_1, s21_decimal value_2, s21_decimal *result) {
  int res_code = 0, sign_val1 = 0, sign_val2 = 0, res_scale = 0;
  sign_val1 = getSign(value_1);
  sign_val2 = getSign(value_2);
  if (is_zero(value_1))
    copy_decimal(value_2, result);
  else if (is_zero(value_2))
    copy_decimal(value_1, result);
  else {
    if (getScale(value_1) != getScale(value_2))
      res_scale = to_same_scale(&value_1, &value_2);
    else
      res_scale = getScale(value_1);
    if (sign_val1 == sign_val2) { // оба знака одинаковы
      res_code =
          binary_sum(value_1, value_2, result); // если переполнение - код 1
      if (sign_val1) {
        setSign(result, sign_val1);
        if (res_code)
          res_code = 2; // если переполнение - код 2
      }
    } else { // знаки разные - вычитаем из большего
      sub_from_big(value_1, value_2, result, sign_val1, sign_val2);
    }
    setScale(result, res_scale);
  }
  if (is_zero(*result))
    setSign(result, 0);
  return res_code;
}