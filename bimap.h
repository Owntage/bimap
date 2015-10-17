#ifndef BIMAP_H
#define BIMAP_H

#include <memory>
#include <iterator>
#include <utility>
#include <functional>

#include<vector>

namespace my
{
    /*
     * в халф ноде не нужно выделять память, т.к. она выделяется при создании целой ноды
     * соответственно функция вставки должва только подшить халф-ноду в нужное место
     * а как быть с удалением?
     */
    template<typename T>
    struct bimap_half_node
    {
        typedef bimap_half_node<T> self_t;
        T data;
        self_t* left;
        self_t* right;
        self_t* parent;
        void* node_ptr;

        bimap_half_node(void* node_ptr)
        {
            left = nullptr;
            right = nullptr;
            parent = nullptr;
            this->node_ptr = node_ptr;
        }



        void apply_forward(std::function<void(self_t*)> func)
        {
            if(left)
            {
                left->apply_forward(func);
            }
            func(this);
            if(right)
            {
                right->apply_forward(func);
            }
        }

        void insert(self_t* ptr)
        {
            if(ptr->data < data)
            {
                if(left)
                {
                    return left->insert(ptr);
                }
                else
                {
                    left = ptr;
                    ptr->parent = this;
                }
            }
            if(ptr->data > data)
            {
                if(right)
                {
                    return right->insert(ptr);
                }
                else
                {
                    right = ptr;
                    ptr->parent = this;
                }
            }
        }

        void insert_left(self_t* ptr)
        {
            if(left)
            {
                left->insert(ptr);
            }
            else
            {
                left = ptr;
                ptr->parent = this;
            }
        }

        self_t* find(const T& value)
        {
            if(value < data)
            {
                if(left)
                {
                    return left->find(value);
                }
                else
                {
                    return left;
                }
            }
            if(value > data)
            {
                if(right)
                {
                    return right->find(value);
                }
                else
                {
                    return right;
                }
            }
            //value == data
            return this;
        }



        self_t* min()
        {
            if(left)
            {
                return left->min();
            }
            else
            {
                return this;
            }
        }

        self_t* max()
        {
            if(right)
            {
                return right->max();
            }
            else
            {
                return this;
            }
        }

        bool is_left_child()
        {
            if(parent->left == this)
            {
                return true;
            }
            return false;
        }

        self_t*& get_parent_ptr()
        {
            if(is_left_child())
            {
                return parent->left;
            }
            return parent->right;
        }

        self_t* erase(const T& value)
        {
            std::cout << "called half_node erase" << std::endl;
            if(value < data)
            {
                if(left)
                {
                    return left->erase(value);
                }
                else
                {
                    return left;
                }
            }
            if(value > data)
            {
                if(right)
                {
                    return right->erase(value);
                }
                else
                {
                    return right;
                }
            }
            //value == data
            std::cout << "erasing " << data << std::endl;
            if(left && right)
            {
                std::cout << "first type erase" << std::endl;
                self_t* min_ptr = right->min();
                if(min_ptr == right)
                {
                    get_parent_ptr() = min_ptr;
                    min_ptr->parent = parent;
                    min_ptr->left = left;
                    if(left)
                    {
                        left->parent = min_ptr;
                    }
                }
                else
                {
                    min_ptr->get_parent_ptr() = min_ptr->right;
                    if(min_ptr->right)
                    {
                        min_ptr->right->parent = min_ptr->parent;
                    }

                    min_ptr->parent = parent;
                    get_parent_ptr() = min_ptr;
                    min_ptr->left = left;
                    if(left)
                    {
                        left->parent = min_ptr;
                    }
                    min_ptr->right = right;
                    if(right)
                    {
                        right->parent = min_ptr;
                    }
                }

                return this;
            }
            //todo fix it
            if(left)
            {
                get_parent_ptr() = left;
                left->parent = parent;
            }
            else
            {
                get_parent_ptr() = right;
                if(right)
                {
                    right->parent = parent;
                }
            }
            return this;
        }

        bool check_invariant()
        {
            if(left && left->parent != this || right && right->parent != this)
            {
                std::cout << "parent fail" << std::endl;
                return false;
            }
            return true;
        }

        bool check_tree()
        {
            bool result = true;
            if(left && left->data > data) result = false;
            if(right && right->data < data) result = false;
            if(left)
            {
                result = result && left->check_tree();
            }
            if(right)
            {
                result = result && right->check_tree();
            }
            return result;
        }

    };

    template<bool cond, typename first, typename second>
    struct conditional
    {
        typedef first type;
    };
    template<typename first, typename second>
    struct conditional<false, first, second>
    {
        typedef second type;
    };

    template<typename T, typename P>
    struct bimap_node
    {
        typedef bimap_node<T, P> self_t;
        bimap_half_node<T> left;
        bimap_half_node<P> right;
        bimap_node() : left(static_cast<void*>(this)), right(static_cast<void*>(this))
        {
        }
        bimap_node(const self_t& other)
        {
            copy(other);
        }
        self_t& operator=(const self_t& other)
        {
            copy(other);
            return *this;
        }


