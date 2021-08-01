#pragma once

#include "ft.hpp"

enum    Compare
{
    EQUAL,
    LESS,
    GREATER
};

template <    class _Key,
            class _T,
            class _Compare,
            class _Allocator >
class ft::Map
{
    public:
/*    MEMBER TYPES    */
        typedef _Key                        key_type;
        typedef _T                            mapped_type;
        typedef _Compare                    key_compare;
        typedef _Allocator                    allocator_type;
        typedef ft::pair<const _Key, _T>    value_type;
        typedef value_type*                    pointer;
        typedef const value_type*            const_pointer;
        typedef value_type&                    reference;
        typedef const value_type&            const_reference;
        typedef std::ptrdiff_t                difference_type;

    private:
        struct    node    /*    узел дерева    */
        {
            value_type    value;        /*    пара ключ - значение                */
            size_t        height;        /*    высота поддерева                    */
            node        *left;        /*    указатель на левое поддерево        */
            node        *right;        /*    указатель на правое поддерево        */
            node        *parent;    /*    указатель на родителя                */
            bool        isBegin;    /*     пустой узел перед минимальным        */
            bool        isEnd;        /*     пустой узел после максимального    */
        /*    конструкторы нового узла    */
            node()
                : value(ft::make_pair(key_type(), mapped_type())),
                height(0),
                left(nullptr), right(nullptr), parent(nullptr),
                isBegin(1), isEnd(1) {}
            node(const_reference val)
                : value(ft::make_pair(val.first, val.second)),
                height(1),
                left(nullptr), right(nullptr), parent(nullptr),
                isBegin(0), isEnd(0) {}
        };
/*    VARIABLES    */
        node            *_tree;
        size_t            _treeSize;
        allocator_type    _alloc;
        node            *_beginNode;
        node            *_endNode;
        key_compare        _cmp;
/*    SECONDARY FUNCTIONS    */
        int        cmp(const key_type &first, const key_type &second) const /*    сравнение ключей    */
        {
            return _cmp(first, second) + _cmp(second, first) * 2;
        }

        size_t    height(node *p)    /*    получение высоты поддерева    */
        {
            return p ? p->height : 0;
        }

        int        b_factor(node *p)    /*    balance factor        (-1, 0 или 1) - поддерево сбалансированно    */
        {
            return height(p->right) - height(p->left);    /*    (-2 или 2) - требуется балансировка левого или правого поддерева , соответственно    */
        }

        void    fix_height(node *p)    /*    корректировка высоты поддерева после вставки или удаления узла    */
        {
            size_t    hl = height(p->left);
            size_t    hr = height(p->right);
            p->height = (hl > hr ? hl : hr) + 1;
        }

    /*    повороты вокруг узла, balance factor корого == 2 или == -2,
    т.е. возникает расбалансировка одного из поддеревьев    */

        node*    rotate_right(node *p)    /*    правый поворот вокруг узла 'p'    */
        {
            node*    q = p->left;    /*    узел, который после поворота займёт место 'p' и станет корнем данного поддерева    */
        /*    поворот    */
            p->left = q->right;
            q->right = p;
        /*    меняем местами родителей повёрнутых узлов    */
            q->parent = p->parent;
            p->parent = q;
            if (q->right->left)
                q->right->left->parent = p;
        /*    корректировка высот новых поддеревьев    */
            fix_height(p);
            fix_height(q);
            return q;    /*    возвращаем новый корень данного поддерева    */
        }

        node*    rotate_left(node *q)    /*    левый поворот вокруг узла 'q'    */
        {
            node*    p = q->right;    /*    узел, который после поворота займёт место 'q' и станет корнем данного поддерева    */
        /*    поворот    */
            q->right = p->left;
            p->left = q;
        /*    меняем местами родителей повёрнутых узлов    */
            p->parent = q->parent;
            q->parent = p;
            if (p->left->right)
                p->left->right->parent = q;
        /*    корректировка высот новых поддеревьев    */
            fix_height(q);
            fix_height(p);
            return p;    /*    возвращаем новый корень данного поддерева    */
        }

