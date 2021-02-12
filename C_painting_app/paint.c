#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h> // for error catch
#include <math.h>
#define DEFAULT_PEN '.'
#define DEFAULT_PENCOLOR RESET

// 発展課題：

// ファイル読み込み時、現状ある絵の上に上書きする
// sin,cosを使うためにmath.hをincludeしたので、-lmでコンパイル

// Structure for canvas

typedef enum color
{
  BLACK,
  RED,
  GREEN,
  YELLOW,
  BLUE,
  MAGENTA,
  CYAN,
  WHITE,
  RESET,

} Color;

typedef struct
{
  int width;
  int height;
  char **canvas;
  Color **nodes_font_color;
  Color **nodes_background_color;
  char **nodes_mode;
  char pen;
  Color penColor;
  char pen_mode;

} Canvas;

typedef struct command Command;
struct command
{
  char *str;
  Command *next;
};

typedef struct
{
  Command *begin;
  Command *garbage;
  size_t bufsize;
} History;

// functions for Canvas type
Canvas *init_canvas(int width, int height, char pen, Color color);
void reset_canvas(Canvas *c);
void print_canvas(FILE *fp, Canvas *c);
void free_canvas(Canvas *c);

// display functions
void rewind_screen(FILE *fp, unsigned int line);
void clear_command(FILE *fp);
void clear_screen(FILE *fp);

// enum for interpret_command results
typedef enum res
{
  EXIT,
  NORMAL,
  COMMAND,
  UNKNOWN,
  ERROR
} Result;

int max(const int a, const int b);
void draw(Canvas *c, const int x, const int y);
void draw_line(Canvas *c, const int x0, const int y0, const int x1, const int y1);
void draw_rect(Canvas *c, const int x0, const int y0, const int rect_width, const int rect_height);
void draw_circle(Canvas *c, const int x0, const int y0, const int r);
Result interpret_command(const char *command, History *his, Canvas *c);
int interpret_color(const char *colorname, Canvas *c);
char *convert_font_color(Color fcolor, Color bcolor, char mode);
char *convert_background_color(Color color, Color bcolor, char mode);
void save_history(const char *filename, History *his);

// new functions for manipulate History
void push_back_str(History *his, char *buf); //strの形でpush_backを行う
void push_back(History *his, Command *new);  //his->beginの末尾にコマンドを保存する
void pop_back(History *his);                 //his->beginの末尾のコマンドを取り出し、his->garbageの先頭に差し込む。
void push_front(History *his, Command *p);   //pop_backで呼び出される。
int pop_front(History *his);                 //redoにおいて、gabageの先頭を消し、beginの末尾に付け加える。

size_t his_len(History *his);
void free_command(Command *node);
void free_history(History *his);

int main(int argc, char **argv)
{
  //for history recording
  const size_t bufsize = 1000;
  History *his = (History *)malloc(sizeof(History));
  *his = (History){.begin = NULL, .bufsize = bufsize};
  // History his =(History) {.begin = NULL, .bufsize= bufsize};

  int width;
  int height;
  if (argc != 3)
  {
    fprintf(stderr, "usage: %s <width> <height>\n", argv[0]);
    return EXIT_FAILURE;
  }
  else
  {
    char *e;
    long w = strtol(argv[1], &e, 10);
    if (*e != '\0')
    {
      fprintf(stderr, "%s: irregular character found %s\n", argv[1], e);
      return EXIT_FAILURE;
    }
    long h = strtol(argv[2], &e, 10);
    if (*e != '\0')
    {
      fprintf(stderr, "%s: irregular character found %s\n", argv[2], e);
      return EXIT_FAILURE;
    }
    width = (int)w;
    height = (int)h;
  }
  char pen = DEFAULT_PEN;
  Color color = DEFAULT_PENCOLOR;

  FILE *fp;
  char buf[bufsize];
  fp = stdout;
  Canvas *c = init_canvas(width, height, pen, color);

  fprintf(fp, "\n"); // required especially for windows env
  int flg = 1;
  while (1)
  {
    print_canvas(fp, c);
    size_t hsize = his_len(his);
    if (flg == 1)
    {
      printf("\nrecommended to command \"load paint4.txt\"\n");
      rewind_screen(fp, 2);
      clear_command(fp);
      flg = 0;
    }
    printf("%zu > ", hsize);
    if (fgets(buf, bufsize, stdin) == NULL || strcmp(buf, "\n") == 0)
    {
      strcpy(buf, "no command\n");
    }

    const Result r = interpret_command(buf, his, c);
    if (r == EXIT)
      break;
    if (r == NORMAL)
    {
      push_back_str(his, buf);
    }

    rewind_screen(fp, 2);          // command results
    clear_command(fp);             // command itself
    rewind_screen(fp, height + 2); // rewind the screen to command input
  }

  clear_screen(fp);
  free_canvas(c);
  free_history(his);
  fclose(fp);

  return 0;
}

