/** ********************************************************************************
 *  @brief Montador: 
 *         Neste projeto em questão foi desenvolvido um algoritmo que abre um arquivo de imagem ou vídeo e permite ao usuário 
 *         clicar sobre um ponto na área da imagem ou vídeo, mostrando os valores da intensidade e a posição do pixel clicado. 
 *         Além disso, também foi feita uma rotina de seleção de pixels para pintar de vermelho os pixels ``parecidos'' com o clicado, 
 *         seguindo um critério de comparação definido pela função pinta.
 *         
 *
 * @author RAPHAEL SOARES - MAT. 14/0160299
 * @version 2.0
 * @since 26/08/2018
 *
 ********************************************************************************/

#include <../opencv2/opencv.hpp>
#include <iostream>
#include <fstream>
#include <cmath>
#include <algorithm>
#include <vector>
#include <string>

using namespace cv;
using namespace std;

#define COLOR_ROWS 80
#define COLOR_COLS 630

bool cinza = false;
int x2, y2;
bool clique = false;
bool clique_inicial = false;

void help(char **argv);
void pinta(Mat **img, Vec3b intensity_clique);
void onMouse(int event, int x, int y, int flags, void* userdata);
void imagem(char** argv);
void video (char** argv);
void checa_cinza(Mat *img);

int main( int argc, char** argv ) {
  if (argc < 2) {
    help(argv);
    return 0;
  }

  if(*argv[1] == '2')
    imagem(argv);
  else if(*argv[1] == '4' || *argv[1] == '3') 
    video(argv);
  else help(argv);

  return 0;
}

void help(char** argv ) {
	cout << "\n"
  << "Digite 2 e <path/imagem> para Req. 2 ou 3 e <path/video> para Req. 3 ou 4 para Req. 4\n"
  << "Por exemplo: \n"
  << argv[0] <<" 2 fruits.jpg\n" << endl;
}

void pinta(Mat** img, Vec3b intensity_clique) {
    // Mat D((*img)->rows, (*img)->cols, CV_8UC3, Scalar(blue_clique, green_clique, red_clique));

    // vector<Mat> bgr(3);
    // split(D, bgr);
    // absdiff(**img, D, E)
    // **img = D;

    for(int j=0; j<(*img)->cols; j++) 
    {
      for (int i=0; i<(*img)->rows; i++)
      {
        Vec3b intensity = (*img)->at<Vec3b>(Point(j,i));

        if((uint)intensity[2] == 255 && (uint)intensity[0] == 0 && (uint)intensity[1] == 0) continue;
        if(norm(intensity_clique, intensity, NORM_L2) < 13) (*img)->at<Vec3b>(Point(j,i)) = {0, 0, 255};


        // uint square = (uint)(blue_clique - blue)*(uint)(blue_clique - blue) + 
        //   (uint)(red_clique - red)*(uint)(red_clique - red) + 
        //   (uint)(green_clique - green)*(uint)(green_clique - green);
        // if((square*square) < 169)
        //     (*img)->at<Vec3b>(Point(j,i)) = {0, 0, 255};
      }
    }
}

void onMouse(int event, int x, int y, int flags, void* userdata) {
  if ( event == EVENT_LBUTTONDOWN ) {
    clique = true;
    clique_inicial = true;
    x2 = x;
    y2 = y;


    Mat* img = (Mat*) userdata;
    Vec3b intensity = img->at< Vec3b >(y, x);
    uchar blue_clique  = intensity[0];
    uchar green_clique = intensity[1];
    uchar red_clique   = intensity[2];
    float luminance = 1 - (0.299*(int)red_clique + 0.587*(int)green_clique + 0.114*(int)blue_clique) / 255;


    Scalar textColor;
    if (luminance < 0.5)
      textColor = Scalar(0,0,0);
    else 
      textColor = Scalar(255,255,255);
    
    Mat colorArray;

    if(!cinza) {

      string rgbText = "(" + to_string(x) + "," + to_string(y) +
            ") " + "=" + " [R = " + to_string(red_clique) + ", G = " + 
            to_string(green_clique) + ", B = " + to_string(blue_clique) + "]";

      colorArray = Mat(COLOR_ROWS, COLOR_COLS, CV_8UC3, Scalar(blue_clique,green_clique,red_clique));
      putText(colorArray, rgbText, Point2d(20, COLOR_ROWS - 20), FONT_HERSHEY_SIMPLEX, 0.8, textColor);
      imshow("Color", colorArray);

      cout << "Em (x,y) = (" << x << ", " << y <<
        "), onde y = coluna e x = linha, temos: " << endl << "(blue, green, red) = (" <<
        (uint)  blue_clique <<
        ", " << (uint)green_clique << ", " <<
        (uint)  red_clique << ")" << endl;
        
    } else {
      string rgbText = "(" + to_string(x) + "," + to_string(y) +
        ") " + "=" + " [CINZA = " + to_string(img->at<uchar>(y,x)) + "]";


      colorArray = Mat(COLOR_ROWS, COLOR_COLS, CV_8UC3, Scalar(blue_clique,green_clique,red_clique));
      putText(colorArray, rgbText, Point2d(20, COLOR_ROWS - 20), FONT_HERSHEY_SIMPLEX, 0.8, textColor);
      imshow("Color", colorArray);

      cout << "Em (x,y) = (" << x << ", " << y <<
        "), onde y = coluna e x = linha, temos o pixel cinza = " <<
        (uint) img->at<uchar>(y, x) << endl;
    }
    pinta(&img, intensity);
  } else {
    clique = false;
  }
}

void checa_cinza(Mat *img) {
  if((int)img->channels() == 1) {
    Mat img_gry;
    cvtColor( *img, img_gry, COLOR_GRAY2BGR, 3);
    img_gry.copyTo(*img);
    cinza = true;
  } else {
    Mat img_gry2;
    cvtColor( *img, img_gry2, COLOR_BGR2GRAY);
    cinza = equal(img->begin<uchar>(), img->end<uchar>(), img_gry2.begin<uchar>());
  } 
}


void imagem(char** argv) {
  Mat img = imread( argv[2], -1);
  
  if( img.empty() ) {
    cout << "Imagem nao encontrada!" << endl; 
    return;
  } 
  checa_cinza(&img);
  namedWindow( "Req1", WINDOW_AUTOSIZE );

  for(;;) {
    setMouseCallback("Req1", onMouse, &img);
    imshow( "Req1", img);
    if((char)waitKey(33) == 27 ) break; // verifica se o ESC foi teclado
  }
}

void video (char **argv) {
  namedWindow( "Req4", WINDOW_AUTOSIZE );

  VideoCapture cap(0); /* Comentar essa linha se deseja abrir vídeo do computador */
  // VideoCapture cap( argv[2] );


  if (!cap.isOpened()) {
    cout << "Error opening VideoCapture." << endl;
    return;
  }
  
  Mat frame;
  Mat* frame_out = &frame;
  double FPS = cap.get(CV_CAP_PROP_FPS);

  for(;;) {
    cap >> frame;
    if( frame.empty() ) break; // end if done
    checa_cinza(&frame);

    setMouseCallback("Req4", onMouse, &frame);
    if(!clique && clique_inicial) pinta(&frame_out, frame.at<Vec3b>(y2, x2));

    imshow( "Req4", frame );
    if((char)waitKey(1000.0/FPS) == 27) break; // verifica se o ESC foi teclado
  }
  cap.release();
}