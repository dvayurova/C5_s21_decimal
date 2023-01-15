#include <limits.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
enum { LOW, MIDLE, HIGHE, SCALE };
typedef struct {
  unsigned int bits[4];
} s21_decimal;

typedef struct {
  unsigned int bits[8];
} s21_decimal_long;

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
int getBit_long(s21_decimal_long d, int i) {
  return !!(d.bits[i / 32] & (1 << i % 32));
}

int setBit_long(s21_decimal_long *x, int index, int bit) {
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

int getSign_long(s21_decimal_long d) {
  int sign = 1;
  if ((d.bits[7] & (1 << 31)) == 0)
    sign = 0;
  return sign;
}

int setSign_long(s21_decimal_long *x, int sign) {
  if (sign == 0)
    setBit_long(x, 255, 0);
  if (sign == 1)
    setBit_long(x, 255, 1);
  return sign;
}

int getScale_long(s21_decimal_long x) {
  int bit = 0;
  int rez = 0;
  for (int i = 240; i < 245; i++) {
    bit = getBit_long(x, i);
    if (bit)
      rez += pow(2, (i - 240));
  }
  return rez;
}

int setScale_long(s21_decimal_long *d, int scale) {
  int bit = 0;
  if (scale <= 28 && scale >= 0) {
    for (int i = 4; i >= 0; i--) {
      bit = !!(scale & (1 << i));
      setBit_long(d, 240 + i, bit);
    }
    bit = 1;
  }
  return bit; // if scale < 0 || > 28 return 0
}

// binary_sum - алгоритм суммирования положительных целых чисел в двоичной
// системе
int binary_sum(s21_decimal_long value_1, s21_decimal_long value_2,
               s21_decimal_long *res) {
  memset(res, 0, sizeof(*res));
  int tmp = 0;
  for (int i = 0; i < 224; i++) {
    if (getBit_long(value_1, i) && getBit_long(value_2, i) && tmp)
      setBit_long(res, i, 1);
    else if (getBit_long(value_1, i) && getBit_long(value_2, i) && !tmp)
      tmp = 1;
    else if ((getBit_long(value_1, i) + getBit_long(value_2, i)) == 1 && !tmp)
      setBit_long(res, i, 1);
    else if (!getBit_long(value_1, i) && !getBit_long(value_2, i) && tmp) {
      setBit_long(res, i, 1);
      tmp = 0;
    }
  }
  return tmp; // возвращает tmp для отслеживания переполнения
}

// dop_Code - перевод в дополнительный код (для вычитания)
void dop_Code(s21_decimal_long value, s21_decimal_long *dop_code,
              int first_index) {
  for (int i = first_index; i >= 0; i--) {
    setBit_long(dop_code, i, getBit_long(value, i) ? 0 : 1);
  }
  dop_code->bits[0] += 1;
}

int first_bit_long(
    s21_decimal_long dec) { // определяет индекс первого ненулевого бита
  int first = 0;
  for (int i = 223; i >= 0 && !first; i--) {
    if (getBit_long(dec, i))
      first = i;
  }
  return first;
}

// binary_sub - алгоритм вычитания положительных целых чисел в двоичной
// системе
void binary_sub(s21_decimal_long value_1, s21_decimal_long value_2,
                s21_decimal_long *res) {
  s21_decimal_long val2_dop_code = {0};
  memset(res, 0, sizeof(*res));
  int first_Bit = first_bit_long(value_1);
  dop_Code(value_2, &val2_dop_code, first_Bit);
  binary_sum(value_1, val2_dop_code, res);
  if (getBit_long(*res, first_Bit + 1))
    setBit_long(res, first_Bit + 1, 0);
}

void copy_decimal_long(s21_decimal_long src, s21_decimal_long *dst) {
  dst->bits[0] = src.bits[0];
  dst->bits[1] = src.bits[1];
  dst->bits[2] = src.bits[2];
  dst->bits[3] = src.bits[3];
  dst->bits[4] = src.bits[4];
  dst->bits[5] = src.bits[5];
  dst->bits[6] = src.bits[6];
  dst->bits[7] = src.bits[7];
}

void from_decimal_to_long(s21_decimal dec, s21_decimal_long *dec_long) {
  memset(dec_long, 0, sizeof(*dec_long));
  dec_long->bits[0] = dec.bits[0];
  dec_long->bits[1] = dec.bits[1];
  dec_long->bits[2] = dec.bits[2];
  dec_long->bits[3] = 0;
  dec_long->bits[4] = 0;
  dec_long->bits[5] = 0;
  dec_long->bits[6] = 0;
  dec_long->bits[7] = dec.bits[3];
}

// void from_long_to_decimal(s21_decimal *dec, s21_decimal_long dec_long) {
//   memset(dec, 0, sizeof(*dec));
//   int scale = 0;
//   scale = getScale(dec_long);
//   for (int i = 6; i != 0; i--) {
//     while (dec->bits[i] != 0) {

//     }
//   }
// }

int scale_up(s21_decimal_long *value_smallScale, int bigger_scale,
             int smaller_scale) {
  int is_inf = 0, sign = 0;
  s21_decimal_long tmp = {0};
  s21_decimal_long res = {0};
  sign = getSign_long(*value_smallScale);
  copy_decimal_long(*value_smallScale, &tmp);
  while (smaller_scale < bigger_scale && !is_inf) {
    for (int i = 1; i < 10; i++) {
      is_inf = binary_sum(tmp, *value_smallScale, &res);
      copy_decimal_long(res, &tmp);
    }
    if (!is_inf) {
      copy_decimal_long(res, value_smallScale);
      copy_decimal_long(*value_smallScale, &tmp);
      smaller_scale += 1;
    }
  }
  setScale_long(value_smallScale, smaller_scale);
  setSign_long(value_smallScale, sign);
  return smaller_scale;
}

// ф-я выравнивания степеней
int to_same_scale(s21_decimal_long *value_1, s21_decimal_long *value_2) {
  int scale1 = getScale_long(*value_1);
  int scale2 = getScale_long(*value_2);
  int new_scale = 0;
  if (scale2 > scale1)
    new_scale = scale_up(value_1, scale2, scale1);
  else if (scale1 > scale2)
    new_scale = scale_up(value_2, scale1, scale2);
  return new_scale;
}

int is_zero(s21_decimal_long dec) {
  int zero = 0;
  if (!dec.bits[0] && !dec.bits[1] && !dec.bits[2] && !dec.bits[3] &&
      !dec.bits[4] && !dec.bits[5] && !dec.bits[6])
    zero = 1;
  return zero;
}

void masCreat(unsigned int *arr, s21_decimal x) {
  int index = 0;
  for (int i = 0; i < 128; i++) {
    arr[index] = getBit(x, i);
    index++;
  }
}

int float_getScale(float value) {
  int res = 0;
  unsigned int fbits = *((unsigned int *)&value);
  int index = 31;
  for (unsigned int mask = 0x80000000; index > 22; mask >>= 1, index--) {
    if (!!(fbits & mask) && (index != 31))
      res += pow(2, index - 23);
  }
  return res - 127;
}

void shift(s21_decimal *x, int sign) {
  if (sign > 0) {
    x->bits[HIGHE] <<= 1;
    if (getBit(*x, 63))
      setBit(x, 64, 1);
    x->bits[MIDLE] <<= 1;
    if (getBit(*x, 31))
      setBit(x, 32, 1);
    x->bits[LOW] <<= 1;
  }

  if (sign < 0) {
    x->bits[LOW] >>= 1;
    if (getBit(*x, 32))
      setBit(x, 31, 1);
    x->bits[MIDLE] >>= 1;
    if (getBit(*x, 64))
      setBit(x, 63, 1);
    x->bits[HIGHE] >>= 1;
  }
}
int s21_is_equal(s21_decimal value_1, s21_decimal value_2) {
  int res = 0, stop = 0;
  s21_decimal_long value_1_long = {0};
  s21_decimal_long value_2_long = {0};
  from_decimal_to_long(value_1, &value_1_long);
  from_decimal_to_long(value_2, &value_2_long);
  if (is_zero(value_1_long) && is_zero(value_2_long))
    res = 1;
  else if (value_1.bits[3] == value_2.bits[3]) {
    if (value_1.bits[0] == value_2.bits[0] &&
        value_1.bits[1] == value_2.bits[1] &&
        value_1.bits[2] == value_2.bits[2])
      res = 1;
  } else if (getSign(value_1) == getSign(value_2)) {
    to_same_scale(&value_1_long,
                  &value_2_long); // сравнение после приведения скейлов
    for (int i = 223; i >= 0 && !stop; i--) {
      if (getBit_long(value_1_long, i) != getBit_long(value_2_long, i))
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
  s21_decimal_long value_1_long = {0};
  s21_decimal_long value_2_long = {0};
  from_decimal_to_long(value_1, &value_1_long);
  from_decimal_to_long(value_2, &value_2_long);
  if (!s21_is_equal(value_1, value_2)) {
    if (is_zero(value_1_long))
      setSign_long(&value_1_long, 0);
    if (is_zero(value_2_long))
      setSign_long(&value_2_long, 0);
    if (getSign(value_1) && !getSign(value_2))
      res = 1;
    else if (getSign(value_1) == getSign(value_2)) {
      to_same_scale(&value_1_long, &value_2_long);
      bit_val1 = first_bit_long(value_1_long);
      bit_val2 = first_bit_long(value_2_long);
      printf("\n bit_val1 = %d, bit_val2 = %d \n", bit_val1, bit_val2);
      printf("\n value_1_long \n");
      for (int i = 223; i >= 0; i--) {
        printf("%d", getBit_long(value_1_long, i));
      }
      printf("\n value_1_long.bits[7]: \n");
      for (int i = 255; i >= 224; i--) {
        printf("%d", getBit_long(value_1_long, i));
      }
      printf("\n value_2_long \n");
      for (int i = 223; i >= 0; i--) {
        printf("%d", getBit_long(value_2_long, i));
      }
      printf("\n value_2_long.bits[7]: \n");
      for (int i = 255; i >= 224; i--) {
        printf("%d", getBit_long(value_2_long, i));
      }

      if (bit_val2 > bit_val1) {
        res = 1;
      } else if (bit_val2 == bit_val1) {
        for (int i = bit_val2; i >= 0 && !stop; i--) {
          if (getBit_long(value_2_long, i) > getBit_long(value_1_long, i))
            stop = 1; // если стоп, значит value_2 больше
          else if (getBit_long(value_1_long, i) > getBit_long(value_2_long, i))
            stop = 2; //  значит value_1 больше
        }
        res = stop == 1 ? 1 : 0;
      }
      res = getSign_long(value_1_long) ? !res : res;
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

void sub_from_big(s21_decimal_long value_1, s21_decimal_long value_2,
                  s21_decimal_long *result, int sign_val1, int sign_val2,
                  s21_decimal value_1, s21_decimal value_2) {
  setSign_long(&value_1_long, 0);
  setSign_long(&value_2_long, 0);
  if (s21_is_greater_or_equal(value_1, value_2)) {
    binary_sub(value_1_long, value_2_long, &result_long);
    if (sign_val1)
      setSign_long(&result_long, sign_val1);
  } else {
    binary_sub(value_1_long, value_2_long, &result_long);
    if (sign_val2)
      setSign_long(&result_long, sign_val2);
  }
}

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

  printf("\n value_1_long \n");
  for (int i = 223; i >= 0; i--) {
    printf("%d", getBit_long(value_1_long, i));
  }
  printf("\n value_1_long.bits[7]: \n");
  for (int i = 255; i >= 224; i--) {
    printf("%d", getBit_long(value_1_long, i));
  }
  printf("\n value_2_long \n");
  for (int i = 223; i >= 0; i--) {
    printf("%d", getBit_long(value_2_long, i));
  }
  printf("\n value_2_long.bits[7]: \n");
  for (int i = 255; i >= 224; i--) {
    printf("%d", getBit_long(value_2_long, i));
  }

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

  printf("\nresult\n");
  for (int i = 223; i >= 0; i--) {
    printf("%d", getBit_long(result_long, i));
  }
  printf("\nres.bits[7]:\n");
  for (int i = 255; i >= 224; i--) {
    printf("%d", getBit_long(result_long, i));
  }
  return res_code;
}

int s21_sub(s21_decimal value_1, s21_decimal value_2, s21_decimal *result) {
  int res_code = 0, res_scale = 0;
  int sign_val1 = getSign(value_1);
  int sign_val2 = getSign(value_2);
  s21_decimal_long value_1_long = {0};
  s21_decimal_long value_2_long = {0};
  s21_decimal_long result_long = {0};
  from_decimal_to_long(value_1, &value_1_long);
  from_decimal_to_long(value_2, &value_2_long);
  from_decimal_to_long(*result, &result_long);
  if (getScale_long(value_1_long) != getScale_long(value_2_long))
    res_scale = to_same_scale(&value_1_long, &value_2_long);
  else
    res_scale = getScale_long(value_1_long);
  if (sign_val1 == sign_val2) { // оба знака одинаковы - вычитаем из большего
    sub_from_big(value_1, value_2, result, sign_val1,
                 sign_val2); // надо передавать лонг
  } else {                   // знаки разные -складываем
    res_code = binary_sum(value_1_long, value_2_long, &result_long);
    if (sign_val1)
      setSign_long(&result_long, sign_val1);
  }
  setScale_long(&result_long, res_scale);
  return res_code; // пока без обработки ошибок
}

int main() {
  // float one = -3.232446546;
  s21_decimal dec = {0};
  // s21_from_int_to_decimal(one, &dec);
  // s21_from_float_to_decimal(one, &dec);
  dec.bits[0] = UINT_MAX;
  dec.bits[1] = UINT_MAX;
  dec.bits[2] = UINT_MAX;
  dec.bits[3] = 0b00000000000111000000000000000000;
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
  s21_decimal val2 = {0};
  // s21_from_int_to_decimal(two, &val2);
  val2.bits[0] = UINT_MAX;
  val2.bits[1] = UINT_MAX;
  val2.bits[2] = UINT_MAX;
  val2.bits[3] = 0b00000000000011000000000000000000;

  printf("\n is less = %d", s21_is_less(dec, val2));
  // 0b00000000000001100000000000000000 = 6
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
  // printf("\n s21_add code result = %d\n", s21_add(dec, val2, &res));

  // printf("\n sclae res =%d", getScale(res));
  // printf("\n IS GREATER = %d\n", s21_is_greater(dec, val2));
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
