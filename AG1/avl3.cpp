#ifndef __PROGTEST__
#include <cassert>
#include <cstdarg>
#include <iomanip>
#include <cstdint>
#include <iostream>
#include <memory>
#include <limits>
#include <optional>
#include <array>
#include <random>
#include <type_traits>

// We use std::set as a reference to check our implementation.
// It is not available in progtest :)
#include <set>

template < typename T >
struct Ref {
  size_t size() const { return _data.size(); }
  const T* find(const T& value) const {
    auto it = _data.find(value);
    if (it == _data.end()) return nullptr;
    return &*it;
  }
  bool insert(const T& value) { return _data.insert(value).second; }
  bool erase(const T& value) { return _data.erase(value); }

  auto begin() const { return _data.begin(); }
  auto end() const { return _data.end(); }

  private:
  std::set<T> _data;
};


#endif

#define DEBUG false

namespace config {
  // Enable to check that the tree is AVL balanced.
  inline constexpr bool CHECK_DEPTH = true;

  // Disable if your implementation does not have parent pointers
  inline constexpr bool PARENT_POINTERS = true;
}


template < typename T >
struct Tree {


  struct Node{

    public:
      Node * parent;
      Node * left;
      Node * right;
      T value;
      size_t size;
    
    ~ Node(){
      delete left;
      delete right;
    }

    Node() = default;
  };

  size_t size() const;
  const T* find(const T& value) const;
  bool insert(T value);
  bool erase(const T& value);

  void show(){
    std::cout << "\n---\t---\t---\n";
    std::vector<Node*> queue;
    queue.push_back(root);
    while(queue.size() > 0){
      Node * node = queue.back();
      queue.pop_back();
      std::cout << " '" << node->value << "' {" << node->size << "}";
      if(node->left != nullptr){
        queue.push_back(node->left);
        std::cout << "\tl: '" << node->left->value << "'";
      }
      if(node->right != nullptr){
        queue.push_back(node->right);
        std::cout << "\tr: '" << node->right->value << "'";
      }
      std::cout << "\n";
    }
  }


  ~ Tree(){
    delete root;
  }

  void rotate_left(Node *& root, Node * node){ 

    Node * prev = node->right;

    if(node == root) root = prev;
    if(node->parent != nullptr){
      if(node->parent->left == node) node->parent->left = prev;
      else node->parent->right = prev;
    }
    node->right = prev->left;
    prev->left = node;
    
    if(node->right != nullptr) node->right->parent = node;
    prev->parent = node->parent;
    node->parent = prev;

    size_t n_left_size = 0;
    size_t n_right_size = 0;
    size_t p_right_size = 0;
    if(node->left != nullptr) n_left_size = node->left->size;
    if(node->right != nullptr) n_right_size = node->right->size;
    if(prev->right != nullptr) p_right_size = prev->right->size; 
    
    size_t n_size = std::max(n_left_size, n_right_size) + 1;
    node->size = n_size;
    prev->size = std::max(n_size, p_right_size) + 1;
  }

  void rotate_right(Node *& root, Node * node){

    Node * prev = node->left;

    if(node == root) root = prev;
    if(node->parent != nullptr){
      if(node->parent->left == node) node->parent->left = prev;
      else node->parent->right = prev;
    }
    node->left = prev->right;
    prev->right = node;
    
    if(node->left != nullptr) node->left->parent = node;
    prev->parent = node->parent;
    node->parent = prev;

    size_t n_left_size = 0;
    size_t n_right_size = 0;
    size_t p_left_size = 0;
    if(node->left != nullptr) n_left_size = node->left->size;
    if(node->right != nullptr) n_right_size = node->right->size;
    if(prev->left != nullptr) p_left_size = prev->left->size; 
    
    size_t n_size = std::max(n_left_size, n_right_size) + 1;
    node->size = n_size;
    prev->size = std::max(p_left_size, n_size) + 1;
  }

