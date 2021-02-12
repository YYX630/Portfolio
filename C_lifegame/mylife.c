#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

// 発展課題変更点：
// 1世代の時間を0.1秒に変更
// 池(青+)を設置。池の一マスは一人分の生命体に相当。動かない。
// 壁(白X)を設置。壁は生命体ではない。動かない
// 生命体の生成条件を、3個or6個にする。
// 50世代ごとにどこかにランダムに変異種が出現。（黄色#）変異種は元の種の生成には寄与しない（壁と同等とカウント）しかし、自力で動き、移動先の生命体を消滅させる。壁にぶつかると死ぬ。共食いもする。
// 結果：池の周りには固定された生命体群が形成されやすい。壁の形によっては、壁の周りにも安定な生息が可能だとわかった。変異体により、いったん固定化された生命体群が再び散ることもおおく、予測不可能性が増した。

void my_init_cells(int height, int width, int cell[height][width], FILE *lgfile, char *ext){
  
// 池の設定(値＝99)
  for(int i=0; i<5; i++){
    int a = rand() % height;
    int b = rand() % width;
    if(a+1<height && b+1<width){
      cell[a][b]=99;
      cell[a][b+1]=99;
      cell[a+1][b]=99;
      cell[a+1][b+1]=99;
    } else {
      i--;
    }
  }
// 壁の設置(値=44)
for (int y=30; y>10; y--){
  if(y<height && 120-4*y<width){
    cell[y][120-4*y]=44;
  }
}





  // ファイル指定なしの場合の設定
    if (lgfile==NULL) {
      for (int i=0; i<height*width/(float)10; i++){
          int a = rand() % height;
          int b = rand() % width;
          if(cell[a][b]!=0) {
            i=i-1; //もしすでに値が入ってたら、やり直し。iを1減らす
          } else if (cell[a][b]==0) {
            cell[a][b]=1;
          }
      }
      return ;
    }

  // lifファイルの読み込み
    if(strcmp(ext, ".lif")==0){
        const size_t bufsize = 20;
        char buf[bufsize];
        int x=0;
        int y=0;
        while(fgets(buf, bufsize, lgfile) != NULL){
            if(buf[0]=='#') continue;
            if(sscanf(buf,"%d %d",&x, &y)==2){
            cell[y][x]=1;
            }
        }
        return;
    }

  // rleファイルの読み込み
    if(strcmp(ext, ".rle")==0){
        const size_t bufsize = 100;
        char buf[bufsize];
        int initial_x=10;
        int initial_y=10;
        int x=initial_x;
        int y=initial_y;
        int num=1; 
        char c;

        while(fgets(buf, bufsize, lgfile) != NULL){
          if (buf[0]=='#' || buf[0]=='x'){
            continue;
          }
            for(int i=0; i<bufsize; i++){
                c = buf[i];
                if(c=='!'){
                    return;
                }
                if(c=='b'){
                    x=x+num;
                    num=1;
                } else if (c == 'o'){
                    for(int n = 0; n<num; n++ ){
                        cell[y][x]=1;
                        x++;
                    }
                    num =1;
                } else if (c == '$'){
                    y++;
                    x=initial_x;
                    num=1;
                } else if (c>='0' && c<='9') {
                    // 数字ならば。
                    char stack[3]={'\0','\0','\0'};
                    stack[0] = c;
                    int j = 0;
                    while(buf[i+1]>='0' && buf[i+1] <='9'){
                        i++;
                        j++;
                        stack[j] = buf[i];
                    }
                    sscanf(stack, "%d", &num);
                }
            }
        }
    }
}



float calculate_percentage(int height, int width, int cell[height][width]){
  int count = 0;
  int all = 0;
  for(int y=0; y<height; y++){
    for (int x=0; x<width; x++){
      all++;
      if(cell[y][x]==1) count++;
    }
  }
  return (float)count/(float)all*100;
}