size_t his_len(History *his)
{
  int count = 0;
  Command *p = his->begin;
  while (p != NULL)
  {
    p = p->next;
    count++;
  }
  return count;
}

void push_back_str(History *his, char *buf)
{
  size_t bufsize = his->bufsize;
  char *str = (char *)malloc(bufsize * sizeof(char));
  strcpy(str, buf);
  Command *new = (Command *)malloc(sizeof(Command));
  *new = (Command){.str = str, .next = NULL};

  push_back(his, new);
}

void push_back(History *his, Command *new)
{
  // もしも何も履歴がなかったら
  if (his->begin == NULL)
  {
    his->begin = new;
    return;
  }

  // ふつうなら、最後まで、つまりnext=NULLまでたどる
  Command *p;
  p = his->begin;
  while (p->next != NULL)
  {
    p = p->next;
  }
  // いま、pは最後のノードを参照。
  p->next = new;
  return;
}

void pop_back(History *his)
{
  // 履歴がなかったら何も消さない
  if (his->begin == NULL)
    return;

  // 履歴が一つしかなかったら
  if (his->begin->next == NULL)
  {
    his->garbage = his->begin;
    his->begin = NULL;
    return;
  }
  // 以下、履歴が少なくとも二つ以上ある前提。
  Command *p = his->begin;
  while (p->next->next != NULL)
  {
    p = p->next;
  }
  // pは最後の一個前になる。単方向リストだからしょうがないよね。

  // garbageに差し込む。
  push_front(his, p->next);

  // free(p->next);
  p->next = NULL;
}

void push_front(History *his, Command *p) //garbage専用なんだけどね....
{
  p->next = his->garbage;
  his->garbage = p;
}

int pop_front(History *his)
{
  Command *p = his->garbage;
  if (p != NULL)
  {
    his->garbage = p->next;
    p->next = NULL;
    push_back(his, p);
  }
  else
  {
    return -1;
  }
  return 0;
}

Canvas *init_canvas(int width, int height, char pen, Color color)
{
  Canvas *new = (Canvas *)malloc(sizeof(Canvas));
  new->width = width;
  new->height = height;
  new->canvas = (char **)malloc(width * sizeof(char *));

  char *tmp = (char *)malloc(width * height * sizeof(char));
  memset(tmp, ' ', width * height * sizeof(char));
  for (int i = 0; i < width; i++)
  {
    new->canvas[i] = tmp + i *height;
  }

  new->nodes_font_color = (Color **)malloc(width * sizeof(Color *));
  Color *tmp2 = (Color *)malloc(width * height * sizeof(Color));
  memset(tmp2, color, width * height * sizeof(Color));
  for (int i = 0; i < width; i++)
  {
    new->nodes_font_color[i] = tmp2 + i *height;
  }

  new->nodes_mode = (char **)malloc(width * sizeof(char *));
  char *tmp3 = (char *)malloc(width * height * sizeof(char));
  memset(tmp3, 0, width * height * sizeof(char));
  for (int i = 0; i < width; i++)
  {
    new->nodes_mode[i] = tmp3 + i *height;
  }

  new->nodes_background_color = (Color **)malloc(width * sizeof(Color *));
  Color *tmp4 = (Color *)malloc(width * height * sizeof(Color));
  memset(tmp4, RESET, width * height * sizeof(Color));
  for (int i = 0; i < width; i++)
  {
    new->nodes_background_color[i] = tmp4 + i *height;
  }

  new->pen = pen;
  new->penColor = color;
  new->pen_mode = 0;
  return new;
}

