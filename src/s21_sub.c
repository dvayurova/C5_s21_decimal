#include "s21_decimal.h"

int s21_sub(s21_decimal value_1, s21_decimal value_2, s21_decimal *result) {
  int res_code = 0, res_scale = 0;
  memset(result, 0, sizeof(*result));
  int sign_val1 = getSign(value_1);
  int sign_val2 = getSign(value_2);
  if (getScale(value_1) != getScale(value_2))
    res_scale = to_same_scale(&value_1, &value_2);
  else
    res_scale = getScale(value_1);
  if (sign_val1 == sign_val2) { // оба знака одинаковы - вычитаем из большего
    sub_from_big(value_1, value_2, result, sign_val1, sign_val2);
  } else { // знаки разные -складываем
    res_code = binary_sum(value_1, value_2, result);
    if (sign_val1)
      setSign(result, sign_val1);
  }
  setScale(result, res_scale);
  return res_code; // пока без обработки ошибок
}