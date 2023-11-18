#ifndef __PROGTEST__
#include <cassert>
#include <iomanip>
#include <cstdint>
#include <iostream>
#include <memory>
#include <limits>
#include <optional>
#include <algorithm>
#include <bitset>
#include <list>
#include <array>
#include <vector>
#include <deque>
#include <unordered_set>
#include <unordered_map>
#include <stack>
#include <queue>
#include <random>
#include <type_traits>

#endif

struct TextEditorBackend {

  struct Node{

    public:
      Node * parent;
      Node * left;
      Node * right;
      char value;
      size_t size;

      size_t left_size;
      size_t right_size;

      size_t left_newlines;
      size_t right_newlines;
    
    // ~ Node(){
    //   delete left;
    //   left = nullptr;
    //   delete right;
    //   right = nullptr;
    // }

    Node() = default;
  };  


  Node * root;
  size_t m_size = 0;

  // ~ TextEditorBackend(){
  //   delete root;
  //   root = nullptr;
  // }

  void show_recursive(Node * node, size_t index) const{

    std::cout << "" << node->value << " |" << index << "| {" << node->size << ", " << node->left_size 
      << ", " << node->right_size << ", "  << node->left_newlines << ", " << node->right_newlines  << "}";
    
    if(node->left != nullptr) std::cout << "\tl: '" << node->left->value << "'"; 
    if(node->right != nullptr) std::cout << "\tr: '" << node->right->value << "'";
    std::cout << "\n";
    if(node->left != nullptr) show_recursive(node->left, index - 1 - node->left->right_size);
    if(node->right != nullptr) show_recursive(node->right, index + 1 + node->right->left_size);

  }

