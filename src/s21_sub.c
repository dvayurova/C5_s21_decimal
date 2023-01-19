#include "s21_decimal.h"

int s21_sub(s21_decimal value_1, s21_decimal value_2, s21_decimal *result) {
  int res_code = 0, res_scale = 0, sign_val1 = 0, sign_val2 = 0;
  sign_val1 = getSign(value_1);
  sign_val2 = getSign(value_2);
  long_decimal value_1_long = {0};
  long_decimal value_2_long = {0};
  long_decimal result_long = {0};
  from_decimal_to_long(value_1, &value_1_long);
  from_decimal_to_long(value_2, &value_2_long);
  if (is_zero(value_1_long))
    copy_decimal_long(value_2_long, &result_long);
  else if (is_zero(value_2_long))
    copy_decimal_long(value_1_long, &result_long);
  else {
    if (getScale_long(value_1_long) != getScale_long(value_2_long))
      res_scale = to_same_scale(&value_1_long, &value_2_long);
    else
      res_scale = getScale_long(value_1_long);
    if (sign_val1 == sign_val2) { // оба знака одинаковы - вычитаем из большего
      sub_from_big(value_1_long, value_2_long, &result_long, sign_val1,
                   sign_val2);
    } else { // знаки разные -складываем
      res_code = binary_sum(value_1_long, value_2_long, &result_long);
      if (sign_val1)
        setSign_long(&result_long, sign_val1);
    }
    setScale_long(&result_long, res_scale);
  }
  div_long(result_long, result);
  return res_code; // пока без обработки ошибок
}