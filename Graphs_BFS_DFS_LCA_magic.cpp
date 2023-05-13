#include <iostream> // Основная
#include <cstring> // Методы строк в С
#include <string> // Методы строк в С++
#include <algorithm> // Методы массивов, swap
#include <cmath> // Математические выражения, abs(x), floor(x), ceil(x), round(x), sqrt, tan, log, exp, pow(x, y),
                  // sin(x), cos(x)
#include <cstdlib> // Библиотека с языком С. Тут есть выделение памяти в стиле С
#include <cstdio> // Ввод-вывод в С
#include <fstream> // Работа с файлами
#include <iomanip> // cout << setprecision(3) << fixed; -- вывод числа с точностью до трех знаков после запятой
#include <vector> // Работа с массивами (a.push_back(el) - как append, a.size() - размер)
#include <random>
#include <optional>
#include <utility>
#include <queue>
#include <deque>
#include <climits>

//Сказочная страна представляет собой множество городов, соединенных дорогами с двухсторонним движением.
//Причем из любого города страны можно добраться в любой другой город либо непосредственно,
//либо через другие города. Известно, что в сказочной стране не существует дорог,
//соединяющих город сам с собой и между любыми двумя разными городами, существует не более одной дороги.

//В сказочной стране живут желтый и синий волшебники. Желтый волшебник, пройдя по дороге,
//перекрашивает ее в желтый цвет, синий — в синий. Как известно, при наложении желтой краски на синюю,
//либо синей краски на желтую, краски смешиваются и превращаются в краску зеленого цвета,
//который является самым нелюбимым цветом обоих волшебников.

//В этом году в столице страны (городе f) проводится конференция волшебников.
//Желтый и синий волшебники хотят узнать, какое минимальное количество дорог им придется
//перекрасить в зеленый цвет, чтобы добраться в столицу. Изначально все дороги не покрашены.

//Начальное положение желтого и синего волшебников заранее не известно.
//Поэтому необходимо решить данную задачу для k возможных случаев их начальных расположений.

//Формат ввода
//Первая строка входного файла содержит целые числа: n (1 ≤ n ≤ 100 000) и m (1 ≤ m ≤ 500 000) —
//количество городов и дорог в волшебной стране соответственно. Третья строка содержит одно
//целое число f (1 ≤ f ≤ n) — номер города, являющегося столицей сказочной страны.
//В следующих m строках, находится описание дорог страны. В этих m строк записано по два
//целых числа ai и bi, означающих, что существует дорога, соединяющая города ai и bi.
//Следующая строка содержит целое число k (1 ≤ k ≤ 100 000) — количество возможных
//начальных расположений волшебников. Далее следуют k строк, каждая из которых содержит
//два целых числа — номера городов, в которых изначально находится желтый и синий волшебники соответственно.

//Формат вывода
//Для каждого из k случаев, ваша программа должна вывести в выходной минимальное количество дорог,
//которое придется покрасить в зеленый цвет волшебникам для того, чтобы добраться в столиц

std::vector<bool> used;
std::vector<int> enter, vspom, colors;
int ttime;
int nn, mm;
std::vector<std::vector<int>> edges;
int maxColor = 0;
std::vector<int> stack;
std::vector<std::pair<int, int>> condensat_edges;

// ----------------------------------------------------------------
// Блок для нахождения конденсации
void paint(int vert) {
    maxColor++;
    int last = -1;
    while (last != vert and not stack.empty()) {
        colors[stack[stack.size() - 1]] = maxColor;
        last = stack[stack.size() - 1];
        stack.pop_back();
    }
}

void DFS(int vert, int prev = -1) {
    used[vert] = true;
    enter[vert] = vspom[vert] = ttime++;
    stack.push_back(vert);
    for (size_t ii = 0; ii < edges[vert].size(); ++ii) {
        int to = edges[vert][ii];
        if (to == prev) {
            continue;
        }
        if (used[to]) {
            vspom[vert] = std::min(vspom[vert], enter[to]);
        }
        else
        {
            DFS(to, vert);
            vspom[vert] = std::min(vspom[vert], vspom[to]);
            if (vspom[to] > enter[vert]) {
                paint(to);
                condensat_edges.push_back(std::pair(vert, to));
                // std::cout << vert + 1 << " " << to + 1 << "\n";
            }
        }
    }
}

