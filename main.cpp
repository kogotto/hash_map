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

// вариант со сдвигом почему-то вызывает ворнинг "overflow"
const size_t BIG_PRIME = 0x7fffFFFF;
//const size_t BIG_PRIME = (1 << 31) - 1;



template<class key_t>
class THash {
public:
    explicit THash(size_t a = 13567923,
                   size_t b = 12734527,
                   size_t c = BIG_PRIME):
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
//=============================================================================

template<>
class THash<std::string> {
public:
    explicit THash(size_t a = 13726483,
                   size_t m = BIG_PRIME):
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
//=============================================================================

class TSquareProbeIterator {
public:
    TSquareProbeIterator(size_t startIndex, size_t mod):
        currentIndex(startIndex),
        iteration(0),
        mod(mod)
    {}

    void next() {
        ++iteration;
        if (iteration < mod) {
            currentIndex += iteration;
            currentIndex %= mod;
            return;
        }

        currentIndex = mod;
    }

    size_t get() const {
        return currentIndex;
    }

private:
    size_t currentIndex;
    size_t iteration;
    size_t mod;
};
//=============================================================================


template<class T, class U, int maxFillingInProcents = 50, class F = THash<T>, class I = TSquareProbeIterator >
class THashMap {
public:
    typedef T key_t;
    typedef U data_t;
    typedef F hfunc_t;
    typedef I probe_iterator_t;
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

    /**
     * TODO capacity must be power of 2
     */
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

        const size_t currentCapacity = capacity();
        for (probe_iterator_t it(hash(key), currentCapacity);
             it.get() != currentCapacity;
             it.next()) {
            if (map[it.get()].state != item_t::ITEM_BUSY) {
                map[it.get()].key = key;
                map[it.get()].data = data;
                map[it.get()].state = item_t::ITEM_BUSY;
                ++size;
                return true;
            }
        }

        assert(false && "can\'t insert");
        return false;
    }

    bool remove(const key_t & key) {
        item_t * const item = find(key);

        if (item == 0) {
            return false;
        }

        item->state = item_t::ITEM_DELETED;
        --size;
        return true;
    }

    const item_t * find(const key_t & key) const {
        const size_t currentCapacity = capacity();
        for (probe_iterator_t it(hash(key), currentCapacity);
             it.get() < currentCapacity;
             it.next()) {

            if (map[it.get()].state == item_t::ITEM_NONE) {
                return 0;
            }

            if (map[it.get()].state == item_t::ITEM_BUSY &&
                    map[it.get()].key == key) {
                return &map[it.get()];
            }
        }

        return 0;
    }

    item_t * find(const key_t & key) {
        return const_cast<item_t *>(const_cast<const THashMap *>(this)->find(key));
    }

    size_t getSize() const {
        return size;
    }

private:
    typedef std::vector<item_t> map_t;

    static void moveItemTo(map_t & newMap, const item_t & item, const hfunc_t & hfunk) {
        const size_t newCapacity = newMap.size();
        const size_t itemHash = hash(item.key, newCapacity, hfunk);
        for (probe_iterator_t it(itemHash, newCapacity);
             it.get() < newCapacity;
             it.next()) {

            if (newMap[it.get()].state != item_t::ITEM_BUSY) {
                newMap[it.get()] = item;
                return;
            }
        }

        assert(false && "can\'t move element to new map while rebuild");
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

using std::string;
struct empty_t {};
typedef THashMap<string, empty_t> THashStringSet;

using std::cin;
using std::cout;
using std::endl;

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


