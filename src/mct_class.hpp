#ifndef MCT_CLASS_HPP
#define MCT_CLASS_HPP
#include <cstddef>
#include "basic.hpp"
#include <math.h>
#include <sys/time.h>
#include <thread>
#include <chrono>
#include "cnn_v1.hpp"

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
                root->getCnn(b);
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
			//nodeType * Expend(nodeType * v,stateType st);
			nodeType * BestChild(nodeType * v,double c);
			rewardType DefaultPolicy(stateType s,Player p);
			void BackUp(nodeType * v, rewardType &r);
            typename nodeType::nodeStatus judgePoint(board::Board<W,H> &b, actionType a);

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
        int check_time = 2;
        auto start = std::chrono::steady_clock::now();
        std::chrono::milliseconds limit(1000*time_limit-200);
        int cnt = 0,check = 0;
        //double timedif;
        //std::cout << "begin tid:" << std::this_thread::get_id() << std::endl;
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
        //std::cout << "tid: " << std::this_thread::get_id() << " total_num: " << cnt << " time used: " << std::chrono::duration_cast<std::chrono::milliseconds>(cur - start).count() << std::endl;
    }


	/////////////////////////////////////
	template<std::size_t W,std::size_t H>
	Action<W,H> MCT<W,H>::MCTSearch(int time_limit){
        if(root_state.isTerminal()) return actionType(true);
        if(root_state.getBoard().getStep()==0) return actionType(pointType(3,3),root->getCurrentPlayer());
        std::vector<std::thread> threads;
        //std::chrono::milliseconds limit(1000*(time_limit-1));
        for(int i=0;i<thread_num;i++){
            threads.emplace_back(&MCT::single_thread, this, getRoot(),time_limit);
        }
        for(auto iter = threads.begin();iter!=threads.end();iter++){
            if(iter->joinable()) iter->join();
        }
        std::cout << "search finished, total number:" << N(root) << std::endl;
        if(root->child.size()==0) return actionType(true);
        nodeType * v = BestChild(root,0);
        std::cout << "get move: ";
        std::cout << (int)v->getAction().point.x << ' ' << (int)v->getAction().point.y << std::endl;
		return v->getAction();
	}

	template<std::size_t W,std::size_t H>
	node<W,H>* MCT<W,H>::TreePolicy(nodeType * v){
		nodeType * n=v,*tmp;
       // std::chrono::nanoseconds interval(10);
        double cp = 0.707;
        stateType level_state(root_state);
		while(!n->isTerminal()){
            if(!n->isGetCnn()){
                unique_writeguard<WfirstRWLock> lk(n->action_mtx);
                if(!n->isGetCnn()) n->getCnn(level_state.getBoard());
            }
            if(!n->isFullExpended()){
                unique_writeguard<WfirstRWLock> lk(n->action_mtx);
                //n->action_mtx.lock_write();
                if (!n->isFullExpended()){
                    //std::cout << std::hex << std::this_thread::get_id() << "::double check success\t" << (n==root) << std::endl;
                    actionType new_a = n->getOneUntriedAction();
                    auto status = judgePoint(level_state.getBoard(),new_a);
                    level_state.doAction(new_a);
                    nodeType * res = new nodeType(level_state,n->getNextPlayer(),n,new_a,level_state.isTerminal(),status);
                    n->addChild(res);
                    return res;
                    //return Expend(n,level_state);
                    //return n;
                }
            }else{
                unique_readguard<WfirstRWLock> lk(n->action_mtx);
                tmp = BestChild(n,cp);
                if(tmp != n) level_state.doAction(tmp->getAction());
                n = tmp;
            }
		}
        //std::cout << "terminal" << std::endl;
		return n;
	}
    /*
	template<std::size_t W,std::size_t H>
	node<W,H>* MCT<W,H>::Expend(nodeType * v,stateType st){

		actionType new_a = v->getOneUntriedAction();
        auto status = judgePoint(st,new_a);
		st.doAction(new_a);

		nodeType * res = new nodeType(st,v->getNextPlayer(),v,new_a,st.isTerminal(),status);
        v->addChild(res);

		return res;
	}
    */
    template<std::size_t W,std::size_t H>
    auto MCT<W,H>::judgePoint(board::Board<W,H> &b, actionType a)->typename nodeType::nodeStatus{
        //close eye
        if(b.isEye(a.point,a.player)) return nodeType::nodeStatus::NEVER;

        size_t x = a.point.x;
        size_t y = a.point.y;
        Player opplayer = board::getOpponentPlayer(a.player);
        int step = b.getStep();
        stateType st(b);
        st.doAction(a);
        if(st.getBoard().getPointGroup(a.point)->getLiberty()==1) return nodeType::nodeStatus::BAD;
        if(step<300 && st.getBoard().getPointGroup(a.point)->getLiberty()>10) return nodeType::nodeStatus::BAD;
        /*

        stt.doAction(a.changePlayer());
        if(x<W && stt.getBoard().isEye(pointType(x+1,y),opplayer)) return nodeType::nodeStatus::PREFER;
        if(x>0 && stt.getBoard().isEye(pointType(x-1,y),opplayer)) return nodeType::nodeStatus::PREFER;
        if(y<H && stt.getBoard().isEye(pointType(x,y+1),opplayer)) return nodeType::nodeStatus::PREFER;
        if(y>0 && stt.getBoard().isEye(pointType(x,y-1),opplayer)) return nodeType::nodeStatus::PREFER;
        */
        //if(b.isSemiEye(a.point,opplayer)) return nodeType::nodeStatus::BAD;
        size_t cW = 3;
        size_t cH = 3;
        size_t d;
        d=1;
        if(step < 30){
            if ((x >= cW - d && x <= cW + d) || (x >= W - cW - 1 - d && x <= W - cW - 1 + d)) {
                if ((y >= cH - d && y <= cH + d) || (y >= H - cH - 1 - d && y <= H - cH - 1 + d))
                    return nodeType::nodeStatus::PREFER;
            }
        }else if(step < 150){
            //corner or edge
            if ((x >= cW - d && x <= cW + d) || (x >= W - cW - 1 - d && x <= W - cW - 1 + d)) {
                if ((y >= cH - d && y <= cH + d) || (y >= H - cH - 1 - d && y <= H - cH - 1 + d))
                    return nodeType::nodeStatus::CORNER;
                else if (y > cH + d && y < H - cH - 1 - d) return nodeType::nodeStatus::EDGE;
            } else if (x > cW + d && x < W - cW - 1 - d)
                if ((y >= cH - d && y <= cH + d) || (y >= H - cH - 1 - d && y <= H - cH - 1 + d))
                    return nodeType::nodeStatus::EDGE;
        }


        //make eye
        if(step>200) {
            if (x < W && st.getBoard().isTrueEye(pointType(x + 1, y), a.player)) return nodeType::nodeStatus::PREFER;
            if (x > 0 && st.getBoard().isTrueEye(pointType(x - 1, y), a.player)) return nodeType::nodeStatus::PREFER;
            if (y < H && st.getBoard().isTrueEye(pointType(x, y + 1), a.player)) return nodeType::nodeStatus::PREFER;
            if (y > 0 && st.getBoard().isTrueEye(pointType(x, y - 1), a.player)) return nodeType::nodeStatus::PREFER;
        }




        return nodeType::nodeStatus::NORMAL;
    }

	template<std::size_t W,std::size_t H>
	node<W,H>* MCT<W,H>::BestChild(nodeType * v,double c){
        if(v->child.size()==0){
            v->change_terminal();
            return v;
        }
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
			if(mid > biggest){
				biggest = mid;
				res = p;
                //std::cout << (int)res->getAction().point.x << ' ' << (int)res->getAction().point.y << std::endl;
			}
		}
        if(res == NULL){
            std::mt19937 gen { std::random_device()() };
            int cnt = 0;
            do{
                std::uniform_int_distribution<> rd(0, v->child.size() - 1);
                int index = rd(gen);
                res = v->child[index];
            }while((res->status != nodeType::nodeStatus::PREFER) && (res->status == nodeType::nodeStatus::BAD)&& cnt++ < 5);
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
        //std::vector<pointType> act_list;
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