#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "FlameBotTypes.h"
#include "FlameBotWrapper.generated.h"

// Async bot hamlesi için delegate
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnBotMoveReady, FFlameMove, Move);

UCLASS(BlueprintType, Blueprintable)
class FLAMEBOTPLUGIN_API UFlameBotWrapper : public UObject
{
    GENERATED_BODY()

public:

    // ─── Yaşam Döngüsü ───────────────────────────────────

    UFUNCTION(BlueprintCallable, Category="FlameBot|Core")
    static void InitEngine();

    UFUNCTION(BlueprintCallable, Category="FlameBot|Core")
    static void NewGame();

    // ─── Tahta Durumu ─────────────────────────────────────

    UFUNCTION(BlueprintCallable, Category="FlameBot|Board")
    static TArray<FFlameSquareInfo> GetBoardState();

    UFUNCTION(BlueprintCallable, Category="FlameBot|Board")
    static FFlameSquareInfo GetSquareInfo(const FString& SquareName);

    UFUNCTION(BlueprintCallable, Category="FlameBot|Board")
    static EFlameChessSide GetTurn();

    UFUNCTION(BlueprintCallable, Category="FlameBot|Board")
    static EFlameGameState GetGameState();

    // ─── Hamle Sistemi ────────────────────────────────────

    UFUNCTION(BlueprintCallable, Category="FlameBot|Move")
    static TArray<FString> GetLegalMovesForSquare(const FString& SquareName);

    UFUNCTION(BlueprintCallable, Category="FlameBot|Move")
    static FFlameMove MakePlayerMove(const FString& FromSquare, const FString& ToSquare);

    UFUNCTION(BlueprintCallable, Category="FlameBot|Move")
    static FFlameMove PromotePawn(const FString& FromSquare, const FString& ToSquare, EFlameChessPiece PromoteTo);

    UFUNCTION(BlueprintCallable, Category="FlameBot|Bot")
    static FFlameMove MakeBotMove(int32 MoveTimeMs = 1000);

    // ─── Async Bot ────────────────────────────────────────

    // Async bot hamlesi — oyunu dondurmaz
    // Hamle hazır olunca OnBotMoveReady tetiklenir
    UFUNCTION(BlueprintCallable, Category="FlameBot|Bot")
    void MakeBotMoveAsync(int32 MoveTimeMs = 1000);

    // Blueprint'te bind edilecek event
    UPROPERTY(BlueprintAssignable, Category="FlameBot|Bot")
    FOnBotMoveReady OnBotMoveReady;

    // ─── Kayıt / Yükleme ──────────────────────────────────

    UFUNCTION(BlueprintCallable, Category="FlameBot|Save")
    static FString SaveGame();

    UFUNCTION(BlueprintCallable, Category="FlameBot|Save")
    static TArray<FFlameSquareInfo> LoadGame(const FString& FEN);
};