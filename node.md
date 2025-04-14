### 写时复制技术 Copy-On-Write
在多线程环境中，多个线程之间常常共享数据结构，当多个线程同时读取和修改这个表，如果不加以同步，就会导致竞争，迭代器失效，死锁
虽然可以使用互斥锁来保护列表，但每次访问都需要加锁，影响性能，并有死锁的风险，于是就有了写时复制技术
这种方式可以避免在读取时加锁，提高性能，同时，写入操作不会影响正在进行的读取操作，避免了迭代器的失效和数据竞争
关键思想
>在读取时使用原始数据，不加锁，写入是时复制一份副本，在副本上修改，然后**原子的**替换数据指针

    //示例
    std::mutex mutex_;
    std::shared_ptr<std::vector<Foo*>> foos_ = std::make_shared<std::vector<Foo*>>();

    void post(Foo* foo) {
        std::lock_guard<std::mutex> lock(mutex_);
        auto newFoos = std::make_shared<std::vector<Foo*>>(*foos_);
        newFoos->push_back(foo);
        foos_ = newFoos;
    }

    void doit() {
        std::shared_ptr<std::vector<Foo*>> foosCopy;
        {
            std::lock_guard<std::mutex> lock(mutex_);
            foosCopy = foos_;
        }
        for (auto& foo : *foosCopy) {
            foo->doit();
        }
    }

在当下，许多函数早已是线程安全的，关键在于不同的线程安全的函数组合到一起就会引发竟态，这是编写多线程程序的主要难点