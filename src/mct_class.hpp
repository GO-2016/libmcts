#ifndef MCT_CLASS_HPP
#define MCT_CLASS_HPP
#include <cstddef>
#include "basic.hpp"
#include <math.h>
#include <sys/time.h>
#include <thread>
#include <chrono>

namespace mct{
	template<std::size_t W,std::size_t H>
	class MCT{
		private:
			node<W,H>* root;
            int thread_num;
            State<W,H> root_state;
		public:
			using nodeType = node<W,H>;
			using stateType = State<W,H>;
			using actionType = Action<W,H>;
			using rewardType = Reward<W,H>;
            using pointType = board::GridPoint<W,H>;
			using Player = board::Player;
			MCT(board::Board<W,H> &b, Player p,int thread = 4){
				root =  new node<W,H>(b,p);
                root_state = stateType(b);
                thread_num = thread;
			}

            ~MCT(){
                clear();
            }

            void clear(){
                root->clear();
                root_state.clear();
            }
            void run(nodeType* rt);
            void single_thread(nodeType* v,int time_limit);
			actionType MCTSearch(int time_limit);
			nodeType * TreePolicy(nodeType * v);
			nodeType * Expend(nodeType * v,stateType st);
			nodeType * BestChild(nodeType * v,double c);
			rewardType DefaultPolicy(stateType s,Player p);
			void BackUp(nodeType * v, rewardType &r);
            typename nodeType::nodeStatus judgePoint(stateType st, actionType a);

            inline nodeType * getRoot(){
                return root;
            }


            inline int N(nodeType* v){
                return v->getNumber();
            }

            inline double Q(nodeType* v){
                //int mid = v->getQuality();
                return (double)v->getQuality()/4096;
            }

            State<W,H> S(nodeType* v){
                std::vector<actionType> acts;
                while(v->getParent() != NULL){
                    acts.push_back(v->getAction());
                    v = v->getParent();
                }
                stateType res(root_state);
                while(!acts.empty()){
                    res.doAction(acts.back());
                    acts.pop_back();
                }
                //std::cout << 1 << std::endl;
                return res;
            }

            State<W,H> S(nodeType* v, actionType a){
                std::vector<actionType> acts;
                while(v->getParent() != NULL){
                    acts.push_back(v->getAction());
                    v = v->getParent();
                }
                stateType res(root_state);
                while(!acts.empty()){
                    res.doAction(acts.back());
                    acts.pop_back();
                }
                res.doAction(a);
                //std::cout << 1 << std::endl;
                return res;
            }

            inline Action<W,H> A(nodeType* v){
                return v->getAction();
            }
	};

    template<std::size_t W,std::size_t H>
    void MCT<W,H>::run(nodeType* rt){
        //std::cout << 1 << std::endl;
        nodeType * v;
        rewardType r;

        //auto start = std::chrono::steady_clock::now();
        v = TreePolicy(rt);
        //auto tree_time = std::chrono::steady_clock::now();
        //std::cout << "tree policy time:" << std::chrono::duration_cast<std::chrono::milliseconds>(tree_time - start).count() << "ms" << std::endl;

        r = DefaultPolicy(S(v),v->getCurrentPlayer());
        //auto default_time = std::chrono::steady_clock::now();
        //std::cout << "tree policy time:" << std::chrono::duration_cast<std::chrono::milliseconds>(default_time - tree_time).count() << "ms" << std::endl;

        BackUp(v,r);
        //auto backup_time = std::chrono::steady_clock::now();
        //std::cout << "tree policy time:" << std::chrono::duration_cast<std::chrono::milliseconds>(backup_time - default_time).count() << "ms" << std::endl;
    }

    template<std::size_t W,std::size_t H>
    void MCT<W,H>::single_thread(nodeType * v, int time_limit){
        int check_time = 5;
        auto start = std::chrono::steady_clock::now();
        std::chrono::milliseconds limit(1000*(time_limit-1));
        int cnt = 0,check = 0;
        //double timedif;
        std::cout << "begin tid:" << std::this_thread::get_id() << std::endl;
        while(1){
            check++;
            if(check%check_time == 0) {
                auto cur = std::chrono::steady_clock::now();
                if (std::chrono::duration_cast<std::chrono::milliseconds>(cur - start) > limit) break;
            }
            cnt++;
            //std::cout << "tid: " << std::this_thread::get_id() << " total_num: " << cnt << std::endl;
            run(v);
        }
        auto cur = std::chrono::steady_clock::now();
        std::cout << "tid: " << std::this_thread::get_id() << " total_num: " << cnt << " time used: " << std::chrono::duration_cast<std::chrono::milliseconds>(cur - start).count() << std::endl;
    }


