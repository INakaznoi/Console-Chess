//
// Created by ilya on 03.11.22.
//

#ifndef CONSOLE_CHESS_CHESS_H
#define CONSOLE_CHESS_CHESS_H
#include <array>
#include <iostream>

enum Error_move{Normal,LeftCastling, RightCastling,Transform,EnPassant,Check,SeparateErrors,
        UncorrectCastling,EmptyCage,KingAttack,OutOfBoard, FigureSameColor,TwoKingClose, SameCage, FigureNotMove,InterfereAnotherFigure};
enum Color{White,Black};

class Figure;
class Position;
//using BoardType = std::array<std::array<Figure*, 8>, 8>;


struct Position{
    int x;
    int y;
    Position operator-(const Position& other) const;
    Position operator+(const Position& other) const;
    bool operator==(const Position& other) const;
    Position operator*(int num) const;
    Position operator=(const Position&);
};

class BoardType: public std::array<std::array<Figure*, 8>, 8>{
public:
    Figure* operator()(const Position& p) const;
    Figure*& operator()(const Position& p);
};


class King;

class Referee{
    BoardType board_;
    King* king_white;
    King* king_black;
public:
    Referee();
    void print() const;
    Error_move move(Position start, Position final);
};

class Figure {
protected:
    Error_move clear_wey(const BoardType &board,const Position& final_pos) const;
    Color color_;
    Position position_;
    virtual Error_move specific_checks(const BoardType& board, const Position& final_pos) const = 0;
    bool used_ = false;
    bool pawn_last_jump_ = false;
public:
    Figure(Position pos, Color color);
    bool used() const;
    virtual char repres() const = 0;
    bool pawn_last_jump() const;
    Position position() const;
    Color color() const;
    Error_move can_move(const BoardType& board, const Position& final_pos) const;
    virtual void move(const Position& final_pos);
    void temp_move(const Position& final_pos);
};

class Pawn : public Figure{
public:
    Pawn(Position pos, Color color);
    void move(const Position& final_pos) override;
    Error_move specific_checks(const BoardType& board, const Position& final_pos) const  override;
    char repres() const override;
};

class Bishop : public Figure{
    Error_move specific_checks(const BoardType& board, const Position& final_pos) const override;
public:
    Bishop(Position pos, Color color);
    char repres() const override;
};

class Knight : public Figure{
    Error_move specific_checks(const BoardType& board, const Position& final_pos) const override;
public:
    Knight(Position pos, Color color);
    char repres() const override;
};

class Rook : public Figure{
    virtual Error_move specific_checks(const BoardType& board, const Position& final_pos) const override;
public:
    char repres() const override;
    Rook(Position pos, Color color);
};

class Queen :public Figure{

    Error_move specific_checks(const BoardType& board, const Position& final_pos) const override;
public:
    Queen(Position pos, Color color);
    char repres() const override;
};

class King : public Figure{
    Error_move specific_checks(const BoardType& board, const Position& final_pos) const override;
public:
    bool check = false;
    King(Position pos, Color color);
    char repres() const override;
};

#endif //CONSOLE_CHESS_CHESS_H
