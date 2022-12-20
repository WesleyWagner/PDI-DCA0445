#include <iostream>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

int main(int argc, char **argv)
{
  int des_altura_hist = 64;
  int des_largura_hist = 256;

  //limite da diferença entre histogramas, servindo como gatilho para
  //detecção
  int limite = 15;

  VideoCapture video;
  Mat frame;

  //É criado vetor para armazenar componentes RGB
  vector<Mat> componentes_rgb;

  //Armazenam histograma do frame passado e do atual
  Mat hist_passado;

  //Intervalo do histograma
  float range[] = {0, 256};
  const float *histRange = {range};
  int tamanho_histograma = 256;

  video.open(argv[1]);
  if (!video.isOpened())
  {
    cout << "Falha na abertura" << endl;
    return (-1);
  }

  int largura = video.get(cv::CAP_PROP_FRAME_WIDTH);

  namedWindow("Video", 1);
  while (true)
  {
    Mat histG, histG_norm;
    video >> frame;

    //Separa a imagem colorida em 3 canais
    split(frame, componentes_rgb);

    //Calcula histograma da componente verde
    calcHist(&componentes_rgb[1], 1, 0, Mat(), histG, 1, &tamanho_histograma, &histRange);

    //Cria matriz para o desenho do histograma
    Mat des_hist_atual(des_altura_hist, des_largura_hist, CV_8UC3, Scalar(0, 0, 0));

    //Normaliza o histograma da cor verde de 0 a 64
    normalize(histG, histG_norm, 0, des_hist_atual.rows, NORM_MINMAX, -1, Mat());

    //Calcula erro relativo médio
    if (!hist_passado.empty())
    {
      double erro_verde = 0.0;

      //Compara cada posição do histograma atual como antigo
      for (int i = 0; i < tamanho_histograma; i++)
      {
        if (histG.at<float>(i) != 0)
        {
          erro_verde += abs((histG.at<float>(i) - hist_passado.at<float>(i)) / histG.at<float>(i));
        }
      }

      //Desenha um circulo vermelho caso detecte movimento
      if (erro_verde > limite)
      {
        circle(frame, Point(largura - 20, 20), 10, Scalar(0, 0, 255), cv::FILLED);
      }

      //Desenha os histogramas
      for (int i = 0; i < des_largura_hist; i++)
      {
        line(des_hist_atual,
             Point(i, des_altura_hist),
             Point(i, des_altura_hist - cvRound(histG_norm.at<float>(i))),
             Scalar(255, 255, 255), 1, 8, 0);
      }
    }

    //Copia histograma no frame
    des_hist_atual.copyTo(frame(Rect(0, 0, des_largura_hist, des_altura_hist)));

    imshow("Video", frame);
    if (waitKey(30) == 27)
      break;

    histG.copyTo(hist_passado);
  }

  return 0;
}