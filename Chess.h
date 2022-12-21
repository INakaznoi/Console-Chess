//
// Created by ilya on 18.11.22.
//

#ifndef CONSOLE_CHESS2_CHESS_H
#define CONSOLE_CHESS2_CHESS_H
//
// Created by ilya on 18.11.22.
//

#include <vector>
#include <unordered_map>
#include <string>
#include <memory>

class Figure;
class Position;

enum Error_move{Normal,LeftCastling, RightCastling,Transform,EnPassant,Check,CheckMate,Pat,SeparateErrors,
    AlienFigure,UncorrectCastling,EmptyCage,KingAttack,OutOfBoard, AttackFigureSameColor,TwoKingClose, SameCage, FigureNotMove,InterfereAnotherFigure};

struct Position{
    int x;
    int y;
    Position operator-(const Position& other) const;
    Position operator+(const Position& other) const;
    bool operator==(const Position& other) const;
    Position operator*(int num) const;
    Position normalize() const;
};



enum FigureType{Figure_Rook, Figure_Knight, Figure_Bishop, Figure_Queen, Figure_King, Figure_Pawn};
enum Color{Black,White};

class Board;

class Figure{
    FigureType type_;
    Color color_;
    int number_used_ = 0;
public:
    Figure(FigureType type,Color color);
    virtual FigureType type() const;
    Color color() const;
    int number_used() const;
    void set_used(int now_step);
    virtual Error_move can_move(const Board& board ,int now_step, Position start_pos, Position final_pos) const = 0;
    virtual char representation() const = 0;
    virtual ~Figure();
};

class Pawn : public Figure{
public:
    Pawn(Color);
    Error_move can_move(const Board& board ,int now_step, Position start_pos, Position final_pos) const override;
    char representation() const override;
};

class Bishop : public Figure{
public:
    Error_move can_move(const Board& board ,int now_step, Position start_pos, Position final_pos) const override;
    Bishop(Color);
    char representation() const override;
};

class Knight : public Figure{
public:
    Error_move can_move(const Board& board ,int now_step, Position start_pos, Position final_pos) const override;
    Knight(Color );
    char representation() const override;
};

class Rook : public Figure{
public:
    Rook(Color );
    Error_move can_move(const Board& board ,int now_step, Position start_pos, Position final_pos) const override;
    char representation() const override;

};

class Queen :public Figure{
public:
    Queen(Color );
    Error_move can_move(const Board& board ,int now_step, Position start_pos, Position final_pos) const override;
    char representation() const override;
};

class King : public Figure{
public:
    King(Color );
    Error_move can_move(const Board& board ,int now_step, Position start_pos, Position final_pos) const override;
    char representation() const override;
};




namespace std{
    template <>
    class hash<Position>{
    public:
        size_t operator()(const Position& p) const {return p.x * 10 + p.y;}
    };
}


class  Board  : public std::unordered_map<Position,std::unique_ptr<Figure>>{
    using Base = std::unordered_map<Position,std::unique_ptr<Figure>>;
public:
    Figure& operator() (const Position& p) {
        auto it = Base::find(p);
        if (it == Base::end())
            throw std::exception();
        else
            return *it->second;
    }
    const Figure& operator() (const Position& p) const {
        auto it = Base::find(p);
        if (it == Base::end())
            throw std::exception();
        else
            return *it->second;
    }
    void motion(const Position& start, const Position& stop){
        auto it_start = Base::find(start);
        if (it_start == Base::end())
            throw std::exception();
        auto pair = std::move(*it_start);
        Base::insert_or_assign(stop, std::move(pair.second));
        Base::erase(start);
    }
};

class Referee{
    Board board_;
    Position white_king_pos;
    Position black_king_pos;
    int number_step_;
    std::vector<Board::value_type> values_save;
    bool is_pos_attack(const Position& pos_attack);
public:
    Referee();
    void print() const;
    int number_step() const;
    Error_move move(Position start_pos, Position final_pos);
    Error_move transform(FigureType, Position);
    bool do_mate();
    void motion(Position start_pos, Position final_pos);
    void remotion(Position start_pos, Position final_pos);

};




#endif //CONSOLE_CHESS2_CHESS_H