        node*    balance(node *p)    /*    балансировка узла 'p'    */
        {
            node    *tmp = p->parent;    /*    запоминаем родителя данного узла    */
            fix_height(p);    /*    корректировка высоты данного поддерева    */
            if (b_factor(p) == 2)    /*    правое поддерево от 'p' выше левого    */
            {
                if (b_factor(p->right) < 0)    /*    требуется поворот правого поддерева от 'p'    */
                    p->right = rotate_right(p->right);
                if (!tmp)    /*    т.е. 'p' - корень всего дерева    */
                    return _tree = rotate_left(p);    /*    балансировка корня всего дерева    */
            /*    балансировка поддерева с корнем 'p'    */
                if (tmp->left == p)
                    return (tmp->left = rotate_left(p));
                else
                    return (tmp->right = rotate_left(p));
            }
            if (b_factor(p) == -2)    /*    левое поддерево от 'p' выше правого    */
            {
                if (b_factor(p->left) > 0)    /*    требуется поворот левого поддерева от 'p'    */
                    p->left = rotate_left(p->left);
                if (!tmp)    /*    т.е. 'p' - корень всего дерева    */
                    return _tree = rotate_right(p);    /*    балансировка корня всего дерева    */
            /*    балансировка поддерева с корнем 'p'    */
                if (tmp->right == p)
                    return (tmp->right = rotate_right(p));
                else
                    return (tmp->left = rotate_right(p));
            }
            return p;    /*    балансировка не потребовалась    */
        }

        node*    find_min(node *p)    /*    поиск узла с минимальным ключом в дереве 'p'    */
        {
            return p->left ? find_min(p->left) : p;
        }

        node*    find_max(node *p)    /*    поиск узла с максимальным ключом в дереве 'p'    */
        {
            return p->right ? find_max(p->right) : p;
        }

        node*    remove_min(node *p)    /*    удаление узла с минимальным ключом из дерева p    */
        {
            if (p->left == 0)    /*    найден минимальный элемент    */
                return p->right;    /*    замена минимального элемента на его правое поддерево    */
            p->left = remove_min(p->left);    /*    рекурсивный поиск минимального элемента    */
            makeBalance(p);    /*    балансировка ветви от 'p' до корня    */
            return p;
        }

        node*    remove(node *p, int k)    /*    удаление узла с ключом 'k' из дерева с корнем 'p'    */
        {
            int        compare = cmp(k, p->value->first);
            if (!p)    /*    если узла с данным ключом 'k' не существует удалять нечего    */
                return 0;
        /*    рекурсивно ищем узел с искомым ключом 'k'    */
            if (compare == LESS)
                p->left = remove(p->left, k);
            else if (compare == GREATER)
                p->right = remove(p->right, k);
            else    /*    k == p->key - найден узел 'p' с искомым ключом 'k'    */
            {
                node *q = p->left;    /*    запоминаем корень левого поддерева узла 'p'    */
                node *r = p->right;    /*    запоминаем корень правого поддерева узла 'p'    */
                delete p;    /*    удаляем узел 'p'    */
                if (!r)    /*    если правое поддерево пустое, то возвращаем указатель на левое поддерево    */
                    return q;    /*    слеав от 'p' может быть один дочерний узел (height == 1), либо узел 'p' - лист, т.е. замена не требуется    */
                node *min = find_min(r);    /*    если правое поддерево не пустое, то находим там элемент 'min' и ставим его на место 'p'    */
                min->right = remove_min(r);    /*    справа к 'min' подвешиваем 'r'    */
                min->left = q;    /*    слева к 'min' подвешиваем 'q'    */
                return balance(min);    /*    возвращаем сбалансированный 'min'    */
            }
            return balance(p);    /*    рекурсивная балансировка всех пройденных узлов    */
        }

