#include "FlameBot_API.h"

#include "BoardHash/BoardHash.h"
#include "Chess/Chess.h"
#include "FlameBoth/FlameBoth.h"
#include "OpeningBook/OpeningBook.h"

#include <cctype>
#include <cstring>
#include <mutex>
#include <sstream>
#include <string>
#include <vector>

static Chess::Board g_board;
static FlameBoth::Bot g_bot;
static bool g_initialized = false;
static std::mutex g_engineMutex;

static Chess::Piece charToPieceAPI(char c) {
    switch (tolower(c)) {
    case 'p': return Chess::Piece::Pawn;
    case 'n': return Chess::Piece::Knight;
    case 'b': return Chess::Piece::Bishop;
    case 'r': return Chess::Piece::Rook;
    case 'q': return Chess::Piece::Queen;
    case 'k': return Chess::Piece::King;
    default:  return Chess::Piece::Empty;
    }
}

static void parseFEN_API(const std::string& fen, Chess::Board& board) {
    for (int f = 1; f <= 8; ++f)
        for (int r = 1; r <= 8; ++r)
            board.setSquare(Chess::Square(
                Chess::BoardCoordinate((Chess::File)f, (Chess::Rank)r),
                Chess::Piece::Empty, Chess::Side::None));

    std::istringstream ss(fen);
    std::string piecePlacement, activeColor, castling, enPassant;
    ss >> piecePlacement >> activeColor >> castling >> enPassant;

    int rank = 8, file = 1;
    for (char c : piecePlacement) {
        if (c == '/') { rank--; file = 1; }
        else if (c >= '1' && c <= '8') { file += (c - '0'); }
        else {
            Chess::Side side = isupper(c) ? Chess::Side::White : Chess::Side::Black;
            Chess::Piece piece = charToPieceAPI(c);
            board.setSquare(Chess::Square(
                Chess::BoardCoordinate((Chess::File)file, (Chess::Rank)rank),
                piece, side));
            file++;
        }
    }

    if (activeColor == "b") board.passTurn();

    board.setKingMoved(Chess::Side::White, true);
    board.setKingRookMoved(Chess::Side::White, true);
    board.setQueenRookMoved(Chess::Side::White, true);
    board.setKingMoved(Chess::Side::Black, true);
    board.setKingRookMoved(Chess::Side::Black, true);
    board.setQueenRookMoved(Chess::Side::Black, true);

    if (castling != "-") {
        for (char c : castling) {
            switch (c) {
            case 'K': board.setKingMoved(Chess::Side::White, false);
                      board.setKingRookMoved(Chess::Side::White, false); break;
            case 'Q': board.setKingMoved(Chess::Side::White, false);
                      board.setQueenRookMoved(Chess::Side::White, false); break;
            case 'k': board.setKingMoved(Chess::Side::Black, false);
                      board.setKingRookMoved(Chess::Side::Black, false); break;
            case 'q': board.setKingMoved(Chess::Side::Black, false);
                      board.setQueenRookMoved(Chess::Side::Black, false); break;
            }
        }
    }

    if (enPassant != "-" && enPassant.length() >= 2) {
        Chess::File epFile = (Chess::File)(tolower(enPassant[0]) - 'a' + 1);
        Chess::Rank epRank = (Chess::Rank)(enPassant[1] - '0');
        board.setEnPassantTarget(Chess::BoardCoordinate(epFile, epRank));
    } else {
        board.clearEnPassantTarget();
    }
}

static std::string moveToStringAPI(const Chess::Move& move) {
    if (move.getFrom().getPieceType() == Chess::Piece::Empty) return "0000";

    int f1 = (int)move.getFrom().getCoordinate().file;
    int r1 = (int)move.getFrom().getCoordinate().rank;
    int f2 = (int)move.getTo().getCoordinate().file;
    int r2 = (int)move.getTo().getCoordinate().rank;

    std::string s;
    s += (char)('a' + f1 - 1);
    s += (char)('0' + r1);
    s += (char)('a' + f2 - 1);
    s += (char)('0' + r2);

    if (move.getFrom().getPieceType() == Chess::Piece::Pawn && (r2 == 8 || r2 == 1))
        s += 'q';

    return s;
}

static int writeString(const std::string& src, char* outBuf, int bufSize) {
    if (!outBuf || bufSize <= 0) return 0;
    int len = (int)src.length();
    if (len >= bufSize) len = bufSize - 1;
    memcpy(outBuf, src.c_str(), len);
    outBuf[len] = '\0';
    return len;
}

void FlameBot_Init() {
    std::lock_guard<std::mutex> lock(g_engineMutex);
    if (!g_initialized) {
        BoardHash::init();
        g_initialized = true;
    }
}

void FlameBot_Destroy() {
    std::lock_guard<std::mutex> lock(g_engineMutex);
    g_initialized = false;
}

void FlameBot_NewGame() {
    std::lock_guard<std::mutex> lock(g_engineMutex);
    g_board = Chess::Board();
    BoardHash::clear();
}

void FlameBot_SetPosition(const char* fen) {
    std::lock_guard<std::mutex> lock(g_engineMutex);
    if (!fen) return;
    parseFEN_API(std::string(fen), g_board);
}

