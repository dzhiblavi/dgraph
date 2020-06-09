#ifndef INTRUSIVE_LIST_H
#define INTRUSIVE_LIST_H

#include <iterator>
#include <cstddef>

template <typename Tag, typename T>
struct intr_list_node {
    template <typename, typename> friend class intrusive_list;
    template <typename, typename> friend class intrusive_list_iterator;
    template <typename, typename> friend class intrusive_list_const_iterator;

    typedef Tag tag;
    typedef intr_list_node<Tag, T>* intr_list_node_ptr;
    typedef T value_type;
    typedef T& reference;
    typedef T const& const_reference;

private:
    intr_list_node_ptr next_node = nullptr;
    intr_list_node_ptr prev_node = nullptr;

private:
    reference get() noexcept {
        return *static_cast<T*>(this);
    }

    const_reference get() const noexcept {
        return *static_cast<T const*>(this);
    }

public:
    intr_list_node() = default;

    ~intr_list_node() = default;

    intr_list_node(intr_list_node_ptr prv, intr_list_node_ptr nxt) noexcept
        : next_node(nxt)
        , prev_node(prv) {}

    reference next() noexcept {
        return this->next_node->get();
    }

    const_reference next() const noexcept {
        return this->next_node->get();
    }

    reference prev() noexcept {
        return this->prev_node->get();
    }

    const_reference prev() const noexcept {
        return this->prev_node->get();
    }

    void insert(intr_list_node_ptr prv) noexcept {
        intr_list_node_ptr nxt = prv->next_node;
        prv->next_node = this;
        next_node = nxt;
        prev_node = prv;
        nxt->prev_node = this;
    }

    void remove() noexcept {
        if (prev_node)
            prev_node->next_node = next_node;
        if (next_node)
            next_node->prev_node = prev_node;
        next_node = prev_node = nullptr;
    }
};

template <typename Tag, typename T>
struct intrusive_list_iterator {
    template <typename, typename> friend class intrusive_list;

    typedef T value_type;
    typedef T& reference;
    typedef std::ptrdiff_t difference_type;
    typedef T* pointer;
    typedef std::bidirectional_iterator_tag iterator_category;

private:
    intr_list_node<Tag, T>* node = nullptr;

private:
    explicit intrusive_list_iterator(intr_list_node<Tag, T>* p)
        : node(p) {}

public:
    intrusive_list_iterator() = default;

    intrusive_list_iterator(intrusive_list_iterator const&) = default;

    intrusive_list_iterator& operator=(intrusive_list_iterator const&) = default;

    intrusive_list_iterator& operator++() {
        node = node->next_node;
        return *this;
    }

    intrusive_list_iterator& operator--() {
        node = node->prev_node;
        return *this;
    }

    const intrusive_list_iterator operator++(int) {
        intrusive_list_iterator nit(*this);
        ++*this;
        return nit;
    }

    const intrusive_list_iterator operator--(int) {
        intrusive_list_iterator nit(*this);
        --*this;
        return nit;
    }

    bool operator==(intrusive_list_iterator const& rhs) const {
        return node == rhs.node;
    }

    bool operator!=(intrusive_list_iterator const& rhs) const {
        return node != rhs.node;
    }

    reference operator*() const {
        return node->get();
    }

    pointer operator->() const {
        return &node->get();
    }
};

template <typename Tag, typename T>
struct intrusive_list_const_iterator {
    template <typename, typename> friend class intrusive_list;

    typedef T value_type;
    typedef T const& const_reference;
    typedef ptrdiff_t difference_type;
    typedef T const* const_pointer;
    typedef std::bidirectional_iterator_tag iterator_category;

private:
    intr_list_node<Tag, T>* node = nullptr;

private:
    intrusive_list_const_iterator(intr_list_node<Tag, T>* p)
        : node(p) {}

public:
    intrusive_list_const_iterator() = default;

    intrusive_list_const_iterator(intrusive_list_const_iterator const&) = default;

    intrusive_list_const_iterator(intrusive_list_iterator<Tag, T> const& p)
        : node(p.node) {}

    intrusive_list_const_iterator& operator=(intrusive_list_const_iterator const&) = default;

    intrusive_list_const_iterator& operator++() {
        node = node->next_node;
        return *this;
    }

    intrusive_list_const_iterator& operator--() {
        node = node->prev_node;
        return *this;
    }

    const intrusive_list_const_iterator operator++(int) {
        intrusive_list_iterator nit(*this);
        ++*this;
        return nit;
    }

    const intrusive_list_const_iterator operator--(int) {
        intrusive_list_iterator nit(*this);
        --*this;
        return nit;
    }

    bool operator==(intrusive_list_const_iterator const& rhs) const {
        return node == rhs.node;
    }

    bool operator!=(intrusive_list_const_iterator const& rhs) const {
        return node != rhs.node;
    }

    const_reference operator*() const {
        return node->get();
    }

    const_pointer operator->() const {
        return &node->get();
    }
};

template <typename TTag, typename T>
intr_list_node<TTag, T>& node_cast(T& obj) {
    return static_cast<intr_list_node<TTag, T>&>(obj);
}

template <typename TTag, typename T>
intr_list_node<TTag, T> const& node_cast(T const& obj) {
    return static_cast<intr_list_node<TTag, T> const&>(obj);
}

template <typename TTag, typename T>
T& next(T& obj) {
    return node_cast<TTag, T>(obj).next();
}

template <typename TTag, typename T>
T const& next(T const& obj) {
    return node_cast<TTag, T>(obj).next();
}

template <typename TTag, typename T>
T& prev(T& obj) {
    return node_cast<TTag, T>(obj).next();
}

template <typename TTag, typename T>
T const& prev(T const& obj) {
    return node_cast<TTag, T>(obj).next();
}

template <typename TTag, typename T>
void remove(T& obj) {
    node_cast<TTag, T>(obj).remove();
}

template <typename TTag, typename T>
void insert(T& prev, T& next) {
    node_cast<TTag, T>(next).insert(&prev);
}

template <typename Tag, typename T>
class intrusive_list {
public:
    typedef intr_list_node<Tag, T> node;
    typedef node* node_ptr;
    typedef intrusive_list_iterator<Tag, T> iterator;
    typedef intrusive_list_const_iterator<Tag, T> const_iterator;

private:
    intr_list_node<Tag, T> root;

public:
    intrusive_list()
        : root(&root, &root) {}

    ~intrusive_list() noexcept = default;

    intrusive_list(intrusive_list const&) = delete;

    intrusive_list& operator=(intrusive_list const&) = delete;

    intrusive_list& operator=(intrusive_list&& list) noexcept {
        swap(list);
        return *this;
    }

    intrusive_list(intrusive_list&& list) noexcept {
        swap(list);
    }

    void swap(intrusive_list& list) {
        std::swap(root, list.root);
    }

    void push_front(intr_list_node<Tag, T>& obj) noexcept {
        obj.insert(&root);
    }

    void push_back(intr_list_node<Tag, T>& obj) noexcept {
        obj.insert(root.prev_node);
    }

    void insert(iterator pos, intr_list_node<Tag, T>& obj) {
        ::insert<Tag>(*pos, obj);
    }

    iterator begin() noexcept {
        return iterator(root.next_node);
    }

    iterator end() noexcept {
        return iterator(&root);
    }

    const_iterator begin() const noexcept {
        return const_iterator(root.next_node);
    }

    const_iterator end() const noexcept {
        return const_iterator(&root);
    }
};

#endif // INTRUSIVE_LIST_H

