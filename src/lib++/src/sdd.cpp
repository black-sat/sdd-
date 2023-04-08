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

#include <sdd++/sdd++.hpp>

#include <sdd/sdd.h>

#include <memory>
#include <algorithm>
#include <vector>
#include <cassert>

#include <iostream>

namespace sdd {

  //
  // manager
  //
  manager::manager(size_t var_count, GC gc) :
    _mgr{
      sdd_manager_create(SddLiteral(var_count), gc == GC::enabled ? 1 : 0),
      &sdd_manager_free
    } { }

  size_t manager::var_count() const {
    return size_t(sdd_manager_var_count(sdd()));
  }

  std::vector<variable> manager::variables() const {
    std::vector<variable> vars;
    for(unsigned i = 1; i <= var_count(); ++i)
      vars.push_back(i);
    return vars;
  }

  std::vector<variable> manager::var_order() const {
    size_t count = var_count();
    auto order = std::make_unique<SddLiteral[]>(count);

    sdd_manager_var_order(order.get(), sdd());

    std::vector<variable> vars;
    std::transform(order.get(), order.get() + count, std::back_inserter(vars),
      [](SddLiteral l) {
        return variable(unsigned(l));
      }
    );

    return vars;
  }

  void manager::add_var_before_first() {
    sdd_manager_add_var_before_first(sdd());
  }

  void manager::add_var_after_last() {
    sdd_manager_add_var_after_last(sdd());
  }

  void manager::add_var_before(variable var) {
    sdd_manager_add_var_before(SddLiteral(unsigned(var)), sdd());
  }

  void manager::add_var_after(variable var) {
    sdd_manager_add_var_after(SddLiteral(unsigned(var)), sdd());
  }

  node manager::literal(sdd::literal lit) {
    sdd::variable var = lit.variable();
    if(unsigned(var) > var_count())
      throw std::invalid_argument("literal too large");
    return node{this, sdd_manager_literal(SddLiteral(lit), sdd())};
  }

  node manager::top() {
    return node{this, sdd_manager_true(sdd())};
  }

  node manager::bottom() {
    return node{this, sdd_manager_false(sdd())};
  }

  //
  // node
  //
  node::node(class manager *mgr, SddNode *n) 
    : _mgr{mgr}, _node{
      std::shared_ptr<SddNode>{
        sdd_ref(n, mgr->sdd()), [=](SddNode *_n) {
          sdd_deref(_n, mgr->sdd());
        }
      }
    } { }

  std::vector<variable> node::variables() const {
    auto vars = make_array_ptr(sdd_variables(sdd(), manager()->sdd()));
    
    std::vector<variable> result;
    for(unsigned i = 1; i <= manager()->var_count(); ++i) {
      if(vars[i])
        result.push_back(variable{i});
    }
    return result;
  }

  node node::operator!() const {
    return node{manager(), sdd_negate(sdd(), manager()->sdd())};
  }

  node operator&&(node n1, node n2) {
    return 
      node(n1.manager(), sdd_conjoin(n1.sdd(), n2.sdd(), n1.manager()->sdd()));
  }

  node operator&&(node n, literal l) {
    return n && n.manager()->literal(l);
  }

  node operator&&(literal l, node n) {
    return n.manager()->literal(l) && n;
  }

  node operator||(node n1, node n2) {
    return 
      node(n1.manager(), sdd_disjoin(n1.sdd(), n2.sdd(), n1.manager()->sdd()));
  }

  node operator||(node n, literal l) {
    return n || n.manager()->literal(l);
  }

  node operator||(literal l, node n) {
    return n.manager()->literal(l) || n;
  }
   
  node exists(variable var, node n) {
    return node{
      n.manager(), 
      sdd_exists(SddLiteral(unsigned{var}), n.sdd(), n.manager()->sdd())
    };
  }
   
