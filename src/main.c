#include <limits.h>
#include <math.h>
#include <stdio.h>
#include <string.h>

typedef struct {
  unsigned int bits[4];
} s21_decimal;

int getBit(s21_decimal d, int i) { return !!(d.bits[i / 32] & (1 << i % 32)); }

void setBits(s21_decimal *d, int i, int bit) {
  if (bit == 1)
    d->bits[i / 32] = d->bits[i / 32] | (1 << i % 32);
  else if (bit == 0)
    d->bits[i / 32] = d->bits[i / 32] & ~(1 << i % 32);
}

int getSign(s21_decimal d) {
  int sign = 1;
  if ((d.bits[3] & (1 << 31)) == 0) sign = 0;
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
    if (bit) rez += pow(2, (i - 112));
  }
  return rez;
}

int setScale(s21_decimal *d, int scale) {
  int bit = 0;
  if (scale <= 28 && scale >= 0) {
    for (int i = 4; i >= 0; i--) {
      bit = !!(scale & (1 << i));
      setBits(d, 112 + i, bit);
    }
    bit = 1;
  }
  return bit;  // if scale < 0 || > 28 return 0
}

// binary_sum - алгоритм суммирования положительных целых чисел в двоичной
// системе
int binary_sum(s21_decimal value_1, s21_decimal value_2, s21_decimal *res) {
  memset(res, 0, sizeof(*res));
  int tmp = 0;
  for (int i = 0; i < 96; i++) {
    if (getBit(value_1, i) && getBit(value_2, i) && tmp)
      setBits(res, i, 1);
    else if (getBit(value_1, i) && getBit(value_2, i) && !tmp)
      tmp = 1;
    else if ((getBit(value_1, i) + getBit(value_2, i)) == 1 && !tmp)
      setBits(res, i, 1);
    else if (!getBit(value_1, i) && !getBit(value_2, i) && tmp) {
      setBits(res, i, 1);
      tmp = 0;
    }
  }
  return tmp;  // возвращает tmp для отслеживания переполнения
}

// dop_Code - перевод в дополнительный код (для вычитания)
void dop_Code(s21_decimal value, s21_decimal *dop_code, int first_index) {
  for (int i = first_index; i >= 0; i--) {
    setBits(dop_code, i, getBit(value, i) ? 0 : 1);
  }
  dop_code->bits[0] += 1;
}

