#include <iostream>
#include <opencv2/opencv.hpp>

using namespace std;
void separar_nome_do_arquivo(const string &path, string &filename, string &extension);

int main(int argc, char **argv)
{
  cv::Mat frame;
  int width, height;
  cv::VideoCapture video;
  std::vector<cv::Mat> planes;
  cv::Mat histR, histG, histB, histR_passado, histG_passado, histB_passado;
  int nbins = 64;
  float range[] = {0, 255};
  const float *histrange = {range};
  bool uniform = true;
  bool acummulate = false;
  uchar key = 0;

  int limite = 15;

  int frames_per_second = 60;
  cv::TickMeter tm;
  bool motion_flag = false;
  tm.reset();

  double t = (double)cv::getTickCount();
  double millis = ((double)cv::getTickCount() - t) / cv::getTickFrequency();

  while (1)
  {
    cout << "\ntecla 'c' para abrir stream da camera virtual:" << key << endl;
    cin >> key;
    cin.ignore();

    if (key == '\0')
      return 0;
    if (key == 'c')
      break;
  }

  if (key == 'c')
    video.open("/dev/video" + string(argv[1]));
  // 0 - Irium CAM Android
  // 1 - Splicam Stream
  // 2 - Desconhecido
  if (!video.isOpened())
  {
    std::cout << "cameras indisponiveis";
    return -1;
  }
  // video.set(cv::CAP_PROP_FRAME_WIDTH, 1024);
  // video.set(cv::CAP_PROP_FRAME_HEIGHT, 768);
  // width = cap.get(cv::CAP_PROP_FRAME_WIDTH);
  // height = cap.get(cv::CAP_PROP_FRAME_HEIGHT);
  int frame_width = static_cast<int>(video.get(cv::CAP_PROP_FRAME_WIDTH));
  int frame_height = static_cast<int>(video.get(cv::CAP_PROP_FRAME_HEIGHT));
  cv::Size frame_size(frame_width, frame_height);
  cv::VideoWriter output("motiondetect_Video.avi", cv::VideoWriter::fourcc('M', 'J', 'P', 'G'), frames_per_second, frame_size);

  std::cout << "largura = " << frame_width << std::endl;
  std::cout << "altura  = " << frame_height << std::endl;

  int histw = nbins, histh = nbins / 2;
  cv::Mat histImgR(histh, histw, CV_8UC3, cv::Scalar(0, 0, 0));
  cv::Mat histImgG(histh, histw, CV_8UC3, cv::Scalar(0, 0, 0));
  cv::Mat histImgB(histh, histw, CV_8UC3, cv::Scalar(0, 0, 0));
  cv::Mat hist_passado_R, hist_passado_G, hist_passado_B;

  while (1)
  {
    t = (((double)cv::getTickCount() - t)/cv::getTickFrequency());
    double erro_B = 0.0;
    double erro_G = 0.0;
    double erro_R = 0.0;
    if (video.isOpened())
      video >> frame;
    else
      frame = cv::imread(argv[1], cv::IMREAD_COLOR);

    
    // std::vector<cv::Mat> eq(3);

    cv::split(frame, planes);

    // cv::equalizeHist(planes[0],eq_B);
    // cv::equalizeHist(planes[1],eq_G);
    // cv::equalizeHist(planes[2],eq_R);

    cv::calcHist(&planes[2], 1, 0, cv::Mat(), histR, 1,
                 &nbins, &histrange,
                 uniform, acummulate);
    cv::calcHist(&planes[1], 1, 0, cv::Mat(), histG, 1,
                 &nbins, &histrange,
                 uniform, acummulate);
    cv::calcHist(&planes[0], 1, 0, cv::Mat(), histB, 1,
                 &nbins, &histrange,
                 uniform, acummulate);

    cv::normalize(histR, histR, 0, histImgR.rows, cv::NORM_MINMAX, -1, cv::Mat());
    cv::normalize(histG, histG, 0, histImgG.rows, cv::NORM_MINMAX, -1, cv::Mat());
    cv::normalize(histB, histB, 0, histImgB.rows, cv::NORM_MINMAX, -1, cv::Mat());

    if (!hist_passado_R.empty())
    {
      for (int i = 0; i < nbins; i++)
      {
        if (histG.at<float>(i) != 0)
        {
          erro_B += abs((histB.at<float>(i) - hist_passado_B.at<float>(i)));
          erro_G += abs((histG.at<float>(i) - hist_passado_G.at<float>(i)));
          erro_R += abs((histR.at<float>(i) - hist_passado_R.at<float>(i)));
        }
      }

      if (erro_B > limite || erro_G > limite || erro_R > limite)
      {
        motion_flag = true;
        millis = t;
        
      }

      cout << setprecision(2) << "\t er B " << erro_B;
        cout << "\t er G " << erro_G;
        cout << "\t er R " << erro_R  << endl;

      if (t - millis > 2)
      {
        motion_flag = false;
        // cout << "Tempo " << (t- millis)  << endl;
      }

      if (motion_flag)
      {
        circle(frame, cv::Point(20, frame.rows - 20), 10, cv::Scalar(0, 0, 255), cv::FILLED);
        cv::putText(frame,                          // target image
                    "Movimento detectado!",         // text
                    cv::Point(50, frame.rows - 10), // botton-left position
                    cv::FONT_HERSHEY_DUPLEX,
                    1.0,
                    CV_RGB(118, 185, 0), // font color
                    2);
      }
    }

    histImgR.setTo(cv::Scalar(0));
    histImgG.setTo(cv::Scalar(0));
    histImgB.setTo(cv::Scalar(0));

    for (int i = 0; i < nbins; i++)
    {
      cv::line(histImgR,
               cv::Point(i, histh),
               cv::Point(i, histh - cvRound(histR.at<float>(i))),
               cv::Scalar(0, 0, 255), 1, 8, 0);
      cv::line(histImgG,
               cv::Point(i, histh),
               cv::Point(i, histh - cvRound(histG.at<float>(i))),
               cv::Scalar(0, 255, 0), 1, 8, 0);
      cv::line(histImgB,
               cv::Point(i, histh),
               cv::Point(i, histh - cvRound(histB.at<float>(i))),
               cv::Scalar(255, 0, 0), 1, 8, 0);
    }
    histImgR.copyTo(frame(cv::Rect(0, 0, nbins, histh)));
    histImgG.copyTo(frame(cv::Rect(0, histh, nbins, histh)));
    histImgB.copyTo(frame(cv::Rect(0, 2 * histh, nbins, histh)));

    cv::namedWindow("image", cv::WINDOW_KEEPRATIO | cv::WINDOW_NORMAL);
    cv::imshow("image", frame);

    cv::normalize(histR, histR, 0, histImgR.rows, cv::NORM_MINMAX, -1, cv::Mat());
    cv::normalize(histG, histG, 0, histImgG.rows, cv::NORM_MINMAX, -1, cv::Mat());
    cv::normalize(histB, histB, 0, histImgB.rows, cv::NORM_MINMAX, -1, cv::Mat());

    histImgR.setTo(cv::Scalar(0));
    histImgG.setTo(cv::Scalar(0));
    histImgB.setTo(cv::Scalar(0));

    for (int i = 0; i < nbins; i++)
    {
      cv::line(histImgR,
               cv::Point(i, histh),
               cv::Point(i, histh - cvRound(histR.at<float>(i))),
               cv::Scalar(0, 0, 255), 1, 8, 0);
      cv::line(histImgG,
               cv::Point(i, histh),
               cv::Point(i, histh - cvRound(histG.at<float>(i))),
               cv::Scalar(0, 255, 0), 1, 8, 0);
      cv::line(histImgB,
               cv::Point(i, histh),
               cv::Point(i, histh - cvRound(histB.at<float>(i))),
               cv::Scalar(255, 0, 0), 1, 8, 0);
    }

    cv::namedWindow("image", cv::WINDOW_KEEPRATIO | cv::WINDOW_NORMAL);
    cv::imshow("image", frame);

    key = cv::waitKey(20);
    if (key == 27)
      break;
    if (key == ' ')
      break;

    output.write(frame);
    // imshow("Video", frame);

    histB.copyTo(hist_passado_B);
    histG.copyTo(hist_passado_G);
    histR.copyTo(hist_passado_R);
  }
  video.release();
  output.release();
  return 0;
}

void separar_nome_do_arquivo(const string &path, string &filename, string &extension)
{
  string
      arquivo(path.substr(path.find_last_of("/\\") + 1)); // Pegar nome do arquivo completo

  size_t pos_formato(arquivo.find_last_of(".")); // Pegar posicao do inicio do formato

  filename = arquivo.substr(0, pos_formato); // Separar nome do arquivo do formato
  extension = arquivo.substr(pos_formato);   // Separar o formato

  cout << " Arquivo: " << arquivo << '\n';           // Nome do arquivo completo
  cout << " nome_do_arquivo: " << filename << '\n';  // Apenas o nome
  cout << " formato_arquivo: " << extension << '\n'; // Apenas o formato
}