  node exists(std::vector<variable> const& vars, node n) {
    std::vector<int> map(n.manager()->var_count() + 1, 0);

    for(auto var : vars)
      map[unsigned(var)] = 1;
    
    return node{
      n.manager(), 
      sdd_exists_multiple(map.data(), n.sdd(), n.manager()->sdd())
    };
  }

  node forall(std::vector<variable> const& vars, node n) {
    return !exists(vars, !n);
  }
   
  node forall(variable var, node n) {
    return node{
      n.manager(), 
      sdd_forall(SddLiteral(unsigned{var}), n.sdd(), n.manager()->sdd())
    };
  }

  node implies(node n1, node n2) { 
    return !n1 || n2;
  }

  node implies(node n, literal l) {
    return implies(n, n.manager()->literal(l));
  }

  node implies(literal l, node n) {
    return implies(n.manager()->literal(l), n);
  }

  node iff(node n1, node n2) {
    return implies(n1, n2) && implies(n2, n1);
  }

  node iff(node n, literal l) {
    return iff(n, n.manager()->literal(l));
  }

  node iff(literal l, node n) {
    return iff(n.manager()->literal(l), n);
  }

  node node::condition(class literal lit) const {
    return node{
      manager(), sdd_condition(SddLiteral(lit), sdd(), manager()->sdd())
    };
  }

  node node::condition(std::vector<class literal> const& lits) const {
    node n = *this;
    for(auto lit : lits) {
      n = n.condition(lit);
      if(n.is_valid() || n.is_unsat())
        break;
    }
    return n;
  }

  std::optional<bool> node::value(class literal lit) const {
    if(condition(lit).is_unsat())
      return false;
    if(condition(!lit).is_unsat())
      return true;
    return {};
  }

  node node::rename(std::function<sdd::variable(sdd::variable)> renaming) {
    size_t n = manager()->var_count();
    auto map = std::make_unique<SddLiteral[]>(n + 1);

    for(unsigned i = 1; i <= n; i++) {
      map[i] = SddLiteral(sdd::literal(renaming(i)));
    }

    assert(manager()->var_count() == n);

    return node{
      manager(),
      sdd_rename_variables(sdd(), map.get(), manager()->sdd())
    };
  }

  node node::rename(
    std::unordered_map<sdd::variable, sdd::variable> const& map
  ) {
    return rename([&](variable var) {
      if(map.contains(var))
        return map.at(var);
      return var;
    });
  }

  bool node::is_valid() const {
    return sdd_node_is_true(sdd());
  }
  
  bool node::is_sat() const {
    return !is_unsat();
  }

  bool node::is_unsat() const {
    return sdd_node_is_false(sdd());
  }

  std::optional<std::vector<literal>> node::model() const 
  {
    std::vector<sdd::literal> model;

    auto vars = variables();
    
    node n = *this;
    size_t i = 0;
    while(true) { // vars.size() iterations max anyway
      if(n.is_unsat())
        return {};
      if(n.is_valid())
        return model;
      
      class literal lit = vars.at(i);
      node choice = n.condition(lit);
      
      if(choice.is_unsat()) {
        lit = !lit;
        choice = n.condition(lit);
      }

      // arbitrary choice now
      model.push_back(lit);
      n = choice;
      ++i;
    }
  }

  bool node::is_literal() const {
    return sdd_node_is_literal(sdd());
  }

  bool node::is_decision() const {
    return sdd_node_is_decision(sdd());
  }

  literal node::literal() const {
    if(!is_literal())
      return 0;
    
    return sdd_node_literal(sdd());
  }

  std::vector<element> node::elements() const {
    if(!is_decision())
      return {};

    SddNode **elems = sdd_node_elements(sdd());
    size_t nelems = sdd_node_size(sdd());

    std::vector<element> result;
    for(size_t i = 0; i < nelems; i++) {
      result.push_back(element{
        node{manager(), elems[2 * i]},
        node{manager(), elems[2 * i + 1]}
      });
    }

    return result;
  }

  size_t node::size() const {
    return sdd_size(sdd());
  }

  size_t node::count() const {
    return sdd_count(sdd());
  }

}