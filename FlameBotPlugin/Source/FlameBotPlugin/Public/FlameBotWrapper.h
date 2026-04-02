#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "FlameBotTypes.h"
#include "FlameBotWrapper.generated.h"

UCLASS(BlueprintType, Blueprintable)
class FLAMEBOTPLUGIN_API UFlameBotWrapper : public UObject
{
    GENERATED_BODY()

public:

    // ─── Yaşam Döngüsü ───────────────────────────────────

    // Motoru başlat — oyun başında bir kere çağır
    UFUNCTION(BlueprintCallable, Category="FlameBot|Core")
    static void InitEngine();

    // Yeni oyun başlat
    UFUNCTION(BlueprintCallable, Category="FlameBot|Core")
    static void NewGame();

    // ─── Tahta Durumu ─────────────────────────────────────

    // 64 karenin tamamını döner — oyun başında veya yüklemede kullan
    UFUNCTION(BlueprintCallable, Category="FlameBot|Board")
    static TArray<FFlameSquareInfo> GetBoardState();

    // Tek karenin bilgisini döner — "e2" formatında ver
    UFUNCTION(BlueprintCallable, Category="FlameBot|Board")
    static FFlameSquareInfo GetSquareInfo(const FString& SquareName);

    // Sıra kimde: White veya Black
    UFUNCTION(BlueprintCallable, Category="FlameBot|Board")
    static EFlameChessSide GetTurn();

    // Oyun durumu: Ongoing, Checkmate, Stalemate
    UFUNCTION(BlueprintCallable, Category="FlameBot|Board")
    static EFlameGameState GetGameState();

    // ─── Hamle Sistemi ────────────────────────────────────

    // Karedeki taşın gidebileceği kareleri döner — highlight için
    // "e2" ver → ["e3", "e4"] döner
    UFUNCTION(BlueprintCallable, Category="FlameBot|Move")
    static TArray<FString> GetLegalMovesForSquare(const FString& SquareName);

    // Hamle yap — "e2", "e4" ver → sonuç döner
    UFUNCTION(BlueprintCallable, Category="FlameBot|Move")
    static FFlameMove MakePlayerMove(const FString& FromSquare, const FString& ToSquare);

    // Terfi — MakePlayerMove Promotion döndürdüyse bunu çağır
    UFUNCTION(BlueprintCallable, Category="FlameBot|Move")
    static FFlameMove PromotePawn(const FString& FromSquare, const FString& ToSquare, EFlameChessPiece PromoteTo);

    // Bot hamlesi — MoveTimeMs kadar düşünür
    UFUNCTION(BlueprintCallable, Category="FlameBot|Bot")
    static FFlameMove MakeBotMove(int32 MoveTimeMs = 1000);

    // ─── Kayıt / Yükleme ──────────────────────────────────

    // Mevcut pozisyonu FEN olarak döner — sen kaydet
    UFUNCTION(BlueprintCallable, Category="FlameBot|Save")
    static FString SaveGame();

    // FEN ver → tahtayı yükler ve 64 kare bilgisini döner
    UFUNCTION(BlueprintCallable, Category="FlameBot|Save")
    static TArray<FFlameSquareInfo> LoadGame(const FString& FEN);
};