	/////////////////////////////////////
	template<std::size_t W,std::size_t H>
	Action<W,H> MCT<W,H>::MCTSearch(int time_limit){
        if(root_state.isTerminal()) return actionType(true);
        std::vector<std::thread> threads;
        //std::chrono::milliseconds limit(1000*(time_limit-1));
        for(int i=0;i<thread_num;i++){
            threads.emplace_back(&MCT::single_thread, this, getRoot(),time_limit);
        }
        for(auto iter = threads.begin();iter!=threads.end();iter++){
            if(iter->joinable()) iter->join();
        }
        std::cout << "search finished, total number:" << N(root) << std::endl;
        nodeType * v = BestChild(root,0);
        std::cout << "finished" << std::endl;
        std::cout << (int)v->getAction().point.x << ' ' << (int)v->getAction().point.y << std::endl;
		return v->getAction();
	}

	template<std::size_t W,std::size_t H>
	node<W,H>* MCT<W,H>::TreePolicy(nodeType * v){
		nodeType * n=v;
       // std::chrono::nanoseconds interval(10);
        double cp = 0.707;
        stateType level_state(root_state);
		while(!n->isTerminal()){
            if(!n->isFullExpended()){
                unique_writeguard<WfirstRWLock> lk(n->action_mtx);
                //n->action_mtx.lock_write();
                if (!n->isFullExpended()){
                    //std::cout << std::hex << std::this_thread::get_id() << "::double check success\t" << (n==root) << std::endl;
                    actionType new_a = n->getOneUntriedAction();
                    //n->action_mtx.release_write();
                    auto status = judgePoint(level_state,new_a);
                    level_state.doAction(new_a);
                    nodeType * res = new nodeType(level_state,n->getNextPlayer(),n,new_a,level_state.isTerminal(),status);
                    n->addChild(res);
                    return res;
                    //return Expend(n,level_state);
                    //return n;
                }else {
                    //std::cout << std::hex << std::this_thread::get_id() << "::double check failed" << std::endl;
                }
            }else{
                unique_readguard<WfirstRWLock> lk(n->action_mtx);
                n = BestChild(n,cp);
                level_state.doAction(n->getAction());
            }
		}
        //std::cout << "terminal" << std::endl;
		return n;
	}

	template<std::size_t W,std::size_t H>
	node<W,H>* MCT<W,H>::Expend(nodeType * v,stateType st){

		actionType new_a = v->getOneUntriedAction();
        auto status = judgePoint(st,new_a);
		st.doAction(new_a);

		nodeType * res = new nodeType(st,v->getNextPlayer(),v,new_a,st.isTerminal(),status);
        v->addChild(res);

		return res;
	}

    template<std::size_t W,std::size_t H>
    auto MCT<W,H>::judgePoint(stateType st, actionType a)->typename nodeType::nodeStatus{
        auto group1 = st.getBoard().getPointGroup(a.point);
        st.doAction(a);
        auto group2 = st.getBoard().getPointGroup(a.point);
        return nodeType::nodeStatus::NORMAL;
    }

	template<std::size_t W,std::size_t H>
	node<W,H>* MCT<W,H>::BestChild(nodeType * v,double c){
        //std::cout << std::hex << std::this_thread::get_id()<< "::get child  " << v->child.size() << std::endl;
		nodeType * res = NULL,*p = NULL;
		double biggest = -2e5;
		double mid;
		for(auto p : v->child){
			//p = iter;
            if(N(p)==0) continue;
            //std::cout << std::hex << std::this_thread::get_id()<< std::dec << Q(p) << '|' << N(p) << '|' << N(v)<< std::endl;
            assert(N(p)>0);
            //std::cout << log(N(v)) << std::endl;
			mid = Q(p)/N(p)+c*sqrt(2*log(N(v))/N(p));
            //std::cout << std::hex << std::this_thread::get_id() << std::dec << mid << std::endl;
			if(mid > biggest && p->status != nodeType::nodeStatus::BAD){
				biggest = mid;
				res = p;
                //std::cout << (int)res->getAction().point.x << ' ' << (int)res->getAction().point.y << std::endl;
			}
		}
        if(res == NULL){
            std::mt19937 gen { std::random_device()() };
            do{
                std::uniform_int_distribution<> rd(0, v->child.size() - 1);
                int index = rd(gen);
                res = v->child[index];
            }while(res->status == nodeType::nodeStatus::BAD);
        }
        //std::cout << std::hex << std::this_thread::get_id()<< "::found child  " << res->isTerminal()<< std::endl;
        if(c == 0){
            std::cout << "num:" << N(res) << std::endl;
            std::cout << "quality:" << Q(res) << std::endl;
        }
        assert(res != NULL);
        return res;
	}

	//////////////////////////////////////////
	template<std::size_t W,std::size_t H>
	Reward<W,H> MCT<W,H>::DefaultPolicy(stateType s, Player p){
        std::vector<pointType> act_list;
        s.fastRollOut(p);
        return s.getReward();
        //return rewardType(0.5);
	}

	template<std::size_t W,std::size_t H>
	void MCT<W,H>::BackUp(nodeType * v, rewardType &r){
		nodeType * p = v;
		while(p != NULL){
			p->increaseNumber();
			p->increaseQuality(r.getReward(p->getAction().player));
			p = p->getParent();
		}
	}
}

#endif //MCT_CLASS_HPP