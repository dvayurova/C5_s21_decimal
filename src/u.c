int fromLongToDecimal(s21_decimal_long value, s21_decimal *result) {
  int status = 0; // не бесконечность
  s21_decimal_long ostatok = {0, 0, 0, 0, 0, 0, 0, 0};
  if (value.bits[3] == 0 && value.bits[4] == 0 && value.bits[5] == 0 &&
      value.bits[7] <=
          28) { // смотрим только эти элементы, и если они все равны 0 и степень
                // входит в разрешенный 28, то число входит в предел и можно
                // переводить без "обрезания"
    converterLongToDecimal(value, result); // переводим с лонга в децимал
  } else {
    while (1) { // начинаем дeлить лонг до тех пор, пока не влезет в децимал или
                // степень станет 0
      if (value.bits[7] ==
          0) { // если степень достиг 0, а число все еще больше максимального,
               // то он уже больше максимального, следовательно инфинити
        status = 1;
        break;
      }
      ostatok = divByTenAndRoundLong(
          &value); // сохраняем остаток каждый раз, тк с последним остатком
                   // будем делать округление
      subOneScaleLong(
          &value); // вычитает только степень value.bits[7] = value.bits[7] - 1;
      // выясняем остаток и округляем
      if (ostatok.bits[0] == 5) {
        if (getBitLong(value, 0) == 1)
          addOneLong(&value);
      } // если число нечетное, то +1 тк идем в ближайщий четный(банковское
        // округление)
      else if (ostatok.bits[0] > 5)
        addOneLong(&value); // ну и естественно, +1 если больше 5ти
      if ((value.bits[3] == 0 && value.bits[4] == 0 && value.bits[5] == 0 &&
           value.bits[7] <= 28))
        break; // проверяем, "влез" ли в децимал
    }
    if (value.bits[5] != 0)
      status = 1; // проверяем, вышел ли за пределы после округления, мало ли
    if (status == 0)
      converterLongToDecimal(value, result); // переводим с лонга в децимал
    else if (status == 1)
      zeroing(result); // зануляем результат тк вышел за пределы
  }
  // if (result->bits[0] == 0 && result->bits[1] == 0 && result->bits[2] == 0)
  // zeroing(result); // зануляем степень(СПОРНЫЙ МОМЕНТ!!!) и знак, тк ответ
  // равен 0.
  return status;
}

s21_decimal_long divByTenAndRoundLong(s21_decimal_long *value) {
  int ten_binary_symbols = 4; // количество символов 1010
  int first_bit = 191; // "первый" бит, где может быть 1
  int bit_now = 0;     // какой бит смотрим сейчас
  int result_pos = 0;
  int result_pos_recourse = 0;
  int number_of_bits = 0;
  int segment_minus_ten_greater = 0;
  s21_decimal_long segment = {
      0, 0, 0, 0,
      0, 0, 0, 0}; // из него будем вычитать 1010 для деления столбиком на 10
  s21_decimal_long segment_minus_ten = {
      0, 0, 0, 0,
      0, 0, 0, 0}; // тут будем хранить ответ после вычетания столбиком
  s21_decimal_long result_recourse = {
      0, 0, 0, 0,
      0, 0, 0, 0}; // ответ (пока будем хранить в рекурсии, пожже перевернем)
  s21_decimal_long result = {
      0, 0, 0, 0, 0,
      0, 0, 0}; // ответ, после рекурсии, счет степени не ведем, тк ведется выше
                // по DOM, а вот знак пожже присвоим
  s21_decimal_long ostatok = {0, 0, 0, 0, 0, 0, 0, 0}; // остаток после деления
  result.bits[7] = value->bits[7]; // переписываем степень, потом вернем, хотя
                                   // зачем? мы ж его не трогаем
  first_bit = firstBitLong(*value); // ищем первый бит с показателем 1
  number_of_bits = getBinarySymbolsForTenLong(
      *value, &segment, ten_binary_symbols,
      first_bit); // смотрим, сколько элементов нужно в первой итерации (4 или
                  // 5). это для деления столбиком
  bit_now = first_bit -
            number_of_bits; // смотрим на какой бит будем смотреть следующим
  setBitLong(&result_recourse, result_pos_recourse++,
             1); // записываем в ответ первый символ. это 1 тк делимое всегда
                 // будет больше 10ти
  if (value->bits[6] == 0 && value->bits[5] == 0 && value->bits[4] == 0 &&
      value->bits[3] == 0 && value->bits[2] == 0 && value->bits[1] == 0 &&
      value->bits[0] <= 9) {
    ostatok.bits[0] = value->bits[0];
    value->bits[0] = 0;
  } else {
    for (;
         bit_now >= 0;) { // смотрим биты до тех пор пока не "уткнемся в конец"
      subTenForDivTenLong(&segment, &segment_minus_ten,
                          number_of_bits); // вычитаем из segment 1010 и
                                           // получаем ответ segment_minus_ten
      bitLeftLong(&segment_minus_ten); //
      setBitLong(
          &segment_minus_ten, 0,
          getBitLong(*value,
                     bit_now--)); // "опускаем" бит "сверху" к ответу,напоминаю
                                  // - мы все еще делим столбиком
      while (segment_minus_ten_greater ==
             0) { // тут мы будем "опускать" бит сверху пока segment_minus_ten
                  // не будет больше или равно 1010
        segment_minus_ten_greater = tenIsGreatestLong(segment_minus_ten);
        if (segment_minus_ten_greater == 0) {
          setBitLong(&result_recourse, result_pos_recourse++, 0);
          bitLeftLong(&segment_minus_ten);
          setBitLong(&segment_minus_ten, 0, getBitLong(*value, bit_now--));
        } else if (segment_minus_ten_greater ==
                   1) { // и когда segment_minus_ten больше 1010, присваиваем
                        // его в segment, а segment_minus_ten обнуляем
          setBitLong(&result_recourse, result_pos_recourse++, 1);
          segment = segment_minus_ten;
          for (int x = 0; x < 8; x++)
            segment_minus_ten.bits[x] = 0;
        }
        if (bit_now < -1)
          break; // прерыватель, если мы достигли "правого края"
      }
      segment_minus_ten_greater =
          0; // обнулеям, после проверки = перед следующей проверкой
    }
    result_pos = result_pos_recourse - 1;
    for (int i = 0; i < result_pos_recourse; i++) { // провецесс рекурсии ответа
      setBitLong(&result, result_pos--, getBitLong(result_recourse, i));
    }
    ostatok = getOstatokOtDeleniyaLong(
        *value, result); // выясняем остаток. пример value = 189, а result 18
    result.bits[6] =
        value->bits[6]; // присваиваем изначальный знак, а то пропадет ведь
    *value = result;
  }
  return ostatok;
}
void converterLongToDecimal(s21_decimal_long value, s21_decimal *result) {
  int i = 0;
  int j = 224; // 224 - первый бит содержащий степень
  for (i = 0; i <= 95; i++)
    setBit(result, i, getBitLong(value, i)); // скопировали биты лонга с 0 по 95
                                             // в децимал (мл,ср,старший)
  setBit(result, 127,
         getBitLong(value, 192)); // присвоили знак из лонг в децимал
  for (i = 112; i <= 116; i++)
    setBit(result, i,
           getBitLong(value, j++)); // присвоили степень из лонг в децимал
}