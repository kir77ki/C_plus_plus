#include <iostream>
#include <vector>
#include <stdexcept>
#include <cstring>
#include <string>
#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <cstdio>
#include <fstream>
#include <iomanip>
#include <random>
#include <optional>
#include <cassert>
#include <experimental/random>

//Реализуйте cледующий класс для хранения множества целых чисел:
//class FixedSet {
//public:
//    FixedSet();
//    void Initialize(const vector<int>& numbers);
//    bool Contains(int number) const;
//};
//При вызове Initialize FixedSet получает набор целых чисел, который впоследствии и будет хранить.
//Набор чисел не будет изменяться с течением времени (до следующего вызова Initialize).
//Операция Contains возвращает true, если число number содержится в наборе.
//Мат. ожидание времени работы Initialize должно составлять O(n), где n — количество чисел в numbers.
//Затраты памяти должны быть порядка O(n) в худшем случае. Операция Contains должна выполняться за O(1)
//в худшем случае.
//С помощью этого класса решите модельную задачу: на вход будет дано множество различных чисел,
//а затем множество запросов — целых чисел. Необходимо для каждого запроса определить,
//лежит ли число из запроса в множестве.
//В задаче строится множество из n различных чисел, после чего выполняется q запросов.

// Класс для линейной хэш-функции
class LinHashFunc {
  private:
      int64_t a_coef_, b_coef_;
      static constexpr int kBigPrimeNum = 2147483647;

  public:
      LinHashFunc(int64_t a_coef, int64_t b_coef) {
          a_coef_ = a_coef;
          b_coef_ = b_coef;
      }

      int64_t operator () (int arg) const {
          int64_t ind = (a_coef_ * arg + b_coef_) % kBigPrimeNum;
          if (ind >= 0) {
              return ind;
          }
          else
          {
              return kBigPrimeNum + ind;
          }
      }
};

// Класс случайной хэш-функции
class RandHashFunc : LinHashFunc {
  private:
    static constexpr int kBigPrimeNum = 2147483647;
  public:
      RandHashFunc() : LinHashFunc(
                  std::experimental::randint(1, kBigPrimeNum - 1),
                  std::experimental::randint(0, kBigPrimeNum - 1)
      ) {}

      int operator () (int arg) const {
          return LinHashFunc::operator()(arg);
      }
};

// Класс случайной хэш-функции с дополнительным взятием остатка по модулю длины
// хэш-таблицы
class ModuledRandHashFunc {
  private:
      int64_t len_of_table_;
      RandHashFunc hash_func_;

  public:
      ModuledRandHashFunc() = default;
      explicit ModuledRandHashFunc(int64_t len_of_table) {
          hash_func_ = RandHashFunc();
          len_of_table_ = len_of_table;
      };

      int64_t operator () (int arg) const {
          int64_t ind = hash_func_(arg) % this->len_of_table_;
          if (ind >= 0) {
              return ind;
          }
          else
          {
              return len_of_table_ + ind;
          }
      }
};

// Класс внутренней хэш-таблицы
class InternalHashFunc{
  private:
      std::vector<std::optional<int64_t>> table_;
      ModuledRandHashFunc hash_func_;

  public:
      InternalHashFunc() = default;
      void Initialize(const std::vector<int>& numbers) {
          table_.clear(); // На случай повторной инициализации
          assert(-pow(2, 63) <= numbers.size() &&
                 numbers.size() <= pow(2, 64) - 1);
          int64_t len = static_cast<int64_t> (numbers.size());
          if (len == 0) {
              table_ = {};
          }
          bool have_collision = false;
          do {
              table_ = std::vector<std::optional<int64_t>>(len * len,
                                                                 std::nullopt);
              have_collision = false;
              hash_func_ = ModuledRandHashFunc(len * len);

              for (int64_t i = 0; i < len; ++i) {
                  int64_t ind = hash_func_(numbers[i]);
                  if (table_[ind] != std::nullopt &&
                          table_[ind] != numbers[i]) {
                      have_collision = true;
                      break;
                  }
                  table_[ind] = numbers[i];
              }
          } while (have_collision);
      }

      bool Contains(int number) const {
          if (table_.empty()) {
              return false;
          }
          return table_[hash_func_(number)] == number;
      }
};

// Основной класс
class FixedSet{
  private:
    std::vector<InternalHashFunc> secondary_hash_functions_;
    ModuledRandHashFunc primary_hash_func_;
    bool is_empty_set_;

  public:
    FixedSet() = default;
    void Initialize(const std::vector<int>& numbers) {
        assert(-pow(2, 63) <= numbers.size() &&
               numbers.size() <= pow(2, 64) - 1);
        int64_t len_of_array = static_cast<int64_t>(numbers.size());
        secondary_hash_functions_.clear(); // На случай повторной инициализации

        if (len_of_array == 0) {
            is_empty_set_ = true;
            return;
        }
        is_empty_set_ = false;

        std::vector<std::vector<int>> buckets(len_of_array);
        int64_t sum_of_squares = 0;
        // Сторим первую хэш-таблицу
        do {
            primary_hash_func_ = ModuledRandHashFunc(len_of_array);
            for (int64_t i = 0; i < len_of_array; ++i) {
                buckets[i].clear();
            }

            // Обновляю сумму квадратов длин бакетов по ходу пьесы, так эффективнее
            sum_of_squares = 0;
            for (int64_t i = 0; i < len_of_array; ++i) {
                int64_t ind = primary_hash_func_(numbers[i]);
                buckets[ind].push_back(numbers[i]);
                assert(-pow(2, 31) <= buckets[ind].size() &&
                       buckets[ind].size() <= pow(2, 31) - 1);
                sum_of_squares = sum_of_squares + 2 *
                        static_cast<int>(buckets[ind].size()) - 1;
            }
        } while (sum_of_squares == 0 || sum_of_squares >= 4 * len_of_array);

        // Сторим второй уровень хэш-таблиц
        secondary_hash_functions_ = std::vector<InternalHashFunc>(len_of_array,
                                                                        InternalHashFunc());
        for (int64_t i = 0; i < len_of_array; ++i) {
            secondary_hash_functions_[i].Initialize(buckets[i]);
        }
    }

    bool Contains(int number) const {
        if (is_empty_set_) {
            return false;
        }
        return secondary_hash_functions_[primary_hash_func_(number)].Contains(number);
    }
};
