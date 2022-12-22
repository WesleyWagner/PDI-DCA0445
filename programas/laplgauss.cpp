#include <iostream>
#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

void printmask(Mat &m)
{
  for (int i = 0; i < m.size().height; i++)
  {
    for (int j = 0; j < m.size().width; j++)
    {
      cout << m.at<float>(i, j) << ",";
    }
    cout << endl;
  }
}

void menu()
{
  cout << "\npressione a tecla para ativar o filtro: \n"
          "a - calcular modulo\n"
          "m - media\n"
          "g - gauss\n"
          "v - vertical\n"
          "h - horizontal\n"
          "l - laplaciano\n"
          "esc - sair\n";
}

int main(int argvc, char **argv)
{
  VideoCapture video;
  float media[] = {1, 1, 1,
                   1, 1, 1,
                   1, 1, 1};
  float gauss[] = {1, 2, 1,
                   2, 4, 2,
                   1, 2, 1};
  float horizontal[] = {-1, 0, 1,
                        -2, 0, 2,
                        -1, 0, 1};
  float vertical[] = {-1, -2, -1,
                      0, 0, 0,
                      1, 2, 1};
  float laplacian[] = {0, -1, 0,
                       -1, 4, -1,
                       0, -1, 0};

  Mat cap, frame, frame32f, frameFiltered;
  Mat mask(3, 3, CV_32F), mask1;
  Mat result, result2;
  double frame_width, frame_height, min, max;
  int absolut;
  char key;

  //Variável que indica se o filtro lapgauss está ativado
  bool lapgauss = false;

  video.open("/dev/video" + string(argv[1]));
  if (!video.isOpened())
  {
    std::cout << "cameras indisponiveis";
    return -1;
  }
  frame_width = video.get(cv::CAP_PROP_FRAME_WIDTH);
  frame_height = video.get(cv::CAP_PROP_FRAME_HEIGHT);
  std::cout << "largura=" << frame_width << "\n";
  ;
  std::cout << "altura =" << frame_height << "\n";
  ;

  namedWindow("filtroespacial", 1);

  mask = Mat(3, 3, CV_32F, media);
  scaleAdd(mask, 1 / 9.0, Mat::zeros(3, 3, CV_32F), mask1);
  swap(mask, mask1);
  absolut = 1; // calcs abs of the image

  menu();
  for (;;)
  {
    video >> cap;
    cvtColor(cap, frame, cv::COLOR_BGR2GRAY);
    imshow("original", frame);
    flip(frame, frame, 1);
    frame.convertTo(frame32f, CV_32F);

    //Se o filtro lapgauss estiver ativado, o programa aplica a máscara do laplaciano após aplicar
    //a máscara gaussiana (que foi definida abaixo). Caso contrário, apenas aplica a máscara selecionada.
    if (lapgauss)
    {
      Mat frameFilteredTemp, mask2(3, 3, CV_32F, laplacian);
      filter2D(frame32f, frameFilteredTemp, frame32f.depth(), mask, Point(1, 1), 0);
      filter2D(frameFilteredTemp, frameFiltered, frameFilteredTemp.depth(), mask2, Point(1, 1), 0);
    }
    else
    {
      filter2D(frame32f, frameFiltered, frame32f.depth(), mask, Point(1, 1), 0);
    }

    if (absolut)
    {
      frameFiltered = abs(frameFiltered);
    }
    frameFiltered.convertTo(result, CV_8U);
    frameFiltered.convertTo(result2, CV_8U);

    flip(result, result, 1);
    imshow("filtro laplgauss", result);
    key = (char)waitKey(30);
    if (key == 27)
      break; // esc pressed!

    switch (key)
    {
    case 'a':
      menu();
      absolut = !absolut;
      lapgauss = false;
      break;
    case 'm':
      menu();
      mask = Mat(3, 3, CV_32F, media);
      scaleAdd(mask, 1 / 9.0, Mat::zeros(3, 3, CV_32F), mask1);
      mask = mask1;
      printmask(mask);
      lapgauss = false;
      break;
    case 'g':
      menu();
      mask = Mat(3, 3, CV_32F, gauss);
      scaleAdd(mask, 1 / 16.0, Mat::zeros(3, 3, CV_32F), mask1);
      mask = mask1;
      printmask(mask);
      lapgauss = false;
      break;
    case 'h':
      menu();
      mask = Mat(3, 3, CV_32F, horizontal);
      printmask(mask);
      lapgauss = false;
      break;
    case 'v':
      menu();
      mask = Mat(3, 3, CV_32F, vertical);
      printmask(mask);
      lapgauss = false;
      break;
    case 'l':
      menu();
      mask = Mat(3, 3, CV_32F, laplacian);
      printmask(mask);
      lapgauss = false;
      break;

    //filtro laplgauss
    case 'x':
      menu();
      mask = Mat(3, 3, CV_32F, gauss);
      scaleAdd(mask, 1 / 16.0, Mat::zeros(3, 3, CV_32F), mask1);
      mask = mask1;
      printmask(mask);
      lapgauss = true;
      break;
    default:
      break;
    }
  }
  return 0;
}