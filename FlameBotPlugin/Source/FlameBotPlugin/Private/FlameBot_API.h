#pragma once

#ifdef __cplusplus
extern "C" {
#endif

void FlameBot_Init();
void FlameBot_Destroy();
void FlameBot_NewGame();

void FlameBot_SetPosition(const char* fen);
int  FlameBot_MakeMove(const char* uciMove);
int  FlameBot_GetFEN(char* outBuf, int bufSize);
char FlameBot_GetTurn();

int  FlameBot_GetLegalMoves(const char* square, char* outBuf, int bufSize);
int  FlameBot_GetAllLegalMoves(char* outBuf, int bufSize);
int  FlameBot_GetBestMove(int moveTimeMs, char* outBuf, int bufSize);
int  FlameBot_GetGameState();

#ifdef __cplusplus
}
#endif