        node*    find_node(node* p, int k) const    /*    поиск узла по ключу    */
        {
            int        compare = cmp(k, p->value.first);
            if (compare == LESS)
            {
                if (p->left && p->left != _beginNode)
                    return find_node(p->left, k);
            }
            else if (compare == GREATER)
            {
                if (p->right && p->right != _endNode)
                    return find_node(p->right, k);
            }
            return p;    /*    если узел с данным ключом отсутствует, то вернётся узел, ближайший по ключу    */
        }

        void    delete_tree(node *p)    /*    очистка дерева    */
        {
            if (p->left != nullptr)
            {
                // std::cout << p->left->value.second << " <- " << p->value.second << std::endl;
                delete_tree(p->left);
            }
            if (p->right != nullptr)
            {
                // std::cout << p->value.second << " -> " << p->right->value.second << std::endl;
                delete_tree(p->right);
            }
            // std::cout << p->value.second << std::endl;
            delete p;
        }

        void    makeBalance(node *node)    /*    балансировка от node до корня    */
        {
            while (node)
            {
                node = balance(node);
                node = node->parent;
            }
        }

    public:
/*    ITERATORS    */
        class iterator;
        class const_iterator;
        class reverse_iterator;
        class const_reverse_iterator;
    /*    Iterator    */
        class    iterator : public ft::iterator_traits<std::bidirectional_iterator_tag, value_type>
        {
            private:
                node    *_node;

                node*    find_min(node *p) { return p->left ? find_min(p->left) : p; }
                node*    find_max(node *p) { return p->right ? find_max(p->right) : p; }

            public:
                iterator() : _node(nullptr) {}
                iterator(node *ptr) : _node(ptr) {}
                ~iterator() {}
                iterator(const iterator &other) { *this = other; }

                node*            getNode() const { return _node; }

                iterator&        operator = (const iterator &other)
                {
                    _node = other._node;
                    return *this;
                }

                bool    operator == (const iterator &other) { return this->_node == other._node; }
                bool    operator != (const iterator &other) { return this->_node != other._node; }
                bool    operator == (const const_iterator &other) { return this->_node == other.getNode(); }
                bool    operator != (const const_iterator &other) { return this->_node != other.getNode(); }

                value_type&        operator * () { return _node->value; }

                iterator&        operator ++ ()
                {
                    if (_node->isBegin)
                        _node = _node->parent;
                    else if (_node->right)
                        _node = find_min(_node->right);
                    else
                    {
                        node    *tmp = _node->parent;
                        while (tmp && tmp->value.first < _node->value.first)
                            tmp = tmp->parent;
                        _node = tmp;
                    }
                    return *this;
                }

                iterator&        operator -- ()
                {
                    if (_node->isEnd)
                        _node = _node->parent;
                    else if (_node->left)
                        _node = find_max(_node->left);
                    else
                    {
                        node    *tmp = _node->parent;
                        while (tmp && tmp->value.first > _node->value.first)
                            tmp = tmp->parent;
                        _node = tmp;
                    }
                    return *this;
                }

                iterator        operator ++ (int)
                {
                    iterator    tmp(_node);
                    operator++();
                    return tmp;
                }

                iterator        operator -- (int)
                {
                    iterator    tmp(_node);
                    operator--();
                    return tmp;
                }
        };
    /*    Const Iterator    */
        class    const_iterator : public ft::iterator_traits<std::bidirectional_iterator_tag, value_type>
        {
            private:
                node    *_node;

                node*    find_min(node *p) { return p->left ? find_min(p->left) : p; }
                node*    find_max(node *p) { return p->right ? find_max(p->right) : p; }

            public:
                const_iterator() : _node(nullptr) {}
                const_iterator(node *ptr) : _node(ptr) {}
                ~const_iterator() {}
                const_iterator(const const_iterator &other) { *this = other; }
                const_iterator(const iterator &other) { *this = other; }

                node*            getNode() const { return _node; }

                const_iterator&        operator = (const const_iterator &other)
                {
                    _node = other._node;
                    return *this;
                }

                const_iterator&        operator = (const iterator &other)
                {
                    _node = other.getNode();
                    return *this;
                }