int FlameBot_MakeMove(const char* uciMove) {
    std::lock_guard<std::mutex> lock(g_engineMutex);
    if (!uciMove) return 0;
    std::string s(uciMove);
    if (s.length() < 4) return 0;

    Chess::BoardCoordinate from = {
        (Chess::File)(tolower(s[0]) - 'a' + 1),
        (Chess::Rank)(s[1] - '0')
    };
    Chess::BoardCoordinate to = {
        (Chess::File)(tolower(s[2]) - 'a' + 1),
        (Chess::Rank)(s[3] - '0')
    };

    Chess::Square sqFrom = g_board.getSquare(from);
    Chess::Square sqTo   = g_board.getSquare(to);
    Chess::Move move(sqFrom, sqTo);

    Chess::Side currentSide = g_board.getTurn();
    Chess::MoveType type = Chess::MoveValidator(move, currentSide, g_board);

    if (type == Chess::MoveType::Invalid || type == Chess::MoveType::inCheck)
        return 0;

    Chess::makeMove(move, currentSide, g_board);
    return 1;
}

int FlameBot_GetFEN(char* outBuf, int bufSize) {
    std::lock_guard<std::mutex> lock(g_engineMutex);
    std::string fen;

    for (int rank = 8; rank >= 1; --rank) {
        int emptyCount = 0;
        for (int file = 1; file <= 8; ++file) {
            Chess::Square sq = g_board.getSquare({(Chess::File)file, (Chess::Rank)rank});
            if (sq.getPieceType() == Chess::Piece::Empty) {
                emptyCount++;
            } else {
                if (emptyCount > 0) { fen += std::to_string(emptyCount); emptyCount = 0; }
                char c = ' ';
                switch (sq.getPieceType()) {
                case Chess::Piece::Pawn:   c = 'p'; break;
                case Chess::Piece::Knight: c = 'n'; break;
                case Chess::Piece::Bishop: c = 'b'; break;
                case Chess::Piece::Rook:   c = 'r'; break;
                case Chess::Piece::Queen:  c = 'q'; break;
                case Chess::Piece::King:   c = 'k'; break;
                default: break;
                }
                if (sq.getPieceSide() == Chess::Side::White) c = toupper(c);
                fen += c;
            }
        }
        if (emptyCount > 0) fen += std::to_string(emptyCount);
        if (rank > 1) fen += '/';
    }

    fen += (g_board.getTurn() == Chess::Side::White) ? " w " : " b ";

    std::string castleStr;
    if (!g_board.isKingMoved(Chess::Side::White)) {
        if (!g_board.isKingRookMoved(Chess::Side::White))  castleStr += 'K';
        if (!g_board.isQueenRookMoved(Chess::Side::White)) castleStr += 'Q';
    }
    if (!g_board.isKingMoved(Chess::Side::Black)) {
        if (!g_board.isKingRookMoved(Chess::Side::Black))  castleStr += 'k';
        if (!g_board.isQueenRookMoved(Chess::Side::Black)) castleStr += 'q';
    }
    fen += castleStr.empty() ? "-" : castleStr;
    fen += " - 0 1";

    return writeString(fen, outBuf, bufSize);
}

char FlameBot_GetTurn() {
    std::lock_guard<std::mutex> lock(g_engineMutex);
    return (g_board.getTurn() == Chess::Side::White) ? 'w' : 'b';
}

int FlameBot_GetLegalMoves(const char* square, char* outBuf, int bufSize) {
    std::lock_guard<std::mutex> lock(g_engineMutex);
    if (!square || !outBuf || bufSize <= 0) return 0;

    std::string sq(square);
    if (sq.length() < 2) return writeString("", outBuf, bufSize);

    int sqFile = tolower(sq[0]) - 'a' + 1;
    int sqRank = sq[1] - '0';

    if (sqFile < 1 || sqFile > 8 || sqRank < 1 || sqRank > 8)
        return writeString("", outBuf, bufSize);

    Chess::Side currentSide = g_board.getTurn();
    std::vector<Chess::Move> allMoves = g_bot.getAllValidMoves(g_board, currentSide);

    std::string result;
    for (const auto& m : allMoves) {
        int mf = (int)m.getFrom().getCoordinate().file;
        int mr = (int)m.getFrom().getCoordinate().rank;
        if (mf == sqFile && mr == sqRank) {
            if (!result.empty()) result += ' ';
            int tf = (int)m.getTo().getCoordinate().file;
            int tr = (int)m.getTo().getCoordinate().rank;
            result += (char)('a' + tf - 1);
            result += (char)('0' + tr);
        }
    }

    return writeString(result, outBuf, bufSize);
}

int FlameBot_GetAllLegalMoves(char* outBuf, int bufSize) {
    std::lock_guard<std::mutex> lock(g_engineMutex);
    if (!outBuf || bufSize <= 0) return 0;

    Chess::Side currentSide = g_board.getTurn();
    std::vector<Chess::Move> allMoves = g_bot.getAllValidMoves(g_board, currentSide);

    std::string result;
    for (const auto& m : allMoves) {
        if (!result.empty()) result += ' ';
        result += moveToStringAPI(m);
    }

    return writeString(result, outBuf, bufSize);
}

int FlameBot_GetBestMove(int moveTimeMs, char* outBuf, int bufSize) {
    std::lock_guard<std::mutex> lock(g_engineMutex);
    if (!outBuf || bufSize <= 0) return 0;

    double timeSeconds = moveTimeMs / 1000.0;
    if (timeSeconds < 0.01) timeSeconds = 1.0;

    g_bot.setTimeLimit(timeSeconds);
    Chess::Move bestMove = g_bot.getBestMove(g_board, 64);
    std::string result = moveToStringAPI(bestMove);

    return writeString(result, outBuf, bufSize);
}

int FlameBot_GetGameState() {
    std::lock_guard<std::mutex> lock(g_engineMutex);
    Chess::GameState state = Chess::getGameState(g_board);
    if (state == Chess::GameState::Checkmate) return 1;
    if (state == Chess::GameState::Stalemate) return 2;
    return 0;
}