int my_print_cells(FILE *fp, int gen, int height, int width, int cell[height][width]){
  
  float occupancy=calculate_percentage(height, width, cell);
  
  fprintf(fp,"generation = %d\n", gen); // この場合 (fp = stdout), printfと同じ
  fprintf(fp, "occupancy = %.2f%%\n", occupancy);
  fprintf(fp,"+");
  for (int x = 0 ; x < width ; x++)
    fprintf(fp, "-");
  fprintf(fp, "+\n");
  
  /* 外壁と 内側のゲーム部分 */
  for (int y = 0; y < height; y++) {
    fprintf(fp,"|");
    for (int x = 0; x < width; x++) {
      // ANSIエスケープコードを用いて、赤い"#" を表示
      // \e[31m で 赤色に変更
      // \e[0m でリセット（リセットしないと以降も赤くなる）
      if(cell[y][x]==1){
	       fprintf(fp, "\e[31m#\e[0m");
      }
      else if(cell[y][x]==99){
        fprintf(fp, "\e[34m+\e[0m");
      }
      else if (cell[y][x]==44){
        fprintf(fp, "X");

      }
      else if (cell[y][x]==2){
        fprintf(fp, "\e[33m#\e[0m");
      }
      else{
	      fprintf(fp, " ");
      }
    }
    fprintf(fp,"|\n");
  }


  // 下の壁
  fprintf(fp, "+");
  for (int x = 0 ; x < width ; x++)
    fprintf(fp, "-");
  fprintf(fp, "+\n");
  
  fflush(fp); // バッファされている文字列を出力する

  return EXIT_SUCCESS; // stdlib.h で定義されている実行成功を表す整数マクロ: 実体は0

}


int judge(int cellvalue){
  if (cellvalue==1) return 1;//生命体
  if (cellvalue==0) return 0;//空白
  if (cellvalue==99) return 1;//池
  if (cellvalue==44) return 0;//壁
  if (cellvalue==2) return 0;//変異種
  return 0;
}

void my_count_adjacent_cells(int height, int width, int cell[height][width], int count[height][width]){

  for (int y=0; y<height; y++){
    for (int x=0; x<width; x++){
      if(x!=0 && x!=width-1 && y!=0 && y!= height-1){
        count[y][x]= judge(cell[y-1][x-1])+judge(cell[y-1][x])+judge(cell[y-1][x+1])+judge(cell[y][x-1])+judge(cell[y][x+1])+judge(cell[y+1][x-1])+judge(cell[y+1][x])+judge(cell[y+1][x+1]);
      } 
      else if (y==0){
        if (x==0) {
          count[y][x] = judge(cell[y][x+1])+judge(cell[y+1][x])+judge(cell[y+1][x+1]);
        } else if (x== width-1) {
          count[y][x] = judge(cell[y][x-1])+judge(cell[y+1][x])+judge(cell[y+1][x-1]); 
        } else {
          count[y][x] = judge(cell[y][x-1])+judge(cell[y][x+1])+judge(cell[y+1][x-1])+judge(cell[y+1][x])+judge(cell[y+1][x+1]);
        }
      }
      else if (y==height-1){
        if(x==0) {
          count[y][x] = judge(cell[y][x+1])+judge(cell[y-1][x])+judge(cell[y-1][x+1]);
        } else if(x== width-1) {
          count[y][x] = judge(cell[y][x-1])+judge(cell[y-1][x])+judge(cell[y-1][x-1]); 
        } else {
          count[y][x] = judge(cell[y][x-1])+judge(cell[y][x+1])+judge(cell[y-1][x-1])+judge(cell[y-1][x])+judge(cell[y-1][x+1]);
        }
      }
      else {
        if(x==0) {
          count[y][x] = judge(cell[y-1][x])+judge(cell[y-1][x+1])+judge(cell[y][x+1])+judge(cell[y+1][x])+judge(cell[y+1][x+1]);
        } else if(x==width-1) {
          count[y][x] = judge(cell[y-1][x-1])+judge(cell[y-1][x])+judge(cell[y][x-1])+judge(cell[y+1][x-1])+judge(cell[y+1][x]);
        }
      }
    }
  }
}