                bool    operator == (const const_iterator &other) { return this->_node == other._node; }
                bool    operator != (const const_iterator &other) { return this->_node != other._node; }
                bool    operator == (const iterator &other) { return this->_node == other.getNode(); }
                bool    operator != (const iterator &other) { return this->_node != other.getNode(); }

                const value_type&        operator * () { return _node->value; }

                const_iterator&        operator ++ ()
                {
                    if (_node->isBegin)
                        _node = _node->parent;
                    else if (_node->right)
                        _node = find_min(_node->right);
                    else
                    {
                        node    *tmp = _node->parent;
                        while (tmp && tmp->value.first < _node->value.first)
                            tmp = tmp->parent;
                        _node = tmp;
                    }
                    return *this;
                }

                const_iterator&        operator -- ()
                {
                    if (_node->isEnd)
                        _node = _node->parent;
                    else if (_node->left)
                        _node = find_max(_node->left);
                    else
                    {
                        node    *tmp = _node->parent;
                        while (tmp && tmp->value.first > _node->value.first)
                            tmp = tmp->parent;
                        _node = tmp;
                    }
                    return *this;
                }

                const_iterator        operator ++ (int)
                {
                    iterator    tmp(_node);
                    operator++();
                    return tmp;
                }

                const_iterator        operator -- (int)
                {
                    iterator    tmp(_node);
                    operator--();
                    return tmp;
                }
        };
    /*    Reverse Iterator */
        class    reverse_iterator : public ft::reverse_iterator<iterator>
        {
            private:
                node    *_node;

                node*    find_min(node *p) { return p->left ? find_min(p->left) : p; }
                node*    find_max(node *p) { return p->right ? find_max(p->right) : p; }

            public:
                reverse_iterator() : _node(nullptr) {}
                reverse_iterator(node *ptr) : _node(ptr) {}
                ~reverse_iterator() {}
                reverse_iterator(const reverse_iterator &other) { *this = other; }

                node*            getNode() const { return _node; }

                reverse_iterator&        operator = (const reverse_iterator &other)
                {
                    _node = other._node;
                    return *this;
                }

                bool    operator == (const reverse_iterator &other) { return this->_node == other._node; }
                bool    operator != (const reverse_iterator &other) { return this->_node != other._node; }
                bool    operator == (const const_reverse_iterator &other) { return this->_node == other.getNode(); }
                bool    operator != (const const_reverse_iterator &other) { return this->_node != other.getNode(); }

                value_type&        operator * () { return _node->value; }

                reverse_iterator&        operator ++ ()
                {
                    if (_node->isEnd)
                        _node = _node->parent;
                    else if (_node->left)
                        _node = find_max(_node->left);
                    else
                    {
                        node    *tmp = _node->parent;
                        while (tmp && tmp->value.first > _node->value.first)
                            tmp = tmp->parent;
                        _node = tmp;
                    }
                    return *this;
                }

                reverse_iterator&        operator -- ()
                {
                    if (_node->isBegin)
                        _node = _node->parent;
                    else if (_node->right)
                        _node = find_min(_node->right);
                    else
                    {
                        node    *tmp = _node->parent;
                        while (tmp && tmp->value.first < _node->value.first)
                            tmp = tmp->parent;
                        _node = tmp;
                    }
                    return *this;
                }

                reverse_iterator        operator ++ (int)
                {
                    iterator    tmp(_node);
                    operator--();
                    return tmp;;
                }

                reverse_iterator        operator -- (int)
                {
                    iterator    tmp(_node);
                    operator++();
                    return tmp;
                }
        };
    /*    Const Reverse Iterator    */
        class    const_reverse_iterator : public ft::reverse_iterator<const_iterator>
        {
            private:
                node    *_node;

                node*    find_min(node *p) { return p->left ? find_min(p->left) : p; }
                node*    find_max(node *p) { return p->right ? find_max(p->right) : p; }

