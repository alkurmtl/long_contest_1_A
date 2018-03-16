//
// Created by Alexander on 10.02.2018.
//

#ifndef UNTITLED6_HASH_MAP_H
#define UNTITLED6_HASH_MAP_H

#endif //UNTITLED6_HASH_MAP_H

#include <vector>
#include <list>
#include <utility>
#include <iostream>
#include <stdexcept>

template<class KeyType, class ValueType, class Hash = std::hash<KeyType>>
class HashMap {
    friend class iterator_template;

    template<bool Const>
    class iterator_template;

public:
    using iterator = iterator_template<false>;
    using const_iterator = iterator_template<true>;
    using bucket_type = std::list<std::pair<const KeyType, ValueType>>;

    explicit HashMap(Hash hash = Hash()): size_(0), hash_(hash) {
        buckets_.resize(1);
    }

    HashMap(const HashMap<KeyType, ValueType, Hash>& arg) {
        *this = arg;
    }

    HashMap& operator=(const HashMap<KeyType, ValueType, Hash>& arg) {
        if (&(*this) == &arg) {
            return *this;
        }
        buckets_.resize(arg.buckets_.size());
        for (size_t i = 0; i < buckets_.size(); ++i) {
            for (const auto& to_copy : arg.buckets_[i]) {
                buckets_[i].push_back(to_copy);
            }
        }
        size_ = arg.size_;
        hash_ = arg.hash_;
        return *this;
    }

    template <typename Iter>
    HashMap(Iter start, Iter end, Hash hash = Hash()) : size_(0), hash_(hash) {
        buckets_.resize(1);
        while (start != end) {
            insert(*start);
            ++start;
        }
    }

    HashMap(std::initializer_list<std::pair<KeyType, ValueType>> list, Hash hash = Hash()) :  HashMap(list.begin(), list.end(), hash) {

    }

    size_t size() const {
        return size_;
    }

    bool empty() const {
        return size_ == 0;
    }

    Hash hash_function() const {
        return hash_;
    }

    iterator begin() {
        return iterator(this, 0, buckets_[0].begin());
    }

    iterator end() {
        return iterator(this, buckets_.size() - 1, buckets_.back().end());
    }

    const_iterator begin() const {
        return const_iterator(this, 0, buckets_[0].begin());
    }

    const_iterator end() const {
        return const_iterator(this, buckets_.size() - 1, buckets_.back().end());
    }

    iterator find(const KeyType& arg) {
        size_t hash = get_hash(arg);
        for (auto iter = buckets_[hash].begin(); iter != buckets_[hash].end(); ++iter) {
            if (iter->first == arg) {
                return iterator(this, hash, iter);
            }
        }
        return end();
    }

    const_iterator find(const KeyType& arg) const {
        size_t hash = get_hash(arg);
        for (auto iter = buckets_[hash].begin(); iter != buckets_[hash].end(); ++iter) {
            if (iter->first == arg) {
                return const_iterator(this, hash, iter);
            }
        }
        return end();
    }

    ValueType& operator [](const KeyType& arg) {
        iterator found = find(arg);
        if (found != end()) {
            return found->second;
        }
        insert({arg, ValueType()});
        found = find(arg);
        return found->second;
    }

    const ValueType& at(const KeyType& arg) const {
        const_iterator found = find(arg);
        if (found == end()) {
            throw std::out_of_range("key is missing");
        }
        return found->second;
    }

    void insert(const std::pair<KeyType, ValueType>& arg) {
        if (find(arg.first) != end()) {
            return;
        }
        ++size_;
        size_t hash = get_hash(arg.first);
        buckets_[hash].push_back(arg);
        if (buckets_.size() * 2 < size_) {
            add_buckets();
        }
        return;
    }

    void erase(const KeyType& arg) {
        if (find(arg) == end()) {
            return;
        }
        size_t hash = get_hash(arg);
        for (auto iter = buckets_[hash].begin(); iter != buckets_[hash].end(); ++iter) {
            if (iter->first == arg) {
                buckets_[hash].erase(iter);
                break;
            }
        }
        --size_;
        if (size_ * 4 < buckets_.size()) {
            remove_buckets();
        }
    }

    void clear() {
        size_ = 0;
        for (auto& to_clear : buckets_) {
            to_clear.clear();
        }
    }


private:
    template<bool Const>
    class iterator_template {
    public:
        using owner_type = typename std::conditional<Const, const HashMap<KeyType, ValueType, Hash>, HashMap<KeyType, ValueType, Hash>>::type;
        using bucket_iterator_type = typename std::conditional<Const, typename owner_type::bucket_type::const_iterator,
                typename owner_type::bucket_type::iterator>::type;
        using iterator_category = std::forward_iterator_tag;
        using value_type = std::pair<const KeyType, ValueType>;
        using reference = typename std::conditional<Const, const value_type&, value_type&>::type;
        using pointer = typename std::conditional<Const, const value_type*, value_type*>::type;
        using difference_type = std::ptrdiff_t;

        iterator_template(owner_type* owner, size_t bucket, bucket_iterator_type iter) :
                owner_(owner), bucket_(bucket), iter_(iter) {
            normalize();
        }

        iterator_template() {

        }

        iterator_template& operator =(const iterator_template& arg) {
            owner_ = arg.owner_;
            bucket_ = arg.bucket_;
            iter_ = arg.iter_;
            return *this;
        }

        iterator_template(const iterator_template& arg) {
            *this = arg;
        }

        iterator_template& operator ++() {
            ++iter_;
            normalize();
            return *this;
        }

        iterator_template operator ++(int) {
            iterator_template t = *this;
            ++(*this);
            return t;
        }

        bool operator ==(const iterator_template& arg) const {
            return bucket_ == arg.bucket_ && iter_ == arg.iter_;
        }

        bool operator !=(const iterator_template& arg) const {
            return !(*this == arg);
        }

        reference operator *() const {
            return *iter_;
        }

        pointer operator ->() const {
            return &(*iter_);
        }


    private:
        owner_type* owner_;
        size_t bucket_;
        bucket_iterator_type iter_;

        void normalize() {
            if (bucket_ + 1 == owner_->buckets_.size() && iter_ == owner_->buckets_[bucket_].end()) {
                return;
            }
            while (iter_ == owner_->buckets_[bucket_].end()) {
                ++bucket_;
                iter_ = owner_->buckets_[bucket_].begin();
                if (bucket_ + 1 == owner_->buckets_.size() && iter_ == owner_->buckets_[bucket_].end()) {
                    return;
                }
            }
        }
    };

    std::vector<bucket_type> buckets_;
    size_t size_;
    Hash hash_;

    size_t get_hash(const KeyType& arg) const {
        return hash_(arg) % buckets_.size();
    }

    void add_buckets() {
        std::vector<std::pair<const KeyType, ValueType>> stored(begin(), end());
        clear();
        buckets_.resize(buckets_.size() * 2);
        size_ = 0;
        for (auto& to_add : stored) {
            insert(to_add);
        }
    }

    void remove_buckets() {
        std::vector<std::pair<const KeyType, ValueType>> stored(begin(), end());
        if (size_ != 0) {
            clear();
        }
        buckets_.resize(std::max(1, (int) size_ * 2));
        size_ = 0;
        for (auto& to_add : stored) {
            insert(to_add);
        }
    }
};