#include <iostream>
#include <vector>
#include <exception>
#include <algorithm>
#include <random>
#include <cassert>
#include <cstdio>
#include <functional>
#include <experimental/random>

//Вы хотите набрать футбольную команду.
//У каждого игрока своя эффективность, она описывается одним целым числом.
//Чем больше число, тем больше эффективность футболиста.
//Обязательным условием для любой команды является сплоченность.
//Если один из игроков играет сильно лучше всех остальных, его будут недолюбливать, и команда распадется.
//Поэтому эффективность любого игрока команды не должна превышать сумму эффективностей любых двух других игроков.
//Ваша задача — набрать команду, которая будет удовлетворять условию сплоченности,
//и при этом иметь наибольшую суммарную эффективность.

//Формат ввода
//В первой строке входа задано число n (1 ≤ n ≤ 100000). Во второй строке —
//эффективности каждого из n игроков — положительные целые числа, не превосходящие 231 - 1.

//Формат вывода
//Выведите две строки. В первую запишите наибольшую возможную сумму эффективностей игроков в команде.
//Во вторую строку выведите номера всех игроков, которых нужно взять в команду, в порядке возрастания.

struct Footballist {
    int effectiveness;
    int index;
};

using FootIter = std::vector<Footballist>::iterator;

bool CompareFootballistsByEffectiveness(const Footballist& first_footballist,
                                       const Footballist& second_footballist) {
    return first_footballist.effectiveness < second_footballist.effectiveness;
}

bool CompareFootballistsByIndex(const Footballist& first_footballist,
                               const Footballist& second_footballist) {
    return first_footballist.index < second_footballist.index;
}

int Random(int low, int high)
{
    return std::experimental::randint(low, high);
}

FootIter SelectPivot(FootIter begin, FootIter end) {
    int pivot_index = Random(0, end - begin - 1);
    return begin + pivot_index;
}

FootIter HoarPartition(FootIter begin, FootIter end,
                       Footballist pivot, bool (*compare_footballists)
                       (const Footballist&, const Footballist&)) {
    FootIter less = begin;
    FootIter more = end;
    while (less < more) {
        if (more != end) {
            ++less;
        }
        --more;
        while ((*compare_footballists)(pivot, *more)) {
            --more;
        }
        while ((*compare_footballists)(*less, pivot)) {
            ++less;
        }
        if (less < more) {
            std::swap(*less, *more);
        }
    }
    return more;
}

void QuickSort(FootIter begin, FootIter end,
               bool (*compare_footballists)
               (const Footballist&, const Footballist&)) {
    while (end - begin > 1) {
        Footballist pivot = *SelectPivot(begin, end);
        FootIter more = HoarPartition(begin, end, pivot, (*compare_footballists));
        QuickSort(begin, more + 1, (*compare_footballists));
        begin = more + 1;
    }
}

int64_t BuildMostEffectiveSolidaryTeam(const std::vector <Footballist>& footballists,
                                         std::vector <Footballist>* team) {
    std::vector<Footballist> sorted_footballists = footballists;
    QuickSort(sorted_footballists.begin(), sorted_footballists.end(),
              CompareFootballistsByEffectiveness);
    if (sorted_footballists.size() == 1) {
         team->resize(1);
         team->at(0).effectiveness = sorted_footballists[0].effectiveness;
         team->at(0).index = 0;
         return sorted_footballists[0].effectiveness;
    }

    int64_t answer = 0;
    int64_t temp_effectiveness = 0;
    FootIter low_eff_player = sorted_footballists.begin();
    FootIter first_from_team = sorted_footballists.begin();
    FootIter last_from_team = sorted_footballists.begin();
    FootIter temp_footballist = sorted_footballists.begin();

    while (temp_footballist < sorted_footballists.end()) {
        int64_t the_most_effective = static_cast<int64_t>(low_eff_player->effectiveness) +
                static_cast<int64_t>((low_eff_player + 1)->effectiveness);
        if (temp_footballist->effectiveness <= the_most_effective) {
             temp_effectiveness += temp_footballist->effectiveness;
             if (sorted_footballists.end() - temp_footballist == 1) {
                 if (temp_effectiveness > answer) {
                      answer = temp_effectiveness;
                      first_from_team = low_eff_player;
                      last_from_team = temp_footballist;
                 }

                 team->clear();
                 team->reserve(100000);
                 for (FootIter i = first_from_team; last_from_team - i >= 0; ++i) {
                     team->push_back(*i);
                 }
                 QuickSort(team->begin(), team->end(), CompareFootballistsByIndex);
                 return answer;
             }
             ++temp_footballist;
        }
        else
        {
             if (temp_effectiveness > answer) {
                  answer = temp_effectiveness;
                  first_from_team = low_eff_player;
                  last_from_team = temp_footballist - 1;
             }
             temp_effectiveness -= low_eff_player->effectiveness;
             ++low_eff_player;
        }
    }
    return answer;
}

void Input (std::vector <Footballist>* footballists) {
    int64_t number_of_footballists;
    std::cin >> number_of_footballists;
    footballists->resize(number_of_footballists);
    int ind = 0;
    for (auto &player : *footballists) {
        player.index = ind;
        ind++;
        std::cin >> player.effectiveness;
    }
}

void Output (std::vector <Footballist>& team,
             int64_t max_effectiveness) {
    std::cout << max_effectiveness << "\n";
    for (auto &player : team) {
        std::cout << player.index + 1 << " ";
    }
    std::cout << "\n";
}

int main() {
    std::vector<Footballist> footballists;
    Input(&footballists);
    std::vector<Footballist> team;
    int64_t max_effectiveness = BuildMostEffectiveSolidaryTeam(footballists, &team);
    Output(team, max_effectiveness);
    return 0;
}
