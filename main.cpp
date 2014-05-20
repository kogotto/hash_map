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

struct empty_t {};

template<class key_t>
class THash {
public:
    explicit THash(size_t a = 13567923,
                   size_t b = 12734527,
                   size_t c = (1 << 31) - 1):
        a(a),
        b(b),
        c(c)
    {}

    size_t operator()(const key_t & key) const {
        return (a + b * key) % c;
    }

private:
    size_t a;
    size_t b;
    size_t c;
};


template<>
class THash<std::string> {
public:
    explicit THash(size_t a = 13726483,
                   size_t m = (1 << 31) - 1):
        a(a),
        m(m)
    {}

    size_t operator()(const std::string & key) const {
        size_t result = 0;
        for (std::string::const_iterator it = key.begin();
             it != key.end();
             ++it) {
            result = (result * a + *it) % m;
        }
        return result;
    }

private:
    size_t a;
    size_t m;
};


template<class T, class U, int maxFillingInProcents = 50, class F = THash<T> >
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

    explicit THashMap(size_t capacity = 4, const hfunc_t & hfunc = hfunc_t()):
        map(capacity),
        size(0),
        hfunc(hfunc)
    {}

    bool insert(const key_t & key, const data_t & data = data_t()) {
        if (find(key) != 0) {
            return false;
        }

        if (filling() > maxFilling()) {
            rebuild();
        }

        size_t index = hash(key);
        while (map[index].state != item_t::ITEM_NONE) {
            index = next(index);
        }

        map[index].key = key;
        map[index].data = data;
        map[index].state = item_t::ITEM_BUSY;
        ++size;
        return true;
    }

    bool remove(const key_t & key) {
        size_t startIndex = hash(key);
        size_t index = startIndex;
        do {
            if (map[index].key == key && map[index].state == item_t::ITEM_BUSY){
                map[index].state = item_t::ITEM_DELETED;
                --size;
                return true;
            }

            index = next(index);
        } while (index != startIndex);

        return false;
    }

    const item_t * find(const key_t & key) const {
        size_t startIndex = hash(key);
        size_t index = startIndex;
        do {
            if (map[index].key == key && map[index].state == item_t::ITEM_BUSY){
                return &map[index];
            }

            index = next(index);
        } while (index != startIndex);

        return 0;
    }

    item_t * find(const key_t & key) {
        size_t startIndex = hash(key);
        size_t index = startIndex;
        do {
            if (map[index].key == key && map[index].state == item_t::ITEM_BUSY){
                return &map[index];
            }

            index = next(index);
        } while (index != startIndex);

        return 0;
    }

    size_t getSize() const {
        return size;
    }

private:
    typedef std::vector<item_t> map_t;

    static void moveItemTo(map_t & newMap, const item_t & item, const hfunc_t & hfunk) {
        size_t newMapSize = newMap.size();
        size_t index = hash(item.key, newMapSize, hfunk);
        while (newMap[index].state != item_t::ITEM_NONE) {
            index = next(index, newMapSize);
        }

        newMap[index] = item;
    }

    void rebuild() {
        map_t newMap(capacity() * 2);

        for (size_t i = 0; i < map.size(); ++i) {
            if (map[i].state != item_t::ITEM_BUSY) {
                continue;
            }
            moveItemTo(newMap, map[i], hfunc);
        }

        map.swap(newMap);
    }

    static size_t next(size_t current, size_t mod) {
        return (current + 1) % mod;
    }

    size_t next(size_t current) const {
        return next(current, capacity());
    }

    static size_t hash(const key_t & key, size_t mod, const hfunc_t & hfunc) {
        return hfunc(key) % mod;
    }

    size_t hash(const key_t & key) const {
        return hash(key, capacity(), hfunc);
    }

    size_t capacity() const {
        return map.size();
    }

    float maxFilling() const {
        return static_cast<float>(maxFillingInProcents) / 100;
    }

    float filling() const {
        return static_cast<float>(getSize()) / capacity();
    }

    map_t map;
    size_t size;
    hfunc_t hfunc;
};
//=============================================================================

void testHash();


using std::string;
using std::cin;
using std::cout;
using std::endl;
typedef THashMap<string, empty_t> THashStringSet;

int main()
{
    const string OK("OK");
    const string FAIL("FAIL");

    THashStringSet stringSet;
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
            if (stringSet.insert(str)) {
                cout << OK << endl;
            } else {
                cout << FAIL << endl;
            }
            break;
        case '-':
            if (stringSet.remove(str)) {
                cout << OK << endl;
            } else {
                cout << FAIL << endl;
            }
            break;
        case'?':
            if (stringSet.find(str) != 0) {
                cout << OK << endl;
            } else {
                cout << FAIL << endl;
            }
            break;
        default:
           assert(false) ;
        }
    }
    return 0;
}

//=============================================================================

void testHash()
{
    THash<string> hash;
    for (char c = 'a'; c <= 'z'; c++) {
        cout << "hash(" << c << ") = " << hash(string(&c, 1)) % 11 << endl;
    }

    for (char c1 = 'a'; c1 <= 'z'; c1++) {
        for (char c2 = 'a'; c2 <= 'z'; c2++) {
            string str;
            str.push_back(c1);
            str.push_back(c2);
            cout << "hash(" << str << ") = " << hash(str) % 11 << endl;
        }
    }

    for (char c1 = 'a'; c1 <= 'z'; c1++) {
        for (char c2 = 'a'; c2 <= 'z'; c2++) {
            for (char c3 = 'a'; c3 <= 'z'; c3++) {
                string str;
                str.push_back(c1);
                str.push_back(c2);
                str.push_back(c3);
                cout << "hash(" << str << ") = " << hash(str) % 11 << endl;
            }
        }
    }
}

