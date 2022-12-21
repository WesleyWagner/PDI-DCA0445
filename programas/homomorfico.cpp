#include <iostream>
#include <cmath>
#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>

using namespace cv;
using namespace std;

int slider_gH = 8;
const int gH_max = 100;
float gH = slider_gH/10.00;

int slider_gL = 6;
const int gL_max = 100;
float gL = slider_gL/10.00;

int slider_c = 10;
const int c_max = 100;
float c = slider_c/10.00;

int slider_d0 = 15;
const int d0_max = 15;
float d0 = slider_d0/1.00;

int altura, largura;
char TrackbarName[50];

void homomorfico();

void on_trackbar_gH(int, void *)
{
    gH = slider_gH/10.00;
    homomorfico();
    // Atualizar valor trackbar
}

void on_trackbar_gL(int, void *)
{
    gL = slider_gL/10.00;
    homomorfico();
    // Atualizar valor trackbar
}

void on_trackbar_c(int, void *)
{
    c = slider_c/10.00;
    homomorfico();

    // Atualizar valor trackbar
}

void on_trackbar_d0(int, void *)
{
    d0 = slider_d0/1.00;
    homomorfico();
    // Atualizar valor trackbar
}

// troca os quadrantes da imagem da DFT
void makeFilter();
void makeImgFinal();

void deslocaDFT(Mat &image)
{
    Mat tmp, A, B, C, D;

    // se a imagem tiver tamanho impar, recorta a regiao para
    // evitar cópias de tamanho desigual
    image = image(Rect(0, 0, image.cols & -2, image.rows & -2));
    int cx = image.cols / 2;
    int cy = image.rows / 2;

    // reorganiza os quadrantes da transformada
    // A B   ->  D C
    // C D       B A
    A = image(Rect(0, 0, cx, cy));
    B = image(Rect(cx, 0, cx, cy));
    C = image(Rect(0, cy, cx, cy));
    D = image(Rect(cx, cy, cx, cy));

    // A <-> D
    A.copyTo(tmp);
    D.copyTo(A);
    tmp.copyTo(D);

    // C <-> B
    C.copyTo(tmp);
    B.copyTo(C);
    tmp.copyTo(B);
}

Mat imaginaryInput, complexImage;
Mat padded, filter, filter_print;
Mat image, tmp,output;
Mat_<float> zeros;
vector<Mat> planos, filtros, dfts;
int dft_M, dft_N;

int main(int argc, char **argv)
{
    image = imread(argv[1], cv::IMREAD_GRAYSCALE);
    imshow("imagem_original", image);
    // waitKey();

    // identifica os tamanhos otimos para
    // calculo do FFT
    dft_M = getOptimalDFTSize(image.rows);
    dft_N = getOptimalDFTSize(image.cols);

    // realiza o padding da imagem
    

    // prepara a matriz complexa e preenche com 0

    // Construir imagem do filtro homomorfico
    
    makeFilter();
    waitKey();

    // Realizar a filtragem

    cv::namedWindow("imagem_final", cv::WINDOW_NORMAL);

    sprintf(TrackbarName, "gH %d", gH_max);
    createTrackbar(TrackbarName, "imagem_final",
                   &slider_gH,
                   gH_max,
                   on_trackbar_gH);
    on_trackbar_gH(slider_gH, 0);

    sprintf(TrackbarName, "gL %d", gL_max);
    createTrackbar(TrackbarName, "imagem_final",
                   &slider_gL,
                   gL_max,
                   on_trackbar_gL);
    on_trackbar_gL(slider_gL, 0);

    sprintf(TrackbarName, "c %d", c_max);
    createTrackbar(TrackbarName, "imagem_final",
                   &slider_c,
                   c_max,
                   on_trackbar_c);
    on_trackbar_c(slider_c, 0);

    sprintf(TrackbarName, "d0 %d", d0_max);
    createTrackbar(TrackbarName, "imagem_final",
                   &slider_d0,
                   d0_max,
                   on_trackbar_d0);
    on_trackbar_d0(slider_d0, 0);

    homomorfico();

    int key = '0';
    for (;;)
    {
   //     imshow("imagem_final", tmp);
        key = cv::waitKey(20);
        if (key == 27) // ESC
            break;
        if (key == ' ')
            break;
    }
    cv::normalize(output, output, 0, 255, cv::NORM_MINMAX);//Converter float para unsigned
    cv::imwrite("homomorfico.png",output);
    image.release();
//     waitKey();
    return 0;
}

void homomorfico(){
    cout<<"\n\tVALOR gH: "<<gH<<"\tValor gL:" <<gL<<"\tValor c: "<<c<<"\tValor d0:"<<d0<<"\n";
    makeFilter();
    makeImgFinal();
}

void makeFilter(){
    copyMakeBorder(image, padded, 0,
                   dft_M - image.rows, 0,
                   dft_N - image.cols,
                   BORDER_CONSTANT, Scalar::all(0));

    imaginaryInput = Mat(padded.size(), CV_32FC1, Scalar(0));

    // Trasforma padded em float
    padded.convertTo(padded, CV_32F);

    normalize(padded, padded, 0.0, 1.0, cv::NORM_MINMAX);

    while(!planos.empty())
    {
        planos.pop_back();
    }

    while(!complexImage.empty())
    {
        complexImage.pop_back();
    }

    planos.push_back(padded);
    planos.push_back(imaginaryInput);

    // Soma ambas as componentes com 1 para evitar a operação ln(0)
    cv::log(padded + 1, padded);
    cv::log(imaginaryInput + 1, imaginaryInput);

    // Concatena plano real e imaginário para realizar a DFT
    

    merge(planos, complexImage);

    filter = Mat((dft_M & -2), (dft_N & -2), CV_32FC2, Scalar(0, 0));

    // Preenche o filtro homomórfico com os valores dados pela função
    for (int i = 0; i < dft_M; i++)
    {
        for (int j = 0; j < dft_N; j++)
        {
            float h = (float)(gH - gL) * (1.0 - exp(-c * (pow(abs(i - dft_M / 2) + abs(j - dft_N / 2), 2) / pow(d0, 2)))) + gL;
            filter.at<Vec2f>(i, j)[0] = h;
            filter.at<Vec2f>(i, j)[1] = h;
        }
    }

    split(filter, filtros);
    normalize(filtros[0], filter_print, 0.0, 1.0, cv::NORM_MINMAX);
    imshow("filtro", filter_print);
}

void makeImgFinal(){
    dft(complexImage, complexImage);
    deslocaDFT(complexImage);
    mulSpectrums(complexImage, filter, complexImage, 0);
    deslocaDFT(complexImage);
    idft(complexImage, complexImage, DFT_SCALE);

    cv::exp(complexImage, complexImage);

    planos.clear();
    split(complexImage, planos);

    // normaliza a parte real para exibicao
    normalize(planos[0], output, 0.0, 1.0, cv::NORM_MINMAX);
    // imshow("imagem_original", tmp);
    imshow("imagem_final", output);
}