int first_bit(s21_decimal dec) {  // определяет индекс первого ненулевого бита
  int first = 0;
  for (int i = 95; i >= 0 && !first; i--) {
    if (getBit(dec, i)) first = i;
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
  if (getBit(*res, first_Bit + 1)) setBits(res, first_Bit + 1, 0);
}

void copy_decimal(s21_decimal src, s21_decimal *dst) {
  dst->bits[0] = src.bits[0];
  dst->bits[1] = src.bits[1];
  dst->bits[2] = src.bits[3];
  dst->bits[3] = src.bits[3];
}

int scale_up(s21_decimal *value_smallScale, int bigger_scale,
             int smaller_scale) {
  int is_inf = 0;
  s21_decimal tmp = {0};
  s21_decimal res = {0};
  copy_decimal(*value_smallScale, &tmp);
  while (smaller_scale < bigger_scale && !is_inf) {
    for (int i = 1; i < 10; i++) {
      is_inf = binary_sum(tmp, *value_smallScale, &res);
      copy_decimal(res, &tmp);
    }
    copy_decimal(res, value_smallScale);
    copy_decimal(*value_smallScale, &tmp);
    smaller_scale += 1;
  }
  setScale(value_smallScale, smaller_scale);
  return is_inf;  // возвращает 1 в случае переполнения
}

// ф-я выравнивания степеней - пока только повышение степени
void to_same_scale(s21_decimal *value_1, s21_decimal *value_2) {
  int scale1 = getScale(*value_1);
  int scale2 = getScale(*value_2);
  // int is_inf = 0;
  if (scale2 > scale1)
    scale_up(value_1, scale2, scale1);
  else if (scale1 > scale2)
    scale_up(value_2, scale1, scale2);
}

// s21_add пока без учета скейла
int s21_add(s21_decimal value_1, s21_decimal value_2, s21_decimal *result) {
  int res_code = 0;
  int sign_val1 = getSign(value_1);
  int sign_val2 = getSign(value_2);
  if (sign_val1 == sign_val2) {  // оба знака одинаковы
    res_code =
        binary_sum(value_1, value_2, result);  // если переполнение - код 1
    if (sign_val1) {
      setSign(result, sign_val1);
      if (res_code) res_code = 2;  // если переполнение - код 2
    }
  } else {  // знаки разные - вычитаем из большего
    // if (value_1 >= value_2) {  // - НУЖНА ФУНКЦИЯ СРАВНЕНИЯ ДЕЦИМАЛ
    //   binary_sub(value_1, value_2, result);
    //   if (sign_val1)
    //     setSign(result, sign_val1);
    // } else {
    //   binary_sub(value_2, value_1, result);
    //   if (sign_val2)
    //     setSign(result, sign_val2);
    // }
  }
  return res_code;
}

// s21_sub пока без учета скейла
int s21_sub(s21_decimal value_1, s21_decimal value_2, s21_decimal *result) {
  int res_code = 0;
  int sign_val1 = getSign(value_1);
  int sign_val2 = getSign(value_2);
  if (sign_val1 == sign_val2) {  // оба знака одинаковы - вычитаем из большего
    // if (value_1 >= value_2)  // - НУЖНА ФУНКЦИЯ СРАВНЕНИЯ ДЕЦИМАЛ
    //   binary_sub(value_1, value_2, result);
    //  else
    //   binary_sub(value_2, value_1, result);
    // if (!sign_val1) //если числа положител-е, то знак у рез-та "-"
    //   setSign(result, 1);
  } else {  // знаки разные -складываем
    binary_sum(value_1, value_2, result);
    if (sign_val1) setSign(result, sign_val1);
  }
  return res_code;  // пока без обработки ошибок
}

// s21_is_equal - добавить выравнивание скейлов - тк 100*10^-2 == 10*10^-1
int s21_is_equal(s21_decimal value_1, s21_decimal value_2) {
  int res = 0;
  if (value_1.bits[3] == value_2.bits[3]) {
    if(value_1.bits[0] == value_2.bits[0] &&
      value_1.bits[1] == value_2.bits[1] &&
      value_1.bits[2] == value_2.bits[2] && )
    res = 1;
    } else 
      to_same_scale(&value_1, &value_2); // сравнение после приведения скейлов
  return res;
}

// s21_is_less еще не готово, в процессе
int s21_is_less(s21_decimal value_1, s21_decimal value_2) {
  int res = 0;
  if (getSign(value_1) && !getSign(value_2)) res = 1;
  //  нормализация
  else if (getScale(value_1) > getScale(value_2))
    res = 1;
  return res;
}

int main() {
  s21_decimal dec;
  dec.bits[0] = 1000;
  dec.bits[1] = 0;
  dec.bits[2] = 0;
  dec.bits[3] = 0b00000000000000000000000000000000;
  printf("\nsclae dec=%d\n", getScale(dec));
  printf("val1=%f, bits:\n", dec.bits[0] * pow(10, -1 * getScale(dec)));
  for (int i = 95; i >= 0; i--) {
    printf("%d", getBit(dec, i));
  }

  // setBits(&dec, 0, 0);
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
  val2.bits[0] = 0;
  val2.bits[1] = 0;
  val2.bits[2] = 2147483648;
  val2.bits[3] = 0b00000000000000010000000000000000;
  printf("\nsclae val2=%d\n", getScale(val2));
  printf("val2=%f, bits:\n", val2.bits[0] * pow(10, -1 * getScale(val2)));
  for (int i = 95; i >= 0; i--) {
    printf("%d", getBit(val2, i));
  }
  to_same_scale(&dec, &val2);
  printf("\nNew sclae val1=%d\n", getScale(dec));
  printf("\n dec.bits[0]=%d\n", dec.bits[0]);
  printf("New val1=%f, bits:\n", dec.bits[0] * pow(10, -1 * getScale(dec)));
  // printf("\nnew val2=%d, bits:\n", val2.bits[0]);
  // s21_decimal res = {0};
  // binary_sum(dec, val2, &res);
  // printf("\nnum=%d, bits:\n", res.bits[0]);
  // s21_sub(dec, val2, &res);
  // printf("\nsub=%d:\n", res.bits[0]);
  // for (int i = 95; i >= 0; i--) {
  //   printf("%d", getBit(res, i));
  // }
}