void my_update_cells(int gen,int height, int width, int cell[height][width]){
  int count[height][width];

  for(int y = 0 ; y < height ; y++){
    for(int x = 0 ; x < width ; x++){
      count[y][x] = 0;
    }
  }

  my_count_adjacent_cells(height, width, cell, count);

   for (int y=0; y<height; y++){
    for (int x=0; x<width; x++){
      if(cell[y][x]==0 || cell[y][x]==1){
                int a = count[y][x];
                switch (a)
                {
                case 2:
                  break;

                case 3:
                  cell[y][x]=1;
                  break; 
                
                case 6:
                  cell[y][x]=1;

                default:
                  cell[y][x]=0;
                  break;
        }
      }
    }
  }
  

  // 50世代ごとに変異種を自動で出現させる
  if(gen % 50 ==0){
    int a = rand() %height;
    int b = rand() % width;
    cell[a][b]=2;
  }


  // 変異種の移動を最後に行う。
  int toChange[100][2];
  for (int i =0; i<100;i++){
    toChange[i][0]=-1;
    toChange[i][1]=-1;
  }

  int i=0;
  for (int y=0; y<height; y++){
    for (int x=0; x<width; x++){
      if(cell[y][x]==2){
        int a = rand() %5 ;
        switch (a)
          {
          case 0:
            break;
          case 1:
            cell[y][x]=0;
            toChange[i][0]=y;
            toChange[i][1]=x+1;
            i++;
            break;
          
          case 2:
            cell[y][x]=0;
            toChange[i][0]=y;
            toChange[i][1]=x-1;
            i++;
            break;
          case 3:
            cell[y][x]=0;
            toChange[i][0]=y+1;
            toChange[i][1]=x;
            i++;
            break;
          case 4:
            cell[y][x]=0;
            toChange[i][0]=y-1;
            toChange[i][1]=x;
            i++;
            break;

          default:
            break;
          }
        }
      }
    }
    for (int j=0; j<i; j++){
      if(toChange[j][0]<height && toChange[j][0]>=0 && toChange[j][1]<width && toChange[j][1]>0 && (cell[toChange[j][0]][toChange[j][1]]==0 || cell[toChange[j][0]][toChange[j][1]]==1 || cell[toChange[j][0]][toChange[j][1]]==2)){
           cell[toChange[j][0]][toChange[j][1]]=2;
      }
    }
  }









int main(int argc, char **argv){
  srand(time(NULL));
  FILE *fp = stdout; //出力先
  const int height = 40;
  const int width = 70;

  int cell[height][width];
  for(int y = 0 ; y < height ; y++){
    for(int x = 0 ; x < width ; x++){
      cell[y][x] = 0;
    }
  }

  /* ファイルを引数にとるか、ない場合はデフォルトの初期値を使う */
  if ( argc > 2 ) {
    fprintf(stderr, "usage: %s [filename for init]\n", argv[0]);
    return EXIT_FAILURE;
  }
  else if (argc == 2) {
    FILE *lgfile;
    if ( (lgfile = fopen(argv[1],"r")) != NULL ) {
        char *ext = strrchr(argv[1], '.');
        my_init_cells(height,width,cell,lgfile, ext); // ファイルによる初期化
    }
    else{
      fprintf(stderr,"cannot open file %s\n",argv[1]);
      return EXIT_FAILURE;
    }
    fclose(lgfile);
  }
  else{
      my_init_cells(height, width, cell, NULL, NULL); // デフォルトの初期値を使う
  }

  my_print_cells(fp, 0, height, width, cell); // 表示する
  sleep(1); // 1秒休止
  fprintf(fp,"\e[%dA",height+4);//height+4 の分、カーソルを上に戻す(壁2、表示部2)

//   /* 世代を進める*/
  for (int gen = 1 ;; gen++) {
    my_update_cells(gen,height, width, cell); // セルを更新
    my_print_cells(fp, gen, height, width, cell);  // 表示する
    usleep(100000); //0.1秒休止する
    fprintf(fp,"\e[%dA",height+4);//height+4 の分、カーソルを上に戻す(壁2、表示部2)
  }

  return EXIT_SUCCESS;
}
