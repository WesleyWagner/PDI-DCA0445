#include <iostream>
#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

float l1 = -100, l2 = 50, d = 6, centro = 100;

int matriz_media_tam = 7;
int altura, largura;

int slider_altura = 0;
int slider_altura_max = 100;

int slider_decaimento = 0;
int slider_decaimento_max = 100;

int slider_deslocamento = 0;
int slider_deslocamento_max = 100;

Mat imagem, imagem_borrada;

char TrackbarName[50];

void aplicar_Efeito();

void on_trackbar_deslocamento(int, void *)
{
    // O centro é definido pela posição do slider, de forma que
    // 0 - topo imagem e 100 -> fundo imagem
    centro = slider_deslocamento * altura / 100;

    aplicar_Efeito();
}

void on_trackbar_altura(int, void *)
{
    // O slider da altura define a distância de l1 e l2 do centro do tilt-shift
    int alt = altura * slider_altura / 100;
    l1 = -alt / 2;
    l2 = alt / 2;

    aplicar_Efeito();
}

void on_trackbar_decaimento(int, void *)
{
    d = slider_decaimento;

    aplicar_Efeito();
}

void aplicar_Efeito()
{
    Mat ponderada(altura, largura, CV_32FC3);
    Mat ponderada_negativa(altura, largura, CV_32FC3);

    cout << "centro: " << centro << ", l1: " << l1 << ", l2: " << l2 << ", decaimento: " << d << endl;

    for (int i = 0; i < altura; i++)
    {
        float fx = 0.0;
        // Como a função não é definida para d = 0, caso isso ocorra, é atribuido um valor
        // pequeno para d (d=0.01), o que deixa a transição entre 0 e 1 quase abrupta
        if (d != 0)
        {
            // função utilizada para ponderar as imagens
            fx = -0.5 * (tanh((i - centro + l1) / d) - tanh((i - centro + l2) / d));
        }
        else
        {
            fx = -0.5 * (tanh((i - centro + l1) / 0.01) - tanh((i - centro + l2) / 0.01));
        }

        // O tilt shift é aplicado em cada camada da imagem RGB
        for (int j = 0; j < largura; j++)
        {
            ponderada.at<Vec3f>(i, j)[0] = fx;
            ponderada.at<Vec3f>(i, j)[1] = fx;
            ponderada.at<Vec3f>(i, j)[2] = fx;
            ponderada_negativa.at<Vec3f>(i, j)[0] = 1.0 - fx;
            ponderada_negativa.at<Vec3f>(i, j)[1] = 1.0 - fx;
            ponderada_negativa.at<Vec3f>(i, j)[2] = 1.0 - fx;
        }
    }

    Mat resultado, res1, res2;

    // Cada imagem é multiplicada por sua respectiva matriz ponderada
    multiply(imagem, ponderada, res1);
    multiply(imagem_borrada, ponderada_negativa, res2);

    // As matrizes ponderadas são somadas
    addWeighted(res1, 1, res2, 1, 0, resultado);

    resultado.convertTo(resultado, CV_8UC3);
    
    imshow("tiltshift", resultado);
}

int main(int argvc, char **argv)
{
    // A másca de média, para borramento, é criada
    float media[matriz_media_tam * matriz_media_tam];
    for (int i = 0; i < matriz_media_tam; i++)
    {
        for (int j = 0; j < matriz_media_tam; j++)
        {
            media[i * matriz_media_tam + j] = 1.0 / (matriz_media_tam * matriz_media_tam);
        }
    }
    Mat masc_media(matriz_media_tam, matriz_media_tam, CV_32F, media);

    vector<Mat> canais;

    imagem = imread(argv[1]);
    imagem.convertTo(imagem, CV_32FC3);

    // O filtro do borramento é aplicado em cada canal separadamente
    split(imagem, canais);

    filter2D(canais[0], canais[0], canais[0].depth(), masc_media, Point(3, 3), 0);
    filter2D(canais[1], canais[1], canais[1].depth(), masc_media, Point(3, 3), 0);
    filter2D(canais[2], canais[2], canais[2].depth(), masc_media, Point(3, 3), 0);

    merge(canais, imagem_borrada);

    largura = imagem.cols;
    altura = imagem.rows;

    // namedWindow("tiltshift", 1);
    cv::namedWindow("tiltshift",cv::WINDOW_NORMAL);

    // Criando os sliders
    sprintf(TrackbarName, "Altura x %d", slider_altura_max);
    createTrackbar(TrackbarName, "tiltshift",
                   &slider_altura,
                   slider_altura_max,
                   on_trackbar_altura);
    on_trackbar_altura(slider_altura, 0);

    sprintf(TrackbarName, "Decaimento x %d", slider_decaimento_max);
    createTrackbar(TrackbarName, "tiltshift",
                   &slider_decaimento,
                   slider_decaimento_max,
                   on_trackbar_decaimento);
    on_trackbar_decaimento(slider_decaimento, 0);

    sprintf(TrackbarName, "Deslocamento x %d", slider_deslocamento_max);
    createTrackbar(TrackbarName, "tiltshift",
                   &slider_deslocamento,
                   slider_deslocamento_max,
                   on_trackbar_deslocamento);
    on_trackbar_deslocamento(slider_deslocamento, 0);

    aplicar_Efeito();

    waitKey(0);
    return 0;
}