            public:
                const_reverse_iterator() : _node(nullptr) {}
                const_reverse_iterator(node *ptr) : _node(ptr) {}
                ~const_reverse_iterator() {}
                const_reverse_iterator(const const_reverse_iterator &other) { *this = other; }
                const_reverse_iterator(const reverse_iterator &other) { *this = other; }

                node*            getNode() const { return _node; }

                const_reverse_iterator&        operator = (const const_reverse_iterator &other)
                {
                    _node = other._node;
                    return *this;
                }

                const_reverse_iterator&        operator = (const reverse_iterator &other)
                {
                    _node = other.getNode();
                    return *this;
                }

                bool    operator == (const const_reverse_iterator &other) { return this->_node == other._node; }
                bool    operator != (const const_reverse_iterator &other) { return this->_node != other._node; }
                bool    operator == (const reverse_iterator &other) { return this->_node == other.getNode(); }
                bool    operator != (const reverse_iterator &other) { return this->_node != other.getNode(); }

                const value_type&        operator * () { return _node->value; }

                const_reverse_iterator&        operator ++ ()
                {
                    if (_node->isEnd)
                        _node = _node->parent;
                    else if (_node->left)
                        _node = find_max(_node->left);
                    else
                    {
                        node    *tmp = _node->parent;
                        while (tmp && tmp->value.first > _node->value.first)
                            tmp = tmp->parent;
                        _node = tmp;
                    }
                    return *this;
                }

                const_reverse_iterator&        operator -- ()
                {
                    if (_node->isBegin)
                        _node = _node->parent;
                    else if (_node->right)
                        _node = find_min(_node->right);
                    else
                    {
                        node    *tmp = _node->parent;
                        while (tmp && tmp->value.first < _node->value.first)
                            tmp = tmp->parent;
                        _node = tmp;
                    }
                    return *this;
                }

                const_reverse_iterator        operator ++ (int)
                {
                    iterator    tmp(_node);
                    operator--();
                    return tmp;
                }

                const_reverse_iterator        operator -- (int)
                {
                    iterator    tmp(_node);
                    operator++();
                    return tmp;
                }
        };
/*    CONSTRUCTORS    */
        Map() : _tree(new node()), _treeSize(0) { _beginNode = _endNode = _tree; }

        template<class _InputIt>
        Map(_InputIt first, _InputIt last) : _tree(new node()), _treeSize(0)
        {
            _beginNode = _endNode = _tree;
            insert(first, last);
        }

        Map(const Map &other) { *this = other; }
/*    DESTRUCTOR    */
        ~Map() { clear(); }
/*    ASSIGNATION OPERATOR OVERLOAD    */
        Map&    operator = (const Map &other)
        {
            if (this != &other)
            {
                clear();
                insert(other.begin(), other.end());
            }
            return *this;
        }
/*    MEMBER FUNCTIONS    */
        iterator                begin() { return _treeSize ? ++iterator(_beginNode) : iterator(_beginNode); }
        iterator                end() { return iterator(_endNode); }
        const_iterator            begin() const { return _treeSize ? ++const_iterator(_beginNode) : const_iterator(_beginNode); }
        const_iterator            end() const { return const_iterator(_endNode); }

        const_iterator            cbegin() const { return _treeSize ? ++const_iterator(_beginNode) : const_iterator(_beginNode); }
        const_iterator            cend() const { return const_iterator(_endNode); }

        reverse_iterator        rbegin() { return --reverse_iterator(_endNode); }
        reverse_iterator        rend() { return reverse_iterator(_beginNode); }
        const_reverse_iterator    rbegin() const { return --const_reverse_iterator(_endNode); }
        const_reverse_iterator    rend() const { return const_reverse_iterator(_beginNode); }

        const_reverse_iterator    crbegin() const { return --const_reverse_iterator(_endNode); }
        const_reverse_iterator    crend() const { return const_reverse_iterator(_beginNode); }

        size_t                    size() const { return _treeSize; }
        size_t                    max_size() const { return _alloc.max_size() / 2; }
        bool                    empty() const { return _treeSize ? false : true; }
        const mapped_type&        at(const key_type& key) const { return at(key); }

