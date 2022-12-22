#include <opencv2/opencv.hpp>
#include <cstdlib>
#include <vector>
#include <iostream>

using namespace std;
using namespace cv;

double frame_width, frame_height;

int main(int argc, char **argv)
{
    int nClusters = 8;
    Mat rotulos;
    int nRodadas = 1;
    int nExecucoes = 20;
    Mat centros;

    if (argc != 2)
    {
        cout<<"\nQuantidade de argumentos errada\nFinalizando...";
        return -1;
    }

    // leitura da imagem
    Mat imagem = imread(argv[1], cv::IMREAD_COLOR);
    Mat samples(imagem.rows * imagem.cols, 3, CV_32F);

    frame_width = imagem.cols;
    frame_height = imagem.rows;
    const int frames_per_second = 2;
    
    cv::Size frame_size(frame_width, frame_height);
    cv::VideoWriter output("kmeans_output.avi", cv::VideoWriter::fourcc('M', 'J', 'P', 'G'), frames_per_second, frame_size);
    std::cout << "\nlargura = " << frame_width << "\n";
    std::cout << "\naltura = " << frame_height << "\n";

    // cv::namedWindow("kmeans", cv::WINDOW_KEEPRATIO | cv::WINDOW_NORMAL);
    // waitKey();
    // Insere cada pixel da matriz da imagem em um vetor
    for (int y = 0; y < frame_height; y++)
    {
        for (int x = 0; x < frame_width; x++)
        {
            for (int z = 0; z < 3; z++)
            {
                samples.at<float>(y + x * frame_height, z) = imagem.at<Vec3b>(y, x)[z];
            }
        }
    }

    // Executa o kmeans nExecucoes vezes, inserindo-os no vetor rotulada
    vector<Mat> rotulada(nExecucoes, Mat(imagem.size(), imagem.type()));
    for (int i = 0; i < nExecucoes; i++)
    {
        kmeans(samples,
               nClusters,
               rotulos,
               TermCriteria(cv::TermCriteria::MAX_ITER | cv::TermCriteria::EPS, 10000, 0.0001),
               nRodadas,
               KMEANS_RANDOM_CENTERS,
               centros);

        for (int y = 0; y < frame_height; y++)
        {
            for (int x = 0; x < frame_width; x++)
            {
                int indice = rotulos.at<int>(y + x * frame_height, 0);
                rotulada.at(i).at<Vec3b>(y, x)[0] = (uchar)centros.at<float>(indice, 0);
                rotulada.at(i).at<Vec3b>(y, x)[1] = (uchar)centros.at<float>(indice, 1);
                rotulada.at(i).at<Vec3b>(y, x)[2] = (uchar)centros.at<float>(indice, 2);
            }
        }

        // imshow("kmeans", rotulada.at(i));
        output.write(rotulada.at(i));
        // waitKey();
    }

    output.release();
    imagem.release();
    return 0;
}