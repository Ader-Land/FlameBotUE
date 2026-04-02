#pragma once

#include "CoreMinimal.h"
#include "FlameBotTypes.generated.h"

// Taş tipleri
UENUM(BlueprintType)
enum class EFlameChessPiece : uint8
{
    Empty   UMETA(DisplayName = "Empty"),
    Pawn    UMETA(DisplayName = "Pawn"),
    Knight  UMETA(DisplayName = "Knight"),
    Bishop  UMETA(DisplayName = "Bishop"),
    Rook    UMETA(DisplayName = "Rook"),
    Queen   UMETA(DisplayName = "Queen"),
    King    UMETA(DisplayName = "King")
};

// Taş renkleri
UENUM(BlueprintType)
enum class EFlameChessSide : uint8
{
    None    UMETA(DisplayName = "None"),
    White   UMETA(DisplayName = "White"),
    Black   UMETA(DisplayName = "Black")
};

// Hamle tipleri
UENUM(BlueprintType)
enum class EFlameMoveType : uint8
{
    Normal              UMETA(DisplayName = "Normal"),
    Capture             UMETA(DisplayName = "Capture"),
    DoublePush          UMETA(DisplayName = "DoublePush"),
    EnPassant           UMETA(DisplayName = "EnPassant"),
    KingsideCastling    UMETA(DisplayName = "KingsideCastling"),
    QueensideCastling   UMETA(DisplayName = "QueensideCastling"),
    Promotion           UMETA(DisplayName = "Promotion")
};

// Oyun durumu
UENUM(BlueprintType)
enum class EFlameGameState : uint8
{
    Ongoing     UMETA(DisplayName = "Ongoing"),
    Checkmate   UMETA(DisplayName = "Checkmate"),
    Stalemate   UMETA(DisplayName = "Stalemate")
};

// Tek kare bilgisi
USTRUCT(BlueprintType)
struct FFlameSquareInfo
{
    GENERATED_BODY()

    // Kare adı: "e2", "a1" gibi
    UPROPERTY(BlueprintReadOnly, Category="FlameBot")
    FString SquareName;

    // Sütun: 1-8 (A=1, H=8)
    UPROPERTY(BlueprintReadOnly, Category="FlameBot")
    int32 File = 0;

    // Sıra: 1-8
    UPROPERTY(BlueprintReadOnly, Category="FlameBot")
    int32 Rank = 0;

    UPROPERTY(BlueprintReadOnly, Category="FlameBot")
    EFlameChessPiece Piece = EFlameChessPiece::Empty;

    UPROPERTY(BlueprintReadOnly, Category="FlameBot")
    EFlameChessSide Side = EFlameChessSide::None;
};

// Hamle sonucu
USTRUCT(BlueprintType)
struct FFlameMove
{
    GENERATED_BODY()

    // "e2" formatında
    UPROPERTY(BlueprintReadOnly, Category="FlameBot")
    FString FromSquare;

    // "e4" formatında
    UPROPERTY(BlueprintReadOnly, Category="FlameBot")
    FString ToSquare;

    UPROPERTY(BlueprintReadOnly, Category="FlameBot")
    EFlameMoveType MoveType = EFlameMoveType::Normal;

    // Terfi olduysa hangi taşa terfi etti
    UPROPERTY(BlueprintReadOnly, Category="FlameBot")
    EFlameChessPiece PromotedTo = EFlameChessPiece::Empty;

    // Hamle geçerli miydi
    UPROPERTY(BlueprintReadOnly, Category="FlameBot")
    bool bIsValid = false;
};