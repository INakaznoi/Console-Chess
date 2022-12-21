#include "Chess.h"
#include <iostream>
#include <stack>


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

Position  Position::normalize() const{
    auto norm = [](int a) {return a == 0? 0 : (a > 0) * 2 - 1;};
    return {norm(x), norm(y)};
}

Figure::Figure(FigureType type, Color color): type_(type), color_(color){}

FigureType Figure::type() const{
    return type_;
}

Color Figure::color() const{
    return color_;
}

int Figure::number_used() const{
    return number_used_;
}
void Figure::set_used(int now_step) {
    number_used_ = now_step;
}

Figure::~Figure()= default;

Error_move clear_wey(const Board &board,const Position& start_pos,const Position& final_pos) {
    Position diff = final_pos.operator-(start_pos);
    Position norm = diff.normalize();
    for (Position p = start_pos + norm;
         p != final_pos ; p = p + norm) {
        if (board.find(p) != board.end())
            return InterfereAnotherFigure;
    }
    return Normal;
}

Pawn::Pawn(Color color): Figure(Figure_Pawn, color){}

Error_move Pawn_can_move(const Board& board,int now_step, Position start_pos, Position final_pos){
    const auto& f_start = board(start_pos);
    int flag_color = (f_start.color() == White)? 1 : -1;
    auto diff= (final_pos - start_pos) * flag_color;
    if (diff.x == 0){
        if (board.find(final_pos) != board.end())
            return InterfereAnotherFigure;
        if ((start_pos.y == (f_start.color() == White ? 1 : 6)) && (diff.y == 2) && (clear_wey(board,start_pos, final_pos) == Normal))
            return Normal;
        if (diff.y != 1)
            return FigureNotMove;
        if (final_pos.y == (f_start.color() == White? 7 : 0))
            return Transform;
        return Normal;
    }
    else{
        if (!(diff.y == 1 && abs(diff.x) == 1))
            return FigureNotMove;
        if (board.find(final_pos) == board.end()){
            auto cage_attack = board.find({final_pos.x, start_pos.y});
            if (cage_attack != board.end() && cage_attack->second->number_used() + 1 == now_step && cage_attack->second->type() == Figure_Pawn)
                return EnPassant;
            else
                return FigureNotMove;
        }
        if (final_pos.y == ((f_start.color() == White)? 7 : 0))
            return Transform;
        return Normal;
    }
}

Error_move Pawn::can_move(const Board & board, int now_step, Position start_pos, Position final_pos) const {
    return Pawn_can_move(board,now_step, start_pos, final_pos);
}

char Pawn::representation() const{
    return (color() == White)? 'P' : 'p';
}



Error_move Bishop_can_move(const Board& board, Position start_pos, Position final_pos) {
    Position diff = final_pos - start_pos;
    if (abs(diff.x) != abs(diff.y))
        return FigureNotMove;
    return clear_wey(board,start_pos,final_pos);
}

Bishop::Bishop(Color color): Figure(Figure_Bishop, color){}

Error_move Bishop::can_move(const Board & board, int now_step, Position start_pos, Position final_pos) const {
    return Bishop_can_move(board, start_pos, final_pos);
}

char Bishop::representation() const{
    return (color() == White)? 'B' : 'b';
}
Error_move Knight_can_move(const Board& board, Position start_pos, Position final_pos) {
    Position diff = final_pos - start_pos;
    if (abs(diff.x * diff.y) == 2)
        return Normal;
    else
        return FigureNotMove;
}

Knight::Knight(Color color): Figure(Figure_Knight, color){}

Error_move Knight::can_move(const Board & board, int now_step, Position start_pos, Position final_pos) const {
    return Knight_can_move(board, start_pos, final_pos);
}

char Knight::representation() const{
    return (color() == White)? 'N' : 'n';
}

Error_move Rook_can_move(const Board& board, Position start_pos, Position final_pos) {
    if (start_pos.x == final_pos.x || start_pos.y == final_pos.y)
        return clear_wey(board,start_pos,final_pos);
    return FigureNotMove;
}


