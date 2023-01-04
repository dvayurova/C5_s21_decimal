#include <limits.h>
#include <stdio.h>

typedef struct {
  unsigned int bits[4];
} s21_decimal;

int getBits(s21_decimal d, int i) { return !!(d.bits[i / 32] & (1 << i % 32)); }

void setBits(s21_decimal *d, int i, int bit) {
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

// binary_sum - алгоритм суммирования положительных целых чисел в двоичной
// системе
int binary_sum(s21_decimal value_1, s21_decimal value_2, s21_decimal *res) {
  int tmp = 0;
  for (int i = 0; i < 96; i++) {
    if (getBits(value_1, i) && getBits(value_2, i) && tmp)
      setBits(res, i, 1);
    else if (getBits(value_1, i) && getBits(value_2, i) && !tmp)
      tmp = 1;
    else if ((getBits(value_1, i) + getBits(value_2, i)) == 1 && !tmp)
      setBits(res, i, 1);
    else if (!getBits(value_1, i) && !getBits(value_2, i) && tmp) {
      setBits(res, i, 1);
      tmp = 0;
    }
  }
  return tmp; // возвращает tmp для отслеживания переполнения
}

// dop_Code - перевод в дополнительный код (для вычитания)
void dop_Code(s21_decimal value, s21_decimal *dop_code, int first_index) {
  for (int i = first_index; i >= 0; i--) {
    setBits(dop_code, i, getBits(value, i) ? 0 : 1);
  }
  dop_code->bits[0] += 1;
}

int first_bit(s21_decimal dec) { // определяет индекс первого ненулевого бита
  int first = 0;
  for (int i = 95; i >= 0 && !first; i--) {
    if (getBits(dec, i))
      first = i;
  }
  return first;
}

// binary_sub -алгоритм вычитания положительных целых чисел в двоичной
// системе
void binary_sub(s21_decimal value_1, s21_decimal value_2, s21_decimal *res) {
  s21_decimal val2_dop_code = {0};
  int first_Bit = first_bit(value_1);
  dop_Code(value_2, &val2_dop_code, first_Bit);
  binary_sum(value_1, val2_dop_code, res);
  if (getBits(*res, first_Bit + 1))
    setBits(res, first_Bit + 1, 0);
}

int s21_add(s21_decimal value_1, s21_decimal value_2, s21_decimal *result) {
  int res_code = 0;
  int sign_val1 = getSign(value_1);
  int sign_val2 = getSign(value_2);
  if (sign_val1 == sign_val2) { // оба знака одинаковы
    res_code = binary_sum(value_1, value_2, result); //если переполнение - код 1
    if (sign_val1) {
      setSign(result, sign_val1);
      if (res_code)
        res_code = 2; //если переполнение - код 2
    }
  } else { // знаки разные - вычитаем из большего
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

int s21_sub(s21_decimal value_1, s21_decimal value_2, s21_decimal *result) {
  int res_code = 0;
  int sign_val1 = getSign(value_1);
  int sign_val2 = getSign(value_2);
  if (sign_val1 == sign_val2) { //оба знака одинаковы - вычитаем из большего
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
  dec.bits[0] = 7;
  dec.bits[1] = 0;
  dec.bits[2] = 0;
  dec.bits[3] = 0b00000000000000010000000000000000;
  // printf("\nSign=%d\n", getSign(dec));
  printf("dec=%d, bits:\n", dec.bits[0]);
  for (int i = 95; i >= 0; i--) {
    printf("%d", getBits(dec, i));
  }
  printf("\nfirst bit dec=%d", first_bit(dec));
  // setBits(&dec, 0, 0);
  // printf("\nnum after=%d, bits:\n", dec.bits[0]);
  // for (int i = 95; i >= 0; i--) {
  //   printf("%d", getBits(dec, i));
  // }
  // setSign(&dec, 1);
  // printf("\nbits[3]=");
  // for (int i = 127; i >= 96; i--) {
  //   printf("%d", getBits(dec, i));
  // }
  // printf("\nSign=%d\n", getSign(dec));
  s21_decimal val2;
  val2.bits[0] = 1;
  val2.bits[1] = 0;
  val2.bits[2] = 0;
  val2.bits[3] = 0b10000000000000010000000000000000;
  printf("\nval2=%d, bits:\n", val2.bits[0]);
  for (int i = 95; i >= 0; i--) {
    printf("%d", getBits(val2, i));
  }
  s21_decimal res = {0};
  // binary_sum(dec, val2, &res);
  // printf("\nnum=%d, bits:\n", res.bits[0]);
  s21_sub(dec, val2, &res);
  printf("\nsub=%d:\n", res.bits[0]);
  for (int i = 95; i >= 0; i--) {
    printf("%d", getBits(res, i));
  }
  printf("\nSign=%d\n", getSign(res));
}
