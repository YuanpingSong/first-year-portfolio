// MyHash.h

// Skeleton for the MyHash class template.  You must implement the first seven
// member functions; we have implemented the eighth.
#include <iostream>
using std::cout;
using std::endl;

template<typename KeyType, typename ValueType>
class MyHash
{
public:
    MyHash(double maxLoadFactor = 0.5);
    ~MyHash();
    void reset();
    void associate(const KeyType& key, const ValueType& value);
    int getNumItems() const;
    double getLoadFactor() const;

      // for a map that can't be modified, return a pointer to const ValueType
    const ValueType* find(const KeyType& key) const;

      // for a modifiable map, return a pointer to modifiable ValueType
    ValueType* find(const KeyType& key)
    {
        return const_cast<ValueType*>(const_cast<const MyHash*>(this)->find(key));
    }

      // C++11 syntax for preventing copying and assignment
    MyHash(const MyHash&) = delete;
    MyHash& operator=(const MyHash&) = delete;
    // make this private after testing
    //void dump() const;
private:
    const int DEFAULT_BUCKET_NUMBER = 100;
    double m_maxLoadFactor;
    int m_nBuckets;
    int m_nItems;
    struct Node {
        KeyType m_key;
        ValueType m_val;
        Node *m_next;
        Node(KeyType key, ValueType val, Node * next);
    };
    Node** m_arr;
    Node* insert(Node* np, Node* arr[]);
    unsigned int getBucketNumber(const KeyType& key) const;
    void deleteList(Node* root);
};

template<typename KeyType, typename ValueType>
MyHash<KeyType, ValueType>::MyHash(double maxLoadFactor) : m_maxLoadFactor(maxLoadFactor), m_nBuckets(DEFAULT_BUCKET_NUMBER), m_nItems(0) {
    if (m_maxLoadFactor > 2) {
        m_maxLoadFactor = 2.0;
    } else if (m_maxLoadFactor <= 0) {
        m_maxLoadFactor = 0.5;
    }
    m_arr = new Node*[m_nBuckets];
    for (int i = 0; i < m_nBuckets; i++) {
        m_arr[i] = nullptr;
    }
}

template<typename KeyType, typename ValueType>
MyHash<KeyType, ValueType>::~MyHash() {
    for (int i = 0; i < m_nBuckets; i++) {
        deleteList(m_arr[i]);
    }
    delete [] m_arr;
}

template<typename KeyType, typename ValueType>
MyHash<KeyType, ValueType>::Node::Node(KeyType key, ValueType val, Node * next) : m_key(key), m_val(val), m_next(next) {

}

template<typename KeyType, typename ValueType>
void MyHash<KeyType, ValueType>::reset(){
    for (int i = 0; i < m_nBuckets; i++) {
        deleteList(m_arr[i]);
    }
    delete [] m_arr;
    m_nBuckets = DEFAULT_BUCKET_NUMBER;
    m_nItems = 0;
    m_arr = new Node*[m_nBuckets];
    for (int i = 0; i < m_nBuckets; i++) {
        m_arr[i] = nullptr;
    }
}

template<typename KeyType, typename ValueType>
void MyHash<KeyType, ValueType>::associate(const KeyType& key, const ValueType& value) {
    ValueType* vp = find(key); // determine if the association already exists
    if (vp == nullptr) { // we need to insert
        if (static_cast<double>(m_nItems + 1) / m_nBuckets > m_maxLoadFactor) { // determine if this insertion will cause the hash table to exceed its maximum load factor
            m_nBuckets *= 2;
            Node** temp = new Node*[m_nBuckets]; // allocate larger array
            for (int i = 0; i < m_nBuckets; i++) {
                temp[i] = nullptr;
            }
            for (int i = 0; i < m_nBuckets / 2; i++) {
                Node* it = m_arr[i];
                while (it != nullptr) { // traverse each linked list
                    it = insert(it, temp); // add each node to temp
                }
            }
            delete [] m_arr; // destroy old dynamically allocated array
            m_arr = temp; // temp goes out of scope while m_arr keeps a pointer to the enlarged array.
        }
        insert(new Node(key, value, nullptr), m_arr);
        m_nItems++;
    } else {
        *vp = value;
    }
    
}

template<typename KeyType, typename ValueType>
const ValueType* MyHash<KeyType, ValueType>::find(const KeyType& key) const {
    for (Node* it = m_arr[getBucketNumber(key)]; it != nullptr; it = it -> m_next) {
        if (it -> m_key == key) {
            return &(it -> m_val);
        }
    }
    return nullptr; // Not found
}

template<typename KeyType, typename ValueType>
int MyHash<KeyType, ValueType>::getNumItems() const {
    return m_nItems;
}

template<typename KeyType, typename ValueType>
double MyHash<KeyType, ValueType>::getLoadFactor() const {
    return static_cast<double>(m_nItems) / m_nBuckets;
}

template<typename KeyType, typename ValueType>
unsigned int MyHash<KeyType, ValueType>::getBucketNumber(const KeyType& key) const {
    unsigned int hash(const KeyType& k);
    return hash(key) % m_nBuckets;
}

// insert node (association) into the given hashtable. this function does not check for duplicate keys.
template<typename KeyType, typename ValueType>
typename MyHash<KeyType, ValueType>::Node* MyHash<KeyType, ValueType>::insert(Node* np, Node* arr[]) {
    Node* head = arr[getBucketNumber(np -> m_key)]; // get the head pointer to the linked list where we append the association
    Node* next = np -> m_next; // we return this so that we can call this function iteratively
    np -> m_next = nullptr; // remove the node's past link
    if (head == nullptr) { // if empty bucket, then put the association in
        arr[getBucketNumber(np -> m_key)] = np;
    } else { // otherwise, traverse to the end of the linked list and append the association.
        Node* it = head;
        while (it -> m_next != nullptr) {
            it = it -> m_next;
        }
        it -> m_next = np;
    }
    return next;
}

template<typename KeyType, typename ValueType>
void MyHash<KeyType, ValueType>::deleteList(Node* root) {
    if (root == nullptr) {
        return;
    } else {
        while (root != nullptr) {
            Node* next = root -> m_next;
            delete root;
            root = next;
        }
    }
}

/*template<typename KeyType, typename ValueType>
void MyHash<KeyType, ValueType>::dump() const {
    for (int i = 0; i < m_nBuckets; i++) { // traverse array
        cout << "Bucket " << i << ": ";
        Node* it = m_arr[i];
        if(m_arr[i] == nullptr) {
            cout << "Nullptr!";
        }
        while (it != nullptr) { // traverse each linked list
            cout << "[" << it -> m_key << ":" << it -> m_val << "] ";
            it = it -> m_next;
        }
        cout << endl;
    }
}*/



