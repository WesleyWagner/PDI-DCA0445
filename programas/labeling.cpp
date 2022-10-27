#include <iostream>
#include <opencv2/opencv.hpp>
#include <imgproc.hpp>

typedef unsigned char uchar;

using namespace cv;
using namespace std;

int remover_objetos_nas_bordas(const Mat &imagem, Mat &imagem_final);
int pintar_background(Mat &imagem);
int contar_bolhas_com_buracos(const Mat& imagem);
int pintar_furos(Mat &imagem);
int pintar_furos_v2(Mat &imagem);
int contar_furos(Mat &imagem);

Vec3b red(0,0,255);
Vec3b green(0,255,0);
Vec3b blue(255,0,0);
uchar COR_BACKGROUND_GRAY = 2;
uchar COR_FURO_GRAY = 100;
uchar COR_BOLHA_GRAY = 200;

int main(int argc, char** argv){
  cv::Mat image;
  int width, height;
  //int nobjects;

  cv::Point p;
  image = cv::imread(argv[1], cv::IMREAD_GRAYSCALE);

  if(!image.data){
    std::cout << "imagem nao carregou corretamente\n";
    return(-1);
  }

  width=image.cols;
  height=image.rows;
  std::cout << width << "x" << height << std::endl;


  //p.x=x;//width
  //p.y=y;//height

  //int x,y;
  //cin>>x;
  //cin>>y;
  //p.x=x;//width
  //p.y=y;//height

  //floodFill(image,p,50);
  //cout<<"\nValor pintado: "<<(int)image.at<uchar>(y,x)<<endl;
  //image.at<uchar>(y,x) -> y height,x width
  //imshow("Testando Floodfill",image);

  Mat imagem_sem_objetos_nas_bordas;

  remover_objetos_nas_bordas(image,imagem_sem_objetos_nas_bordas);
  pintar_background(imagem_sem_objetos_nas_bordas);
  contar_furos(imagem_sem_objetos_nas_bordas);
  imshow("Contagem dos Furos", imagem_sem_objetos_nas_bordas);

  // busca objetos presentes
  //nobjects=0;
  //for(int i=0; i<height; i++){
  //  for(int j=0; j<width; j++){
  //    if(image.at<uchar>(i,j) == 255){
        // achou um objeto
  //      nobjects++;
  //     p.x=j;
  //     p.y=i;
  		// preenche o objeto com o contador
		  //cv::floodFill(image,p,nobjects);
    //  }
   // }
  //}

  //std::cout << "a figura tem " << nobjects << " bolhas\n";
  //cv::equalizeHist(image, realce);
  //cv::imshow("image", image);
  //cv::imshow("realce", realce);
  //cv::imwrite("labeling.png", image);
  //cv::imshow("Sem Bordas", imagem_sem_objetos_nas_bordas);
  cv::waitKey();
  return 0;
}

int remover_objetos_nas_bordas(const Mat &imagem, Mat &imagem_final)//ok
{
    Point prov_point;
    imagem.copyTo(imagem_final);
    int width = imagem_final.cols,height = imagem_final.rows;

    //int x,y;
    //cin>>x;//width
    //cin>>y;//height
    //p.x=x;//width
    //p.y=y;//height

    //floodFill(image,p,50);
    //cout<<"\nValor pintado: "<<(int)image.at<uchar>(y,x)<<endl;
    //image.at<uchar>(y,x) -> y height,x width
    //imshow("Testando Floodfill",image);

    //i = width = cols
    //j = height = rows



    for(int j = 0; j < height; j++)//Borda esquerda
    {
        if(imagem_final.at<uchar>(j,0) == 255)
        {
            prov_point.x=0;//width
            prov_point.y=j;//height
            floodFill(imagem_final,prov_point,0);
        }
    }

    for(int i = 0; i < width; i++)//Borda Superior
    {
        if(imagem_final.at<uchar>(0,i) == 255)
        {
            prov_point.x=i;//width
            prov_point.y=0;//height
            floodFill(imagem_final,prov_point,0);
        }
    }

    for(int j = 0; j < height; j++)//Borda direita
    {
        if(imagem_final.at<uchar>(j,width-1) == 255)
        {
            prov_point.x=width-1;//width
            prov_point.y=j;//height
            floodFill(imagem_final,prov_point,0);
        }
    }

    for(int i = 0; i < width; i++)//Borda inferior
    {
        if(imagem_final.at<uchar>(height-1,i) == 255)
        {
            prov_point.x=i;//width
            prov_point.y=height-1;//height
            floodFill(imagem_final,prov_point,0);
        }
    }
    return 0;
}

int pintar_background(Mat &imagem)
{
    Point prov_point;
    prov_point.x = 0;
    prov_point.y = 0;
    floodFill(imagem,prov_point,COR_BACKGROUND_GRAY);//mudar cor do plano de fundo para 10
    return 1;
}

int contar_furos(Mat &imagem)
{
    //busca objetos presentes
    int width=imagem.cols, height=imagem.rows, no_furos=0,no_bolhas_pintadas=0;
    Point p;
    Mat color_imagem;
    imagem.copyTo(color_imagem);
    cvtColor(imagem,color_imagem,cv::COLOR_GRAY2BGR);//Converter matriz de escala de cinza para BGR

    for(int j=0; j<height; j++)
    {
        for(int i=0; i<width; i++)
        {
            if(imagem.at<uchar>(j,i) == 0)
            {
                // achou um objeto
                no_furos++;
                p.x=i;
                p.y=j;
                // preenche o objeto com o contador
                cv::floodFill(imagem,p,COR_FURO_GRAY);
                if(imagem.at<uchar>(j,i-1) == 255)
                {
                    p.x=i-1;
                    p.y=j;
                    cv::floodFill(imagem,p,COR_BOLHA_GRAY);
                    cv::floodFill(color_imagem,p,red);
                    no_bolhas_pintadas++;
                }
            }
        }
    }
    imshow("Colorfull",color_imagem);
    cout<<"\n\nNo Furos: "<<no_furos<<endl;
    cout<<"\n\nNo Bolhas pintadas com furos: "<<no_bolhas_pintadas<<endl;
    return no_furos;
}
