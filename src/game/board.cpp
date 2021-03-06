#include "board.h"

#include "piece/bishop.h"
#include "piece/king.h"
#include "piece/knight.h"
#include "piece/pawn.h"
#include "piece/piece.h"
#include "piece/queen.h"
#include "piece/rook.h"

Board::Board() {
  board = new Piece **[HEIGHT];
  for (int rank = 0; rank < HEIGHT; ++rank) {
    board[rank] = new Piece *[WIDTH];
    for (int file = 0; file < WIDTH; ++file) {
      board[rank][file] = nullptr;
    }
  }

  // Populate the board with pieces
  for (int file = 0; file < WIDTH; ++file) {
    // Insert white pawn
    board[1][file] = new Pawn(this, Piece::Color::WHITE, Coordinate(1, file));
    // Insert black pawn
    board[HEIGHT - 2][file] =
        new Pawn(this, Piece::Color::BLACK, Coordinate(HEIGHT - 2, file));
  }

  // Insert rooks
  board[0][0] = new Rook(this, Piece::Color::WHITE, Coordinate(0, 0));
  board[0][WIDTH - 1] =
      new Rook(this, Piece::Color::WHITE, Coordinate(0, WIDTH - 1));
  board[HEIGHT - 1][0] =
      new Rook(this, Piece::Color::BLACK, Coordinate(HEIGHT - 1, 0));
  board[HEIGHT - 1][WIDTH - 1] =
      new Rook(this, Piece::Color::BLACK, Coordinate(HEIGHT - 1, WIDTH - 1));

  // Insert knights
  board[0][1] = new Knight(this, Piece::Color::WHITE, Coordinate(0, 1));
  board[0][WIDTH - 2] =
      new Knight(this, Piece::Color::WHITE, Coordinate(0, WIDTH - 2));
  board[HEIGHT - 1][1] =
      new Knight(this, Piece::Color::BLACK, Coordinate(HEIGHT - 1, 1));
  board[HEIGHT - 1][WIDTH - 2] =
      new Knight(this, Piece::Color::BLACK, Coordinate(HEIGHT - 1, WIDTH - 2));

  // Insert bishops
  board[0][2] = new Bishop(this, Piece::Color::WHITE, Coordinate(0, 2));
  board[0][WIDTH - 3] =
      new Bishop(this, Piece::Color::WHITE, Coordinate(0, WIDTH - 3));
  board[HEIGHT - 1][2] =
      new Bishop(this, Piece::Color::BLACK, Coordinate(HEIGHT - 1, 2));
  board[HEIGHT - 1][WIDTH - 3] =
      new Bishop(this, Piece::Color::BLACK, Coordinate(HEIGHT - 1, WIDTH - 3));

  // Insert queens
  board[0][3] = new Queen(this, Piece::Color::WHITE, Coordinate(0, 3));
  board[HEIGHT - 1][3] =
      new Queen(this, Piece::Color::BLACK, Coordinate(HEIGHT - 1, 3));

  // Insert kings
  board[0][4] = new King(this, Piece::Color::WHITE, Coordinate(0, 4));
  board[HEIGHT - 1][4] =
      new King(this, Piece::Color::BLACK, Coordinate(HEIGHT - 1, 4));

  for (int rank = 0; rank < HEIGHT; ++rank) {
    for (int file = 0; file < WIDTH; ++file) {
      if (board[rank][file] != nullptr) {
        pieces_[board[rank][file]->get_color()].insert(board[rank][file]);
      }
    }
  }
}

Board &Board::operator=(const Board &b) {
  if (this != &b) {
    destroy();
    copy(b);
  }
  return *this;
}

Piece *Board::piece_at(const Coordinate &coordinate) {
  if (!contains(coordinate)) {
    return nullptr;
  }
  return board[coordinate.get_rank()][coordinate.get_file()];
}

const Piece *Board::piece_at(const Coordinate &coordinate) const {
  if (!contains(coordinate)) {
    return nullptr;
  }
  return board[coordinate.get_rank()][coordinate.get_file()];
}

void Board::copy(const Board &b) {
  board = new Piece **[HEIGHT];
  for (int rank = 0; rank < HEIGHT; ++rank) {
    for (int file = 0; file < WIDTH; ++file) {
      switch (b.board[rank][file]->get_type()) {
      case Piece::Type::PAWN:
        board[rank][file] =
            new Pawn(*dynamic_cast<Pawn *>(b.board[rank][file]));
        break;
      case Piece::Type::KNIGHT:
        board[rank][file] =
            new Knight(*dynamic_cast<Knight *>(b.board[rank][file]));
        break;
      case Piece::Type::BISHOP:
        board[rank][file] =
            new Bishop(*dynamic_cast<Bishop *>(b.board[rank][file]));
        break;
      case Piece::Type::ROOK:
        board[rank][file] =
            new Rook(*dynamic_cast<Rook *>(b.board[rank][file]));
        break;
      case Piece::Type::QUEEN:
        board[rank][file] =
            new Queen(*dynamic_cast<Queen *>(b.board[rank][file]));
        break;
      case Piece::Type::KING:
        board[rank][file] =
            new King(*dynamic_cast<King *>(b.board[rank][file]));
        break;
      }
    }
  }
}

void Board::destroy() {
  for (int rank = 0; rank < HEIGHT; ++rank) {
    for (int file = 0; file < WIDTH; ++file) {
      delete board[rank][file];
    }
    delete[] board[rank];
  }
  delete[] board;
}

std::pair<bool, Piece *> Board::in_check() const {
  for (int rank = 0; rank < HEIGHT; ++rank) {
    for (int file = 0; file < WIDTH; ++file) {
      std::vector<Coordinate> moves = board[rank][file]->get_moves();
      for (const Coordinate &move : moves) {
        if (board[move.get_rank()][move.get_file()]->get_type() ==
            Piece::Type::KING) {
          return std::make_pair(true, board[move.get_rank()][move.get_file()]);
        }
      }
    }
  }

  return std::make_pair(false, nullptr);
}

std::pair<bool, Piece *> Board::checkmated() const {
  std::pair<bool, Piece *> in_check_ = in_check();
  if (!in_check_.first) {
    return std::make_pair(false, nullptr);
  }

  Piece *king = in_check_.second;
  std::unordered_map<Piece::Color, std::unordered_set<Piece *>>::const_iterator
      pieces_itr = pieces_.find(king->get_color());
  if (pieces_itr != pieces_.end()) {
    for (Piece *piece : pieces_itr->second) {
      if (!piece->get_moves().empty()) {
        return std::make_pair(false, nullptr);
      }
    }
  }

  return std::make_pair(true, king);
}