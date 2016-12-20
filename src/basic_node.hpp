#ifndef MCT_NODE_DEFINATION_HPP
#define MCT_NODE_DEFINATION_HPP
#include <iostream>
#include <cstddef>
#include <vector>
#include <cstdlib>
#include <mutex>
#include <atomic>
#include "basic_action.hpp"
#include "basic_state.hpp"
#include "rwlock.h"
/*
a class of node, should include:
visit count						N(v);
total simulation reward			Q(v);
the associated state			s(v);
the incoming action				a(v);

 a class of state, should include:
final state or not?						is_terminal;
list of action.(tried or not)			A(s);
next state after one action				f(s,a);
state evaluation?						eval(s)
*/

namespace mct{

    template<std::size_t W,std::size_t H>
	class node{
	private:
        using pointType = board::GridPoint<W,H>;
		std::atomic_int number;
        int max_child_number;
		std::atomic_int quality;
		//State<W,H> state;
		Action<W,H> act;
		bool full_expended;
        //bool full_child;
        bool is_terminal;
		node<W,H>* parent;
        board::Player player;
        std::vector<pointType> valid_act;
        std::mt19937 gen { std::random_device()() };

	public:
        //std::mutex mtx;
        WfirstRWLock action_mtx;
        WfirstRWLock child_mtx;
        using nodeType = node<W,H>;
        using stateType = State<W,H>;
        using boardType = board::Board<W,H>;
        using actionType = Action<W,H>;
        using Player = board::Player;
        std::vector<nodeType*> child;

		node(const board::Board<W,H> &b,Player p):number(0),quality(0){
			parent = NULL;
			act = actionType();
            stateType s(b);
            player = p;
            //std::vector<pointType> mid = state.getAllValidPosition(p);
            //for(auto iter = mid.cbegin();iter != mid.cend();iter++) valid_act.push_back(*iter);
            valid_act = s.getAllValidPosition(p);
            max_child_number = valid_act.size();
            s.clear();
            //std::cout << valid_act.size() << std::endl;
            full_expended = (valid_act.size()==0);
            is_terminal = (max_child_number==0);
		}
        node(stateType &s,Player p,nodeType* par,actionType action,bool t):number(0),quality(0){
            parent = par;
            //state = s;
            act = action;
            player = p;
            valid_act = s.getAllValidPosition(p);
            max_child_number = valid_act.size();
            full_expended = (valid_act.size()==0);
            is_terminal = t || (max_child_number==0);
        }
        node(const nodeType &other){
            number = other.number;
            quality = other.quality;
            parent = other.parent;
            //state = other.state;
            act = other.act;
            player = other.player;
            valid_act = other.valid_act;
            max_child_number = other.max_child_number;
            full_expended = other.full_expended;
            is_terminal = other.is_terminal;
        }

		~node(){
            std::cout << "clear" << std::endl;
            clear();
		}

        void clear(){
            nodeType * p;
            while(!child.empty()){
                p = child.back();
                child.pop_back();
                p->clear();
            }
            child.clear();
            valid_act.clear();
            //state.clear();
        }

		inline int getNumber(){
			return number;
		}

        inline void increaseNumber(){
            number++;
        }

        inline int getQuality(){
			return quality;
		}

        inline void increaseQuality(double q){
            //std::cout << res << std::endl;
            quality += (int)(q*4096);
        }

        /*stateType getState(){
            return state;
        }*/

        inline actionType getAction(){
            return act;
        }

        inline bool isFullExpended(){
            unique_readguard<WfirstRWLock> lk(child_mtx);
            return full_expended;
        }

        inline bool isFullChild(){

            return (child.size()==max_child_number);
        }

        actionType getOneUntriedAction(){
            assert(!isFullExpended());
            std::uniform_int_distribution<> rd(0, valid_act.size() - 1);
            int index = rd(gen);
            //std::cout << "size:" << valid_act.size() << "index:" << index << std::endl;
            pointType res = valid_act[index];
            valid_act.erase(valid_act.begin()+index);
            //std::cout << "size:" << valid_act.size() << std::endl;
            //full_expended = (valid_act.size()==0);
            return actionType(res,player);
        }



        inline bool isTerminal(){
            return is_terminal;
        }

        inline node* getParent(){
            return parent;
        }

        void addChild(nodeType* c){
            child.push_back(c);
            if(child.size()==max_child_number){
                unique_writeguard<WfirstRWLock> lk(child_mtx);;
                full_expended=true;
            }
        }

        inline board::Player getCurrentPlayer(){
            return player;
        }

        inline board::Player getNextPlayer(){
            return getOpponentPlayer(player);
        }

        inline int getMaxChild(){
            return max_child_number;
        }
	};



}

#endif //MCT_NODE_DEFINATION_HPP