Rook::Rook(Color color): Figure(Figure_Rook, color){}

Error_move Rook::can_move(const Board & board, int now_step, Position start_pos, Position final_pos) const {
    return Rook_can_move(board, start_pos, final_pos);
}

char Rook::representation() const{
    return (color() == White)? 'R' : 'r';
}

Error_move Queen_can_move(const Board& board, Position start_pos, Position final_pos) {
    Position diff = final_pos - start_pos;
    if (abs(diff.x) == abs(diff.y) || !diff.x || !diff.y)
        return clear_wey(board, start_pos, final_pos);
    else
        return FigureNotMove;
}


Queen::Queen(Color color): Figure(Figure_Queen, color){}

Error_move Queen::can_move(const Board & board, int now_step, Position start_pos, Position final_pos) const {
    return Queen_can_move(board, start_pos, final_pos);
}

char Queen::representation() const{
    return (color() == White)? 'Q' : 'q';
}

Error_move King_can_move(const Board& board, Position start_pos, Position final_pos){
    Position diff = final_pos - start_pos;
    bool right_cast = diff.x > 0;
    auto cage_rock = board.find(final_pos + Position{diff.x - right_cast, 0});
    if (!board(start_pos).number_used() &&
    diff.y == 0 &&
    abs(diff.x) == 2 &&
    (cage_rock != board.end()) && !cage_rock->second->number_used()){
        return right_cast? RightCastling : LeftCastling;
    }
    if (diff != Position{0,0} && abs(diff.x) < 2 && abs(diff.y) < 2)
        return Normal;
    return FigureNotMove;
}

King::King(Color color): Figure(Figure_King, color){};

Error_move King::can_move(const Board & board, int now_step, Position start_pos, Position final_pos) const {
    return King_can_move(board, start_pos, final_pos);
}

char King::representation() const{
    return (color() == White)? 'K' : 'k';
}

Referee::Referee() {
    auto fill_senior = [&](Color color) {
        auto y = (color == White) ? 0 : 7;
        for (int i = 0; i < 8; ++i) {
            Position pos = {i,y};
            if (i == 0 || i == 7)
                board_.insert({pos,std::make_unique<Rook>(color)});
            if (i == 1 || i == 6)
                board_.insert({pos,std::make_unique<Knight>(color)});
            if (i == 2 || i == 5)
                board_.insert({pos,std::make_unique<Bishop>(color)});
            if (i == 3)
                board_.insert({pos,std::make_unique<Queen>(color)});
            if (i == 4) {
                board_.insert({pos,std::make_unique<King>(color)});
            }
        }
        return;
    };
    black_king_pos = {4,7};
    white_king_pos = {4,0};
    number_step_ = 1;
    fill_senior(White);
    fill_senior(Black);
    for (int i = 0 ; i < 8; ++i){
        board_.insert({{i,1},std::make_unique<Pawn>(White)});
        board_.insert({{i,6},std::make_unique<Pawn>(Black)});
    }
}


bool Referee::is_pos_attack(const Position& pos_attack) {
    bool ans = false;
    for (const auto& el : board_) {
        if ((board_.find(pos_attack) == board_.end() ) || (el.second->color() != board_(pos_attack).color()))
            if (pos_attack != el.first)
                ans |= el.second->can_move(board_, number_step_, el.first, pos_attack) < SeparateErrors;
    }
    return ans;
}

int Referee::number_step() const {
    return number_step_;
}

