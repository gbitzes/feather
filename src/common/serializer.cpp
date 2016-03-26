#include <common/serializer.h>
#include <sstream>
#include <iostream>
#include <base/int-ranges.h>
#include <common/util.h>

namespace {
    using namespace feather;
    struct SimpleRange {
        Int a, b;
    };
    Constraint* deserializeConstraint(int id, std::string s) {
        #include <common/constr-serializer.icc>
    }
    Goal* deserializeGoal(int id, std::string s) {
        if(id == Goals::InDomain)
            return Goal_InDomain::deserialize(s);
        if(id == Goals::Labeling)
            return Goal_Labeling::deserialize(s);
        if(id == Goals::ParallelInDomain)
            return Goal_ParallelInDomain::deserialize(s);
        if(id == Goals::ParallelLabeling)
            return Goal_ParallelLabeling::deserialize(s);
    }
}

namespace feather {
    std::string StateSerializer::serialize(const SearchState &state, bool has_objective) {
        std::ostringstream ss;
        ss << state.decisions.size() << " ";
        for(int i = 0; i < state.decisions.size(); i++) {
            if(state.decisions[i] == true) ss << "1 ";
            if(state.decisions[i] == false) ss << "0 ";
            ss << " ";

            if(has_objective) {
                ss << state.objectives[i];
            }
            ss << " ";
        }
        return ss.str();
    }

    SearchState StateSerializer::deserialize(const std::string& str, bool has_objective) {
        SearchState state;
        std::stringstream ss(str);
        int size;
        ss >> size;
        for(int i = 0; i < size; i++) {
            bool decision = false;
            int dec;
            ss >> dec;
            if(dec == 1) decision = true;
            state.decisions.push_back(decision);

            if(has_objective) {
                Int obj;
                ss >> obj;
                state.objectives.push_back(obj);
            }
        }
        return state;
    }

    std::string RepresentationSerializer::serialize(const Representation& repr) {
        std::ostringstream ss;
        for(int i = 0; i < repr.constraints.size(); i++) {
            ss << repr.constraints[i]->serialize() << std::endl;
        }
        for(int i = 0; i < repr.arrays.size(); i++) {
            ss << "VARARR ";
            ss << repr.arrays[i]->id << " ";
            ss << repr.arrays[i]->vars.size() << " ";
            for(int j = 0; j < repr.arrays[i]->vars.size(); j++)
                ss << repr.arrays[i]->vars[j] << " ";
            ss << std::endl;
        }
        for(int i = 0; i < repr.deques.size(); i++) {
            ss << "DEQUE ";
            ss << repr.deques[i]->id << " ";
            ss << repr.deques[i]->contents.size() << " ";
            for(int j = 0; j < repr.deques[i]->contents.size(); j++)
                ss << repr.deques[i]->contents[j] << " ";
            ss << std::endl;
        }
        for(int i = 0; i < repr.vars.size(); i++) {
            ss << "VAR ";
            ss << repr.vars[i].id << " ";
            ss << repr.vars[i].domain->toString() << std::endl;
        }
        for(int i = 0; i < repr.goals.size(); i++) {
            ss << repr.goals[i]->serialize() << std::endl;
        }
        ss << "MINOBJ " << repr.minObj << std::endl;
        if(repr.limitedReporting) {
            ss << "LIMITEDREPORT " << repr.reportedVars.size() << " ";
            std::set<IntVarID>::iterator iter;
            for(iter = repr.reportedVars.begin(); iter != repr.reportedVars.end(); ++iter) {
                ss << *iter << " ";
            }
            ss << std::endl;
        }
        ss << "END REPRESENTATION" << std::endl;
        return ss.str();
    }

    Representation* RepresentationSerializer::deserialize(std::string& str) {
        Representation* repr = new Representation();
        std::stringstream ss(str);
        std::string line;
        while(std::getline(ss, line, '\n')) {
            std::stringstream ssline(line);
            std::string type;
            ssline >> type;
            if(type == "CONSTR") {
                int id;
                ssline >> id;
                //std::cout << type << " " << id << std::endl;
                std::string remaining;
                getline(ssline, remaining);
                Constraint* con = deserializeConstraint(id, remaining);
                //std::cout << con->serialize() << std::endl;
                repr->constraints.push_back(con);
            }
            if(type == "VARARR") {
                RepresentationIntVarArray *varr = new RepresentationIntVarArray();
                int size;
                IntVarArrayID id;
                ssline >> id >> size;
                varr->id = id;
                //std::cout << "VARARR " << id << " " << size << " ";
                for(int i = 0; i < size; i++) {
                    IntVarID vid;
                    ssline >> vid;
                    varr->vars.push_back(vid);
                    //std::cout << vid << " ";
                }
                //std::cout << std::endl;
                repr->arrays.push_back(varr);
            }
            if(type == "DEQUE") {
                RepresentationIntDeque *deque = new RepresentationIntDeque();
                int size;
                IntDequeID id;
                ssline >> id >> size;
                deque->id = id;
                //std::cout << "DEQUE " << id << " " << size << " ";
                for(int i = 0; i < size; i++) {
                    Int num;
                    ssline >> num;
                    deque->contents.push_back(num);
                    //std::cout << num << " ";
                }
                //std::cout << std::endl;
                repr->deques.push_back(deque);
            }
            if(type == "VAR") {
                RepresentationIntVar v;
                IntVarID id;
                ssline >> id;
                std::string chopped;
                getline(ssline, chopped);
                chopped = chopped.substr(1, chopped.length()-1);
                IntRanges *domain = deserializeDomain(chopped);
                v.domain = domain;
                v.id = id;
                repr->vars.push_back(v);
            }
            if(type == "GOAL") {
                int id;
                ssline >> id;
                std::string remaining;
                getline(ssline, remaining);
                Goal* g = deserializeGoal(id, remaining);
                repr->goals.push_back(g);
            }
            if(type == "MINOBJ") {
                IntVarID id;
                ssline >> id;
                repr->minObj = id;
            }
            if(type == "LIMITEDREPORT") {
                repr->limitedReporting = true;
                int size;
                ssline >> size;
                for(int i = 0; i < size; i++) {
                    IntVarID reportid;
                    ssline >> reportid;
                    repr->reportedVars.insert(reportid);
                }

            }
        }
        return repr;
    }
}
