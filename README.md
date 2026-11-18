# FlameBotPlugin — Unreal Engine 5 Chess Engine Plugin

FlameBot satranç motorunun Unreal Engine 5 plugin versiyonu.

## Kurulum

1. `FlameBotPlugin` klasörünü projenin `Plugins/` klasörüne kopyala
2. Unreal Editor'ü aç
3. Edit → Plugins → "FlameBot Chess Engine" aktif et
4. Projeyi yeniden derle

## Blueprint Kullanımı

Tüm fonksiyonlar Blueprint'ten erişilebilir. `UFlameBotWrapper` sınıfını ara.

| Fonksiyon | Açıklama |
|---|---|
| InitEngine | Motoru başlatır, oyun başında bir kere çağır |
| NewGame | Yeni oyun başlatır |
| SetPositionFEN | FEN ile pozisyon yükler |
| GetBestMove | En iyi hamleyi döner ("e2e4" formatında) |
| MakeMove | Hamle uygular |
| GetCurrentFEN | Mevcut FEN'i döner |
| GetLegalMovesForSquare | Karedeki taşın gidebileceği kareleri döner |
| GetAllLegalMoves | Tüm legal hamleleri döner |
| GetGameState | 0=Devam, 1=Mat, 2=Pat |
| GetTurn | Sıra kimde: "w" veya "b" |

## Gereksinimler

- Unreal Engine 5.6
- Windows 64-bit

## Teşekkürler

Katkıları ve desteği için [@superroket169](https://github.com/superroket169) hesabına teşekkürler.
