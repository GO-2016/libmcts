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
	public:
        State(const State<W,H> &other){
            b = other.b;
            is_terminal = other.is_terminal;
        }
        State(const boardType &other){
            b = other;
            is_terminal = (b.getAllGoodPosition(Player::B).size()==0 || b.getAllGoodPosition(Player::W).size()==0);
        }
        State(){
            b = boardType();
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

        boardType getBoard(){
            return b;
        }

        inline std::vector<pointType> getAllValidPosition(Player player){
            return b.getAllGoodPosition(player);
        }

        inline void doAction(actionType a){
            b.place(a.point,a.player);
            is_terminal = (b.getAllGoodPosition(Player::B).size()==0 || b.getAllGoodPosition(Player::W).size()==0);
        }

        void fastRollOut(Player player){
            auto start = std::chrono::steady_clock::now();
            srand(time(0));
            pointType p;
            int size,op_size,index,cnt=0;
            bool flag = false;
            Player opplayer = board::getOpponentPlayer(player);
            std::vector<pointType> act_list = b.getAllGoodPosition(player);
            std::vector<pointType> op_act_list = b.getAllGoodPosition(opplayer);
            while(act_list.size()>0 && op_act_list.size()>0 && cnt < 100) {
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
                    while (b.getPosStatus(p,player) != board::Board<W,H>::PositionStatus::OK) {
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
                    while (b.getPosStatus(p,opplayer) != board::Board<W,H>::PositionStatus::OK) {
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
            for(std::size_t j=1;j<H;j++) for(std::size_t i=0;i<W;i++){
                    pointType p(i,j);
                    if(b.getPointState(p) != board::PointState::NA) {
                        auto group = b.getPointGroup(p);
                        switch(group->getPlayer()){
                            case Player::B:
                                Bnum++;
                                break;
                            case Player::W:
                                Wnum++;
                                break;
                        }
                    }
                }
            //double score = log(abs(Wnum-Bnum));
            if(Wnum>Bnum) return rewardType(1,Player::W);
            else return rewardType(1,Player::B);
        }

        State<H,W>& operator=(const State<H,W> &other){
            b = other.b;
            return *this;
        };
	};
}


#endif //MCT_STATE_DEFINATION_H