  void show() const{
    std::cout << "\n---\t---\t---\n";
    show_recursive(root, root->left_size);
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
    node->right_size = prev->left_size;
    prev->left_size = prev->left_size + 1 + node->left_size;
    
    node->right_newlines = prev->left_newlines;
    prev->left_newlines = prev->left_newlines + node->left_newlines;
    if(node->value == '\n') prev->left_newlines++;
    
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
    node->left_size = prev->right_size;
    prev->right_size = prev->right_size + 1 + node->right_size;

    node->left_newlines = prev->right_newlines;
    prev->right_newlines = prev->right_newlines + node->right_newlines;
    if(node->value == '\n') prev->right_newlines++;
    
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


  TextEditorBackend(const std::string& text);

  size_t size() const;
  size_t lines() const;

  char at(size_t i) const;
  void edit(size_t i, char c);
  void insert(size_t i, char c);
  void erase(size_t i);

  size_t line_start(size_t r) const;
  Node * line_start_node(size_t r) const;
  size_t line_length(size_t r) const;
  size_t char_to_line(size_t i) const;
};



TextEditorBackend::TextEditorBackend(const std::string& text){

  root = nullptr;
  size_t index = 0;

  for(auto & value : text){

    Node * newNode = new Node();
    newNode->value = value;
    newNode->left = nullptr;
    newNode->right = nullptr;
    newNode->size = 1;
    newNode->left_size = 0;
    newNode->right_size = 0;
    newNode->left_newlines = 0;
    newNode->right_newlines = 0;
    
    if(m_size == 0){

      newNode->parent = nullptr;
      root = newNode;
      m_size ++;

    }else{

      Node * node = root;
      size_t node_index = node->left_size;
      while(true){
        if(index < node_index){
          if(node->left != nullptr){
            node = node->left;
            node_index = node_index -1 - node->right_size;
          }else{
            newNode->parent = node;
            node->left = newNode;

            Node * prev = node;
            Node * n_node = node->parent;
            while(n_node!=nullptr){
              if(n_node->left == prev){
                n_node->left_size ++;
                if(value == '\n') n_node->left_newlines ++;
              }
              else{
                n_node->right_size ++;
                if(value == '\n') n_node->right_newlines ++;
              }
              prev = n_node;
              n_node = n_node->parent;
            }

            node->left_size ++;
            if(value == '\n') node->left_newlines ++;
            m_size ++;
            break;
          }
        }else if(index > node_index){
          if(node->right != nullptr){
            node = node->right;
            node_index = node_index + 1 + node->left_size;
          }else{
            newNode->parent = node;
            node->right = newNode;
            
            Node * prev = node;
            Node * n_node = node->parent;
            while(n_node!=nullptr){
              if(n_node->left == prev){
                n_node->left_size ++;
                if(value == '\n') n_node->left_newlines ++;
              }
              else{
                n_node->right_size ++;
                if(value == '\n') n_node->right_newlines ++;
              }
              prev = n_node;
              n_node = n_node->parent;
            }
            
            node->right_size ++;
            if(value == '\n') node->right_newlines ++;
            m_size ++;
            break;
          }
        }else{

          if(node->left != nullptr){
            Node* n_point = node->left;
            while (n_point->right != nullptr) n_point = n_point->right;
            newNode->parent = n_point;
            n_point->right = newNode;

            Node * prev = n_point;
            Node * n_node = n_point->parent;
            while(n_node!=nullptr){
              if(n_node->left == prev){
                n_node->left_size ++;
                if(value == '\n') n_node->left_newlines ++;
              }
              else{
                n_node->right_size ++;
                if(value == '\n') n_node->right_newlines ++;
              }
              prev = n_node;
              n_node = n_node->parent;
            }

            n_point->right_size ++;
            if(value == '\n') node->right_newlines ++;

          }else{
            newNode->parent = node;
            node->left = newNode;

            Node * prev = node;
            Node * n_node = node->parent;
            while(n_node!=nullptr){
              if(n_node->left == prev){
                n_node->left_size ++;
                if(value == '\n') n_node->left_newlines ++;
              }
              else{
                n_node->right_size ++;
                if(value == '\n') n_node->right_newlines ++;
              }
              prev = n_node;
              n_node = n_node->parent;
            }

            node->left_size ++;
            if(value == '\n') node->left_newlines ++;
          }

          m_size ++;
          break;

        }
      }

      bubble_up(newNode->parent, root);
    }

    index ++;
  }
}

size_t TextEditorBackend::size() const{

  return m_size;
}

size_t TextEditorBackend::lines() const{

  if(root == nullptr) return 0;
  size_t c = 0;
  if(root->value == '\n') c++;
  return root->left_newlines + root->right_newlines + c + 1; 
}

char TextEditorBackend::at(size_t i) const{

  if(i < 0 || i >= m_size) throw std::out_of_range("out_of_range");

  Node * node = root;
  size_t node_index = node->left_size;
  while(node!=nullptr){

    if(i < node_index){
      if(node->left != nullptr){
        node = node->left;
        node_index = node_index - 1 - node->right_size;
      }else{
        break;
      }
    }else if(i > node_index){
      if(node->right != nullptr){
        node = node->right;     
        node_index = node_index + 1 + node->left_size;   
      }else{
        break;
      }
    }else return node->value;
    
  }
  throw std::out_of_range("out_of_range");

}

void TextEditorBackend::edit(size_t i, char c){

  if(i < 0 || i >= m_size) throw std::out_of_range("out_of_range");

  Node * node = root;
  size_t node_index = node->left_size;
  while(node!=nullptr){

    if(i < node_index){
      if(node->left != nullptr){
        node = node->left;
        node_index = node_index - 1 - node->right_size;
      }else{
        break;
      }
    }else if(i > node_index){
      if(node->right != nullptr){
        node = node->right;     
        node_index = node_index + 1 + node->left_size;   
      }else{
        break;
      }
    }else{

      if(node->value == '\n' && node->parent){

        Node * prev = node;
        Node * n_node = node->parent;

        while(n_node!=nullptr){

          if(n_node->left == prev) n_node->left_newlines --;
          else n_node->right_newlines --;
          
          prev = n_node;
          n_node = n_node->parent;
        }
      }

      node->value = c; 

      if(node->value == '\n' && node->parent){
        
        Node * prev = node;
        Node * n_node = node->parent;

        while(n_node!=nullptr){

          if(n_node->left == prev) n_node->left_newlines ++;
          else n_node->right_newlines ++;
          
          prev = n_node;
          n_node = n_node->parent;
        }
      }

      return;
    }
    
  }
  throw std::out_of_range("out_of_range");

}

void TextEditorBackend::insert(size_t index, char value){

  if(index < 0 || index > m_size) throw std::out_of_range("out_of_range");

  Node * newNode = new Node();
  newNode->value = value;
  newNode->left = nullptr;
  newNode->right = nullptr;
  newNode->size = 1;
  newNode->left_size = 0;
  newNode->right_size = 0;
  newNode->left_newlines = 0;
  newNode->right_newlines = 0;
  
  if(m_size == 0){

    newNode->parent = nullptr;
    root = newNode;
    m_size ++;

    return;

  }else{

    Node * node = root;
    size_t node_index = node->left_size;
    while(true){
      if(index < node_index){
        if(node->left != nullptr){
          node = node->left;
          node_index = node_index -1 - node->right_size;
        }else{
          newNode->parent = node;
          node->left = newNode;

          if(node->parent){
            Node * prev = node;
            Node * n_node = node->parent;
            while(n_node!=nullptr){
              if(n_node->left == prev){
                n_node->left_size ++;
                if(value == '\n') n_node->left_newlines ++;
              }
              else{
                n_node->right_size ++;
                if(value == '\n') n_node->right_newlines ++;
              }
              prev = n_node;
              n_node = n_node->parent;
            }
          }

          node->left_size ++;
          if(value == '\n') node->left_newlines ++;
          m_size ++;
          break;
        }
      }else if(index > node_index){
        if(node->right != nullptr){
          node = node->right;
          node_index = node_index + 1 + node->left_size;
        }else{
          newNode->parent = node;
          node->right = newNode;
          
          if(node->parent){
            Node * prev = node;
            Node * n_node = node->parent;
            while(n_node!=nullptr){
              if(n_node->left == prev){
                n_node->left_size ++;
                if(value == '\n') n_node->left_newlines ++;
              }
              else{
                n_node->right_size ++;
                if(value == '\n') n_node->right_newlines ++;
              }
              prev = n_node;
              n_node = n_node->parent;
            }
          }
            
          node->right_size ++;
          if(value == '\n') node->right_newlines ++;
          m_size ++;
          break;
        }
      }else{

        if(node->left != nullptr){
          Node* n_point = node->left;
          while (n_point->right != nullptr) n_point = n_point->right;
          newNode->parent = n_point;
          n_point->right = newNode;

          if(n_point->parent){
            Node * prev = n_point;
            Node * n_node = n_point->parent;
            while(n_node!=nullptr){
              if(n_node->left == prev){
                n_node->left_size ++;
                if(value == '\n') n_node->left_newlines ++;
              }
              else{
                n_node->right_size ++;
                if(value == '\n') n_node->right_newlines ++;
              }
              prev = n_node;
              n_node = n_node->parent;
            }
          }

          n_point->right_size ++;
          if(value == '\n') node->right_newlines ++;

        }else{
          newNode->parent = node;
          node->left = newNode;

          if(node->parent != nullptr){
            Node * prev = node;
            Node * n_node = node->parent;
            while(n_node!=nullptr){
              if(n_node->left == prev){
                n_node->left_size ++;
                if(value == '\n') n_node->left_newlines ++;
              }
              else{
                n_node->right_size ++;
                if(value == '\n') n_node->right_newlines ++;
              }
              prev = n_node;
              n_node = n_node->parent;
            }
          }

          node->left_size ++;
          if(value == '\n') node->left_newlines ++;
        }

        m_size ++;
        break;

      }
    }

    bubble_up(newNode->parent, root);

    return;
  }
  
}

void TextEditorBackend::erase(size_t index){


  if(index < 0 || index >= m_size) throw std::out_of_range("out_of_range");

  Node * node = root;
  Node * parent = nullptr;
  bool left = false;
  size_t node_index = node->left_size;

  while(true){

    if(index < node_index){
      if(node->left != nullptr){
        parent = node;
        node = node->left;
        left = true;
        node_index = node_index -1 - node->right_size;
      }else{
        break;
      }
    }else if(index > node_index){
      if(node->right != nullptr){
        parent = node;
        left = false;
        node = node->right;
        node_index = node_index +1 + node->left_size;
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

        if(node->value == '\n'){

          Node * n_prev = node;
          Node * nn_node = parent;
          // Node * n_prev = n_point;
          // Node * nn_node = n_parent;

          while(nn_node!=nullptr){
            
            if(nn_node->left == n_prev) nn_node->left_newlines --;
            else nn_node->right_newlines --;
            
            n_prev = nn_node;
            nn_node = nn_node->parent;
          }
        }

        node->value = n_point->value;

        if (n_point->left != nullptr) n_point->left->parent = n_parent;

        Node * prev = n_point;
        Node * n_node = n_parent;
        bool ignore = false;
        while(n_node!=nullptr){
          if(n_node->left == prev){
            n_node->left_size--;
            if(n_point->value == '\n' && !ignore) n_node->left_newlines --;
          }
          else{
            n_node->right_size --;
            if(n_point->value == '\n' && !ignore) n_node->right_newlines --;
          } 
          if(n_node == node) ignore = true;
          prev = n_node;
          n_node = n_node->parent;
        }

        if (n_left) n_parent->left = n_point->left;
        else n_parent->right = n_point->left;

        n_point->left = nullptr;
        n_point->right = nullptr;

        delete n_point;

        bubble_up(n_parent, root); 

      }
      else{  
        Node * child = (node->left != nullptr) ? node->left : node->right;

        Node * prev = node;
        Node * n_node = parent;
        while(n_node!=nullptr){
          if(n_node->left == prev){
            n_node->left_size--;
            if(node->value == '\n') n_node->left_newlines --;
          }
          else{
            n_node->right_size --;
            if(node->value == '\n') n_node->right_newlines --;
          } 
          prev = n_node;
          n_node = n_node->parent;
        }

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
      return;
    }
  }
  
  throw std::out_of_range("out_of_range");
}

size_t TextEditorBackend::line_start(size_t r)const{

  if(r < 0 || r >= lines()) throw std::out_of_range("line_start 1");

  Node * node = root;
  size_t node_index = node->left_size;
  size_t newlines_count = 0;
  while(node!=nullptr){

    if(r < newlines_count + node->left_newlines){
      if(node->left != nullptr){
        node = node->left;
        node_index = node_index - 1 - node->right_size;
      }else{
        if(node->value == '\n'){ // ?
          return node_index + 1; // ? 
        }
        break;
      }
    }else if(r > newlines_count + node->left_newlines){
      if(node->right != nullptr){
        
        if(node->value=='\n') newlines_count = newlines_count + 1 + node->left_newlines;
        
        node = node->right;     
        node_index = node_index + 1 + node->left_size;
      }else{
        if(node->value == '\n'){
          return node_index + 1;
        }
        break;
      }
    }else {
      if(node->left == nullptr){
        if(node->value == '\n') return node_index;
        throw std::out_of_range("line_start 2");
      }


      Node * n_point = node->left;
      node_index = node_index - 1 - n_point->right_size;
      
      while (n_point->left != nullptr || n_point->right != nullptr){
        if(r > n_point->left_newlines + newlines_count){
          if(n_point->value=='\n') newlines_count = newlines_count + 1 + n_point->left_newlines;
          n_point = n_point->right;     
          node_index = node_index + 1 + n_point->left_size;   
        }else{
          n_point = n_point->left;
          node_index = node_index - 1 - n_point->right_size;
        }
      }
      if(n_point->value == '\n') return node_index+1;
      return node_index;
    }
  }
  throw std::out_of_range("line_start 3");

}

size_t TextEditorBackend::line_length(size_t r)const{

  if(r < 0 || r >= lines()) throw std::out_of_range("out_of_range");

  if(r == lines() - 1) return size() - line_start(r);
  return line_start(r+1) - line_start(r);
}

size_t TextEditorBackend::char_to_line(size_t i)const{
  
  if(i < 0 || i >= m_size) throw std::out_of_range("out_of_range");

  Node * node = root;
  size_t node_index = node->left_size;
  size_t newlines = 0;
  
  while(node){
  
    if(i < node_index){
      if(node->left != nullptr){
        node = node->left;
        node_index = node_index -1 - node->right_size;
      }
      else throw std::out_of_range("out_of_range");
    }
    else if(i > node_index){
      if(node->right != nullptr){
        if(node->value == '\n') newlines ++ ;
        newlines += node->left_newlines;
        node = node->right;
        node_index = node_index + 1 + node->left_size;
      }
      else throw std::out_of_range("out_of_range");
      
    }else{
      newlines += node->left_newlines;
      return newlines;
    }
  }
  return newlines;
}


#ifndef __PROGTEST__

////////////////// Dark magic, ignore ////////////////////////

template < typename T >
auto quote(const T& t) { return t; }

std::string quote(const std::string& s) {
  std::string ret = "\"";
  for (char c : s) if (c != '\n') ret += c; else ret += "\\n";
  return ret + "\"";
}

#define STR_(a) #a
#define STR(a) STR_(a)

#define CHECK_(a, b, a_str, b_str) do { \
    auto _a = (a); \
    decltype(a) _b = (b); \
    if (_a != _b) { \
      std::cout << "Line " << __LINE__ << ": Assertion " \
        << a_str << " == " << b_str << " failed!" \
        << " (lhs: " << quote(_a) << ")" << std::endl; \
      fail++; \
    } else ok++; \
  } while (0)

#define CHECK(a, b) CHECK_(a, b, #a, #b)

#define CHECK_ALL(expr, ...) do { \
    std::array _arr = { __VA_ARGS__ }; \
    for (size_t _i = 0; _i < _arr.size(); _i++) \
      CHECK_((expr)(_i), _arr[_i], STR(expr) "(" << _i << ")", _arr[_i]); \
  } while (0)

