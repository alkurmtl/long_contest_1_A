#include <iostream>
#include <utility>

class List {
public:
    class Node {
    private:
        Node* next;
        int val;
        friend class List;

    public:
        Node(int x) : next(nullptr), val(x) {}
    };

private:
    Node* head;
    Node* tail;
    int sz;

public:
    class Iterator {
    private:
        Node* cur;

    public:
        Iterator(Node* cur) : cur(cur) {}

        Iterator& operator++() {
            cur = cur->next;
            return *this;
        }

        Iterator operator++(int) {
            Iterator t = *this;
            ++(*this);
            return t;
        }

        bool operator==(Iterator& arg) const {
            return cur == arg.cur;
        }

        bool operator!=(Iterator& arg) const {
            return !(*this == arg);
        }

        int& operator*() const {
            return cur->val;
        }
    };

    List() : head(nullptr), tail(nullptr), sz(0) {}

    List(const List& arg) {
        head = nullptr;
        tail = nullptr;
        sz = 0;
        Node* cur = arg.head;
        while (cur != nullptr) {
            push_back(cur);
            cur = cur->next;
        }
    }

    List& operator=(const List& arg) {
        head = nullptr;
        tail = nullptr;
        sz = 0;
        Node* cur = arg.head;
        while (cur != nullptr) {
            push_back(cur);
            cur = cur->next;
        }
    }

    int size() const {
        return sz;
    }

    Iterator begin() const {
        return Iterator(head);
    }

    Iterator end() const {
        return Iterator(nullptr);
    }

    void push_back(Node* x) {
        if (size() == 0) {
            head = tail = x;
        } else {
            tail->next = x;
            tail = x;
        }
        ++sz;
    }

    std::pair<List, List> split(int k) {
        std::pair<List, List> res;
        Node* cur = head;
        int i = 0;
        while (cur != nullptr) {
            if (i < k) {
                res.first.push_back(cur);
            } else {
                res.second.push_back(cur);
            }
            if (i == k - 1) {
                Node* temp = cur->next;
                cur->next = nullptr;
                cur = temp;
            } else {
                cur = cur->next;
            }
            ++i;
        }
        return res;
    };

    List merge(List arg) {
        List res;
        Node* cur1 = head;
        Node* cur2 = arg.head;
        while (cur1 != nullptr && cur2 != nullptr) {
            if (cur1->val < cur2->val) {
                res.push_back(cur1);
                cur1 = cur1->next;
            } else {
                res.push_back(cur2);
                cur2 = cur2->next;
            }
        }
        if (cur1 == nullptr) {
            while (cur2 != nullptr) {
                res.push_back(cur2);
                cur2 = cur2->next;
            }
        } else if (cur2 == nullptr) {
            while (cur1 != nullptr) {
                res.push_back(cur1);
                cur1 = cur1->next;
            }
        }
        return res;
    }

    void clear() {
        if (head == nullptr) {
            return;
        }
        Node* cur = head;
        Node* next = cur->next;
        while (true) {
            delete(cur);
            cur = next;
            if (cur == nullptr) {
                break;
            }
            next = cur->next;
        }
    }
};

List merge_sort(List list) {
    if (list.size() < 2) {
        return list;
    } else {
        std::pair<List, List> to_sort = list.split(list.size() / 2);
        to_sort.first = merge_sort(to_sort.first);
        to_sort.second = merge_sort(to_sort.second);
        List res = to_sort.first.merge(to_sort.second);
        return res;
    }
}

int main() {
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(0);
    int n;
    std::cin >> n;
    List list;
    int x;
    for (int i = 0; i < n; ++i) {
        std::cin >> x;
        list.push_back(new List::Node(x));
    }
    List res = merge_sort(list);
    for (int x : res) {
        std::cout << x << ' ';
    }
    res.clear();
}
