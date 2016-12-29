//
// Created by shark on 16-12-14.
//
#include "mct_class.hpp"

int main(){
    /*
    board::Board<9,9> newboard;
    newboard.clear();
    newboard.place(board::GridPoint<9,9>(4,3),board::Player::W);
    newboard.place(board::GridPoint<9,9>(3,5),board::Player::W);
    newboard.place(board::GridPoint<9,9>(4,7),board::Player::W);
    newboard.place(board::GridPoint<9,9>(5,4),board::Player::W);
    newboard.place(board::GridPoint<9,9>(5,5),board::Player::W);
    newboard.place(board::GridPoint<9,9>(3,4),board::Player::W);
    newboard.place(board::GridPoint<9,9>(3,6),board::Player::W);
    newboard.place(board::GridPoint<9,9>(5,6),board::Player::W);
    newboard.place(board::GridPoint<9,9>(3,7),board::Player::W);
    newboard.place(board::GridPoint<9,9>(5,7),board::Player::B);
    //std::cout << newboard.getAllGoodPosition(board::Player::W).size() << std::endl;
    //std::cout << newboard.getAllGoodPosition(board::Player::B).size() << std::endl;
    newboard.place(board::GridPoint<9,9>(6,7),board::Player::W);
    //newboard.place(board::GridPoint<9,9>(3,4),board::Player::B);
    //newboard.place(board::GridPoint<9,9>(4,5),board::Player::B);
    //newboard.place(board::GridPoint<9,9>(4,3),board::Player::B);
    std::cout << newboard << std::endl;
    std::cout << newboard.isEye(board::GridPoint<9,9>(4,4),board::Player::W) << std::endl;
    std::cout << newboard.isEye(board::GridPoint<9,9>(4,4),board::Player::B) << std::endl;

    auto group = newboard.getPointGroup(board::GridPoint<9,9>(4,5));
    switch(group->getPlayer()){
        case board::Player::W:
            std::cout << "W" << std::endl;
            break;
        case board::Player::B:
            std::cout << "B" << std::endl;
            break;
        default:
            std::cout << "noGroup" << std::endl;
    }
    std::cout << (int)group->getLiberty() << std::endl;
    board::Board<9,9>::PositionStatus st = newboard.getPosStatus(board::GridPoint<9,9>(4,4),board::Player::W);
    switch(st){
        case board::Board<9,9>::PositionStatus::KO:
            std::cout << "KO" << std::endl;
            break;
        case board::Board<9,9>::PositionStatus::NOCHANGE:
            std::cout << "NOCHANGE" << std::endl;
            break;
        case board::Board<9,9>::PositionStatus::NOTEMPTY:
            std::cout << "NOTEMPTY" << std::endl;
            break;
        case board::Board<9,9>::PositionStatus::OK:
            std::cout << "OK" << std::endl;
            break;
        case board::Board<9,9>::PositionStatus::SUICIDE:
            std::cout << "SUICID" << std::endl;
            break;

    }
    st = newboard.getPosStatus(board::GridPoint<9,9>(4,4),board::Player::B);
    switch(st){
        case board::Board<9,9>::PositionStatus::KO:
            std::cout << "KO" << std::endl;
            break;
        case board::Board<9,9>::PositionStatus::NOCHANGE:
            std::cout << "NOCHANGE" << std::endl;
            break;
        case board::Board<9,9>::PositionStatus::NOTEMPTY:
            std::cout << "NOTEMPTY" << std::endl;
            break;
        case board::Board<9,9>::PositionStatus::OK:
            std::cout << "OK" << std::endl;
            break;
        case board::Board<9,9>::PositionStatus::SUICIDE:
            std::cout << "SUICID" << std::endl;
            break;

    }
    //std::cout << newboard << std::endl;
    */
    //board::Board<9,9> test = newboard;
    //std::vector<board::GridPoint<9,9>> v = test.getAllValidPosition(p);


    board::Player p = board::Player::W;
    board::Board<19,19> norm_board;
    norm_board.place(board::GridPoint<19,19>(5,7),board::Player::B);
    mct::MCT<19,19> tree(norm_board,p,8);
    mct::Action<19,19> a = tree.MCTSearch(10);
    tree.clear();

    return 0;
}