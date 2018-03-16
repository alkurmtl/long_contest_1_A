#include <iostream>
#include <type_traits>
#include <utility>

class List {
    friend List merge_sort(List& list);
    class Node;
    template <bool Const>
    class IteratorTemplate;
public:
    using Iterator = IteratorTemplate<false>;
    using ConstIterator = IteratorTemplate<true>;

    List() : head_(nullptr), tail_(nullptr), size_(0) {}

    friend void swap(List& lhs, List& rhs) {
        std::swap(lhs.head_, rhs.head_);
        std::swap(lhs.tail_, rhs.tail_);
        std::swap(lhs.size_, rhs.size_);
    }

    List(const List& other) : List() {
        for (int x : other) {
            push_back(x);
        }
    }

    List(List&& other) noexcept {
        head_ = other.head_;
        tail_ = other.tail_;
        size_ = other.size_;
        other.head_ = nullptr;
        other.tail_ = nullptr;
        other.size_ = 0;
    }

    List& operator=(List other) {
        if (head_ != other.head_) {
            clear();
        }
        swap(*this, other);
        other.head_ = nullptr;
        return *this;
    }

    int size() const {
        return size_;
    }

    Iterator begin() {
        return Iterator(head_);
    }

    Iterator end() {
        return Iterator(nullptr);
    }

    ConstIterator begin() const {
        return ConstIterator(head_);
    }

    ConstIterator end() const {
        return ConstIterator(nullptr);
    }

    void push_back(int x) {
        push_back_node(new Node(x));
    }

    std::pair<List, List> split(int partSize) {
        std::pair<List, List> res;
        Node* node = head_;
        int i = 0;
        while (node != nullptr) {
            if (i < partSize) {
                res.first.push_back_node(node);
            } else {
                res.second.push_back_node(node);
            }
            if (i == partSize - 1) {
                Node* temp = node->next;
                node->next = nullptr;
                node = temp;
            } else {
                node = node->next;
            }
            ++i;
        }
        return std::move(res);
    };

    List mergeWith(List& other) {
        List res;
        Node* nodeThis = head_;
        Node* nodeOther = other.head_;
        while (nodeThis != nullptr || nodeOther != nullptr) {
            if (nodeThis != nullptr && (nodeOther == nullptr || nodeThis->val < nodeOther->val)) {
                res.push_back_node(nodeThis);
                nodeThis = nodeThis->next;
            } else {
                res.push_back_node(nodeOther);
                nodeOther = nodeOther->next;
            }
        }
        return std::move(res);
    }

    void clear() {
        if (head_ == nullptr) {
            return;
        }
        Node* node = head_;
        Node* nextNode = node->next;
        while (node != nullptr) {
            delete(node);
            node = nextNode;
            if (node != nullptr) {
                nextNode = node->next;
            }
        }
        head_ = nullptr;
        tail_ = nullptr;
        size_= 0;
    }

    ~List() {
        clear();
    }

private:
    class Node {
        friend class List;

    private:
        Node* next;
        int val;

    public:
        Node(int x) : next(nullptr), val(x) {}
    };

    template <bool Const>
    class IteratorTemplate {
    private:
        using NodeType = typename std::conditional<Const, const Node*, Node*>::type;
        NodeType node_;

    public:
        using iterator_category = std::forward_iterator_tag;
        using value_type = int;
        using reference = typename std::conditional<Const, const int&, int&>::type;
        using pointer  = typename std::conditional<Const, const int*, int*>::type;
        using difference_type = std::ptrdiff_t;

        IteratorTemplate(NodeType node) : node_(node) {}

        IteratorTemplate& operator++() {
            node_ = node_->next;
            return *this;
        }

        IteratorTemplate operator++(int) {
            IteratorTemplate t = *this;
            ++(*this);
            return t;
        }

        bool operator==(const IteratorTemplate& other) const {
            return node_ == other.node_;
        }

        bool operator!=(const IteratorTemplate& other) const {
            return !(*this == other);
        }

        reference operator*() const {
            return node_->val;
        }
    };

    void push_back_node(Node* x) {
        if (size() == 0) {
            head_ = tail_ = x;
        } else {
            tail_->next = x;
            tail_ = x;
        }
        ++size_;
    }

    Node* head_;
    Node* tail_;
    int size_;
};

List merge_sort(List& list) {
    if (list.size() < 2) {
        return std::move(list);
    }
    std::pair<List, List> to_sort = list.split(list.size() / 2);
    to_sort.first = merge_sort(to_sort.first);
    to_sort.second = merge_sort(to_sort.second);
    List res = to_sort.first.mergeWith(to_sort.second);
    to_sort.first.head_ = nullptr;
    to_sort.second.head_ = nullptr;
    list.head_ = nullptr;
    return std::move(res);
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
        list.push_back(x);
    }
    List res = merge_sort(list);
    for (int i : res) {
        std::cout << i << ' ';
    }
}
