#include "FlameBotWrapper.h"
#include "FlameBot_API.h"

// ─── Yardımcı Fonksiyonlar ────────────────────────────────

static FFlameSquareInfo MakeSquareInfo(int32 File, int32 Rank, 
                                        EFlameChessPiece Piece, 
                                        EFlameChessSide Side)
{
    FFlameSquareInfo Info;
    Info.File = File;
    Info.Rank = Rank;
    Info.Piece = Piece;
    Info.Side = Side;

    // Kare adı: "e2" formatında
    TCHAR FileChar = TEXT('a') + (File - 1);
    TCHAR RankChar = TEXT('1') + (Rank - 1);
    Info.SquareName = FString::Printf(TEXT("%c%c"), FileChar, RankChar);

    return Info;
}

static EFlameChessPiece CharToPiece(char c)
{
    switch (tolower(c))
    {
    case 'p': return EFlameChessPiece::Pawn;
    case 'n': return EFlameChessPiece::Knight;
    case 'b': return EFlameChessPiece::Bishop;
    case 'r': return EFlameChessPiece::Rook;
    case 'q': return EFlameChessPiece::Queen;
    case 'k': return EFlameChessPiece::King;
    default:  return EFlameChessPiece::Empty;
    }
}

static char PieceToChar(EFlameChessPiece Piece)
{
    switch (Piece)
    {
    case EFlameChessPiece::Pawn:   return 'p';
    case EFlameChessPiece::Knight: return 'n';
    case EFlameChessPiece::Bishop: return 'b';
    case EFlameChessPiece::Rook:   return 'r';
    case EFlameChessPiece::Queen:  return 'q';
    case EFlameChessPiece::King:   return 'k';
    default:                       return 'q'; // fallback
    }
}

// FEN'deki karakter → EFlameChessSide
static EFlameChessSide CharToSide(char c)
{
    if (isupper(c)) return EFlameChessSide::White;
    if (islower(c)) return EFlameChessSide::Black;
    return EFlameChessSide::None;
}

// MoveType int → EFlameMoveType
static EFlameMoveType IntToMoveType(int t)
{
    switch (t)
    {
    case 2: return EFlameMoveType::Capture;
    case 3: return EFlameMoveType::DoublePush;
    case 4: return EFlameMoveType::EnPassant;
    case 5: return EFlameMoveType::Capture;      // PawnCapture
    case 6: return EFlameMoveType::KingsideCastling;
    case 7: return EFlameMoveType::QueensideCastling;
    case 8: return EFlameMoveType::Promotion;
    default: return EFlameMoveType::Normal;
    }
}

// Tahta verisini TArray olarak çek
static TArray<FFlameSquareInfo> FetchBoardState()
{
    TArray<FFlameSquareInfo> Result;
    Result.Reserve(64);

    char fenBuf[256] = {};
    FlameBot_GetFEN(fenBuf, sizeof(fenBuf));
    FString FEN = FString(UTF8_TO_TCHAR(fenBuf));

    // FEN'in piece placement kısmını parse et
    FString PiecePlacement = FEN.Left(FEN.Find(TEXT(" ")));

    int32 File = 1;
    int32 Rank = 8;

    for (int32 i = 0; i < PiecePlacement.Len(); i++)
    {
        TCHAR c = PiecePlacement[i];

        if (c == '/')
        {
            Rank--;
            File = 1;
        }
        else if (c >= '1' && c <= '8')
        {
            int32 EmptyCount = c - '0';
            for (int32 j = 0; j < EmptyCount; j++)
            {
                Result.Add(MakeSquareInfo(File, Rank, 
                    EFlameChessPiece::Empty, EFlameChessSide::None));
                File++;
            }
        }
        else
        {
            EFlameChessPiece Piece = CharToPiece((char)c);
            EFlameChessSide Side = CharToSide((char)c);
            Result.Add(MakeSquareInfo(File, Rank, Piece, Side));
            File++;
        }
    }

    return Result;
}

// ─── Core ─────────────────────────────────────────────────

void UFlameBotWrapper::InitEngine()
{
    FlameBot_Init();
}

void UFlameBotWrapper::NewGame()
{
    FlameBot_NewGame();
}

// ─── Tahta Durumu ─────────────────────────────────────────

TArray<FFlameSquareInfo> UFlameBotWrapper::GetBoardState()
{
    return FetchBoardState();
}

FFlameSquareInfo UFlameBotWrapper::GetSquareInfo(const FString& SquareName)
{
    TArray<FFlameSquareInfo> Board = FetchBoardState();
    for (const FFlameSquareInfo& Info : Board)
    {
        if (Info.SquareName == SquareName)
            return Info;
    }
    return FFlameSquareInfo();
}

EFlameChessSide UFlameBotWrapper::GetTurn()
{
    char c = FlameBot_GetTurn();
    return (c == 'w') ? EFlameChessSide::White : EFlameChessSide::Black;
}

EFlameGameState UFlameBotWrapper::GetGameState()
{
    int32 state = FlameBot_GetGameState();
    switch (state)
    {
    case 1:  return EFlameGameState::Checkmate;
    case 2:  return EFlameGameState::Stalemate;
    default: return EFlameGameState::Ongoing;
    }
}

