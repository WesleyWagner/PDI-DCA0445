#include <iostream>
#include <opencv2/opencv.hpp>

using namespace std;
void separar_nome_do_arquivo(const string& path,string & filename,string & extension);

int main(int argc, char** argv){
  cv::Mat image;
  int width, height;
  cv::VideoCapture cap;
  std::vector<cv::Mat> planes;
  cv::Mat histR, histG, histB;
  int nbins = 64;
  float range[] = {0, 255};
  const float *histrange = { range };
  bool uniform = true;
  bool acummulate = false;
  uchar key=0;

    while(1)
    {
        cout<<"\ntecla 'c' para abrir stream da camera virtual:"<<key<<endl;
        cin>>key;
        cin.ignore();

        if(key == '\0')return 0;
        if(key=='c')break;
    }

  if(key == 'c')cap.open(1);
	//0 - Irium CAM Android
	//1 - Splicam Stream
	//2 - Desconhecido
  if(!cap.isOpened()){
    std::cout << "cameras indisponiveis";
    //return -1;
  }else
    {

  cap.set(cv::CAP_PROP_FRAME_WIDTH, 1024);
  cap.set(cv::CAP_PROP_FRAME_HEIGHT, 768);
  width = cap.get(cv::CAP_PROP_FRAME_WIDTH);
  height = cap.get(cv::CAP_PROP_FRAME_HEIGHT);

  std::cout << "largura = " << width << std::endl;
  std::cout << "altura  = " << height << std::endl;
    }
  int histw = nbins, histh = nbins/2;
  cv::Mat histImgR(histh, histw, CV_8UC3, cv::Scalar(0,0,0));
  cv::Mat histImgG(histh, histw, CV_8UC3, cv::Scalar(0,0,0));
  cv::Mat histImgB(histh, histw, CV_8UC3, cv::Scalar(0,0,0));

  while(1){
    if(cap.isOpened())cap >> image;
    else image = cv::imread(argv[1],cv::IMREAD_COLOR);
    cv::Mat eq_R,eq_G,eq_B;
    std::vector<cv::Mat> eq(3);

    cv::split (image, planes);
    //cv::Mat planoGray_R(planes[2].rows,planes[2].cols,0);
    //cv::cvtColor(planes[2],planoGray_R,cv::COLOR_BGR2GRAY);
    //cv::imshow("\nPlano Split 0B ",planes[0]);
    //std::cout<<"\n\nChannels planoGray_R "<<planes[0].channels()<<" type: "<< planes[0].type();
    cv::equalizeHist(planes[0],eq_B);
    cv::equalizeHist(planes[1],eq_G);
    cv::equalizeHist(planes[2],eq_R);
    //cv::imshow("\ Equalize hist B",eq_B);
    //cv::imshow("\ Equalize hist G",eq_G);
    //cv::imshow("\ Equalize hist R",eq_R);

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

    histImgR.setTo(cv::Scalar(0));
    histImgG.setTo(cv::Scalar(0));
    histImgB.setTo(cv::Scalar(0));

    for(int i=0; i<nbins; i++){
      cv::line(histImgR,
               cv::Point(i, histh),
               cv::Point(i, histh-cvRound(histR.at<float>(i))),
               cv::Scalar(0, 0, 255), 1, 8, 0);
      cv::line(histImgG,
               cv::Point(i, histh),
               cv::Point(i, histh-cvRound(histG.at<float>(i))),
               cv::Scalar(0, 255, 0), 1, 8, 0);
      cv::line(histImgB,
               cv::Point(i, histh),
               cv::Point(i, histh-cvRound(histB.at<float>(i))),
               cv::Scalar(255, 0, 0), 1, 8, 0);
    }
    histImgR.copyTo(image(cv::Rect(0, 0       ,nbins, histh)));
    histImgG.copyTo(image(cv::Rect(0, histh   ,nbins, histh)));
    histImgB.copyTo(image(cv::Rect(0, 2*histh ,nbins, histh)));
    cv::namedWindow("image", cv::WINDOW_KEEPRATIO|cv::WINDOW_NORMAL);
    cv::imshow("image", image);


    cv::calcHist(&eq_R, 1, 0, cv::Mat(), histR, 1,
                 &nbins, &histrange,
                 uniform, acummulate);
    cv::calcHist(&eq_G, 1, 0, cv::Mat(), histG, 1,
                 &nbins, &histrange,
                 uniform, acummulate);
    cv::calcHist(&eq_B, 1, 0, cv::Mat(), histB, 1,
                 &nbins, &histrange,
                 uniform, acummulate);

    cv::normalize(histR, histR, 0, histImgR.rows, cv::NORM_MINMAX, -1, cv::Mat());
    cv::normalize(histG, histG, 0, histImgG.rows, cv::NORM_MINMAX, -1, cv::Mat());
    cv::normalize(histB, histB, 0, histImgB.rows, cv::NORM_MINMAX, -1, cv::Mat());

    histImgR.setTo(cv::Scalar(0));
    histImgG.setTo(cv::Scalar(0));
    histImgB.setTo(cv::Scalar(0));

    for(int i=0; i<nbins; i++){
      cv::line(histImgR,
               cv::Point(i, histh),
               cv::Point(i, histh-cvRound(histR.at<float>(i))),
               cv::Scalar(0, 0, 255), 1, 8, 0);
      cv::line(histImgG,
               cv::Point(i, histh),
               cv::Point(i, histh-cvRound(histG.at<float>(i))),
               cv::Scalar(0, 255, 0), 1, 8, 0);
      cv::line(histImgB,
               cv::Point(i, histh),
               cv::Point(i, histh-cvRound(histB.at<float>(i))),
               cv::Scalar(255, 0, 0), 1, 8, 0);
    }

    cv::Mat image_equalizada;
    eq_B.copyTo(eq[0]);
    eq_G.copyTo(eq[1]);
    eq_R.copyTo(eq[2]);
    cv::merge(eq,image_equalizada);

    histImgR.copyTo(image_equalizada(cv::Rect(0, 0       ,nbins, histh)));
    histImgG.copyTo(image_equalizada(cv::Rect(0, histh   ,nbins, histh)));
    histImgB.copyTo(image_equalizada(cv::Rect(0, 2*histh ,nbins, histh)));
    cv::namedWindow("image_equalizada", cv::WINDOW_KEEPRATIO|cv::WINDOW_NORMAL);
    cv::imshow("image_equalizada", image_equalizada);

    if(!cap.isOpened())
    {
        cv::waitKey(0);
        string caminho(argv[1]),formato,nome_imagem_saida;
        separar_nome_do_arquivo(caminho,nome_imagem_saida,formato);
        nome_imagem_saida.append(" - Equalizada");
        nome_imagem_saida.append(formato);
        cout<<"\n Imagem editada salva como: "<<nome_imagem_saida<<endl;
        vector<int> compression_params;
        compression_params.push_back(cv::IMWRITE_JPEG_QUALITY);
        compression_params.push_back(100);
        cv::imwrite(nome_imagem_saida,image_equalizada,compression_params);//Salvar imagem
        return 0;
    }
    key = cv::waitKey(20);
    if(key == 27) break;
    if(key == ' ') break;
  }
  return 0;
}

void separar_nome_do_arquivo(const string& path,string & filename,string & extension)
{
  string
  arquivo(path.substr(path.find_last_of("/\\")+1));//Pegar nome do arquivo completo

  size_t pos_formato(arquivo.find_last_of("."));//Pegar posicao do inicio do formato

  filename = arquivo.substr(0,pos_formato);//Separar nome do arquivo do formato
  extension = arquivo.substr(pos_formato);//Separar o formato

  cout << " Arquivo: " << arquivo << '\n';//Nome do arquivo completo
  cout << " nome_do_arquivo: " << filename << '\n';//Apenas o nome
  cout << " formato_arquivo: " << extension << '\n';//Apenas o formato
}
