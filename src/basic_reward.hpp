#ifndef MCT_REWARD_DEFINATION_HPP
#define MCT_REWARD_DEFINATION_HPP
#include <cstddef>
#include <board.hpp>
/*
a struct of reward, should include:
reward value(vector)					R;
the reward of Player p in each node v 	R(v,p);

*/

namespace mct{
	template<std::size_t W,std::size_t H>
	class Reward{
	public:
		using Player = board::Player;
		double val;
		Player winPlayer;
        Reward(double x=0,Player p = Player::W):val(x){
            winPlayer = p;
        }

		double getReward(Player p){
			return p==winPlayer?val:0;
		}
	};
}

#endif //MCT_REWARD_DEFINATION_HPP