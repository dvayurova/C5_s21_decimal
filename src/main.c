#include <limits.h>
#include <math.h>
#include <stdio.h>
#include <string.h>

typedef struct {
  unsigned int bits[4];
} s21_decimal;

int getBit(s21_decimal d, int i) { return !!(d.bits[i / 32] & (1 << i % 32)); }

int setBit(s21_decimal *x, int index, int bit) {
  unsigned int mask = 1u << index % 32;
  if (bit == 1) {
    x->bits[index / 32] |= mask;
  } else {
    x->bits[index / 32] &= ~mask;
  }
  return 0;
}

int getSign(s21_decimal d) {
  int sign = 1;
  if ((d.bits[3] & (1 << 31)) == 0)
    sign = 0;
  return sign;
}

int setSign(s21_decimal *x, int sign) {
  if (sign == 0)
    setBit(x, 127, 0);
  if (sign == 1)
    setBit(x, 127, 1);
  return sign;
}

int getScale(s21_decimal x) {
  int bit = 0;
  int rez = 0;
  for (int i = 112; i < 117; i++) {
    bit = getBit(x, i);
    if (bit)
      rez += pow(2, (i - 112));
  }
  return rez;
}

int setScale(s21_decimal *d, int scale) {
  int bit = 0;
  if (scale <= 28 && scale >= 0) {
    for (int i = 4; i >= 0; i--) {
      bit = !!(scale & (1 << i));
      setBit(d, 112 + i, bit);
    }
    bit = 1;
  }
  return bit; // if scale < 0 || > 28 return 0
}

// binary_sum - алгоритм суммирования положительных целых чисел в двоичной
// системе
int binary_sum(s21_decimal value_1, s21_decimal value_2, s21_decimal *res) {
  memset(res, 0, sizeof(*res));
  int tmp = 0;
  for (int i = 0; i < 96; i++) {
    if (getBit(value_1, i) && getBit(value_2, i) && tmp)
      setBit(res, i, 1);
    else if (getBit(value_1, i) && getBit(value_2, i) && !tmp)
      tmp = 1;
    else if ((getBit(value_1, i) + getBit(value_2, i)) == 1 && !tmp)
      setBit(res, i, 1);
    else if (!getBit(value_1, i) && !getBit(value_2, i) && tmp) {
      setBit(res, i, 1);
      tmp = 0;
    }
  }
  return tmp; // возвращает tmp для отслеживания переполнения
}

// dop_Code - перевод в дополнительный код (для вычитания)
void dop_Code(s21_decimal value, s21_decimal *dop_code, int first_index) {
  for (int i = first_index; i >= 0; i--) {
    setBit(dop_code, i, getBit(value, i) ? 0 : 1);
  }
  dop_code->bits[0] += 1;
}

int first_bit(s21_decimal dec) { // определяет индекс первого ненулевого бита
  int first = 0;
  for (int i = 95; i >= 0 && !first; i--) {
    if (getBit(dec, i))
      first = i;
  }
  return first;
}

// binary_sub - алгоритм вычитания положительных целых чисел в двоичной
// системе
void binary_sub(s21_decimal value_1, s21_decimal value_2, s21_decimal *res) {
  s21_decimal val2_dop_code = {0};
  int first_Bit = first_bit(value_1);
  dop_Code(value_2, &val2_dop_code, first_Bit);
  binary_sum(value_1, val2_dop_code, res);
  if (getBit(*res, first_Bit + 1))
    setBit(res, first_Bit + 1, 0);
}

void copy_decimal(s21_decimal src, s21_decimal *dst) {
  dst->bits[0] = src.bits[0];
  dst->bits[1] = src.bits[1];
  dst->bits[2] = src.bits[3];
  dst->bits[3] = src.bits[3];
}

int scale_up(s21_decimal *value_smallScale, int bigger_scale,
             int smaller_scale) {
  printf("\n smaller_scale = %d\n", smaller_scale);
  int is_inf = 0, sign = 0;
  s21_decimal tmp = {0};
  s21_decimal res = {0};
  sign = getSign(*value_smallScale);
  copy_decimal(*value_smallScale, &tmp);
  while (smaller_scale < bigger_scale && !is_inf) {
    for (int i = 1; i < 10; i++) {
      is_inf = binary_sum(tmp, *value_smallScale, &res);
      copy_decimal(res, &tmp);
    }
    if (!is_inf) {
      copy_decimal(res, value_smallScale);
      copy_decimal(*value_smallScale, &tmp);
      smaller_scale += 1;
    }
  }
  setScale(value_smallScale, smaller_scale);
  setSign(value_smallScale, sign);
  printf("\n SCALE UP = %d\n", is_inf);
  return is_inf ? smaller_scale
                : -1; // возвращает -1 в случае успешного повышения или
                      // smaller_scale в случае переполнения
}

// ф-я выравнивания степеней
int to_same_scale(s21_decimal *value_1, s21_decimal *value_2) {
  int scale1 = getScale(*value_1);
  int scale2 = getScale(*value_2);
  int new_scale = 0;
  if (scale2 > scale1)
    new_scale = scale_up(value_1, scale2, scale1);
  else if (scale1 > scale2)
    new_scale = scale_up(value_2, scale1, scale2);
  if (new_scale != -1)
    new_scale = 0; // потом убрать
  // scale_down(value_2, scale1, new_scale);
  new_scale = getScale(*value_2);
  // printf("\nNEW_SCALE= %d\n", new_scale);
  return new_scale;
}