        /*
        static self_t* get_node(bimap_half_node<T>* half_node)
        {
            return static_cast<self_t*>(half_node->node_ptr);
        }

        static self_t* get_node(bimap_half_node<P>* half_node)
        {
            return static_cast<self_t*>(half_node->node_ptr);
        }
         */



        void copy(const self_t& other)
        {
            std::function<void(bimap_half_node<T>*)> func = [this](bimap_half_node<T>* ptr)
            {
                bimap_node* n = this->get_node(ptr);
                bimap_node* new_n = new bimap_node;
                new_n->left->data = n->left->data;
                new_n->right->data = n->right->data;
                this->left.insert(&new_n->left);
                this->right.insert(&new_n->right);
            };
            other.left->apply_forward(func);
        }

        bool check_invariant()
        {
            if(!left.check_invariant())
            {
                std::cout << "left invariant failed" << std::endl;
                return false;
            }
            if(!right.check_invariant())
            {
                std::cout << "right invariant failed" << std::endl;
                return false;
            }
            return true;
        }



    };

    template<typename T, typename P, bool is_left>
    struct node_traits
    {
        static bimap_half_node<T>* get_half_node(bimap_node<T, P>* node)
        {
            return &node->left;
        }
    };
    template<typename T, typename P>
    struct node_traits<T, P, false>
    {
        static bimap_half_node<P>* get_half_node(bimap_node<T, P>* node)
        {
            return &node->right;
        }
    };

    template<typename T, typename G, bool is_left>
    struct bimap_iterator
    {
        typedef typename conditional<is_left, T, G>::type value_type;
        typedef typename conditional<is_left, bimap_half_node<T>, bimap_half_node<G>>::type half_node_type;
        typedef bimap_node<T, G> node_type;
        typedef bimap_iterator<T, G, is_left> self_t;

        bool end;
        half_node_type* half_node;
        half_node_type* root;

        bimap_iterator()
        {
            end = false;
        }
        bimap_iterator(const self_t& other)
        {
            half_node = other.half_node;
            root = other.root;
            end = other.end;
        }
        self_t& operator=(const self_t& other)
        {
            half_node = other.half_node;
            root = other.root;
            end = other.end;
            return *this;
        }

        self_t& operator++()
        {
            half_node_type* current = root->left;
            half_node_type* result = nullptr;
            while(current)
            {
                if(current->data > half_node->data)
                {
                    result = current;
                    current = current->left;
                }
                else
                {
                    current = current->right;
                }
            }
            if(result)
            {
                if(result == root)
                {
                    end = true;
                }
                half_node = result;
            }
            else
            {
                end = true;
            }
            return *this;
        }
        bimap_iterator operator++(int)
        {
            self_t result = *this;
            ++(*this);
            return result;
        }
        bimap_iterator& operator--()
        {
            half_node_type* current = root;
            half_node_type* result = nullptr;
            while(current)
            {
                if(current->data < half_node->data)
                {
                    result = current;
                    current = current->right;
                }
                else
                {
                    current = current->left;
                }
            }
            if(result)
            {
                half_node = result;
            }
            else
            {
                //throw something;
            }
            return *this;
        }
        bimap_iterator operator--(int)
        {
            self_t result = *this;
            --(*this);
            return result;
        }

        value_type const& operator*() const
        {
            return half_node->data;
        }
        value_type const* operator->() const
        {
            return &half_node->data;
        }

        bool operator==(const self_t& other)
        {
            return (end == other.end);
        }

        bool operator!=(const self_t& other)
        {
            return !this->operator==(other);
        }

        bool is_end()
        {
            return end;
        }

        bimap_iterator<T, G, !is_left> to_other_type()
        {
            bimap_iterator<T, G, !is_left> res;
            res.end = end;
            node_type* temp_node = (node_type*) half_node->node_ptr;
            node_type* temp_root = (node_type*) root->node_ptr;
            res.half_node = node_traits<T, G, !is_left>::get_half_node(temp_node);
            res.root = node_traits<T, G, !is_left>::get_half_node(temp_root);
            return res;
        }
    };

    template<typename KEY_T, typename VALUE_T>
    struct bimap
    {
        typedef bimap<KEY_T, VALUE_T> self_t;
        typedef bimap_iterator<KEY_T, VALUE_T, true> left_iterator;
        typedef bimap_iterator<KEY_T, VALUE_T, false> right_iterator;
        typedef bimap_node<KEY_T, VALUE_T> node_t;
        int size;
        bimap()
        {
            size = 0;
        }

        bimap(const self_t& other)
        {
            copy(other);
        }

        self_t& operator=(const self_t& other)
        {
            copy(other);
            return *this;
        }

        void copy(const self_t& other)
        {
            fake_root = other.fake_root;
            size = other.size();
        }

