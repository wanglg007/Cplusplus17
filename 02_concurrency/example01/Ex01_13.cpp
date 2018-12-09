//使用 boost::shared_mutex 对数据结构进行保护
#include <map>
#include <string>
#include <mutex>
#include <boost/thread/shared_mutex.hpp>

class dns_entry {
};

class dns_cache {
    std::map<std::string, dns_entry> entries;
    boost::shared_mutex entry_mutex;
public:
    dns_entry find_entry(std::string const &domain) {
        boost::shared_lock<boost::shared_mutex> lk(entry_mutex);            // 1
        std::map<std::string, dns_entry>::const_iterator const it = entries.find(domain);
        return (it == entries.end()) ? dns_entry() : it->second;
    }

    void update_or_add_entry(std::string const &domain, dns_entry const &dns_details) {
        std::lock_guard<boost::shared_mutex> lk(entry_mutex);               // 2
        entries[domain] = dns_details;
    }
};

int main() {}

/**
 * find_entry()使用了 boost::shared_lock<> 实例来保护其共享和只读权限①；这就使得，多线程可以同时调用find_entry()，且不会出错。另一方面，update_or_add_entry()
 * 使用 std::lock_guard<> 实例，当表格需要更新时②，为其提供独占访问权限；在update_or_add_entry()函数调用时，独占锁会阻止其他线程对数据结构进行修改，并且这些
 * 线程在这时，也不能调用find_entry()。
 */
