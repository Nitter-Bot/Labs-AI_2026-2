#ifndef DPLL_HPP
#define DPLL_HPP

#include <vector>
#include <cmath>
#include <set>

using Literal = int;
using Clause = std::vector<Literal>;
using CNF = std::vector<Clause>;

CNF assign_literal(const CNF& phi,Literal l){
	CNF new_phi;
	for(const auto& clause:phi){
		bool satisfied = false;
		Clause new_clause;
		for (Literal x:clause){
		    if (x==l)satisfied=true;
		    else if(x!=-l)new_clause.push_back(x);
		}
		if(!satisfied)new_phi.push_back(new_clause);
	    }
	    return new_phi;
}

bool run_dpll(CNF phi){
	if(phi.empty())return true;

	for(const auto& clause:phi)
		if(clause.empty())return false;


	for(const auto& clause:phi)
		if(clause.size()==1)
			return run_dpll(assign_literal(phi, clause[0]));

	std::set<Literal> literals;
	for (const auto& clause:phi)
		for (Literal l:clause)literals.insert(l);

	for(Literal l:literals){
		if (literals.find(-l)==literals.end())
		    return run_dpll(assign_literal(phi, l));

	Literal v = std::abs(*literals.begin());
	if(run_dpll(assign_literal(phi, v)))return true;
	return run_dpll(assign_literal(phi,-v));
}

#endif
