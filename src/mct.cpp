//
// Created by shark on 16-12-14.
//
#include "mct_class.hpp"

int main(){
    /*
    board::Board<3,3> newboard;
    newboard.clear();
    newboard.place(board::GridPoint<3,3>(0,1),board::Player::W);
    newboard.place(board::GridPoint<3,3>(1,0),board::Player::W);
    newboard.place(board::GridPoint<3,3>(1,2),board::Player::W);
    newboard.place(board::GridPoint<3,3>(2,1),board::Player::W);
    newboard.place(board::GridPoint<3,3>(0,0),board::Player::B);
    newboard.place(board::GridPoint<3,3>(2,0),board::Player::B);
    newboard.place(board::GridPoint<3,3>(2,2),board::Player::B);
    newboard.place(board::GridPoint<3,3>(0,2),board::Player::B);
    std::cout << newboard.getAllGoodPosition(board::Player::W).size() << std::endl;
    std::cout << newboard.getAllGoodPosition(board::Player::B).size() << std::endl;
    //newboard.place(board::GridPoint<9,9>(5,4),board::Player::B);
    //newboard.place(board::GridPoint<9,9>(3,4),board::Player::B);
    //newboard.place(board::GridPoint<9,9>(4,5),board::Player::B);
    //newboard.place(board::GridPoint<9,9>(4,3),board::Player::B);
    //std::cout << newboard.isEye(board::GridPoint<9,9>(4,4),board::Player::W) << std::endl;

    auto group = newboard.getPointGroup(board::GridPoint<9,9>(4,4));
    std::cout << (group->getPlayer() == board::Player::B) << std::endl;
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

    //board::Board<9,9> test = newboard;
    //std::vector<board::GridPoint<9,9>> v = test.getAllValidPosition(p);
    */

    board::Player p = board::Player::W;
    board::Board<5,5> test_board;
    board::Board<3,3> small_board;
    board::Board<9,9> nine_board;
    board::Board<19,19> norm_board;
    //mct::node<3,3> test_node(test_board,p);

    test_board.place(board::GridPoint<5,5>(0,0),board::Player::W);
    test_board.place(board::GridPoint<5,5>(0,1),board::Player::W);
    test_board.place(board::GridPoint<5,5>(0,2),board::Player::W);
    test_board.place(board::GridPoint<5,5>(0,3),board::Player::W);
    test_board.place(board::GridPoint<5,5>(0,4),board::Player::B);
    test_board.place(board::GridPoint<5,5>(1,0),board::Player::B);
    test_board.place(board::GridPoint<5,5>(1,1),board::Player::B);
    test_board.place(board::GridPoint<5,5>(1,2),board::Player::B);
    test_board.place(board::GridPoint<5,5>(1,3),board::Player::W);
    test_board.place(board::GridPoint<5,5>(1,4),board::Player::W);
    test_board.place(board::GridPoint<5,5>(2,0),board::Player::W);
    test_board.place(board::GridPoint<5,5>(2,1),board::Player::W);
    test_board.place(board::GridPoint<5,5>(2,3),board::Player::B);
    test_board.place(board::GridPoint<5,5>(2,4),board::Player::B);
    test_board.place(board::GridPoint<5,5>(3,0),board::Player::B);
    test_board.place(board::GridPoint<5,5>(3,1),board::Player::B);
    test_board.place(board::GridPoint<5,5>(3,2),board::Player::W);
    test_board.place(board::GridPoint<5,5>(3,3),board::Player::W);
    test_board.place(board::GridPoint<5,5>(3,4),board::Player::W);
    test_board.place(board::GridPoint<5,5>(4,0),board::Player::W);
    test_board.place(board::GridPoint<5,5>(4,1),board::Player::B);
    test_board.place(board::GridPoint<5,5>(4,2),board::Player::B);
    test_board.place(board::GridPoint<5,5>(4,3),board::Player::B);
    test_board.place(board::GridPoint<5,5>(4,4),board::Player::B);
    std::cout << test_board << std::endl;

    //test_board.clear();
    mct::MCT<19,19> tree(norm_board,p,8);
    //mct::node<3,3>* tmp = tree.getRoot();
    //std::cout << tmp->getMaxChild() << std::endl;
    //while(i++<10000) tmp = tree.TreePolicy(tmp);
    mct::Action<19,19> a = tree.MCTSearch(3);

    return 0;
}