void find_bridges() {
    ttime = 0;
    for (int i = 0; i < nn; ++i) {
        used[i] = false;
    }
    for (int i = 0; i < nn; ++i) {
        if (!used[i]) {
            DFS(i);
        }
    }
}

// ----------------------------------------------------------------

// ----------------------------------------------------------------
// Блок для нахождения расстояний от столицы (ее компоненты реберной двусвязности)
// до остальных компонент
std::vector<int> BFS(int vert) {
    std::queue<int> que;
    que.push(vert);
    std::vector<bool> used(nn);
    std::vector<int> dist(nn, 0);
    used[vert] = true;
    while (!que.empty()) {
        int cur_v = que.front();
        que.pop();
        for (size_t i = 0; i < edges[cur_v].size(); ++i) {
            int to = edges[cur_v][i];
            if (!used[to]) {
                used[to] = true;
                que.push(to);
                dist[to] = dist[cur_v] + 1;
            }
        }
    }
    return dist;
}

//----------------------------------------------------------------

//----------------------------------------------------------------
// Блок для нахождения lca
int len;
std::vector<int> time_in, time_out;
int timer;
std::vector<std::vector<int>> up;

void DfsForCondensat(int vert, int prev) {
    time_in[vert] = ++timer;
    up[vert][0] = prev;
    for (int i = 1; i <= len; ++i) {
        up[vert][i] = up[up[vert][i-1]][i-1];
    }
    for (size_t i = 0; i < edges[vert].size(); ++i) {
        int to = edges[vert][i];
        if (to != prev)
            DfsForCondensat(to, vert);
    }
    time_out[vert] = ++timer;
}

bool upper (int one, int two) {
    return time_in[one] <= time_in[two] && time_out[one] >= time_out[two];
}

int lca (int one, int two) {
    if (upper(one, two))  return one;
    if (upper(two, one))  return two;
    for (int i = len; i >= 0; --i) {
        if (!upper(up[one][i], two)) {
            one = up[one][i];
        }
    }
    return up[one][0];
}

//----------------------------------------------------------------

int main()
{
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(nullptr);
    std::cin >> nn >> mm;
    edges.resize(nn);
    used.resize(nn);
    vspom.resize(nn);
    enter.resize(nn);
    colors.resize(nn);
    int capital;
    std::cin >> capital;
    capital--;
    for (int i = 0; i < mm; ++i) {
        int from, to;
        std::cin >> from >> to;
        edges[from - 1].push_back(to - 1);
        edges[to - 1].push_back(from - 1);
    }
    find_bridges();

//    for(int i = 0; i < nn; ++i) {
//        std::cout << colors[i] << " ";
//    }
//    std::cout << "\n";
    // Сторим новый граф - конденсацию по реберной двусвязности, получаем дерево
    nn = maxColor + 1;
    edges.clear();
    edges.resize(nn);
    for (int i = 0; i < nn - 1; ++i) {
        int from, to;
        from = colors[condensat_edges[i].first];
        to = colors[condensat_edges[i].second];
        edges[from].push_back(to);
        edges[to].push_back(from);
    }
    capital = colors[capital];
    std::vector<int> dists = BFS(capital);
//    std::cout << "cap: " << capital << "\n";
//    for(int i = 0; i < nn; ++i) {
//        std::cout << dists[i] << " ";
//    }
//    std::cout << "\n";

    time_in.resize(nn),  time_out.resize(nn),  up.resize(nn);
    len = 1;
    while ((1 << len) <= nn)  ++len;
    for (int i = 0; i < nn; ++i) {
        up[i].resize(len + 1);
    }
    DfsForCondensat(capital, capital);

    int kk;
    std::cin >> kk;
    for (int i = 0; i < kk; ++i) {
        int start_one, start_two;
        std::cin >> start_one >> start_two;
        start_one = colors[start_one - 1];
        start_two = colors[start_two - 1];
//        std::cout << start_one << " " << start_two << "\n";
//        std::cout << lca(start_one, start_two) << "\n";
        std::cout << dists[lca(start_one, start_two)] << "\n";
    }
    return 0;
}