Error_move Referee::move(Position start_pos, Position final_pos){
    auto do_interval = [](int a) {return -1 < a && a < 8;};
    if (!(do_interval(start_pos.x) && do_interval(start_pos.y) && do_interval(final_pos.x) && do_interval(final_pos.y)))
        return OutOfBoard;
    const auto& it = board_.find(start_pos);
    if (it == board_.end())
        return EmptyCage;
    const auto& start_figure = *it->second;
    if (start_figure.color() != static_cast<Color>(number_step_ % 2))
        return AlienFigure;
    if (start_pos == final_pos)
        return SameCage;

    bool exist_final_figure = board_.find(final_pos) != board_.end();
    if (exist_final_figure && board_(final_pos).color() == start_figure.color())
        return AttackFigureSameColor;
    const auto err = start_figure.can_move(board_, number_step_, start_pos, final_pos);
    if (err > SeparateErrors)
        return err;
    // err == Normal || LeftCastling || RightCastling || Transform || EnPassant

    if (err == LeftCastling || err == RightCastling){
        if(is_pos_attack(start_pos))
            return UncorrectCastling;
        if (err == LeftCastling) {
            Position p_start =Position{1, start_pos.y} ;
            Position p_stop =  start_pos;
            for (; p_start.x < p_stop.x; p_start = p_start + Position{1, 0}) {
                if (board_.find(p_start) != board_.end())
                    return UncorrectCastling;
                if (is_pos_attack(p_start))
                    return UncorrectCastling;
            }
            motion(start_pos, final_pos);
            board_(final_pos).set_used(number_step_);
            motion({0,start_pos.y},{2, start_pos.y});
            board_({2, start_pos.y}).set_used(number_step_);

        }
        if (err == RightCastling){
            Position p_start = Position{start_pos.x + 1,start_pos.y};
            Position p_stop = Position{7, start_pos.y};
            for (; p_start.x < p_stop.x; p_start = p_start + Position{1, 0}) {
                if (board_.find(p_start) != board_.end())
                    return UncorrectCastling;
                if (is_pos_attack(p_start))
                    return UncorrectCastling;
            }

            motion(start_pos, final_pos);
            board_(final_pos).set_used(number_step_);
            motion({7,start_pos.y},{5, start_pos.y});
            board_({5, start_pos.y}).set_used(number_step_);

        }
       number_step_++;
        if (is_pos_attack(start_pos))
            return Check;
        return Normal;
    }
    std::pair<Position, std::unique_ptr<Figure>> value_save;
    if (err == EnPassant) {
        value_save = std::move(*board_.find({final_pos.x, start_pos.y}));
        board_.erase(value_save.first);
    }
    //std::swap(board_.find(start_pos)->second,board_.find(final_pos)->second);
    std::pair<Position, std::unique_ptr<Figure>> final_figure_save;
    motion(start_pos, final_pos);
    bool mine_king = is_pos_attack(white_king_pos);
    bool alien_king = is_pos_attack(black_king_pos);
    if (start_figure.color() == Black){
        std::swap(mine_king,alien_king);
    }

    Error_move global_error = Normal;
    auto diff = black_king_pos - white_king_pos;
    if (abs(diff.x) < 2 && abs(diff.y) < 2)
        global_error =TwoKingClose;
    if (mine_king)
        global_error = KingAttack;
    if (global_error != Normal){
        motion(final_pos, start_pos);
        if (err == EnPassant){
            board_.insert(std::move(value_save));
        }
        return global_error;
    }
    if (err == Transform)
        return err;
    else
        return number_step_++, alien_king ? Check : Normal;
}

void Referee::motion(Position start_pos, Position final_pos) {
    auto it_start = board_.find(start_pos), it_final = board_.find(final_pos);
    if (it_start == board_.end())
        throw std::exception();
    if (it_final != board_.end())
        values_save.push_back(std::move(*it_final));
    if (it_start->second->type() == Figure_King){
        if (it_start->second->color() == White)
            white_king_pos = final_pos;
        else
            black_king_pos = final_pos;
    }
    auto pair = std::move(*it_start);
    board_.insert_or_assign(final_pos, std::move(pair.second));
    board_.erase(start_pos);
}

void Referee::remotion(Position start_pos, Position final_pos){
    auto it_start = board_.find(start_pos);
    if (it_start == board_.end())
        throw std::exception();
    if (it_start->second->type() == Figure_King){
        if (it_start->second->color() == White)
            white_king_pos = final_pos;
        else
            black_king_pos = final_pos;
    }
    board_.insert({final_pos,std::move(it_start->second)});

    auto it_final =  std::find_if(values_save.begin(), values_save.end(), [&final_pos](const Board::value_type& p){return (p.first) == final_pos;});
    if (it_final != values_save.end()) {
        board_.insert(std::move(*it_final));
    }
}