  void bubble_up(Node * node, Node *& root){
    int prev_diff = 0;
    
    while(node != nullptr){
      size_t left_size = 0;
      size_t right_size = 0;
      if(node->left != nullptr) left_size = node->left->size;
      if(node->right != nullptr) right_size = node->right->size;
      size_t max_size = std::max(left_size, right_size);
      int diff = right_size - left_size;

      if(diff > 1){

        if(node->right != nullptr){
          size_t n_left_size = 0;
          size_t n_right_size = 0;
          if(node->right->left != nullptr) n_left_size = node->right->left->size;
          if(node->right->right != nullptr) n_right_size = node->right->right->size;
          prev_diff = n_right_size - n_left_size;

          if(prev_diff < 0){
            rotate_right(root, node->right);
          }
        }

        rotate_left(root, node);
        node = node->parent;

      }else if(diff < - 1){

        if(node->left != nullptr){
          size_t n_left_size = 0;
          size_t n_right_size = 0;
          if(node->left->left != nullptr) n_left_size = node->left->left->size;
          if(node->left->right != nullptr) n_right_size = node->left->right->size;
          prev_diff = n_right_size - n_left_size;

          if(prev_diff > 0){
            rotate_left(root, node->left);
          }
        }
        
        rotate_right(root, node);
        node = node->parent;

      }else if(node->size == max_size + 1 ){
        break;
      }else{
        node->size = max_size + 1;
      }

      node = node->parent;
    }
  }

  Node * root;
  size_t m_size = 0;

  // Needed to test the structure of the tree.
  // Replace Node with the real type of your nodes
  // and implementations with the ones matching
  // your attributes.
  struct TesterInterface {
    // using Node = ...
    static const Node *root(const Tree *t) { return t->root; }
    // Parent of root must be nullptr, ignore if config::PARENT_POINTERS == false
    static const Node *parent(const Node *n) { return n->parent; }
    static const Node *right(const Node *n) { return n->right; }
    static const Node *left(const Node *n) { return n->left; }
    static const T& value(const Node *n) { return n->value; }
  };
};

template <typename T>
size_t Tree<T>::size() const {


  return m_size;
}



template <typename T>
bool Tree<T>::insert(T value) {

  Node * newNode = new Node();
  newNode->value = value;
  newNode->left = nullptr;
  newNode->right = nullptr;
  newNode->size = 1;
  
  if(m_size == 0){

    newNode->parent = nullptr;
    root = newNode;
    m_size ++;

    std::cout << "inserted " << value << "\n";
    return true;

  }else{

    Node * node = root;
    while(true){

      if(value < node->value){
        if(node->left != nullptr){
          node = node->left;
        }else{
          newNode->parent = node;
          node->left = newNode;
          m_size ++;
          break;
        }
      }else if(value > node->value){
        if(node->right != nullptr){
          node = node->right;
        }else{
          newNode->parent = node;
          node->right = newNode;
          m_size ++;
          break;
        }
      }else{
        delete newNode;
        return false;
      }
    }
  }

  bubble_up(newNode->parent, root);

  std::cout << "inserted " << value << "\n";
  return true;
    
}

template <typename T>
const T* Tree<T>::find(const T& value) const {
    
  Node * node = root;
  while(true){

    if(value < node->value){
      if(node->left != nullptr){
        node = node->left;
      }else{
        break;
      }
    }else if(value > node->value){
      if(node->right != nullptr){
        node = node->right;
      }else{
        break;
      }
    }else{
      return &(node->value);
    }
  }
  
  return nullptr;
}


