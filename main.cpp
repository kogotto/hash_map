/**
  * Didenko Yakov APO-11
  * module 3
  * task 1.1
  *
  * Реализуйте структуру данных типа “множество строк” на основе динамической хеш-таблицы с открытой адресацией.
  * Хранимые строки непустые и состоят из строчных латинских букв. Начальный размер таблицы должен быть равным 8-ми.
  * Структура данных должна поддерживать операции добавления строки в множество,
  * удаления строки из множества и проверки принадлежности данной строки множеству.
  * Для разрешения коллизий используйте квадратичное пробирование.
  * i-ая проба :
  *     g(k, i)=g(k, i-1) + i (mod m). m - степень двойки.
  */

#include <iostream>
#include <string>
#include <assert.h>
#include <vector>
using namespace std;

struct empty_t {};

template<class key_t>
class THash {
public:
    THash(size_t a = 13567923,
          size_t b = 12734527,
          size_t c = (1 << 31) - 1):
        a(a),
        b(b),
        c(c)
    {}

    size_t operator()(const key_t & key)
    {
        return (a + b * key) % c;
    }

private:
    size_t a;
    size_t b;
    size_t c;
};


template<class T, class U, class F = THash<T> >
class THashMap {
public:
    typedef T key_t;
    typedef U data_t;
    typedef F hfunc_t;
    struct item_t {
        item_t():
            state(ITEM_NONE)
        {}

        key_t key;
        data_t data;
        enum item_state_t {
            ITEM_NONE,
            ITEM_BUSY,
            ITEM_DELETED
        } state;
    };

    THashMap(size_t capacity = 10, const hfunc_t & hfunc = hfunc_t()):
        map(capacity),
        size(0),
        hfunc(hfunc)
    {}

    void insert(const key_t & key, const data_t & data) {
        size_t index = hash(key);
        while (map[index].state != item_t::ITEM_NONE) {
            index = next(index);
        }

        data[index].key = key;
        data[index].data = data;
        data[index].state = item_t::ITEM_BUSY;
    }

    bool remove(const key_t & key) {
        size_t index = hash(key);
        while (map[index].key != key) {
            index = next(index);
        }

    }

    const item_t * find(const key_t & elem) const{}
    item_t * find(const key_t & elem){}

private:
    void insertNoRebuild(const key_t & key, const data_t & data){}
    void rebuild();

    size_t next(size_t current) const {
        return (current + 1) % capacity();
    }

    size_t hash(const key_t & key) const {
        return hfunc(key) % capacity();
    }

    size_t capacity() const {
        return map.size();
    }

    vector<item_t> map;
    size_t size;
    hfunc_t hfunc;
};
//=============================================================================

int main()
{
    THashMap<string, empty_t> hashTable;
    while(true) {
        char action = 0;
        cin >> action;

        if (cin.eof()) {
            break;
        }

        string str;
        cin >> str;

        switch (action) {
        case '+' :
            cout << "add " << str;
            break;
        case '-':
            cout << "delete " << str;
            break;
        case'?':
            cout << "find " << str;
            break;
        default:
           assert(false) ;
        }
    }
    return 0;
}

//=============================================================================

