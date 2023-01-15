#include "s21_decimal.h"
// int s21_mul(s21_decimal value_1, s21_decimal value_2, s21_decimal *res) {
//   int res_code = 0, resScale = 0;
//   res->bits[LOW] = 0, res->bits[MIDLE] = 0, res->bits[HIGHE] = 0,
//   res->bits[SCALE] = 0;
//   resScale = getScale(value_1) + getScale(value_2);
//   setScale(res, resScale);

//   if (getSign(value_1)) {
//     if (getSign(value_2))
//       setSign(res, 0);
//     if (!getSign(value_2))
//       setSign(res, 1);
//   } else {
//     if (getSign(value_2))
//       setSign(res, 1);
//   }

//   for (int i = 0; i < 96; i++) {
//     if (getBit(value_2, i)) {
//       res_code = binary_sum(*res, value_1, res);
//     }
//     shift(&value_1, 1);
//   }
//   if (getScale(*res) > 28)
//     res_code = 2;

//   return res_code;
// }