template <typename T>
bool Tree<T>::erase(const T& value) {
  
  Node * node = root;
  Node * parent = nullptr;
  bool left = false;
  while(true){

    if(value < node->value){
      if(node->left != nullptr){
        parent = node;
        node = node->left;
        left = true;
      }else{
        break;
      }
    }else if(value > node->value){
      if(node->right != nullptr){
        parent = node;
        left = false;
        node = node->right;
      }else{
        break;
      }
    }else{
      
      if( node->left != nullptr && node->right != nullptr ){ 
        Node* n_point = node->left;
        Node* n_parent = node;
        bool n_left = true;
        while (n_point->right != nullptr) {
            n_parent = n_point;
            n_point = n_point->right;
            n_left = false;
        }

        node->value = n_point->value;

        if (n_left)
            n_parent->left = n_point->left;
        else
            n_parent->right = n_point->left;

        if (n_point->left != nullptr)
            n_point->left->parent = n_parent;

        n_point->left = nullptr;
        n_point->right = nullptr;

        delete n_point;

        bubble_up(n_parent, root); 

      }
      else{  
        Node* child = (node->left != nullptr) ? node->left : node->right;

        if (left)
            if(parent) parent->left = child;
            else root = child;
        else
            if(parent) parent->right = child;
            else root = child;

        if (child != nullptr)
            child->parent = parent;

        node->left = nullptr;
        node->right = nullptr;

        delete node;

        bubble_up(parent, root); 

      }

      m_size --;
      
      return true;
    }
  }
  
  return false;
}


#ifndef __PROGTEST__

struct TestFailed : std::runtime_error {
  using std::runtime_error::runtime_error;
};

std::string fmt(const char *f, ...) {
  va_list args1;
  va_list args2;
  va_start(args1, f);
  va_copy(args2, args1);
  
  std::string buf(vsnprintf(nullptr, 0, f, args1), '\0');
  va_end(args1);
  
  vsnprintf(buf.data(), buf.size() + 1, f, args2);
  va_end(args2);

  return buf;
}

template < typename T >
struct Tester {
  Tester() = default;

  void size() const {
    size_t r = ref.size();
    size_t t = tested.size();
    if (r != t) throw TestFailed(fmt("Size: got %zu but expected %zu.", t, r));
  }

  void find(const T& x) const {
    auto r = ref.find(x);
    auto t = tested.find(x);
    bool found_r = r != nullptr;
    bool found_t = t != nullptr;

    if (found_r != found_t) _throw("Find mismatch", found_r);
    if (found_r && *t != x) throw TestFailed("Find: found different value");
  }

  void insert(const T& x, bool check_tree_ = false) {
    auto succ_r = ref.insert(x);
    auto succ_t = tested.insert(x);
    if (succ_r != succ_t) _throw("Insert mismatch", succ_r);
    size();
    if (check_tree_) check_tree();
  }

  void erase(const T& x, bool check_tree_ = false) {
    bool succ_r = ref.erase(x);
    auto succ_t = tested.erase(x);
    // if (succ_r != succ_t) _throw("Erase mismatch", succ_r);
    if (succ_r != succ_t) throw TestFailed("Erase mismatch");
    size();
    if (check_tree_) check_tree();
  }

  struct NodeCheckResult {
    const T* min = nullptr;
    const T* max = nullptr;
    int depth = -1;
    size_t size = 0;
  };

  void check_tree() const {
    using TI = typename Tree<T>::TesterInterface;
    auto ref_it = ref.begin();
    bool check_value_failed = false;
    auto check_value = [&](const T& v) {
      if (check_value_failed) return;
      check_value_failed = (ref_it == ref.end() || *ref_it != v);
      if (!check_value_failed) ++ref_it;
    };

    auto r = check_node(TI::root(&tested), decltype(TI::root(&tested))(nullptr), check_value);
    size_t t_size = tested.size();

    if (t_size != r.size) throw TestFailed(
      fmt("Check tree: size() reports %zu but expected %zu.", t_size, r.size));

    if (check_value_failed) throw TestFailed(
      "Check tree: element mismatch");

    size();
  }

