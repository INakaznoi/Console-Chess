#include <iostream>
#include <vector>
#include <array>
#include "Chess.h"

Position Position::operator-(const Position& other) const{
    return {x - other.x, y - other.y};
}

Position Position::operator+(const Position& other) const{
    return {x + other.x, y + other.y};
}

bool Position::operator==(const Position& other) const {
    return x == other.x && y == other.y;
}

Position Position::operator*(int num) const{
    return {x * num, y * num};
}

Position Position::operator=(const Position& p){
    x = p.x;
    y = p.y;
    return *this;
}

Figure* BoardType::operator()(const Position& p) const{
    return (*this)[p.x][p.y];
}

Figure*& BoardType::operator()(const Position& p){
    return (*this)[p.x][p.y];
}

    Referee::Referee() {
        for(auto& el : board_)
            for (auto& e :el)
                e = nullptr;

        auto fill_senior = [&](Color color) {
            auto y = (color == White) ? 0 : 7;
            for (int i = 0; i < 8; ++i) {
                Position pos = {i,y};
                if (i == 0 || i == 7)
                    board_[i][y] = new Rook(pos,color);
                if (i == 1 || i == 6)
                    board_[i][y] = new Knight(pos,color);
                if (i == 2 || i == 5)
                    board_[i][y] = new Bishop(pos,color);
                if (i == 3)
                    board_[i][y] = new Queen(pos, color);
                if (i == 4) {
                    if (color == White)
                        board_[i][y] = king_white = new King(pos, White);
                    else
                        board_[i][y] = king_black = new King(pos, Black);
                }
            }
            return;
        };
        fill_senior(White);
        fill_senior(Black);
        for (int i = 0 ; i < 8; ++i){
            board_[i][1] = new Pawn(Position{i, 1},White);
            board_[i][6] = new Pawn(Position{i,6},Black);
        }
    }

