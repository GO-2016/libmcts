#ifndef MCT_NODE_DEFINATION_HPP
#define MCT_NODE_DEFINATION_HPP
#include <iostream>
#include <cstddef>
#include <vector>
#include <cstdlib>
#include <mutex>
#include <atomic>
#include <random>
#include "basic_action.hpp"
#include "basic_state.hpp"
#include "cnn_v1.hpp"
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
        bool get_cnn;
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
        enum struct nodeStatus{
            BAD,
            NEVER,    //never do this
            NORMAL, //normal place
            CORNER, //place at corner
            EDGE,   //place at edge
            PREFER  //prefer to do this
        };
        nodeStatus status;

		node(const board::Board<W,H> &b,Player p):number(0),quality(0){
			parent = NULL;
			act = actionType();
            //stateType s(b);
            player = p;
            //std::vector<pointType> mid = state.getAllValidPosition(p);
            //for(auto iter = mid.cbegin();iter != mid.cend();iter++) valid_act.push_back(*iter);
            //valid_act = getCNNGoodPositions(b,p);
           // max_child_number = valid_act.size();
            //s.clear();
            //std::cout << valid_act.size() << std::endl;
            //full_expended = (valid_act.size()==0);
            is_terminal = false;
            get_cnn = false;
		}
        node(stateType &s,Player p,nodeType* par,actionType action,bool t,nodeStatus st):number(0),quality(0){
            parent = par;
            //state = s;
            act = action;
            player = p;
            //valid_act = s.getAllValidPosition(p);
            //max_child_number = valid_act.size();
            full_expended = false;
            is_terminal = t;
            status = st;
            get_cnn = false;
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
            //std::cout << "clear" << std::endl;
            clear();
		}

        void clear(){
            nodeType * p;
            while(!child.empty()){
                p = child.back();
                child.pop_back();
                p->clear();
                delete p;
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
            switch(status){
                case nodeStatus::BAD:
                    q *= 0.9;
                    break;
                case nodeStatus::NORMAL:
                    q *= 1;
                    break;
                case nodeStatus::EDGE:
                    q *= 1.05;
                    break;
                case nodeStatus::CORNER:
                    q *= 1.1;
                    break;
                case nodeStatus::PREFER:
                    q *= 1.15;
                    break;
            }
            quality += (int)(q*4096);
        }

        /*stateType getState(){
            return state;
        }*/

        inline actionType getAction(){
            return act;
        }

        inline bool isFullExpended(){
            //unique_readguard<WfirstRWLock> lk(child_mtx);
            return full_expended;
        }

        inline bool isFullChild(){

            return (child.size()==max_child_number);
        }

        actionType getOneUntriedAction(){
            pointType res = valid_act.back();
            valid_act.pop_back();
            return actionType(res,player);
        }

        bool isGetCnn(){
            return get_cnn;
        }

        void getCnn(board::Board<W,H> &b){
            valid_act = getCNNGoodPositions(b,player);
            max_child_number = valid_act.size();
            full_expended = (valid_act.size()==0);
            is_terminal = is_terminal | (max_child_number==0);
            get_cnn = true;
        }

        inline bool isTerminal(){
            return is_terminal;
        }

        inline node* getParent(){
            return parent;
        }

        void addChild(nodeType* c){
            if(c->status == nodeStatus::NEVER) max_child_number--;
            else child.push_back(c);
            if(child.size()==max_child_number){
                unique_writeguard<WfirstRWLock> lk(child_mtx);;
                full_expended=true;
            }
        }

        void change_terminal(){
            is_terminal = true;
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
        /*
        RequestV1Service reqv1Service;
        auto getCNNGoodPositions(board::Board<W, H> &b, Player player) -> std::vector<pointType> {
            auto requestV1 = b.generateRequestV1(player);
            auto resp = reqv1Service.sync_call(requestV1);
            auto &possibility = *resp.mutable_possibility();
            using PairT = std::pair<pointType, double>;
            std::vector<PairT> vp;
            vp.reserve(W * H);
            for (std::size_t i=0; i<possibility.size(); ++i)
                vp.emplace_back(pointType(i / H, i % H), possibility.data()[i]);
            std::sort(vp.begin(), vp.end(), [](const PairT &a, const PairT &b) {
                return a.second > b.second;
            }); // vp: possibility large -> small

            auto goodPosVec = b.getAllGoodPosition(player);
            const double ACCUM_THRES = b.getStep() > 100 ? (b.getStep() > 200 ? 0.95 : 0.87): 0.8;
            double accum = 0.0;
            auto it = vp.begin();
            int cnt = 0;
            for (; it != vp.end() && (accum < ACCUM_THRES || cnt < 2); ++it)
            {
                if (std::find(goodPosVec.begin(), goodPosVec.end(), it->first) != goodPosVec.end()) {
                    accum += it->second;
                    ++cnt;
                }
            }
            vp.erase(it, vp.end());

            std::vector<pointType> ans; ans.reserve(W * H);
            std::for_each(vp.rbegin(), vp.rend(), [&](const PairT &p) {
                if (b.getPosStatus(p.first, player) == board::Board<W, H>::PositionStatus::OK && std::find(goodPosVec.begin(), goodPosVec.end(), p.first) != goodPosVec.end())
                    ans.push_back(p.first);
            }); // ans: small to large
            return ans;
        }
        */

        RequestV2Service reqv2Service;
        auto getCNNGoodPositions(board::Board<W, H> &b, Player player) -> std::vector<pointType> {
            auto requestV2 = b.generateRequestV2(player);
            auto resp = reqv2Service.sync_call(requestV2);
            auto &possibility = *resp.mutable_possibility();
            using PairT = std::pair<pointType, double>;
            std::vector<PairT> vp;
            vp.reserve(W * H);
            for(std::size_t i=0;i<possibility.size();++i) vp.emplace_back(pointType(i/H, i%H), possibility.data()[i]);
            std::sort(vp.begin(), vp.end(), [](const PairT &a, const PairT &b) {
                return a.second > b.second;
            }); // vp: possibility large -> small

            auto goodPosVec = b.getAllGoodPosition(player);
            const double ACC_THRES = b.getStep() > 100 ? (b.getStep() > 200 ? 0.9 : 0.8): 0.7;
            const double AVG_ACC = double(1)/361;
            double accum = 0.0;
            auto it = vp.begin();
            int cnt = 0;
            for(;it!=vp.end() && (accum < ACC_THRES || cnt < 2) && it->second > AVG_ACC;++it){
                if (std::find(goodPosVec.begin(), goodPosVec.end(), it->first) != goodPosVec.end()){
                    accum += it->second;
                    ++cnt;
                }
            }
            vp.erase(it, vp.end());

            std::vector<pointType> ans; ans.reserve(W * H);
            std::for_each(vp.rbegin(), vp.rend(), [&](const PairT &p){
                if (b.getPosStatus(p.first, player) == board::Board<W, H>::PositionStatus::OK &&
                        std::find(goodPosVec.begin(), goodPosVec.end(), p.first) != goodPosVec.end())
                    ans.push_back(p.first);
            }); // ans: small to large
            return ans;
        }
	};



}

#endif //MCT_NODE_DEFINATION_HPP