void reset_canvas(Canvas *c)
{
  const int width = c->width;
  const int height = c->height;
  memset(c->canvas[0], ' ', width * height * sizeof(char));
  memset(c->nodes_font_color[0], RESET, width * height * sizeof(Color));
  memset(c->nodes_mode[0], 0, width * height * sizeof(char));
}

void print_canvas(FILE *fp, Canvas *c)
{
  const int height = c->height;
  const int width = c->width;
  char **canvas = c->canvas;
  Color **fcolors = c->nodes_font_color;
  Color **bcolors = c->nodes_background_color;
  char **modes = c->nodes_mode;

  // 上の壁
  fprintf(fp, "┌");
  for (int x = 0; x < width; x++)
    fprintf(fp, "─");
  fprintf(fp, "┐\n");

  // 外壁と内側
  for (int y = 0; y < height; y++)
  {
    fprintf(fp, "│");
    for (int x = 0; x < width; x++)
    {
      const char character = canvas[x][y];
      const Color f_color = fcolors[x][y];
      const Color b_color = bcolors[x][y];
      const char mode = modes[x][y];

      char *font_col = convert_font_color(f_color, b_color, mode);
      char *background_col = convert_background_color(f_color, b_color, mode);
      fprintf(fp, "%s", font_col);
      fprintf(fp, "%s", background_col);
      fputc(character, fp);
      fprintf(fp, "\e[0m");
    }
    fprintf(fp, "│\n");
  }

  // 下の壁
  fprintf(fp, "└");
  for (int x = 0; x < width; x++)
  {
    fprintf(fp, "─");
  }
  fprintf(fp, "┘\n");
  fflush(fp);
}

int interpret_color(const char *colorname, Canvas *c)
{
  if (strcmp(colorname, "black") == 0)
  {
    c->penColor = BLACK;
    return 0;
  }
  if (strcmp(colorname, "red") == 0)
  {
    c->penColor = RED;
    return 0;
  }
  if (strcmp(colorname, "green") == 0)
  {
    c->penColor = GREEN;
    return 0;
  }
  if (strcmp(colorname, "yellow") == 0)
  {
    c->penColor = YELLOW;
    return 0;
  }
  if (strcmp(colorname, "blue") == 0)
  {
    c->penColor = BLUE;
    return 0;
  }
  if (strcmp(colorname, "magenta") == 0)
  {
    c->penColor = MAGENTA;
    return 0;
  }
  if (strcmp(colorname, "cyan") == 0)
  {
    c->penColor = CYAN;
    return 0;
  }
  if (strcmp(colorname, "white") == 0)
  {
    c->penColor = WHITE;
    return 0;
  }
  if (strcmp(colorname, "reset") == 0)
  {
    c->penColor = DEFAULT_PENCOLOR;
    return 0;
  }

  return -1;
}

char *convert_font_color(Color font_color, Color background_color, char mode)
{
  if (mode == 1)
  {
    font_color = background_color; //値渡しなので問題なし
  }
  char *col;
  switch (font_color)
  {
  case BLACK:
    col = "\e[30m";
    break;
  case RED:
    col = "\e[31m";
    break;
  case GREEN:
    col = "\e[32m";
    break;
  case YELLOW:
    col = "\e[33m";
    break;
  case BLUE:
    col = "\e[34m";
    break;
  case MAGENTA:
    col = "\e[35m";
    break;
  case CYAN:
    col = "\e[36m";
    break;
  case WHITE:
    col = "\e[37m";
    break;
  case RESET:
    col = "\e[0m";
    break;
  default:
    col = "\e[0m";
    break;
  }
  return col;
}

char *convert_background_color(Color font_color, Color background_color, char mode)
{
  char *col;
  switch (background_color)
  {
  case BLACK:
    col = "\e[40m";
    break;
  case RED:
    col = "\e[41m";
    break;
  case GREEN:
    col = "\e[42m";
    break;
  case YELLOW:
    col = "\e[43m";
    break;
  case BLUE:
    col = "\e[44m";
    break;
  case MAGENTA:
    col = "\e[45m";
    break;
  case CYAN:
    col = "\e[46m";
    break;
  case WHITE:
    col = "\e[47m";
    break;
  case RESET:
    col = "\e[0m";
    break;
  default:
    col = "\e[0m";
    break;
  }
  return col;
}