        mapped_type&        at(const key_type& key)
        {
            node *res = find_node(_tree, key);
            if (cmp(key, res->value.first) == EQUAL)
                return res->value.second;
            else
                throw std::out_of_range("map::at:  key not found");
        }

        mapped_type&    operator [] (const key_type key)
        {
            ft::pair<iterator, bool>    res = insert(ft::make_pair(key, mapped_type()));
            return (*res.first).second;
        }

        size_t    count(const key_type& key) const
        {
            node    *res = find_node(_tree, key);
            return cmp(key, res->value.first) == EQUAL ? 1 : 0;
        }

        void    clear()
        {
            delete_tree(_tree);
            _treeSize = 0;
        }

        iterator        find(const key_type& key)
        {
            node    *res = find_node(_tree, key);
            return cmp(res->value.first, key) == EQUAL ? iterator(res) : end();
        }

        const_iterator    find(const key_type& key) const
        {
            node    *res = find_node(_tree, key);
            return cmp(res->value.first, key) == EQUAL ? const_iterator(res) : cend();
        }

        iterator        lower_bound(const key_type& key)
        {
            iterator it(find_node(_tree, key));
            if (cmp((*it).first, key) == LESS)
                ++it;
            return it;
        }

        const_iterator    lower_bound(const key_type& key) const
        {
            const_iterator it(find_node(_tree, key));
            if (cmp((*it).first, key) == LESS)
                ++it;
            return it;
        }

        iterator    upper_bound(const key_type& key)
        {
            iterator    it(find_node(_tree, key));
            int            compare = cmp((*it).first, key);
            if (compare == LESS || compare == EQUAL)
                ++it;
            return it;
        }

        const_iterator    upper_bound(const key_type& key) const
        {
            const_iterator    it(find_node(_tree, key));
            int                compare = cmp((*it).first, key);
            if (compare == LESS || compare == EQUAL)
                ++it;
            return it;
        }

        ft::pair<iterator, iterator>                equal_range(const key_type& key)
        {
            return ft::make_pair(iterator(lower_bound(key)), iterator(upper_bound(key)));
        }

        ft::pair<const_iterator, const_iterator>    equal_range(const key_type& key) const
        {
            return ft::make_pair(const_iterator(lower_bound(key)), const_iterator(upper_bound(key)));
        }

        ft::pair<iterator, bool>    insert(const value_type &value)
        {
            node    *newNode = find_node(_tree, value.first);    /*    ищем место для добавления элемента    */
            if (_treeSize && cmp(value.first, newNode->value.first) == EQUAL)    /*    если элемент уже существует    */
                return ft::make_pair(iterator(newNode), false);
            if (!_treeSize)    /*    вставка самого первого элемента    */
            {
                newNode = new node (value);    /*    новый узел для вставки (корень всего дерева)    */
                newNode->right = _tree;    /*    указатель на .end()    */
                newNode->left = new node();    /*    указатель на .rend()    */
                newNode->right->isBegin = 0;
                newNode->left->isEnd = 0;
                newNode->left->parent = newNode->right->parent = newNode;
                _beginNode = newNode->left;    /*    указатель на начало депева (элемент перед первым)    */
                _tree = newNode;    /*     новый корень всего дерева    */
            }
            else
            {    /*    newNode - родитель нового элемента!    */
                if (cmp(value.first, newNode->value.first) == LESS)    /*    вставка слева    */
                {
                    if (newNode->left)    /*    если newNode - миинимальный элемент дерева    */
                    {
                        node    *tmp = new node(value);    /*    новый узел для вставки (минимальный элемент)    */
                        tmp->left = newNode->left;    /*    указатель на .rend()    */
                        tmp->left->parent = tmp;
                        tmp->parent = newNode;
                        newNode->left = tmp;
                    }
                    else    /*    обычная вставка слева    */
                    {
                        newNode->left = new node(value);
                        newNode->left->parent = newNode;
                    }
                    newNode = newNode->left;    /*    newNode - добавленный узел    */
                }
                else if (cmp(value.first, newNode->value.first) == GREATER)    /*    вставка справа    */
                {
                    if (newNode->right)    /*    если newNode - максимальный элемент дерева    */
                    {
                        node    *tmp = new node(value);    /*    новый узел для вставки (максимальный элемент)    */
                        tmp->right = newNode->right;    /*    указатель на .end()    */
                        tmp->right->parent = tmp;
                        tmp->parent = newNode;
                        newNode->right = tmp;
                    }
                    else    /*    обычная вставка справа    */
                    {
                        newNode->right = new node(value);
                        newNode->right->parent = newNode;
                    }
                    newNode = newNode->right;    /*    newNode - добавленный узел    */
                }
                makeBalance(newNode);    /*    балансировка ветви от newNode до корня    */
            }
            _treeSize++;
            return ft::make_pair(iterator(newNode), true);
        }

