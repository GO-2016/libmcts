#ifndef MCT_ACTION_DEFINATION_HPP
#define MCT_ACTION_DEFINATION_HPP

/*
a class of action, should include:


*/
#include <cstddef>
#include <board.hpp>
namespace mct{
	template<std::size_t W,std::size_t H>
	struct Action{
        using pointType = board::GridPoint<W,H>;
        using Player = board::Player;
        pointType point;
        bool pass;
        Player player;
        Action(pointType p, Player player1){
            point = p;
            player = player1;
            pass = false;
        }
        Action(const Action<W,H> &other){
            point = other.point;
            player = other.player;
            pass = other.pass;
        }
        Action(){
            point = pointType((char)0,(char)0);
            player = Player::W;
            pass = false;
        }
        Action(bool b){
            pass = b;
        }

        Action<W,H>& operator=(const Action<W,H> &other){
            point = other.point;
            player = other.player;
            return *this;
        };

	};
}

#endif //MCT_ACTION_DEFINATION_HPP