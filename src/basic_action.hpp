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
        Player player;
        Action(pointType p, Player player1){
            point = p;
            player = player1;
        }
        Action(const Action<W,H> &other){
            point = other.point;
            player = other.player;
        }
        Action(){
            point = pointType((char)0,(char)0);
            player = Player::W;
        }

        Action<W,H>& operator=(const Action<W,H> &other){
            point = other.point;
            player = other.player;
            return *this;
        };

	};
}

#endif //MCT_ACTION_DEFINATION_HPP