        void    erase(iterator pos)
        {
            node    *rm = pos.getNode();
            node    *l = rm->left;    /*    запоминаем корень левого поддерева узла 'p'     */
            node    *r = rm->right;    /*    запоминаем корень правого поддерева узла 'p'    */
            node    *parent = rm->parent;
            if (rm == _beginNode->parent)
            {
                if (_treeSize == 1)
                {
                    clear();
                    _beginNode = _endNode = _tree = new node();
                    return ;
                }
                if (r)
                {
                    node    *newBegin = find_min(r);
                    if (parent)
                        parent->left = r;
                    else
                        _tree = newBegin;
                    r->parent = parent;
                    newBegin->left = _beginNode;
                    _beginNode->parent = newBegin;
                }
                else
                {
                    parent->left = _beginNode;
                    _beginNode->parent = parent;
                }
                if (parent)
                    makeBalance(parent);
                delete rm;
                _treeSize--;
                return ;
            }
            if (rm == _endNode->parent)
            {
                if (l)
                {
                    node    *newEnd = find_max(l);
                    if (parent)
                        parent->right = l;
                    else
                        _tree = newEnd;
                    l->parent = parent;
                    newEnd->right = _endNode;
                    _endNode->parent = newEnd;
                }
                else
                {
                    _endNode->parent = parent;
                    parent->right = _endNode;
                }
                if (parent)
                    makeBalance(parent);
                delete rm;
                _treeSize--;
                return ;
            }
            if (l)
            {
                node    *replacement = find_max(l);
                if (replacement->parent == rm)
                {
                    replacement->parent = parent;
                    replacement->right = r;
                    if (r)
                        r->parent = replacement;
/*    !!!    */            if (parent)
                    {
                        if (parent->left == rm)
                            parent->left = replacement;
                        else
                            parent->right = replacement;
                    }
                    else
                        _tree = replacement;
                    makeBalance(replacement);
                    delete rm;
                    _treeSize--;
                    return ;
                }
                replacement->parent->right = replacement->left;
                if (replacement->left)
                    replacement->left->parent = replacement->parent;
                replacement->left = l;
                l->parent = replacement;
                replacement->right = r;
                if (r)
                    r->parent = replacement;
                replacement->parent = parent;
/*    !!!    */        if (parent)
                {
                    if (parent->left == rm)
                        parent->left = replacement;
                    else
                        parent->right = replacement;
                }
                else
                    _tree = replacement;
                makeBalance(replacement);
            }
/*    !!!    */    else
            {
                if (parent->left == rm)
                {
                    if ((parent->left = r))
                        parent->left->parent = parent;
                }
                else if (parent->right == rm)
                {
                    if ((parent->right = r))
                        parent->right->parent = parent;
                }
                makeBalance(parent);
            }
            delete rm;
            _treeSize--;
            return ;
        }

        template< class _InputIt >
        void insert(_InputIt first, _InputIt last)
        {
            for ( ; first != last; ++first)
                insert(ft::make_pair((*first).first, (*first).second));
        }

        void    erase(iterator first, iterator last)
        {
            while (first != last)
                erase(first++);
        }
};
