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
#include <array>
#include <iomanip>
#include <iostream>
#include <string>

using namespace std;

class Ebook {
public:
    Ebook();
    void ReadUserPage(int user, int new_page_num);
    double CheerUser(int user);

private:
    void AddNewUser(int new_page_num);
    void UpdateUsersPage(int user, int new_page_num);

    static const int max_users_count_ = 100001; // фиксируем для служебных целей нулевой элемент
    static const int max_pages_count_ = 1001;   // фиксируем для служебных целей нулевой элемент

    int max_page_ = 0;
    int users_count_ = 0;
    array<int, max_users_count_> users_page_;     // номер страницы, до которой дочитал пользователь id, равный n
    array<int, max_pages_count_> read_least_page_;// количество пользователей, дочитавших до страницы m и меньше
};

Ebook::Ebook() {
    users_page_.fill(0);
    read_least_page_.fill(0);
}
/*
 * При добавлении нового пользователя:
 * - ищем ближайшую ненулевую предшествующую страницу;
 * - если если номер страницы ещё не зафиксирован в таблице read_least_page_, то копируем
 * значение с ближайшей ненулевой предшествующей страницы;
 * - увеличиваем значения всех [new_page_num ... max__page_] на 1
 */
void Ebook::AddNewUser(int new_page_num) {
    int i = new_page_num - 1;
    while ((i > 0) && (read_least_page_[i] == 0)) {
        --i;
    }
    if (read_least_page_[new_page_num] == 0) {
        for (int j = i + 1; j <= new_page_num; ++j) {
            read_least_page_[j] = read_least_page_[i];
        }
    }

    for (int j = new_page_num; j <= max_page_; ++j) {
        ++read_least_page_[j];
    }
}

/*
 * При обновлении страницы пользователя:
 * - в промежутке [old_page_num ... new_page_num) уменьшаем значения пользователей на 1 (для ненулевых значений),
 */
void Ebook::UpdateUsersPage(int user, int new_page_num) {
    int old_page_num = users_page_[user];

    if (read_least_page_[new_page_num] == 0) {
        int i = new_page_num - 1;
        while ((i > 0) && (read_least_page_[i] == 0)) {
            --i;
        }
        for (int j = i + 1; j <= new_page_num; ++j) {
            read_least_page_[j] = read_least_page_[i];
        }
    }

    for (int i = old_page_num; i != new_page_num; ++i) {
        if (read_least_page_[i] > 0) {
            --read_least_page_[i];
        }
    }
}

void Ebook::ReadUserPage(int user, int new_page_num) {
    if (new_page_num > max_page_) {
        max_page_ = new_page_num;
    }
    if (users_page_[user] == 0) {
        ++users_count_;
    }

    if (users_page_[user] == 0) {
        AddNewUser(new_page_num);
    } else {
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

    return static_cast<double>(read_least_page_[users_page_[user] - 1]) / static_cast<double>(users_count_ - 1);
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