bool Referee::do_mate() {
    Color color = static_cast<Color>(number_step() % 2);
    for (const auto& el : board_){
        if (el.second->color() == color){
            for (int i = 0 ; i < 8; ++i){
                for (int j = 0; j < 8; ++j){
                    Position start_pos = el.first, final_pos = {i,j};
                    if (final_pos == start_pos)
                        continue;
                    Error_move err = el.second->can_move(board_, number_step(), start_pos, final_pos);
                    if (err < SeparateErrors){
                        bool attack;
                        motion(start_pos, final_pos);
                        auto king_pos = color == White? white_king_pos : black_king_pos;
                        if (err == LeftCastling){
                            motion({0,start_pos.y},{2,start_pos.y});
                            attack = is_pos_attack(king_pos);
                            remotion({2,start_pos.y},{0,start_pos.y});
                        }else
                        if (err == RightCastling){
                            motion({7,start_pos.y},{5,start_pos.y});
                            attack = is_pos_attack(king_pos);
                            remotion({5,start_pos.y},{7,start_pos.y});
                        }else
                        if (err == EnPassant){
                            auto value_save = std::move(*board_.find({final_pos.x, start_pos.y}));
                            board_.erase({final_pos.x, start_pos.y});
                            attack = is_pos_attack(king_pos);
                            board_.insert(std::move(value_save));
                        }
                        else
                            attack = is_pos_attack(king_pos);
                        remotion(final_pos, start_pos);
                        if (!attack)
                            return false;
                        print();
                    }
                }
            }
        }
    }
    return true;
}

void Referee::print() const {
    for(int i = 0 ; i < 8; ++i)
        std::cout << "--";
    std::cout << std::endl;
    for (int y = 0; y < 8; ++y){
        for (int x = 0; x < 8; ++x) {
            Position pos = {x,7 - y};
            const auto& it = board_.find(pos);
            if (it != board_.end())
                std::cout << it->second->representation();
            else
                std::cout << ' ';
            std::cout << '|';
        }
        std::cout << std::endl;
        for(int i = 0 ; i < 8; ++i)
            std::cout << "--";
        std::cout << std::endl;
    }
}


Error_move Referee::transform(FigureType type, Position pos) {
    Color color = static_cast<Color>(number_step());
    board_.erase(pos);
    std::unique_ptr<Figure> ptr;
    if (type == Figure_Rook)
        ptr = std::make_unique<Rook>(color);
    if (type == Figure_Knight)
        ptr = std::make_unique<Rook>(color);
    if (type == Figure_Bishop)
        ptr = std::make_unique<Rook>(color);
    if (type == Figure_Queen)
        ptr = std::make_unique<Rook>(color);
    board_.insert({pos, std::move(ptr)});
    if (is_pos_attack(color == White? white_king_pos : black_king_pos))
        return Check;
    else
        return Normal;
}

void Chess(){
    Referee referee;
    int i = 0;
    while(1){
        referee.print();
        int x1, y1, x2, y2;
        std::cin >> x1 >> y1 >> x2 >> y2;
        Color color = static_cast<Color>(referee.number_step());
        auto rez = referee.move({x1,y1},{x2,y2});
        while (rez == Transform){
            char ch;
            std::cin >> ch;
            std::string s = "RNBQ";
            auto num_type = s.find(std::toupper(ch));
            if ((color == White) == std::isupper(ch) && (s.size() != num_type)){
                rez = referee.transform(static_cast<FigureType>(num_type), {x2,y2});
            }
            else
                std::cout << "Uncorrect char" << std::endl;
        }
        bool flag = referee.do_mate();
        if (rez == Check ){
            if (flag) {
                std::cout << "Win " << (color ? "White" : "Black");
                break;
            }
            else std::cout << "Check" << std::endl;
        }else
        if (flag){
            std::cout << "Draw";
            break;
        }
    }
}




int main(){
    Chess();
    return 0;
}