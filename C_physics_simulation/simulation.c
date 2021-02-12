#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include "physics2.h"

int main(int argc, char **argv)
{
  const Condition cond = {
		    .width  = 75,
		    .height = 40,
		    .G = 1.0,
		    .dt = 0.5,
		    .cor = 0.7
  };
  
 /* ファイルを引数にとるか、ない場合はデフォルトの初期値を使う */
  size_t objnum = 3;
  if (argc >=3 ){
    objnum =(size_t) atoi(argv[1]);
  } 
  Object objects[objnum];


  if ( argc > 3 || argc ==2 ) {
    fprintf(stderr, "usage: %s [number of object] [filename for init]\n", argv[0]);
    return EXIT_FAILURE;
  }
  else if (argc == 3) {    
    FILE *lgfile;
    if ( (lgfile = fopen(argv[2],"r")) != NULL ) {
      //char *ext = strrchr(argv[2], '.'); //拡張子
      
      const size_t bufsize = 100;
      char buf[bufsize];
      double c1,c2,c3,c4,c5;
      int flg = 0;
      while(fgets(buf, bufsize, lgfile) != NULL){
          if(buf[0]=='#') {
            continue;
          }
          if(sscanf(buf,"%lf %lf %lf %lf %lf%*[^\n]%*[\n]",&c1, &c2, &c3, &c4, &c5)==5){
            // 最初から壁にのめりこんでしまったら、少しづらしてあげる。
            if (c2 == cond.width/2) c2 = cond.width/2-0.01;
            if (c2 == -cond.width/2) c2 = -cond.width/2 + 0.01;
            if (c3 == cond.height/2) c3 = cond.height/2 -0.01;
            if (c3 == -cond.height/2) c3 = -cond.height/2 + 0.01;

            objects[flg] = (Object){.m = c1, .x = c2, .y = c3, .vx = c4, .vy = c5, .show_flg = 1};
            flg++;
          }
       }
       if(flg< objnum){
         for (int i=flg; i<objnum; i++){
           objects[i]=(Object){.m=0, .x = 0, .y = 0, .vx = 0, .vy = 0, .show_flg = 0};
         }
         objnum = flg;
       }
     }
    else{
      fprintf(stderr,"cannot open file %s\n",argv[2]);
      return EXIT_FAILURE;
    }
    fclose(lgfile);
  }
  else{
    // デフォルト値
    objects[0] = (Object){ .m = 600.0, .y = -19.9, .vy = 20.0, .x =0, .vx = 10.0, .show_flg = 1};
    objects[1] = (Object){ .m = 10000.0, .y =  1000.0, .vy = 0.0, .x = 0, .vx = 0.0, .show_flg = 1};
    objects[2] = (Object){ .m = 50.0, .y = 20, .y =10.0, .x =20, .vx = -10.0, .show_flg = 1};
  }
  // シミュレーション. ループは整数で回しつつ、実数時間も更新する

  const double stop_time = 4000;
  double t = 0;
  printf("\n");
  for (int i = 0 ; t <= stop_time ; i++){
    t = i * cond.dt;
    my_combine(objects, objnum, cond);
    my_update_velocities(objects, objnum, cond);
    my_update_positions(objects, objnum, cond);
    my_bounce(objects, objnum, cond);
    // 表示の座標系は width/2, height/2 のピクセル位置が原点となるようにする
    my_plot_objects(objects, objnum, t, cond);
    usleep(100 * 1000); // 200 x 1000us = 200 ms ずつ停止
    printf("\e[%dA", cond.height+4+(int)objnum);// 壁とパラメータ表示分で3行 と思いきやなんかバグるので4行。
  }
  return EXIT_SUCCESS;
}

// 実習: 以下に my_ で始まる関数を実装する
// 最終的に phisics2.h 内の事前に用意された関数プロトタイプをコメントアウト

void my_plot_objects(Object objs[], const size_t numobj, const double t, const Condition cond)
{
  int height = cond.height;
  int width = cond.width;
  
  printf("+");
  for (int x = floor(-width/2); x <= floor(width/2) ; x++ ){
    printf("-");
  }
  printf("+\n");


  for (int y=floor(-height/2); y<=floor(height/2) ; y++){
    printf("|");
    for (int x = floor(-width/2); x <= floor(width/2) ; x++){
      int flg = 0;
      for(int i=0; i<numobj; i++){ 
         if(objs[i].show_flg == 1 && x == (int)objs[i].x && y == (int)objs[i].y){
           printf("o");
           flg = 1;
           break;
         }
      }
      if(flg == 0){
           printf(" ");
      } 
    }
    printf("|");
    printf("\n");
  }


  printf("+");
  for (int x = floor(-width/2); x <= floor(width/2) ; x++ ){
    printf("-");
  }
  printf("+\n");
  printf("t = %4.1lf\n", t);
  for (int i = 0; i <numobj; i++){
    if (objs[i].show_flg == 1){
        printf("objs[%d].m = %9.2lf, objs[%d].x = %6.2lf, objs[%d].y = %6.2lf, objs[%d].vx = %6.2lf, objs[%d].vy = %6.2lf\n", i, objs[i].m, i, objs[i].x, i, objs[i].y, i, objs[i].vx, i, objs[i].vy);
    } else {
        printf("                                                                                                                                                \n");
    }
  }
}

