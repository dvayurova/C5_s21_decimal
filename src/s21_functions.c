#include "s21_decimal.h"

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
int long_getBit(long_decimal x, int index) {
  unsigned int mask = 1u << index % 32;
  return !!(x.long_bits[index / 32] & mask);
}

int long_setBit(long_decimal *x, int index, int bit) {
  unsigned int mask = 1u << index % 32;
  if (bit == 1) {
    x->long_bits[index / 32] |= mask;
  } else {
    x->long_bits[index / 32] &= ~mask;
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
int getSign_long(long_decimal d) {
  int sign = 1;
  if ((d.long_bits[6] & (1 << 31)) == 0)
    sign = 0;
  return sign;
}
int setSign_long(long_decimal *x, int sign) {
  if (sign == 0)
    long_setBit(x, 223, 0);
  if (sign == 1)
    long_setBit(x, 223, 1);
  return sign;
}
int getScale_long(long_decimal x) {
  int bit = 0;
  int rez = 0;
  for (int i = 208; i < 213; i++) {
    bit = long_getBit(x, i);
    if (bit)
      rez += pow(2, (i - 208));
  }
  return rez;
}

int setScale_long(long_decimal *d, int scale) {
  int bit = 0;
  if (scale <= 28 && scale >= 0) {
    for (int i = 4; i >= 0; i--) {
      bit = !!(scale & (1 << i));
      long_setBit(d, 208 + i, bit);
    }
    bit = 1;
  }
  return bit; // if scale < 0 || > 28 return 0
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

int first_bit(s21_decimal dec) { // определяет индекс первого ненулевого бита
  int first = 0;
  for (int i = 95; i >= 0 && !first; i--) {
    if (getBit(dec, i))
      first = i;
  }
  return first;
}

int first_bit_long(
    long_decimal dec) { // определяет индекс первого ненулевого бита
  int first = 0;
  for (int i = 192; i >= 0 && !first; i--) {
    if (long_getBit(dec, i))
      first = i;
  }
  return first;
}

// binary_sum - алгоритм суммирования положительных целых чисел в двоичной
// системе
int binary_sum(long_decimal value_1, long_decimal value_2, long_decimal *res) {
  memset(res, 0, sizeof(*res));
  int tmp = 0;
  for (int i = 0; i < 192; i++) {
    if (long_getBit(value_1, i) && long_getBit(value_2, i) && tmp)
      long_setBit(res, i, 1);
    else if (long_getBit(value_1, i) && long_getBit(value_2, i) && !tmp)
      tmp = 1;
    else if ((long_getBit(value_1, i) + long_getBit(value_2, i)) == 1 && !tmp)
      long_setBit(res, i, 1);
    else if (!long_getBit(value_1, i) && !long_getBit(value_2, i) && tmp) {
      long_setBit(res, i, 1);
      tmp = 0;
    }
  }
  return tmp; // возвращает tmp для отслеживания переполнения
}

// dop_Code - перевод в дополнительный код (для вычитания)
void dop_Code(long_decimal value, long_decimal *dop_code, int first_index) {
  for (int i = first_index; i >= 0; i--) {
    long_setBit(dop_code, i, long_getBit(value, i) ? 0 : 1);
  }
  dop_code->long_bits[0] += 1;
}

// binary_sub - алгоритм вычитания положительных целых чисел в двоичной
// системе
void binary_sub(long_decimal value_1, long_decimal value_2, long_decimal *res) {
  long_decimal val2_dop_code = {0};
  memset(res, 0, sizeof(*res));
  int first_Bit = first_bit_long(value_1);
  dop_Code(value_2, &val2_dop_code, first_Bit);
  binary_sum(value_1, val2_dop_code, res);
  if (long_getBit(*res, first_Bit + 1))
    long_setBit(res, first_Bit + 1, 0);
}

void copy_decimal_long(long_decimal src, long_decimal *dst) {
  dst->long_bits[0] = src.long_bits[0];
  dst->long_bits[1] = src.long_bits[1];
  dst->long_bits[2] = src.long_bits[2];
  dst->long_bits[3] = src.long_bits[3];
  dst->long_bits[4] = src.long_bits[4];
  dst->long_bits[5] = src.long_bits[5];
  dst->long_bits[6] = src.long_bits[6];
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

void long_shift(long_decimal *x, int sign) {
  if (sign > 0) {
    x->long_bits[5] <<= 1;
    if (long_getBit(*x, 159))
      long_setBit(x, 160, 1);
    x->long_bits[4] <<= 1;
    if (long_getBit(*x, 127))
      long_setBit(x, 128, 1);
    x->long_bits[3] <<= 1;
    if (long_getBit(*x, 95))
      long_setBit(x, 96, 1);
    x->long_bits[HIGHE] <<= 1;
    if (long_getBit(*x, 63))
      long_setBit(x, 64, 1);
    x->long_bits[MIDLE] <<= 1;
    if (long_getBit(*x, 31))
      long_setBit(x, 32, 1);
    x->long_bits[LOW] <<= 1;
  }

  if (sign < 0) {
    x->long_bits[LOW] >>= 1;
    if (long_getBit(*x, 32))
      long_setBit(x, 31, 1);
    x->long_bits[MIDLE] >>= 1;
    if (long_getBit(*x, 64))
      long_setBit(x, 63, 1);
    x->long_bits[HIGHE] >>= 1;
    x->long_bits[3] >>= 1;
    if (long_getBit(*x, 96))
      long_setBit(x, 95, 1);
    x->long_bits[4] >>= 1;
    if (long_getBit(*x, 128))
      long_setBit(x, 127, 1);
    x->long_bits[5] >>= 1;
    if (long_getBit(*x, 160))
      long_setBit(x, 159, 1);
  }
}
void from_decimal_to_long(s21_decimal dec, long_decimal *dec_long) {
  memset(dec_long, 0, sizeof(*dec_long));
  for (int i = 0; i < 3; i++) {
    dec_long->long_bits[i] = dec.bits[i];
  }
  dec_long->long_bits[6] = dec.bits[3];
}

int s21_greater_or_equal10(long_decimal value_1_long) {
  int res = 0, stop = 0;
  int bit_val1 = 0;
  int bit_ten = 0;
  long_decimal value_ten = {0};
  value_ten.long_bits[0] = 10;
  bit_val1 = first_bit_long(value_1_long);
  bit_ten = first_bit_long(value_ten);
  if (bit_val1 > bit_ten) {
    res = 1;
  } else if (bit_ten == bit_val1) {
    for (int i = bit_ten; i >= 0 && !stop; i--) {
      if (long_getBit(value_ten, i) > long_getBit(value_1_long, i)) {
        stop = 1;
      } // если стоп, значит 10 больше
      else if (long_getBit(value_1_long, i) > long_getBit(value_ten, i)) {
        stop = 2;
      } //  значит value_1 больше
    }
    res = stop == 2 ? 1 : 0;
    if (stop == 0)
      res = 1;
  }
  return res;
}

// сравнение биг децимал без учета знака
int s21_greater_or_equal_long(long_decimal value_1_long,
                              long_decimal value_2_long) {
  int res = 0, stop = 0;
  int bit_val1 = 0;
  int bit_val2 = 0;
  bit_val1 = first_bit_long(value_1_long);
  bit_val2 = first_bit_long(value_2_long);
  if (bit_val1 > bit_val2) {
    res = 1;
  } else if (bit_val2 == bit_val1) {
    for (int i = bit_val2; i >= 0 && !stop; i--) {
      if (long_getBit(value_2_long, i) > long_getBit(value_1_long, i)) {
        stop = 1;
      } // если стоп, значит value_2 больше
      else if (long_getBit(value_1_long, i) > long_getBit(value_2_long, i)) {
        stop = 2;
      } //  значит value_1 больше
    }
    res = stop == 2 ? 1 : 0;
    if (stop == 0) //  значит равны
      res = 1;
  }
  return res;
}

void from_long_to_dec(long_decimal src, s21_decimal *dst) {
  for (int i = 0; i < 96; i++) {
    setBit(dst, i, long_getBit(src, i));
  }
}

// long_decimal divide(long_decimal value_1, long_decimal *result) {
//   long_decimal temp = {0};
//   long_decimal value_2 = {0};
//   value_2.long_bits[0] = 10;
//   result->long_bits[0] = 0;
//   result->long_bits[1] = 0;
//   result->long_bits[2] = 0;
//   int i = 191;
//   while (long_getBit(value_1, i) == 0)
//     i--;
//   int sub = 0;
//   for (; i >= 0; i--) {
//     long_shift(&temp, 1);
//     long_setBit(&temp, 0, long_getBit(value_1, i));
//     if (s21_greater_or_equal10(temp) == 1) {
//       sub += 1;
//       long_shift(result, 1);
//       long_setBit(result, 0, 1);
//       binary_sub(temp, value_2, &temp);
//     } else if (sub != 0)
//       long_shift(result, 1);
//   }
//   printf("\n result->long_bits[0]= %d\n", result->long_bits[0]);
//   return temp;
// }

// void from_long_to_decimal(s21_decimal *dec, long_decimal dec_long) {
//   memset(dec, 0, sizeof(*dec));
//   long_decimal tmp = {0};
//   int scale = 0;
//   scale = getScale_long(dec_long);
//   if (dec_long.long_bits[3] == 0 && dec_long.long_bits[4] == 0 &&
//       dec_long.long_bits[5] == 0) {
//     dec->bits[0] = dec_long.long_bits[0];
//     dec->bits[1] = dec_long.long_bits[1];
//     dec->bits[2] = dec_long.long_bits[2];
//     dec->bits[3] = dec_long.long_bits[6];
//   } else {

//     printf("dec_long.long_bits[3] = %d\n, dec_long.long_bits[4] = %d, "
//            "dec_long.long_bits[5] = %d\n",
//            dec_long.long_bits[3], dec_long.long_bits[4],
//            dec_long.long_bits[5]);
//     do {
//       printf("\n ----CHECK!!!!------ \n");
//       divide(dec_long, &tmp);
//       copy_decimal_long(tmp, &dec_long);
//       scale += 1;
//     } while (dec_long.long_bits[3] > 0 && dec_long.long_bits[4] > 0 &&
//              dec_long.long_bits[5] > 0);
//   }
// }
int scale_up(long_decimal *value_smallScale, int bigger_scale,
             int smaller_scale) {
  int is_inf = 0, sign = 0;
  long_decimal tmp = {0};
  long_decimal res = {0};
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
int to_same_scale(long_decimal *value_1, long_decimal *value_2) {
  int scale1 = getScale_long(*value_1);
  int scale2 = getScale_long(*value_2);
  int new_scale = 0;
  if (scale2 > scale1)
    new_scale = scale_up(value_1, scale2, scale1);
  else if (scale1 > scale2)
    new_scale = scale_up(value_2, scale1, scale2);
  return new_scale;
}

int is_zero(long_decimal dec) {
  int zero = 0;
  if (!dec.long_bits[0] && !dec.long_bits[1] && !dec.long_bits[2] &&
      !dec.long_bits[3] && !dec.long_bits[4] && !dec.long_bits[5])
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
void sub_from_big(long_decimal value_1_long, long_decimal value_2_long,
                  long_decimal *result_long, int sign_val1, int sign_val2) {
  if (s21_greater_or_equal_long(value_1_long, value_2_long)) {
    binary_sub(value_1_long, value_2_long, result_long);
    if (sign_val1) {
      setSign_long(result_long, sign_val1);
    }
  } else {
    binary_sub(value_2_long, value_1_long, result_long);
    printf("\n ==CHECK== \n");
    if (sign_val2)
      setSign_long(result_long, sign_val2);
  }
}

int div_long(long_decimal val1, s21_decimal *result) {
  int ret = 0;
  int scale = 0;
  int sub = 0;
  int k = 0;
  int sign = 0;
  long_decimal res = {0};
  long_decimal temp = {0};
  long_decimal ten = {0};
  ten.long_bits[0] = 10;
  scale = getScale_long(val1);
  sign = getSign_long(val1);
  int i = 191;
  while (long_getBit(val1, i) == 0)
    i--;
  if (val1.long_bits[3] != 0 || val1.long_bits[4] != 0 ||
      val1.long_bits[5] != 0) {
    for (; k < 96; i--) {
      long_shift(&temp, 1);
      long_setBit(&temp, 0, long_getBit(val1, i));
      if (s21_greater_or_equal10(temp) == 1) {
        sub += 1;
        long_shift(&res, 1);
        long_setBit(&res, 0, 1);
        binary_sub(temp, ten, &temp);
      } else if (sub != 0)
        long_shift(&res, 1);
      if (sub != 0)
        k++;
      scale--;
    }
    if (scale >= 0 && scale < 29) {
      from_long_to_dec(res, result);
      setScale(result, scale);
    } else
      ret = 1;
  } else {
    from_long_to_dec(val1, result);
    setScale(result, scale);
  }
  setSign(result, sign); // добавила знак
  return ret;
}
