#include <limits.h>
#include <math.h>
#include <stdio.h>
#include <string.h>

typedef struct {
  unsigned int bits[4];
} s21_decimal;

int getBit(s21_decimal d, int i) { return !!(d.bits[i / 32] & (1 << i % 32)); }

void setBit(s21_decimal *d, int i, int bit) {
  if (bit == 1)
    d->bits[i / 32] = d->bits[i / 32] | (1 << i % 32);
  else if (bit == 0)
    d->bits[i / 32] = d->bits[i / 32] & ~(1 << i % 32);
}

int getSign(s21_decimal d) {
  int sign = 1;
  if ((d.bits[3] & (1 << 31)) == 0)
    sign = 0;
  return sign;
}

void setSign(s21_decimal *d, int sign) {
  if (sign == 1)
    d->bits[3] = d->bits[3] | (1 << 31);
  else if (sign == 0)
    d->bits[3] = d->bits[3] & ~(1 << 31);
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

// int less_than10(s21_decimal value, s21_decimal ten) {
//   int res = 0, stop = 0;
//   int bit_val = 0;
//   bit_val = first_bit(value);
//   if (bit_val < 3)
//     res = 1;
//   else if (bit_val == 3) {
//     if (value.bits[0] == ten.bits[0])
//       res = 0;
//     else {
//       for (int i = 3; i >= 0 && !stop; i--) {
//         if (getBit(value, i) > getBit(ten, i))
//           stop = 1;
//       }
//       res = stop ? 0 : 1;
//     }
//   }
//   return res;
// }

// s21_decimal div_ten(s21_decimal dec) {
//   int bit = 0;
//   s21_decimal ten = {0};
//   ten.bits[0] = 10;
//   s21_decimal tmp_dec = {0};
//   s21_decimal tmp = {0};
//   // for (int i = 95; i >= 0; i--) {
//   //   printf("%d", getBit(ten, i));
//   // }
//   int i = 3;
//   for (bit = first_bit(dec); bit > first_bit(dec) - 4; bit--) {
//     setBit(&tmp_dec, i, getBit(dec, bit));
//     i--;
//   }
//   // printf("\ntmp_dec\n");
//   for (int i = 95; i >= 0; i--) {
//     printf("%d", getBit(tmp_dec, i));
//   }
//   binary_sub(tmp_dec, ten, &tmp);
//   tmp.bits[0] <<= 1;
//   // printf("\ntmp\n");
//   // for (int i = 95; i >= 0; i--) {
//   //   printf("%d", getBit(tmp, i));
//   // }
//   // printf("\n less_than10 = %d\n", less_than10(tmp, ten));
//   return ten;
// }

// int scale_down(s21_decimal *value_bigScale, int bigger_scale,
//                int smaller_scale) {}

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

// s21_add пока без учета скейла
int s21_add(s21_decimal value_1, s21_decimal value_2, s21_decimal *result) {
  int res_code = 0;
  int sign_val1 = getSign(value_1);
  int sign_val2 = getSign(value_2);
  int res_scale = 0;
  if (getScale(value_1) != getScale(value_2))
    res_scale = to_same_scale(&value_1, &value_2);
  else
    res_scale = getScale(value_1);
  // printf("\n res_scale= %d\n", res_scale);
  if (sign_val1 == sign_val2) { // оба знака одинаковы
    res_code =
        binary_sum(value_1, value_2, result); // если переполнение - код 1
    if (sign_val1) {
      setSign(result, sign_val1);
      if (res_code)
        res_code = 2; // если переполнение - код 2
    }
  } else { // знаки разные - вычитаем из большего
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
  setScale(result, res_scale);
  // printf("\n =======res_scale= %d\n", getScale(*result));
  return res_code;
}

// s21_sub пока без учета скейла
int s21_sub(s21_decimal value_1, s21_decimal value_2, s21_decimal *result) {
  int res_code = 0;
  int sign_val1 = getSign(value_1);
  int sign_val2 = getSign(value_2);
  if (sign_val1 == sign_val2) { // оба знака одинаковы - вычитаем из большего
    // if (value_1 >= value_2)  // - НУЖНА ФУНКЦИЯ СРАВНЕНИЯ ДЕЦИМАЛ
    //   binary_sub(value_1, value_2, result);
    //  else
    //   binary_sub(value_2, value_1, result);
    // if (!sign_val1) //если числа положител-е, то знак у рез-та "-"
    //   setSign(result, 1);
  } else { // знаки разные -складываем
    binary_sum(value_1, value_2, result);
    if (sign_val1)
      setSign(result, sign_val1);
  }
  return res_code; // пока без обработки ошибок
}

int main() {
  s21_decimal dec;
  // dec.bits[0] = 1316134912;
  // dec.bits[1] = 3128;
  // dec.bits[2] = 0;
  // dec.bits[3] = 0b10000000000000100000000000000000;
  dec.bits[0] = 1000;
  dec.bits[1] = 0;
  dec.bits[2] = 0;
  dec.bits[3] = 0b00000000000001100000000000000000;

  // float in = s21_INF;
  // if (dec.bits[0] == s21_INF)
  // printf("\n in = %f\n", in);
  // printf("\nsclae dec=%d\n", getScale(dec));
  // printf("val1=%f, bits:\n", dec.bits[0] * pow(10, -1 * getScale(dec)));
  for (int i = 95; i >= 0; i--) {
    printf("%d", getBit(dec, i));
  }

  // setBit(&dec, 0, 0);
  // printf("\nnum after=%d, bits:\n", dec.bits[0]);
  // for (int i = 95; i >= 0; i--) {
  //   printf("%d", getBit(dec, i));
  // }
  // setSign(&dec, 1);
  // printf("\nbits[3]=");
  // for (int i = 127; i >= 96; i--) {
  //   printf("%d", getBit(dec, i));
  // }
  // printf("\nSign=%d\n", getSign(dec));
  s21_decimal val2;
  // val2.bits[0] = 1215752192;
  // val2.bits[1] = 31;
  // val2.bits[2] = 0;
  // val2.bits[3] = 0b10000000000000000000000000000000;
  val2.bits[0] = 1000;
  val2.bits[1] = 0;
  val2.bits[2] = 0;
  val2.bits[3] = 0b00000000000001100000000000000000;
  printf("\n!!!!sclae val2 =%d\n", getScale(val2));
  printf("\nval2=%f:\n", val2.bits[0] * pow(10, -1 * getScale(val2)));
  // // printf("val1=%f, bits:\n", dec.bits[0] * pow(10, -1 * getScale(dec)));
  for (int i = 95; i >= 0; i--) {
    printf("%d", getBit(val2, i));
  }

  // printf("\nsclae val2=%d\n", getScale(val2));
  // for (int i = 95; i >= 0; i--) {
  //   printf("%d", getBit(val2, i));
  // }
  // printf("\nval2=%f, bits:\n", val2.bits[0] * pow(10, -1 * getScale(val2)));
  // printf("\nsign val1 = %d, sign val2 = %d\n", getSign(dec), getSign(val2));
  // printf("\n s21_is_equal = %d\n", s21_is_equal(dec, val2));
  // printf("\nNew sclae val1=%d\n", getScale(dec));
  // printf("\nNew sclae val2=%d\n", getScale(val2));
  // printf("\n dec.bits[0]=%d\n", dec.bits[0]);
  // printf("New val2=%d\n", val2.bits[0]);
  // printf("\nnew val2=%d, bits:\n", val2.bits[0]);
  s21_decimal res = {0};
  // binary_sum(dec, val2, &res);
  // printf("\nnum=%d, bits:\n", res.bits[0]);
  s21_add(dec, val2, &res);
  printf("\nsclae res =%d\n", getScale(res));
  for (int i = 95; i >= 0; i--) {
    printf("%d", getBit(res, i));
  }
  printf("\nadd=%f:\n", res.bits[0] * pow(10, -1 * getScale(res)));
  // for (int i = 95; i >= 0; i--) {
  //   printf("%d", getBit(res, i));
  // }

  //  printf("\nsign val1 = %d\n", getSign(value_1));
  //     printf("\nsclae val1=%d\n", getScale(value_1));
  //     printf("val1=%f, bits:\n",
  //            value_1.bits[0] * pow(10, -1 * getScale(value_1)));
  //     for (int i = 95; i >= 0; i--) {
  //       printf("%d", getBit(value_1, i));
  //     }

  //     printf("\nsign val2 = %d\n", getSign(value_2));
  //     printf("\nsclae val2=%d\n", getScale(value_2));
  //     printf("val1=%f, bits:\n",
  //            value_2.bits[0] * pow(10, -1 * getScale(value_2)));
  //     for (int i = 95; i >= 0; i--) {
  //       printf("%d", getBit(value_2, i));
  //     }
  return 0;
}