void my_update_velocities(Object objs[], const size_t numobj, const Condition cond)
{
  double ay[numobj];
  double ax[numobj];
  for (int i =0; i<numobj; i++){
    ay[i]=0;
    ax[i]=0;
  }

  const double G = cond.G;
  const double dt = cond.dt;


  // 加速度更新
  for (int i = 0; i < numobj; i++){
    for (int j = 0; j <numobj; j++){
      if(j != i && objs[i].show_flg == 1 && objs[j].show_flg == 1) {
        const double xi = objs[i].x, yi = objs[i].y, xj = objs[j].x, yj = objs[j].y;
        double r = sqrt(pow(xj-xi, 2)+pow(yj-yi,2));
        if (r > 0.1){
          ay[i] = ay[i] + G* objs[j].m *(yj-yi) / pow(r,3);
          ax[i] = ax[i] + G* objs[j].m *(xj-xi) / pow(r,3);
        }        
      }
    }
  }

  // 速度更新
  for (int i=0; i<numobj; i++){
    objs[i].vy = objs[i].vy + ay[i]*dt;
    objs[i].vx = objs[i].vx + ax[i]*dt;
  }
}


void my_update_positions(Object objs[], const size_t numobj, const Condition cond)
{
  double dt = cond.dt;
  for (int i=0; i<numobj; i++){
    objs[i].prev_y = objs[i].y;
    objs[i].prev_x = objs[i].x;
    objs[i].y = objs[i].y + objs[i].vy * dt;
    objs[i].x = objs[i].x + objs[i].vx * dt;
  }
}

void my_bounce(Object objs[], const size_t numobj, const Condition cond)
{
  const double e = cond.cor;
  const int height = cond.height;
  const int width = cond.width;
  for (int i=0; i<numobj; i++){
    while(objs[i].show_flg ==1 && ((abs(objs[i].y)>=(height/2) && abs(objs[i].prev_y) <= (height/2)) || (abs(objs[i].y)<=(height/2) && abs(objs[i].prev_y) >= (height/2)) || (abs(objs[i].x)>=(width/2) && abs(objs[i].prev_x) <= (width/2)) || (abs(objs[i].x)<=(width/2) && abs(objs[i].prev_x) >= (width/2)) )){
      // 初期位置から壁にめり込んでたら動けなくなるので、ずらしてあげる。
      if(objs[i].prev_y==height/2 || objs[i].prev_y==-height/2 || objs[i].prev_x == width/2 || objs[i].prev_x == -width/2){
        break;
      }
        // 中から下へ。
          if(objs[i].y>=(height/2) && objs[i].prev_y <= (height/2)){
            objs[i].y = height/2 - (objs[i].y-height/2)*e;
            objs[i].vy = - objs[i].vy *e ;
          }
          // 中から上へ
          if(objs[i].y<=(-height/2) && objs[i].prev_y >= (-height/2)){
            objs[i].y = -height/2 + (-height/2-objs[i].y)*e;
            objs[i].vy = - objs[i].vy *e ;
          }

          // 下から中へ。
          if(objs[i].y<=(height/2) && objs[i].prev_y >= (height/2)){
            objs[i].y = height/2 + (height/2-objs[i].y)*e;
            objs[i].vy = - objs[i].vy *e ;
          }
          // 上から中へ
          if(objs[i].y>=(-height/2) && objs[i].prev_y <= (-height/2)){
            objs[i].y = -height/2 - (objs[i].y-height/2)*e;
            objs[i].vy = - objs[i].vy *e ;
          }
          // 中から右へ。
          if(objs[i].x>=(width/2) && objs[i].prev_x <= (width/2)){
            objs[i].x = width/2 - (objs[i].x-width/2)*e;
            objs[i].vx = - objs[i].vx *e ;
          }
          // 中から左へ
          if(objs[i].x<=(-width/2) && objs[i].prev_x >= (-width/2)){
            objs[i].x = -width/2 + (-width/2-objs[i].x)*e;
            objs[i].vx = - objs[i].vx *e ;
          }

          // 右から中へ。
          if(objs[i].x<=(width/2) && objs[i].prev_x >= (width/2)){
            objs[i].x = width/2 + (width/2-objs[i].x)*e;
            objs[i].vx = - objs[i].vx *e ;
          }
          // 左から中へ
          if(objs[i].x>=(-width/2) && objs[i].prev_x <= (-width/2)){
            objs[i].x = -width/2 - (objs[i].x-width/2)*e;
            objs[i].vx = - objs[i].vx *e ;
         }
      }
   }
}

void my_combine(Object objs[], const size_t numobj, const Condition cond){
  for(int i=0; i<numobj; i++){
    for (int j=i+1; j<numobj; j++){
        double r = sqrt(pow(objs[j].x-objs[i].x, 2)+pow(objs[j].y-objs[i].y, 2));
        if((objs[i].show_flg ==1  && objs[j].show_flg == 1 ) && r< 3){
          double sum_weight = objs[i].m + objs[j].m;
          objs[i].vx = (objs[i].m * objs[i].vx + objs[j].m * objs[j].vx)/sum_weight;
          objs[i].vy = (objs[i].m * objs[i].vy + objs[j].m * objs[j].vy)/sum_weight;
          objs[i].m = sum_weight;
          objs[j].m = 0;
          objs[j].x = 0;
          objs[j].y = 0;
          objs[j].vx = 0;
          objs[j].vy = 0;
          objs[j].show_flg = 0;
        }
    }
  }
}