void Referee::print() const {
    for(auto& _ :board_)
        std::cout << '-';
    std::cout << std::endl;
    for (size_t y = 0; y < 8; ++y){
        for (size_t x = 0; x < 8; ++x) {
            if (board_[x][7 - y] != nullptr)
                std::cout << board_[x][7 - y]->repres();
            else
                std::cout << ' ';
            std::cout << '|';
        }
        std::cout << std::endl;
        for(auto& _ :board_)
            std::cout << '-';
        std::cout << std::endl;
    }
}

    Error_move Referee::move(Position start, Position final) {
        auto err = board_(start)->can_move(board_,final);
        auto start_figure = board_(start);
        if (err > SeparateErrors)
            return err;
        auto is_figure_attack = [this](Figure* figure) {
            bool ans = false;
            for (int x = 0; x < 8; ++x)
                for (int y = 0; y < 8; ++y)
                    if (board_({x, y}) != nullptr)
                        if (figure->color() != board_[x][y]->color())
                            ans |= board_[x][y]->can_move(board_, figure->position()) < SeparateErrors;
            return ans;
        };
        if (err == LeftCastling || err == RightCastling){
            if(is_figure_attack(start_figure))
                return UncorrectCastling;
            Position p_start = (err == LeftCastling)? Position{1,start.y} : Position{start.x + 1, start.y};
            Position p_stop = (err == LeftCastling)? start : Position{7, start.y};
            for (;p_start.x < p_stop.x; p_start = p_start + Position{1, 0}){
                if (board_(p_start) != nullptr)
                    return UncorrectCastling;
                board_(p_start) = new Bishop(p_start, start_figure->color());
                if (is_figure_attack(board_(p_start))) {
                    delete board_(p_start);
                    board_(p_start) = nullptr;
                    return UncorrectCastling;
                }
                delete board_(p_start);
                board_(p_start) = nullptr;
            }
            if (LeftCastling == err) {
                std::swap(board_(Position{0, start.y}), board_(Position{3, start.y}));
                board_(Position{3, start.y})->move(Position{3, start.y});
            }
            else{
                std::swap(board_(Position{7,start.y}),board_(Position{5, start.y}));
                board_(Position{5, start.y})->move(Position{5, start.y});
            }
            start_figure->move(final);
            board_(final) = start_figure;
            board_(start) = nullptr;
            if (is_figure_attack(start_figure->color() == White? king_black : king_white))
                return Check;
            return Normal;
        }
        auto cage_final = board_(final);
        board_(final) = board_(start);
        board_(start) = nullptr;
        board_(final)->temp_move(final);
        Figure* figure_save = nullptr;
        if (err == EnPassant)
            std::swap(figure_save,board_[final.x][start.y]);
        bool mine_king = is_figure_attack(king_white);
        bool alien_king = is_figure_attack(king_black);
        if (start_figure->color() == Black){
            std::swap(mine_king,alien_king);
        }
        if (err == EnPassant)
            std::swap(figure_save,board_[final.x][start.y]);
        auto diff = king_black->position() - king_white->position();
        board_(start) = board_(final);
        board_(final) = cage_final;
        board_(start)->temp_move(start);
        if (abs(diff.x) < 2 && abs(diff.y) < 2)
            return TwoKingClose;
        if (mine_king)
            return KingAttack;
        board_(final) = board_(start);
        board_(start) = nullptr;
        board_(final)->move(final);
        if (err == EnPassant)
            std::swap(figure_save,board_[final.x][start.y]);
        if (err == Transform)
            return err;
        else
            return alien_king? Check : Normal;
    }


    Error_move Figure::clear_wey(const BoardType &board,const Position& final_pos) const {
        Position diff = final_pos.operator-(position_);
        auto normalize = [](int a) {return a == 0? 0 : (a > 0) * 2 - 1;};
        Position norm = {normalize(diff.x), normalize(diff.y)};
        for (Position p = position_ + norm;
             p != final_pos ; p = p + norm) {
            if (board[p.x][p.y] != nullptr)
                return InterfereAnotherFigure;
        }
        return Normal;
    }

    Figure::Figure(Position pos, Color color): position_(pos), color_(color){}
    Position Figure::position() const {return position_;}
    Color Figure::color() const {return color_;}
    bool Figure::pawn_last_jump() const {return pawn_last_jump_;}
    bool Figure::used() const {return used_;}
    Error_move Figure::can_move(const BoardType& board, const Position& final_pos) const{
        if (this == nullptr)
            return EmptyCage;
        if (position_ == final_pos)
            return SameCage;
        auto in_interval= [](int x){return -1 < x && x < 8;};
        if (!(in_interval(position_.x) && in_interval(position_.y)
              && in_interval(final_pos.x) && in_interval(final_pos.y)))
            return OutOfBoard;
        if ((board[final_pos.x][final_pos.y] != nullptr) &&
        (board[position_.x][position_.y]->color() == board[final_pos.x][final_pos.y]->color()))
            return FigureSameColor;
        Error_move error = specific_checks(board, final_pos);
        if (error != Normal)
            return error;
        return Normal;
    }

    void Figure::temp_move(const Position& final_pos){
        position_ = final_pos;
    }

    void Figure::move(const Position& final_pos) {
        temp_move(final_pos);
        used_ = true;
    }
    //Error_move chess_move(const Position& start_pos, const Position& final_pos)


    Pawn::Pawn(Position pos, Color color): Figure(pos,color){}
    void Pawn::move(const Position& final){
        pawn_last_jump_ = abs(final.y - position().y) == 2;
        Figure::move(final);
}
    Error_move Pawn::specific_checks(const BoardType& board, const Position& final_pos) const  {
        auto final_cage = board[final_pos.x][final_pos.y];
        int flag_color = (color() == White)? 1 : -1;
        auto diff= (final_pos - position()) * flag_color;
        auto rez = Normal;
        if (diff.x == 0){
            if (board(final_pos) != nullptr)
                return InterfereAnotherFigure;
            if ((position().y == (color() == White ? 1 : 6)) && (diff.y == 2) && (clear_wey(board, final_pos) == Normal))
                return Normal;
            if (diff.y != 1)
                return FigureNotMove;
            if (final_pos.y == (color() == White)? 7 : 0)
                return Transform;
            return Normal;
        }
        else{
            if (!(diff.y == 1 && abs(diff.x) == 1))
                return FigureNotMove;
            if (final_cage == nullptr){
                auto cage_attack = board({final_pos.x, position().y});
                if (cage_attack != nullptr && cage_attack->pawn_last_jump())
                    return EnPassant;
                else
                    return FigureNotMove;
            }
            if (final_pos.y == (color() == White)? 7 : 0)
                return Transform;
            return Normal;
        }
    }
    char Pawn::repres() const {return color() == White? 'P' :'p';}

    Error_move Bishop::specific_checks(const BoardType& board, const Position& final_pos) const {
        Position diff = final_pos - position();
        if (abs(diff.x) != abs(diff.y))
            return FigureNotMove;
        return clear_wey(board, final_pos);
    }
    Bishop::Bishop(Position pos, Color color): Figure(pos,color){}
    char Bishop::repres() const {return color_ == White? 'B' :'b';}

    Error_move Knight::specific_checks(const BoardType& board, const Position& final_pos) const {
        Position diff = final_pos - position_;
        if (abs(diff.x * diff.y) == 2)
            return Normal;
        else
            return FigureNotMove;
    }
    Knight::Knight(Position pos, Color color): Figure(pos,color){}
    char Knight::repres() const {return color_ == White? 'N' :'n';}


     Error_move Rook::specific_checks(const BoardType& board, const Position& final_pos) const{
        if (position_.x == final_pos.x || position_.y == final_pos.y)
            return clear_wey(board, final_pos);
        return FigureNotMove;
    }
    char Rook::repres() const {return color_ == White? 'R' :'r';}
    Rook::Rook(Position pos, Color color): Figure(pos,color) {}

    Error_move Queen::specific_checks(const BoardType& board, const Position& final_pos) const {
        Position diff = final_pos - position_;
        if (abs(diff.x) == abs(diff.y) || !diff.x || !diff.y)
            return clear_wey(board, final_pos);
        else
            return FigureNotMove;
    }
    Queen::Queen(Position pos, Color color): Figure(pos,color){}
    char Queen::repres() const {return color_ == White? 'Q' :'q';}

    Error_move King::specific_checks(const BoardType& board, const Position& final_pos) const{
        Position diff = final_pos - position_;
        bool right_cast = diff.x > 0;
        if (!used() && diff.y == 0 && abs(diff.x) == 2 && !board(final_pos + Position{diff.x - right_cast,0})->used()){
            return right_cast? RightCastling : LeftCastling;
        }
        if (diff != Position{0,0} && abs(diff.x) < 2 && abs(diff.y) < 2)
            return Normal;
        return FigureNotMove;

    }
    King::King(Position pos, Color color): Figure(pos,color){
        check = false;
    };
    char King::repres() const  {return color_ == White? 'K' :'k';}


    void Chess(){
        Referee referee;
        while(1){
            int x1,y1,x2,y2;
            referee.print();
            std::cin >> x1 >> y1 >> x2 >> y2;
            std::cout << std::endl << referee.move({x1,y1},{x2,y2}) << std::endl;
        }
    }