int is_zero(s21_decimal dec) {
  int zero = 0;
  if (!dec.bits[0] && !dec.bits[1] && !dec.bits[2])
    zero = 1;
  return zero;
}
int s21_is_equal(s21_decimal value_1, s21_decimal value_2) {
  int res = 0, stop = 0;
  if (is_zero(value_1) && is_zero(value_2))
    res = 1;
  else if (value_1.bits[3] == value_2.bits[3]) {
    if (value_1.bits[0] == value_2.bits[0] &&
        value_1.bits[1] == value_2.bits[1] &&
        value_1.bits[2] == value_2.bits[2])
      res = 1;
  } else if (getSign(value_1) == getSign(value_2)) {
    to_same_scale(&value_1, &value_2); // сравнение после приведения скейлов
    for (int i = 95; i >= 0 && !stop; i--) {
      if (getBit(value_1, i) != getBit(value_2, i))
        stop = 1;
    }
    res = stop ? 0 : 1;
  }
  return res;
}

int s21_is_less(s21_decimal value_1, s21_decimal value_2) {
  int res = 0, stop = 0;
  int bit_val1 = 0;
  int bit_val2 = 0;
  if (!s21_is_equal(value_1, value_2)) {
    if (is_zero(value_1))
      setSign(&value_1, 0);
    if (is_zero(value_2))
      setSign(&value_2, 0);
    if (getSign(value_1) && !getSign(value_2))
      res = 1;
    else if (getSign(value_1) == getSign(value_2)) {
      to_same_scale(&value_1, &value_2);
      bit_val1 = first_bit(value_1);
      bit_val2 = first_bit(value_2);
      if (bit_val2 > bit_val1)
        res = 1;
      else if (bit_val2 == bit_val1) {
        for (int i = bit_val2; i >= 0 && !stop; i--) {
          if (getBit(value_1, i) > getBit(value_2, i))
            stop = 1; // если стоп, значит value_1 больше
        }
        res = stop ? 0 : 1;
      }
      res = getSign(value_1) ? !res : res;
    }
  }
  return res;
}

int s21_is_less_or_equal(s21_decimal value_1, s21_decimal value_2) {
  int res = 0;
  if (s21_is_less(value_1, value_2) || s21_is_equal(value_1, value_2))
    res = 1;
  return res;
}

int s21_is_greater(s21_decimal value_1, s21_decimal value_2) {
  int res = 0;
  if (!s21_is_less(value_1, value_2) && !s21_is_equal(value_1, value_2))
    res = 1;
  return res;
}

int s21_is_greater_or_equal(s21_decimal value_1, s21_decimal value_2) {
  int res = 0;
  if (!s21_is_less(value_1, value_2))
    res = 1;
  return res;
}

int s21_is_not_equal(s21_decimal value_1, s21_decimal value_2) {
  int res = 0;
  if (!s21_is_equal(value_1, value_2))
    res = 1;
  return res;
}

void sub_from_big(s21_decimal value_1, s21_decimal value_2, s21_decimal *result,
                  int sign_val1, int sign_val2) {
  setSign(&value_1, 0);
  setSign(&value_2, 0);
  if (s21_is_greater_or_equal(value_1, value_2)) {
    binary_sub(value_1, value_2, result);
    if (sign_val1)
      setSign(result, sign_val1);
  } else {
    binary_sub(value_2, value_1, result);
    if (sign_val2)
      setSign(result, sign_val2);
  }
}

void masCreat(unsigned int *arr, s21_decimal x) {
  int index = 0;
  for (int i = 0; i < 128; i++) {
    arr[index] = getBit(x, i);
    index++;
  }
}

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

int s21_sub(s21_decimal value_1, s21_decimal value_2, s21_decimal *result) {
  int res_code = 0, res_scale = 0;
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

int main() {
  s21_decimal dec;
  dec.bits[0] = 0;
  dec.bits[1] = 0;
  dec.bits[2] = 0;
  dec.bits[3] = 0b00000000000001100000000000000000;
  for (int i = 95; i >= 0; i--) {
    printf("%d", getBit(dec, i));
  }
  printf("\ndec.bits[3]:\n");
  for (int i = 127; i >= 96; i--) {
    printf("%d", getBit(dec, i));
  }
  printf("\n sclae val1 =%d", getScale(dec));
  printf("\n sign val1 =%d", getSign(dec));
  int sign = getSign(dec) ? -1 : 1;
  printf("\n VALUE1=%f\n", sign * (dec.bits[0] * pow(10, -1 * getScale(dec))));
  s21_decimal val2;
  val2.bits[0] = 0;
  val2.bits[1] = 0;
  val2.bits[2] = 0;
  val2.bits[3] = 0b00000000000001100000000000000000;
  // 0b00000000000001100000000000000000 = 6
  for (int i = 95; i >= 0; i--) {
    printf("%d", getBit(val2, i));
  }
  printf("\nval2.bits[3]:\n");
  for (int i = 127; i >= 96; i--) {
    printf("%d", getBit(val2, i));
  }
  sign = getSign(val2) ? -1 : 1;
  printf("\n sclae val2 =%d", getScale(val2));
  printf("\n sign val2 =%d", getSign(val2));
  printf("\n VALUE2=%f\n",
         sign * (val2.bits[0] * pow(10, -1 * getScale(val2))));

  s21_decimal res = {0};
  printf("\n s21_add code result = %d\n", s21_add(dec, val2, &res));
  for (int i = 95; i >= 0; i--) {
    printf("%d", getBit(res, i));
  }
  sign = getSign(res) ? -1 : 1;
  printf("\n sclae res =%d", getScale(res));
  printf("\n sign res =%d\n", getSign(res));
  printf("\n SUMM=%f\n", sign * (res.bits[0] * pow(10, -1 * getScale(res))));

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

  // int equal = s21_is_equal(res, etalon);
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