#define CHECK_EX(expr, ex) do { \
    try { \
      (expr); \
      fail++; \
      std::cout << "Line " << __LINE__ << ": Expected " STR(expr) \
        " to throw " #ex " but no exception was raised." << std::endl; \
    } catch (const ex&) { ok++; \
    } catch (...) { \
      fail++; \
      std::cout << "Line " << __LINE__ << ": Expected " STR(expr) \
        " to throw " #ex " but got different exception." << std::endl; \
    } \
  } while (0)
 
////////////////// End of dark magic ////////////////////////


std::string text(const TextEditorBackend& t) {
  std::string ret;
  for (size_t i = 0; i < t.size(); i++) ret.push_back(t.at(i));
  return ret;
}

void test1(int& ok, int& fail) {
	TextEditorBackend s("123\n456\n789");
  CHECK(s.size(), 11);
	CHECK(text(s), "123\n456\n789");
  CHECK(s.lines(), 3);
  CHECK_ALL(s.char_to_line, 0,0,0,0, 1,1,1,1, 2,2,2);
  CHECK_ALL(s.line_start, 0, 4, 8);
  CHECK_ALL(s.line_length, 4, 4, 3);
}

void test2(int& ok, int& fail) {
	TextEditorBackend t("123\n456\n789\n");
  CHECK(t.size(), 12);
	CHECK(text(t), "123\n456\n789\n");
  CHECK(t.lines(), 4);
  CHECK_ALL(t.char_to_line, 0,0,0,0, 1,1,1,1, 2,2,2,2);
  CHECK_ALL(t.line_start, 0, 4, 8, 12);
  CHECK_ALL(t.line_length, 4, 4, 4, 0);
}