        ~bimap()
        {
            std::vector<KEY_T> keys;
            for(auto it = l_begin(); it != l_end(); it++)
            {
                keys.push_back(*it);
            }
            for(int i = 0; i < keys.size(); i++)
            {
                l_erase(keys[i]);
            }
        }

        std::pair<left_iterator, right_iterator> insert(KEY_T key, VALUE_T value)
        {
            left_iterator left_res;
            right_iterator right_res;
            left_res.end = false;
            right_res.end = false;
            node_t* new_node = new node_t;
            new_node->left.data = key;
            new_node->right.data = value;

            fake_root.left.insert_left(&new_node->left);
            fake_root.right.insert_left(&new_node->right);
            left_res.root = &fake_root.left;
            right_res.root = &fake_root.right;
            left_res.half_node = &new_node->left;
            right_res.half_node = &new_node->right;
            size++;
            return std::make_pair(left_res, right_res);
        }

        template<bool is_left>
        bimap_iterator<KEY_T, VALUE_T, is_left> end()
        {
            bimap_iterator<KEY_T, VALUE_T, is_left> result;
            result.end = true;
            result.root = node_traits<KEY_T, VALUE_T, is_left>::get_half_node(&fake_root);
            result.half_node = node_traits<KEY_T, VALUE_T, is_left>::get_half_node(&fake_root);
            return result;

        };

        left_iterator l_end()
        {
            return end<true>();
        }

        right_iterator r_end()
        {
            return end<false>();
        }

        template<bool is_left>
        bimap_iterator<KEY_T, VALUE_T, is_left> begin()
        {
            bimap_iterator<KEY_T, VALUE_T, is_left> result;

            result.root = node_traits<KEY_T, VALUE_T, is_left>::get_half_node(&fake_root);
            if(!node_traits<KEY_T, VALUE_T, is_left>::get_half_node(&fake_root)->left)
            {
                return end<is_left>();
            }
            result.half_node = node_traits<KEY_T, VALUE_T, is_left>::get_half_node(&fake_root)->left->min();
            std::cout << "begin result value: " << result.half_node->data << std::endl;
            return result;

        };

        left_iterator l_begin()
        {
            return begin<true>();
        }

        right_iterator r_begin()
        {
            return begin<false>();
        }

        template<bool is_left>
        bimap_iterator<KEY_T, VALUE_T, is_left> find(typename conditional<is_left, KEY_T, VALUE_T>::type value)
        {

            typedef bimap_half_node<typename std::conditional<is_left, KEY_T, VALUE_T>::type>* half_node_ptr;
            if(!node_traits<KEY_T, VALUE_T, is_left>::get_half_node(&fake_root)->left)
            {
                return end<is_left>();
            }
            half_node_ptr f = node_traits<KEY_T, VALUE_T, is_left>::get_half_node(&fake_root)->left->find(value);
            if(f)
            {
                bimap_iterator<KEY_T, VALUE_T, is_left> res;
                res.end = false;
                res.half_node = f;
                res.root = node_traits<KEY_T, VALUE_T, is_left>::get_half_node(&fake_root);
                return res;
            }
            else
            {
                return end<is_left>();
            }

        }

        left_iterator l_find(KEY_T key)
        {
            return find<true>(key);
        }

        right_iterator r_find(VALUE_T value)
        {
            return find<false>(value);
        }


        template<bool is_left>
        bimap_iterator<KEY_T, VALUE_T, is_left> erase(typename conditional<is_left, KEY_T, VALUE_T>::type value)
        {
            std::cout << "called bimap erase" << std::endl;
            auto it = find<is_left>(value);
            if(it == end<is_left>())
            {
                std::cout << "not exists" << std::endl;
            }
            auto next = it;
            next++;
            auto next_value = *next;
            auto other_value = *it.to_other_type();

            node_t* delete_node = (node_t*) it.half_node;
            node_traits<KEY_T, VALUE_T, is_left>::get_half_node(&fake_root)->left->erase(value);
            node_traits<KEY_T, VALUE_T, !is_left>::get_half_node(&fake_root)->left->erase(other_value);

            delete delete_node;
            std::cout << "exit bimap erase" << std::endl;
            return find<is_left>(next_value);
        }

        left_iterator l_erase(KEY_T key)
        {
            return erase<true>(key);
        }
        right_iterator r_erase(VALUE_T value)
        {
            return erase<false>(value);
        }




        bool check_invariant()
        {
            return fake_root.check_invariant();
        }

        bool check_tree()
        {
            bool left_check;
            if(fake_root.left.left)
            {
                left_check = fake_root.left.left->check_tree();
            }
            else
            {
                left_check = true;
            }
            bool right_check;
            if(fake_root.left.right)
            {
                right_check = fake_root.left.right->check_tree();
            }
            return left_check && right_check;
        }

    private:
        node_t fake_root;
    };
}

#endif