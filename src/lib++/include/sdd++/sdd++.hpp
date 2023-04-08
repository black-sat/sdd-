//
// SDD++ - C++ wrapper library for libsdd 2.0
//
// (C) 2023 Nicola Gigante
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//

#ifndef SDDPP_SDDPP_HPP
#define SDDPP_SDDPP_HPP

#include <memory>
#include <optional>
#include <vector>
#include <functional>
#include <cstdint>
#include <cstdlib>

// forward declarations from the C API
struct vtree_t;
struct sdd_node_t;
struct sdd_manager_t;

namespace sdd {

  template<typename T>
  auto make_array_ptr(T *ptr) {
    return std::unique_ptr<T[], void(*)(void*)>(ptr, &free);
  }

  enum class GC {
    disabled = 0,
    enabled
  };

  class node;
  class variable;
  class literal;

  class manager {
  public:
    manager(size_t var_count, GC gc = GC::disabled);
    manager(manager const&) = delete;
    manager(manager &&) = default;
    
    manager &operator=(manager const&) = delete;
    manager &operator=(manager &&) = default;

    size_t var_count() const;
    std::vector<variable> variables() const;

    std::vector<variable> var_order() const;
    void add_var_before_first();
    void add_var_after_last();
    void add_var_before(variable var);
    void add_var_after(variable var);

    node literal(sdd::literal lit);
    node top();
    node bottom();

    sdd_manager_t *sdd() const { return _mgr.get(); }

  private:
    std::unique_ptr<sdd_manager_t, void(*)(sdd_manager_t*)> _mgr;
  };

  class variable {
    static_assert(sizeof(unsigned) < sizeof(long));

  public:
    variable() = default;
    variable(unsigned var) : _var{var} { }

    explicit operator unsigned() const { return _var; }

    bool operator==(variable const&other) const = default;

    literal operator!() const;

  private:
    unsigned _var = 0;
  };

  class literal {
  public:
    literal() = default;
    literal(class variable var) : _lit{unsigned(var)} { }
    literal(long lit) : _lit{lit} { }

    explicit operator long() const { return long(_lit); }

    bool operator==(literal const&other) const = default;

    literal operator!() const {
      return literal{-_lit};
    }

    explicit operator bool() const {
      return _lit > 0;
    }

    class variable variable() const {
      return _lit > 0 ? unsigned(_lit) : unsigned(-_lit);
    }

  private:
    long _lit = 0;
  };

  inline literal variable::operator!() const {
    return !literal{*this};
  }

  struct element;

  class node {
  public:
    node(node const&) = default;
    node(node &&) = default;
    
    node &operator=(node const&) = default;
    node &operator=(node &&) = default;

    class manager *manager() const { return _mgr; }
    sdd_node_t *sdd() const { return _node.get(); }

    std::vector<variable> variables() const;

    node operator!() const;
    friend node operator&&(node n1, node n2);
    friend node operator&&(node n, class literal l);
    friend node operator&&(class literal l, node n);
    friend node operator||(node n1, node n2);
    friend node operator||(node n, class literal l);
    friend node operator||(class literal l, node n);

    bool operator==(node const&other) const = default;

    friend node exists(variable var, node n);
    friend node forall(variable var, node n);
    friend node exists(std::vector<variable> const& vars, node n);
    friend node forall(std::vector<variable> const& vars, node n);

    node condition(class literal lit) const;
    node condition(std::vector<class literal> const& lits) const;

    std::optional<bool> value(literal lit) const;
    
    node rename(std::function<sdd::variable(sdd::variable)> renaming);
    node rename(std::unordered_map<sdd::variable, sdd::variable> const&);

    bool is_valid() const;
    bool is_sat() const;
    bool is_unsat() const;
    std::optional<std::vector<class literal>> model() const;
    
    bool is_literal() const;
    bool is_decision() const;

    class literal literal() const;
    std::vector<element> elements() const;

    size_t size() const;
    size_t count() const;

  private:
    friend class manager;
    node(class manager *, sdd_node_t *);
    
    class manager *_mgr;
    std::shared_ptr<sdd_node_t> _node;
  };

  node exists(variable var, node n);
  node forall(variable var, node n);
  node exists(std::vector<variable> const& vars, node n);
  node forall(std::vector<variable> const& vars, node n);
  node implies(node n1, node n2);
  node implies(node n, literal l);
  node implies(literal l, node n);
  node iff(node n1, node n2);
  node iff(node n, literal l);
  node iff(literal l, node n);

  struct element {
    node prime;
    node sub;
  };

}

template<>
struct std::hash<sdd::variable> {
  size_t operator()(sdd::variable var) const {
    return std::hash<unsigned>{}(unsigned(var));
  }
};

template<>
struct std::hash<sdd::literal> {
  size_t operator()(sdd::literal lit) const {
    return std::hash<long>{}(long(lit));
  }
};

template<>
struct std::hash<sdd::node> {
  size_t operator()(sdd::node node) const {
    return std::hash<sdd_node_t *>{}(node.sdd());
  }
};

#endif // SDDPP_SDDPP_HPP