void test3(int& ok, int& fail) {
	TextEditorBackend t("asdfasdfasdf");

  CHECK(t.size(), 12);
  CHECK(text(t), "asdfasdfasdf");
	CHECK(t.lines(), 1);
  CHECK_ALL(t.char_to_line, 0,0,0,0, 0,0,0,0, 0,0,0,0);
	CHECK(t.line_start(0), 0);
	CHECK(t.line_length(0), 12);
	
	t.insert(0, '\n');
  CHECK(t.size(), 13);
  CHECK(text(t), "\nasdfasdfasdf");
	CHECK(t.lines(), 2);
  CHECK_ALL(t.line_start, 0, 1);
	
  t.insert(4, '\n');
  CHECK(t.size(), 14);
  CHECK(text(t), "\nasd\nfasdfasdf");
	CHECK(t.lines(), 3);
  CHECK_ALL(t.line_start, 0, 1, 5);
	
  t.insert(t.size(), '\n');
  CHECK(t.size(), 15);
  CHECK(text(t), "\nasd\nfasdfasdf\n");
	CHECK(t.lines(), 4);
  CHECK_ALL(t.line_start, 0, 1, 5, 15);
	
	t.edit(t.size() - 1, 'H');
  CHECK(t.size(), 15);
  CHECK(text(t), "\nasd\nfasdfasdfH");
	CHECK(t.lines(), 3);
  CHECK_ALL(t.line_start, 0, 1, 5);

	t.erase(8);
  CHECK(t.size(), 14);
  CHECK(text(t), "\nasd\nfasfasdfH");
	CHECK(t.lines(), 3);
  CHECK_ALL(t.line_start, 0, 1, 5);

  t.erase(4);
  CHECK(t.size(), 13);
  CHECK(text(t), "\nasdfasfasdfH");
	CHECK(t.lines(), 2);
  CHECK_ALL(t.line_start, 0, 1);
}

