// シミュレーション条件を格納する構造体
// 反発係数CORを追加
typedef struct condition
{
  const int width; // 見えている範囲の幅
  const int height; // 見えている範囲の高さ
  const int depth; //奥行(z軸)
  const double G; // 重力定数
  const double k; //クーロン定数
  const double dt; // シミュレーションの時間幅
  const double cor; // 壁の反発係数
  const double c; //光速
} Condition;

// 個々の物体を表す構造体
typedef struct object
{
  double m; //質量
  double q; //電荷
  double x;
  double y;
  double z;
  double vx;
  double vy;
  double vz;
  double prev_x;
  double prev_y;
  double prev_z;
  int show_flg;
} Object;

// 授業で用意した関数のプロトタイプ宣言

//void plot_objects(Object objs[], const size_t numobj, const double t, const Condition cond);
//void update_velocities(Object objs[], const size_t numobj, const Condition cond);
//void update_positions(Object objs[], const size_t numobj, const Condition cond);
//void bounce(Object objs[], const size_t numobj, const Condition cond);

// To do: 以下に上記と同じ引数で実行される my_* を作成
// 実装できたらmain関数で上記と入れ替えていく
// my_plot_objects(), my_update_velocities(), my_update_positions(), my_bounce の4つ 

void my_plot_objects(Object objs[], const size_t numobj, const double t, const Condition cond);
void my_3dplot_objects(Object objs[], const size_t numobj,Object line[], const size_t len, const double t, const Condition cond);
void my_update_velocities(Object objs[], const size_t numobj, const Condition cond);
void my_update_positions(Object objs[], const size_t numobj, const Condition cond);
void my_bounce(Object objs[], const size_t numobj, const Condition cond);
void my_combine(Object objs[], const size_t numobj, const Condition cond);
void seki(double A[3][3], double X[3][1] );