void free_canvas(Canvas *c)
{
  free(c->canvas[0]); //  for 2-D array free
  free(c->canvas);
  free(c->nodes_font_color[0]);
  free(c->nodes_font_color);
  free(c->nodes_mode[0]);
  free(c->nodes_mode);
  free(c);
}

void free_command(Command *node)
{
  free(node->str);
  free(node);
}

void free_history(History *his)
{
  Command *p = his->begin;
  Command *next;
  while (p != NULL)
  {
    next = p->next;
    free_command(p);
    p = next;
  }

  Command *g = his->garbage;
  Command *next2;
  while (g != NULL)
  {
    next2 = g->next;
    free_command(g);
    g = next2;
  }

  free(his);
}

void rewind_screen(FILE *fp, unsigned int line)
{
  fprintf(fp, "\e[%dA", line);
}

void clear_command(FILE *fp)
{
  fprintf(fp, "\e[2K");
}

void clear_screen(FILE *fp)
{
  fprintf(fp, "\e[2J");
}

int max(const int a, const int b)
{
  return (a > b) ? a : b;
}

void draw(Canvas *c, const int x, const int y)
{
  switch (c->pen_mode)
  {
  case 0:
    c->canvas[x][y] = c->pen;
    c->nodes_font_color[x][y] = c->penColor;
    break;
  case 1:
    c->nodes_background_color[x][y] = c->penColor;
  }
  c->nodes_mode[x][y] = c->pen_mode;
}

void draw_line(Canvas *c, const int x0, const int y0, const int x1, const int y1)
{
  const int width = c->width;
  const int height = c->height;

  const int n = max(abs(x1 - x0), abs(y1 - y0));
  draw(c, x0, y0);
  for (int i = 1; i <= n; i++)
  {
    const int x = x0 + i * (x1 - x0) / n;
    const int y = y0 + i * (y1 - y0) / n;
    if ((x >= 0) && (x < width) && (y >= 0) && (y < height))
    {
      draw(c, x, y);
    }
  }
}

void draw_rect(Canvas *c, const int x0, const int y0, const int rect_width, const int rect_height)
{
  const int width = c->width;
  const int height = c->height;

  for (int i = 0; i <= rect_width; i++)
  {
    if (i == 0 || i == rect_width)
    {
      for (int j = 0; j <= rect_height; j++)
      {
        if (x0 + i >= 0 && x0 + i < width && y0 + j >= 0 && y0 + j < height)
        {
          draw(c, x0 + i, y0 + j);
        }
      }
    }
    else
    {
      switch (c->pen_mode)
      {
      case 0:
        for (int j = 0; j <= rect_width; j = j + rect_height)
        {
          draw(c, x0 + i, y0 + j);
        }
        break;
      case 1:
        for (int j = 0; j <= rect_height; j++)
        {
          if (x0 + i >= 0 && x0 + i < width && y0 + j >= 0 && y0 + j < height)
          {
            draw(c, x0 + i, y0 + j);
          }
        }
        break;
      default:
        break;
      }
    }
  }
}

void draw_circle(Canvas *c, const int x0, const int y0, const int r)
{
  const int width = c->width;
  const int height = c->height;

  //　角度で描画
  switch (c->pen_mode)
  {
  case 0:
    for (int theta = 0; theta < 360; theta = theta + 1)
    {
      double rad = theta * 3.141592 / 180;
      int x = (int)x0 + r * cos(rad);
      int y = (int)y0 + r * sin(rad);
      if (x >= 0 && x < width && y >= 0 && y < height)
      {
        draw(c, x, y);
      }
    }
    break;

  case 1:
    for (int theta = 0; theta < 360; theta = theta + 1)
    {
      for (int R = 0; R <= r; R++)
      {
        double rad = theta * 3.141592 / 180;
        int xR = (int)x0 + R * cos(rad);
        int yR = (int)y0 + R * sin(rad);
        if (xR >= 0 && xR < width && yR >= 0 && yR < height)
        {
          draw(c, xR, yR);
        }
      }
    }
  }
}