// ─── Hamle Sistemi ────────────────────────────────────────

TArray<FString> UFlameBotWrapper::GetLegalMovesForSquare(const FString& SquareName)
{
    TArray<FString> Result;

    char buf[512] = {};
    FlameBot_GetLegalMoves(TCHAR_TO_UTF8(*SquareName), buf, sizeof(buf));

    FString MovesStr = FString(UTF8_TO_TCHAR(buf));
    if (MovesStr.IsEmpty()) return Result;

    TArray<FString> Moves;
    MovesStr.ParseIntoArray(Moves, TEXT(" "), true);
    return Moves;
}

FFlameMove UFlameBotWrapper::MakePlayerMove(const FString& FromSquare, 
                                             const FString& ToSquare)
{
    FFlameMove Result;
    Result.FromSquare = FromSquare;
    Result.ToSquare = ToSquare;

    FString UCIMove = FromSquare + ToSquare;
    int32 success = FlameBot_MakeMove(TCHAR_TO_UTF8(*UCIMove));

    if (success == 0)
    {
        Result.bIsValid = false;
        return Result;
    }

    Result.bIsValid = true;

    // Hamle tipini belirle
    // Rok kontrolü
    if (FromSquare == TEXT("e1") && ToSquare == TEXT("g1")) 
        Result.MoveType = EFlameMoveType::KingsideCastling;
    else if (FromSquare == TEXT("e1") && ToSquare == TEXT("c1")) 
        Result.MoveType = EFlameMoveType::QueensideCastling;
    else if (FromSquare == TEXT("e8") && ToSquare == TEXT("g8")) 
        Result.MoveType = EFlameMoveType::KingsideCastling;
    else if (FromSquare == TEXT("e8") && ToSquare == TEXT("c8")) 
        Result.MoveType = EFlameMoveType::QueensideCastling;
    else
        Result.MoveType = EFlameMoveType::Normal;

    return Result;
}

FFlameMove UFlameBotWrapper::PromotePawn(const FString& FromSquare, 
                                          const FString& ToSquare, 
                                          EFlameChessPiece PromoteTo)
{
    FFlameMove Result;
    Result.FromSquare = FromSquare;
    Result.ToSquare = ToSquare;

    char promoChar = PieceToChar(PromoteTo);
    FString UCIMove = FromSquare + ToSquare + FString::Printf(TEXT("%c"), promoChar);

    int32 success = FlameBot_MakeMove(TCHAR_TO_UTF8(*UCIMove));

    Result.bIsValid = (success != 0);
    Result.MoveType = EFlameMoveType::Promotion;
    Result.PromotedTo = PromoteTo;

    return Result;
}

FFlameMove UFlameBotWrapper::MakeBotMove(int32 MoveTimeMs)
{
    FFlameMove Result;

    char buf[16] = {};
    FlameBot_GetBestMove(MoveTimeMs, buf, sizeof(buf));

    FString UCIMove = FString(UTF8_TO_TCHAR(buf));
    if (UCIMove.IsEmpty() || UCIMove == TEXT("0000"))
    {
        Result.bIsValid = false;
        return Result;
    }

    // Nereden nereye
    Result.FromSquare = UCIMove.Left(2);
    Result.ToSquare   = UCIMove.Mid(2, 2);

    // Terfi var mı
    if (UCIMove.Len() >= 5)
    {
        Result.MoveType  = EFlameMoveType::Promotion;
        Result.PromotedTo = EFlameChessPiece::Queen;
    }
    else
    {
        // Rok kontrolü
        if (Result.FromSquare == TEXT("e1") && Result.ToSquare == TEXT("g1"))
            Result.MoveType = EFlameMoveType::KingsideCastling;
        else if (Result.FromSquare == TEXT("e1") && Result.ToSquare == TEXT("c1"))
            Result.MoveType = EFlameMoveType::QueensideCastling;
        else if (Result.FromSquare == TEXT("e8") && Result.ToSquare == TEXT("g8"))
            Result.MoveType = EFlameMoveType::KingsideCastling;
        else if (Result.FromSquare == TEXT("e8") && Result.ToSquare == TEXT("c8"))
            Result.MoveType = EFlameMoveType::QueensideCastling;
        else
            Result.MoveType = EFlameMoveType::Normal;
    }

    // Hamleyi uygula
    FlameBot_MakeMove(TCHAR_TO_UTF8(*UCIMove));
    Result.bIsValid = true;

    return Result;
}

// ─── Kayıt / Yükleme ──────────────────────────────────────

FString UFlameBotWrapper::SaveGame()
{
    char buf[256] = {};
    FlameBot_GetFEN(buf, sizeof(buf));
    return FString(UTF8_TO_TCHAR(buf));
}

TArray<FFlameSquareInfo> UFlameBotWrapper::LoadGame(const FString& FEN)
{
    FlameBot_SetPosition(TCHAR_TO_UTF8(*FEN));
    return FetchBoardState();
}