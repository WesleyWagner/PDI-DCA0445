#include <iostream>
#include <opencv2/opencv.hpp>

using namespace std;

int main(int argc, char **argv)
{
    cv::Mat image, op1,op2,op3,op4,op5,op6, output;
    cv::Mat str1,str2,str3,str4;

    if (argc != 2)
    {
        std::cout << "morfologia entrada saida\n";
    }

    image = cv::imread(argv[1], cv::IMREAD_UNCHANGED);

    //  image = cv::imread(argv[1], -1);

    if (image.empty())
    {
        std::cout << "Erro ao carregar a imagem: " << argv[1] << std::endl;
        return -1;
    }

    // elemento estruturante
    str1 = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(1, 6));
    
    // erosao [1]
    cv::erode(image, op1, str1);
    
    // abertura -> fechamento [2]
    cv::morphologyEx(op1, op2, cv::MORPH_OPEN, str1);
    cv::morphologyEx(op2, op2, cv::MORPH_CLOSE, str1);

    // dilatação [3]
    str2 = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(2, 4));
    cv::dilate(op2, op3, str2);

    cv::Mat matArray[] = {image, op1,op2,op3};
    cv::hconcat(matArray, 4, output);

    cv::imshow("output", output);
    cv::imwrite("saida_combinada.png",output);
    cv::imwrite("processado.png",op3);

    cv::imwrite("str1.png",str1);
    cv::imwrite("str2.png",str2);

    cv::waitKey(0);
    return 0;
}