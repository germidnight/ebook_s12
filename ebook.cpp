/*
 * Разработайте систему стимулирования чтения электронных книг.
 * Для простоты будем считать, что книга всего одна, но её одновременно читают много людей.
 * Нужно следить за прогрессом чтения у всех пользователей и выводить мотивирующие уведомления.
 * А именно, программа должна обрабатывать следующие события:
 * - READ user page — сохранить факт того, что пользователь под номером user дочитал книгу до страницы page.
 * Если ранее такой пользователь не встречался, нужно его добавить.
 * Гарантируется, что в рамках одного пользователя номера страниц в соответствующих ему событиях возрастают.
 * - CHEER user — сообщить пользователю user, какая доля существующих пользователей, не считая его самого,
 * прочитала меньшую часть книги, чем он. Если этот пользователь на данный момент единственный, доля считается равной 1.
 * Если для данного пользователя пока не было ни одного события READ, доля считается равной 0,
 * а сам пользователь не учитывается при вычислении долей для других пользователей до тех пор, пока для него не случится событие READ.
 *
 * Формат входных данных:
 * В первой строке вводится количество запросов Q — натуральное число, не превосходящее 10⁶.
 * В следующих Q строках вводятся запросы.
 * Гарантируется, что все вводимые числа целые и положительные, при этом номера пользователей не превосходят 10⁵,
 * а номера страниц не превосходят 1000.
 *
 * Формат выходных данных:
 * Для каждого запроса CHEER user выведите единственное вещественное число от 0 до 1 — долю пользователей, прочитавших меньше страниц, чем user.
 * Для вывода числа на экран используйте setprecision(6).
 */
#include <algorithm>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>

using namespace std;

class Ebook {
public:
    Ebook();
    void ReadUserPage(int user, int new_page_num);
    double CheerUser(int user);

private:
    void AddNewUser(int new_page_num);
    void UpdateUsersPage(int user, int new_page_num);
    int max_page_ = 0;
    int users_count_ = 0;
    vector<int> users_page_;     // номер страницы, до которой дочитал пользователь id, равный n
    vector<int> read_least_page_;// количество пользователей, дочитавших до страницы m и меньше
};

Ebook::Ebook() {
    const int max_users_count = 100001; // фиксируем для служебных целей нулевой элемент
    const int max_pages_count = 1001;   // фиксируем для служебных целей нулевой элемент
    users_page_.resize(max_users_count, 0);
    read_least_page_.resize(max_pages_count, 0);
}
/*
 * При добавлении нового пользователя:
 * - ищем ближайшую ненулевую предшествующую страницу;
 * - если если номер страницы ещё не зафиксирован в таблице read_least_page_, то копируем
 * значение с ближайшей ненулевой предшествующей страницы;
 * - увеличиваем количество дочитавших до новой страницы;
 * - увеличиваем все ненулевые значения всех [new_page_num ... max__page_]
 */
void Ebook::AddNewUser(int new_page_num) {
    int i = new_page_num - 1;
    while ((i > 0) && (read_least_page_[i] == 0)) {
        --i;
    }
    if (read_least_page_[new_page_num] == 0) {
        read_least_page_[new_page_num] = read_least_page_[i];
    }
    ++read_least_page_[new_page_num];
    for (int j = new_page_num + 1; j <= max_page_; ++j) {
        read_least_page_[j] = (read_least_page_[j] == 0) ? 0 : (read_least_page_[j] + 1);
    }
}

/*
 * При обновлении страницы пользователя:
 * - если read_least_page_[new_page_num] ещё не считалось (значение 0), то копируем значение из предыдущей страницы и
 * нужна будет конечная корректировка значения;
 * - ищем ближайшую ненулевую предшествующую страницу;
 * - если ближайшая ненулевая предшествующая страница существует (не равна 0) и она последовательно уменьшается
 * (нет 2х и болеее пользователей на одной странице), то обнуляем значение на старой странице,
 * в противном случае уменьшаем значение на единицу;
 * - в промежутке (old_page_num ... new_page_num) уменьшаем значения пользователей на 1 (для ненулевых значений),
 * а также подсчитываем возможную корректировку;
 * - применяем корректировку по необходимости
 */
void Ebook::UpdateUsersPage(int user, int new_page_num) {
    int old_page_num = users_page_[user];

    bool use_corr = false;
    if (read_least_page_[new_page_num] == 0) {
        read_least_page_[new_page_num] = read_least_page_[old_page_num];
        use_corr = true;
    }

    int i = old_page_num - 1;
    while ((i > 0) && (read_least_page_[i] == 0)) {
        --i;
    }
    if ((i > 0) && (read_least_page_[i] + 1 == read_least_page_[old_page_num])) {
        read_least_page_[old_page_num] = 0;
    } else {
        --read_least_page_[old_page_num];
    }

    int corr_count = 0;
    for (int i = old_page_num + 1; i != new_page_num; ++i) {
        if (read_least_page_[i] > 0) {
            --read_least_page_[i];
            ++corr_count;
        }
    }
    if (use_corr) {
        read_least_page_[new_page_num] += corr_count;
    }
}

void Ebook::ReadUserPage(int user, int new_page_num) {
    if (new_page_num > max_page_) {
        max_page_ = new_page_num;
    }
    if (users_page_[user] == 0) {
        ++users_count_;
    }

    if (users_page_[user] == 0) { // новый пользователь
        AddNewUser(new_page_num);
    } else {    // Обновление пользователя
        UpdateUsersPage(user, new_page_num);
    }

    users_page_[user] = new_page_num;
}

double Ebook::CheerUser(int user) {
    if (users_page_[user] == 0) {
        return 0;
    }
    if (users_count_ == 1) {
        return 1;
    }

    int i = users_page_[user] - 1;
    while ((i > 0) && (read_least_page_[i] == 0)) {
        --i;
    }
    int people_num = read_least_page_[i];

    if ((users_count_ > 0) && (people_num > 0)) {
        return static_cast<double>(people_num) / static_cast<double>(users_count_ - 1);
    }
    return 0;
}

int main() {
    long long count;
    const int precision = 6;

    cin >> count;
    Ebook ebook;
    cout << setprecision(precision);

    for (long long i = 0; i < count; ++i) {
        string command;
        int user, page;
        cin >> command;
        if (command == "READ"s) {
            cin >> user >> page;
            ebook.ReadUserPage(user, page);
        } else if (command == "CHEER") {
            cin >> user;
            cout << ebook.CheerUser(user) << endl;
        }
    }

    return 0;
}