  template < typename Node, typename F >
  NodeCheckResult check_node(const Node* n, const Node* p, F& check_value) const {
    if (!n) return {};
    
    using TI = typename Tree<T>::TesterInterface;
    if constexpr(config::PARENT_POINTERS) {
      if (TI::parent(n) != p){
        std::cout << "mismatch at n: '" << n->value << "'->parent != '" << p->value << "'\n";
        throw TestFailed("Parent mismatch.");
      }
    }

    auto l = check_node(TI::left(n), n, check_value);
    check_value(TI::value(n));
    auto r = check_node(TI::right(n), n, check_value);

    if (l.max && !(*l.max < TI::value(n)))
      throw TestFailed("Max of left subtree is too big.");
    if (r.min && !(TI::value(n) < *r.min))
      throw TestFailed("Min of right subtree is too small.");

    if (config::CHECK_DEPTH && abs(l.depth - r.depth) > 1) throw TestFailed(fmt(
      "Tree is not avl balanced: left depth %i and right depth %i.",
      l.depth, r.depth
    ));

    return {
      l.min ? l.min : &TI::value(n),
      r.max ? r.max : &TI::value(n),
      std::max(l.depth, r.depth) + 1, 1 + l.size + r.size
    };
  }

  static void _throw(const char *msg, bool s) {
    throw TestFailed(fmt("%s: ref %s.", msg, s ? "succeeded" : "failed"));
  }

  Tree<T> tested;
  Ref<T> ref;
};


void test_insert() {

  Tester<int> t;

  for (int i = 0; i < 10; i++) t.insert(i, true);
  for (int i = -10; i < 20; i++) t.find(i);

  for (int i = 0; i < 10; i++) t.insert((1 + i * 7) % 17, true);
  for (int i = -10; i < 20; i++) t.find(i);

}

void test_erase() {
  Tester<int> t;

  for (int i = 0; i < 10; i++) t.insert((1 + i * 7) % 17, true);
  for (int i = -10; i < 20; i++) t.find(i);

  for (int i = 3; i < 22; i += 2) t.erase(i, true);
  for (int i = -10; i < 20; i++) t.find(i);
  
  for (int i = 0; i < 10; i++) t.insert((1 + i * 13) % 17 - 8, true);
  for (int i = -10; i < 20; i++) t.find(i);
  
  for (int i = -4; i < 10; i++) t.erase(i, true);
  for (int i = -10; i < 20; i++) t.find(i);
  
}

enum RandomTestFlags : unsigned {
  SEQ = 1, NO_ERASE = 2, CHECK_TREE = 4
};

void test_random(size_t size, unsigned flags = 0) {
  Tester<size_t> t;
  std::mt19937 my_rand(24707 + size);

  bool seq = flags & SEQ;
  bool erase = !(flags & NO_ERASE);
  bool check_tree = flags & CHECK_TREE;

  for (size_t i = 0; i < size; i++)
    t.insert(seq ? 2*i : my_rand() % (3*size), check_tree);

  t.check_tree();

  for (size_t i = 0; i < 3*size + 1; i++) t.find(i);

  for (size_t i = 0; i < 30*size; i++) switch (my_rand() % 5) {
    case 1: t.insert(my_rand() % (3*size), check_tree);
      break;
    case 2: if (erase) t.erase(my_rand() % (3*size), check_tree);
      break;
    default:
      t.find(my_rand() % (3*size));
  }

  t.check_tree();
}

int main() {
  try {
    std::cout << "Insert test..." << std::endl;
    test_insert();

    std::cout << "Erase test..." << std::endl;
    test_erase();
    
    std::cout << "Tiny random test..." << std::endl;
    test_random(20, CHECK_TREE);
      
    std::cout << "Small random test..." << std::endl;
    test_random(200, CHECK_TREE);
    
    std::cout << "Big random test..." << std::endl;
    test_random(50'000);
    
    std::cout << "Big sequential test..." << std::endl;
    test_random(50'000, SEQ);

    std::cout << "All tests passed." << std::endl;
    
  } catch (const TestFailed& e) {
    std::cout << "Test failed: " << e.what() << std::endl;
  }
}

#endif


