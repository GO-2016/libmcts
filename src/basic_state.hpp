#ifndef MCT_STATE_DEFINATION_HPP
#define MCT_STATE_DEFINATION_HPP
#include <cstddef>
#include <iostream>
#include <vector>
#include <ctime>
#include <chrono>
#include <cstdlib>
#include "basic_action.hpp"
#include "basic_reward.hpp"
#include <board.hpp>
#include "cnn_v1.hpp"
/*
a class of state, should include:
final state or not?						is_terminal;
list of action.(tried or not)			A(s);
next state after one action				f(s,a);	
state evaluation?						eval(s)

*/
namespace mct{
	template<std::size_t W,std::size_t H>
	class State{
    private:
        using boardType = board::Board<W,H>;
        using actionType = Action<W,H>;
        using pointType = board::GridPoint<W,H>;
        using rewardType = Reward<W,H>;
        using Player = board::Player;
        boardType b;
        bool is_terminal;
        bool full;

        inline double sigmoid(double x)
        {
            double ans = 1 / (1+exp(-x));
            return ans;
        }

	public:
        State(const State<W,H> &other){
            b = other.b;
            is_terminal = other.is_terminal;
            full = other.full;
        }
        State(const boardType &other){
            b = other;
            is_terminal = (b.getAllGoodPosition(Player::B).size()==0 || b.getAllGoodPosition(Player::W).size()==0);
            full = (b.getStep()>150);
        }
        State(){
            //b = boardType();
            is_terminal = false;
        }

        ~State(){
            clear();
        }

        inline bool isTerminal(){
            return is_terminal;
        }

        void clear(){
            b.clear();
        }

        boardType& getBoard(){
            return b;
        }

        inline std::vector<pointType> getAllValidPosition(Player player){
            return b.getAllGoodPosition(player);
        }

        inline void doAction(actionType a){
            b.place(a.point,a.player);
            is_terminal = (b.getAllGoodPosition(Player::B).size()==0 || b.getAllGoodPosition(Player::W).size()==0);
        }
        /*
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
            double accum = 0.0;
            auto it = vp.begin();
            int cnt = 0;
            for(;it!=vp.end() && (accum < ACC_THRES || cnt < 2);++it){
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
        }*/

        void fastRollOut(Player player){
            auto start = std::chrono::steady_clock::now();
            srand(time(0));
            pointType p;
            int size,op_size,index,cnt=0;
            bool flag = false;
            Player opplayer = board::getOpponentPlayer(player);
            std::vector<pointType> act_list = b.getAllGoodPosition(player);
            std::vector<pointType> op_act_list = b.getAllGoodPosition(opplayer);
            while(act_list.size()>0 && op_act_list.size()>0 && cnt < 125) {
                if (cnt++ % 5==0){
                    act_list = b.getAllGoodPosition(player);
                    op_act_list = b.getAllGoodPosition(opplayer);
                    //cnt = 0;
                }
                size = act_list.size();
                op_size = op_act_list.size();
                if (size > 0) {
                    index = rand() % (size--);
                    p = act_list[index];
                    act_list.erase(act_list.begin() + index);
                    flag = true;
                    while (b.getPosStatus(p,player) != board::Board<W,H>::PositionStatus::OK || b.isEye(p,player)) {
                        if (act_list.size() == 0) {
                            flag = false;
                            break;
                        }
                        index = rand() % (size--);
                        p = act_list[index];
                        act_list.erase(act_list.begin() + index);
                    }
                    if (flag) b.place(p,player);
                    else break;
                }
                if (op_size > 0) {
                    index = rand() % (op_size--);
                    p = op_act_list[index];
                    op_act_list.erase(op_act_list.begin() + index);
                    flag = true;
                    while (b.getPosStatus(p,opplayer) != board::Board<W,H>::PositionStatus::OK || b.isEye(p,opplayer)) {
                        if (op_act_list.size() == 0) {
                            flag = false;
                            break;
                        }
                        index = rand() % (op_size--);
                        p = op_act_list[index];
                        op_act_list.erase(op_act_list.begin() + index);
                    }
                    if (flag) b.place(p,opplayer);
                    else break;
                }
            }
            auto end = std::chrono::steady_clock::now();

            //std::cout << "fast roll out cost time:" << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << "ms"<< std::endl;
        }



        rewardType getReward(){
            int Wnum=0;
            int Bnum=0;
            int Wlib=0;
            int Blib=0;
            double winnum;
            double score;
            if(full) {
                for (std::size_t j = 1; j < H; j++)
                    for (std::size_t i = 0; i < W; i++) {
                        pointType p(i, j);
                        if (b.getPointState(p) != board::PointState::NA) {
                            auto group = b.getPointGroup(p);
                            switch (group->getPlayer()) {
                                case Player::B:
                                    Bnum++;
                                    break;
                                case Player::W:
                                    Wnum++;
                                    break;
                            }
                        }
                    }
                winnum = Bnum - Wnum - 6.5;
                score = log(abs(winnum) + 1) + sigmoid(abs(winnum));
            }else{
                for (std::size_t j = 1; j < H; j++)
                    for (std::size_t i = 0; i < W; i++) {
                        pointType p(i, j);
                        if (b.getPointState(p) != board::PointState::NA) {
                            auto group = b.getPointGroup(p);
                            switch (group->getPlayer()) {
                                case Player::B:
                                    Bnum += (group->getLiberty()>2);
                                    break;
                                case Player::W:
                                    Wnum += (group->getLiberty()>2);
                                    break;
                            }
                        }
                    }
                winnum = Bnum - Wnum;
                score = sigmoid(abs(winnum));
            }

            if(winnum > 0) return rewardType(score,Player::B);
            else return rewardType(score,Player::W);
        }

        State<H,W>& operator=(const State<H,W> &other){
            b = other.b;
            return *this;
        };
	};
}


#endif //MCT_STATE_DEFINATION_H