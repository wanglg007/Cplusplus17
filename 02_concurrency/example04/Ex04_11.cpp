//线程安全的查询表
#include <vector>
#include <memory>
#include <mutex>
#include <functional>
#include <list>
#include <utility>
#include <boost/thread/shared_mutex.hpp>
#include <boost/thread/mutex.hpp>

template<typename Key, typename Value, typename Hash=std::hash<Key> >
class threadsafe_lookup_table {
private:
    class bucket_type {
    private:
        typedef std::pair<Key, Value> bucket_value;
        typedef std::list<bucket_value> bucket_data;
        typedef typename bucket_data::iterator bucket_iterator;

        bucket_data data;
        mutable boost::shared_mutex mutex;                          // 1

        bucket_iterator find_entry_for(Key const &key) const {      // 2
            return std::find_if(data.begin(), data.end(), [&](bucket_value const &item) { return item.first == key; });
        }

    public:
        Value value_for(Key const &key, Value const &default_value) const {
            boost::shared_lock<boost::shared_mutex> lock(mutex);      // 3
            bucket_iterator const found_entry = find_entry_for(key);
            return (found_entry == data.end()) ? default_value : found_entry->second;
        }

        void add_or_update_mapping(Key const &key, Value const &value) {
            std::unique_lock<boost::shared_mutex> lock(mutex);        // 4
            bucket_iterator const found_entry = find_entry_for(key);
            if (found_entry == data.end()) {
                data.push_back(bucket_value(key, value));
            } else {
                found_entry->second = value;
            }
        }

        void remove_mapping(Key const &key) {
            std::unique_lock<boost::shared_mutex> lock(mutex);      // 5
            bucket_iterator const found_entry = find_entry_for(key);
            if (found_entry != data.end()) {
                data.erase(found_entry);
            }
        }
    };

    std::vector<std::unique_ptr<bucket_type> > buckets;             // 6
    Hash hasher;

    bucket_type &get_bucket(Key const &key) const {               // 7
        std::size_t const bucket_index = hasher(key) % buckets.size();
        return *buckets[bucket_index];
    }

public:
    typedef Key key_type;
    typedef Value mapped_type;
    typedef Hash hash_type;

    threadsafe_lookup_table(unsigned num_buckets = 19, Hash const &hasher_ = Hash()) :
            buckets(num_buckets),
            hasher(hasher_) {
        for (unsigned i = 0; i < num_buckets; ++i) {
            buckets[i].reset(new bucket_type);
        }
    }

    threadsafe_lookup_table(threadsafe_lookup_table const &other) = delete;

    threadsafe_lookup_table &operator=(threadsafe_lookup_table const &other) = delete;

    Value value_for(Key const &key, Value const &default_value = Value()) const {
        return get_bucket(key).value_for(key, default_value);               // 8
    }

    void add_or_update_mapping(Key const &key, Value const &value) {
        get_bucket(key).add_or_update_mapping(key, value);                  // 9
    }

    void remove_mapping(Key const &key) {
        get_bucket(key).remove_mapping(key);                                // 10
    }
};

int main() {}
/**
 * 使用了std::vector<std::unique_ptr<bucket_type>> ⑥来保存桶，其允许在构造函数中指定构造桶的数量。默认为19个，其是一个任意的质数;哈希表在有质数个桶时，工作效
 * 率最高。每一个桶都会被一个boost::shared_mutex ①实例锁保护来允许并发读取，或对每一个桶只有一个线程对其进行修改。
 *
 * 因为桶的数量是固定的，所以get_bucket()⑦可以无锁调用，⑧⑨⑩也都一样。并且对桶的互斥量上锁，要不就是共享(只读)所有权的时候③，要不就是在获取唯一(读/写)权的
 * 时候④⑤。这里的互斥量可适用于每个成员函数。这三个函数都使用到了find_entry_for()成员函数②，在桶上用来确定数据是否在桶中。每一个桶都包含一个“键值-数据”的
 * std::list<>列表，所以添加和删除数据就会很简单。
 *
 * 已经从并发的角度考虑，并且所有成员都会被互斥锁保护，所以这样的实现就是“异常安全”的吗？value_for是不能修改任何值的，所以其不会有问题；如果value_for抛出异常，
 * 也不会对数据结构有任何影响。remove_mapping修改链表时将会调用erase，不过这就能保证没有异常抛出，那么这里也是安全的。那么add_or_update_mapping其可能会在其两个
 * if分支上抛出异常。push_back是异常安全的，如果有异常抛出，其也会将链表恢复成原来的状态，所以这个分支是没有问题的。唯一的问题就是在赋值阶段，这将替换已有的数据；
 * 当复制阶段抛出异常，用于原依赖的始状态没有改变。不过这不会影响数据结构的整体，以及用户提供类型的属性，所以可以放心的将问题交给用户处理。
 *
 */