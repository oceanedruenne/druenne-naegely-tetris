
enum TetrominoType { I = 1, J = 2, L = 3, O = 4, S = 5, T = 6, Z = 7 };

// Pas sûr de garder les types dans les structures, c'est pas forcément utile

struct TetrominoI {
  int shape[4][4] = {{0, 0, 0, 0}, {1, 1, 1, 1}, {0, 0, 0, 0}, {0, 0, 0, 0}};
  TetrominoType type = I;
};

struct TetrominoJ {
  int shape[3][3] = {{0, 0, 0}, {2, 2, 2}, {0, 0, 2}};
  TetrominoType type = J;
};

struct TetrominoL {
  int shape[3][3] = {{0, 0, 0}, {3, 3, 3}, {3, 0, 0}};
  TetrominoType type = L;
};

struct TetrominoO {
  int shape[2][2] = {{4, 4}, {4, 4}};
  TetrominoType type = O;
};

struct TetrominoS {
  int shape[3][3] = {{0, 0, 0}, {0, 5, 5}, {5, 5, 0}};
  TetrominoType type = S;
};

struct TetrominoT {
  int shape[3][3] = {{0, 0, 0}, {6, 6, 6}, {0, 6, 0}};
  TetrominoType type = T;
};

struct TetrominoZ {
  int shape[3][3] = {{0, 0, 0}, {7, 7, 0}, {0, 7, 7}};
  TetrominoType type = Z;
};