void save_history(const char *filename, History *his)
{
  const char *default_history_file = "history.txt";
  if (filename == NULL)
    filename = default_history_file;

  FILE *fp;
  if ((fp = fopen(filename, "w")) == NULL)
  {
    fprintf(stderr, "error: cannot open %s.\n", filename);
    return;
  }

  // 文字列を指すポインタが欲しい
  Command *p = his->begin;

  while (p != NULL)
  {
    fprintf(fp, "%s", p->str);
    p = p->next;
  }

  fclose(fp);
}

int load_history(const char *filename, History *his, Canvas *c)
{

  FILE *fp;
  if ((fp = fopen(filename, "r")) == NULL)
  {
    return -1;
  }
  else
  {
    char str[his->bufsize];
    while (fgets(str, his->bufsize, fp))
    {
      const Result r = interpret_command(str, his, c);
      if (r == EXIT)
        break;
      if (r == NORMAL)
      {
        push_back_str(his, str);
      }

      rewind_screen(stdout, 1);
      clear_command(stdout);
    }
    rewind_screen(stdout, 1);
    clear_command(stdout);
    printf("\n");
  }
  fclose(fp);
  return 0;
}

Result interpret_command(const char *command, History *his, Canvas *c)
{
  char buf[his->bufsize];
  strcpy(buf, command);
  buf[strlen(buf) - 1] = 0; // remove the newline character at the end

  const char *s = strtok(buf, " ");

  // The first token corresponds to command
  if (strcmp(s, "line") == 0)
  {
    int p[4] = {0}; // p[0]: x0, p[1]: y0, p[2]: x1, p[3]: x1
    char *b[4];
    for (int i = 0; i < 4; i++)
    {
      b[i] = strtok(NULL, " ");
      if (b[i] == NULL)
      {
        clear_command(stdout);
        printf("the number of point is not enough.\n");
        return ERROR;
      }
    }
    for (int i = 0; i < 4; i++)
    {
      char *e;
      long v = strtol(b[i], &e, 10);
      if (*e != '\0')
      {
        clear_command(stdout);
        printf("Non-int value is included.\n");
        return ERROR;
      }
      p[i] = (int)v;
    }

    draw_line(c, p[0], p[1], p[2], p[3]);
    clear_command(stdout);
    printf("1 line drawn\n");
    return NORMAL;
  }

  if (strcmp(s, "rect") == 0)
  {                 //TODO
    int p[4] = {0}; // p[0]: x0, p[1]: y0, p[2]: rext_width, p[3]: rect_height
    char *b[4];
    for (int i = 0; i < 4; i++)
    {
      b[i] = strtok(NULL, " ");
      if (b[i] == NULL)
      {
        clear_command(stdout);
        printf("the number of point is not enough.\n");
        return ERROR;
      }
    }
    for (int i = 0; i < 4; i++)
    {
      char *e;
      long v = strtol(b[i], &e, 10);
      if (*e != '\0')
      {
        clear_command(stdout);
        printf("Non-int value is included.\n");
        return ERROR;
      }
      p[i] = (int)v;
    }

    draw_rect(c, p[0], p[1], p[2], p[3]);
    clear_command(stdout);
    printf("1 rectangle drawn\n");
    return NORMAL;
  }

  if (strcmp(s, "circle") == 0)
  {                 //TODO
    int p[3] = {0}; // p[0]: x0, p[1]: y0, p[2]: x1, p[3]: x1
    char *b[3];
    for (int i = 0; i < 3; i++)
    {
      b[i] = strtok(NULL, " ");
      if (b[i] == NULL)
      {
        clear_command(stdout);
        printf("the number of point is not enough.\n");
        return ERROR;
      }
    }
    for (int i = 0; i < 3; i++)
    {
      char *e;
      long v = strtol(b[i], &e, 10);
      if (*e != '\0')
      {
        clear_command(stdout);
        printf("Non-int value is included.\n");
        return ERROR;
      }
      p[i] = (int)v;
    }

    draw_circle(c, p[0], p[1], p[2]);
    clear_command(stdout);
    printf("1 circle drawn\n");
    return NORMAL;
  }
  // color コマンド
  if (strcmp(s, "color") == 0)
  {
    s = strtok(NULL, " ");
    if (s == NULL)
    {
      clear_command(stdout);
      printf("the number of argument is not enough. Usage: color {white, black, blue, green, red, yellow, magenta, cyan, reset}\n");
      return ERROR;
    }

    int error = interpret_color(s, c);
    if (error == -1)
    {
      clear_command(stdout);
      printf("error: undefined color\n");
      return ERROR;
    }
    clear_command(stdout);
    printf("changed color to %s\n", s);
    return NORMAL;
  }

  // brushコマンド
  if (strcmp(s, "brush") == 0)
  {
    c->pen = ' ';
    c->pen_mode = 1;
    clear_command(stdout);
    printf("changed to brush mode\n");
    return NORMAL;
  }

  // penコマンド
  if (strcmp(s, "pen") == 0)
  {
    c->pen = DEFAULT_PEN;
    c->pen_mode = 0;
    clear_command(stdout);
    printf("changed to pen mode\n");
    return NORMAL;
  }

  // save コマンド
  if (strcmp(s, "save") == 0)
  {
    s = strtok(NULL, " ");
    save_history(s, his);
    clear_command(stdout);
    printf("saved as \"%s\"\n", (s == NULL) ? "history.txt" : s);
    return COMMAND;
  }

  // loadコマンド
  if (strcmp(s, "load") == 0)
  {
    const char *filename = strtok(NULL, " ");
    if (filename == NULL)
    {
      clear_command(stdout);
      printf("the number of argument is not enough. Usage: load filename\n");
      return ERROR;
    }

    int error = load_history(filename, his, c);
    clear_command(stdout);
    if (error == -1)
    {
      printf("error: cannot open file \"%s\"\n", filename);
    }
    else if (error == 0)
    {
      printf("loaded %s\n", filename);
    }
    return COMMAND;
  }

  // chpenコマンド
  if (strcmp(s, "chpen") == 0)
  {
    s = strtok(NULL, " ");
    if (s == NULL)
    {
      clear_command(stdout);
      printf("error. usage: chpen (pen)\n");
      return ERROR;
    }

    c->pen = s[0];

    clear_command(stdout);
    printf("changed pen to %c\n", s[0]);
    return NORMAL;
  }

  // undo コマンド
  if (strcmp(s, "undo") == 0)
  {
    reset_canvas(c);

    // 履歴の最後消さなきゃ
    pop_back(his);

    Command *p = his->begin;
    // 最初にデフォルトフォントにする
    c->pen = DEFAULT_PEN;
    c->penColor = DEFAULT_PENCOLOR;
    for (int i = 0; i < his_len(his); i++)
    {
      // 履歴を最初から全実行
      char *str = p->str;
      interpret_command(str, his, c);
      rewind_screen(stdout, 1);
      clear_command(stdout);
      p = p->next;
    }
    clear_command(stdout);
    printf("undo!\n");
    return COMMAND;
  }

  // redo コマンド
  if (strcmp(s, "redo") == 0)
  {
    reset_canvas(c);

    // 履歴を一つ戻す
    int error = pop_front(his);

    Command *p = his->begin;
    // 最初にデフォルトフォントにする
    c->pen = DEFAULT_PEN;
    c->penColor = DEFAULT_PENCOLOR;
    for (int i = 0; i < his_len(his); i++)
    {
      // 履歴を最初から全実行
      char *str = p->str;
      interpret_command(str, his, c);
      rewind_screen(stdout, 1);
      clear_command(stdout);
      p = p->next;
    }
    clear_command(stdout);
    if (error == -1)
    {
      printf("nothing to redo\n");
    }
    else
    {
      printf("redo!\n");
    }
    return COMMAND;
  }

  // quit コマンド
  if (strcmp(s, "quit") == 0)
  {
    return EXIT;
  }

  printf("error: unknown command\n");
  return UNKNOWN;
}