void test_ex(int& ok, int& fail) {
	TextEditorBackend t("123\n456\n789\n");
  CHECK_EX(t.at(12), std::out_of_range);

  CHECK_EX(t.insert(13, 'a'), std::out_of_range);
  CHECK_EX(t.edit(12, 'x'), std::out_of_range);
  CHECK_EX(t.erase(12), std::out_of_range);

  CHECK_EX(t.line_start(4), std::out_of_range);
  CHECK_EX(t.line_start(40), std::out_of_range);
  CHECK_EX(t.line_length(4), std::out_of_range);
  CHECK_EX(t.line_length(6), std::out_of_range);
  CHECK_EX(t.char_to_line(12), std::out_of_range);
  CHECK_EX(t.char_to_line(25), std::out_of_range);
}

int main() {

  int ok = 0, fail = 0;

  // TextEditorBackend t("\n\n\n\n\n\n");
  // CHECK(t.size(), 6);
	// CHECK(t.lines(), 7);

  // t.show();
  // t.insert(1, 'a');
  // t.insert(3, 'b');
  // t.insert(5, 'c');
  // t.show();
  // std::cout << "inserted\n\n\n\n";

  // t.erase(0); 
  // t.erase(2);
  // t.erase(4);
  // t.show();
  // std::cout << "erased\n\n\n\n";

	// CHECK(t.lines(), 5);

  // t.edit(t.size() - 1, 'd');
  // t.show();
  // t.insert(t.size(), 'f');
  // t.show();
  // t.edit(t.size() - 1, '\n');
  // t.show();
  // std::cout << "edited\n\n\n\n";

	// CHECK(t.lines(), 5);

  // t.erase(2); 
  // t.erase(1);
  // t.erase(2);
  // t.show();
  // std::cout << "erased\n\n\n\n";

	// CHECK(t.lines(), 2);

  // TextEditorBackend t2("\n\n\n\n\n\n\n\n\na");
	
  // t2.show();
  // t2.edit(t2.size()-1, '\n');
  // t2.edit(0, 'a');
  // t2.erase(3);
  // t2.erase(0);
  // t2.edit(t2.size()-1, 'b');
  // t2.erase(7);
  // t2.edit(2, 'c');
  // t2.erase(5);
  // t2.edit(2, '\n');
  // t2.edit(3, 'd');
  // t2.erase(1);
  // t2.edit(t2.size()-1, 'e');
  // t2.show();
  // std::cout << text(t2);


  // t2.erase(4);
  // t2.edit(6, 'a');
  // t2.erase(5);
  // t2.edit(2, 'b');
  // t2.erase(3);
  // t2.edit(0, 'c');
  // t2.erase(0);
  // t2.show();
	//CHECK(t2.lines(), 5);

  if (!fail) test1(ok, fail);
  if (!fail) test2(ok, fail);
  if (!fail) test3(ok, fail);
  if (!fail) test_ex(ok, fail);
  
  if (!fail) std::cout << "Passed all " << ok << " tests!" << std::endl;
  else std::cout << "Failed " << fail << " of " << (ok + fail) << " tests." << std::endl;
}

#endif


