#include <iostream>
#include <opencv2/opencv.hpp>

#define CV_LOAD_IMAGE_COLOR IMREAD_COLOR

using namespace std;
using namespace cv;


int trocar_regioes(const Mat &imagem,Mat &imagem_trocada);
void separar_nome_do_arquivo(const string& path,string & filename,string & extension);

int main(int argc, char** argv)
{
    if(argc == 2)//Verifica a quantidade de argumentos
    {
        Mat imagem;
        imagem = imread(argv[1], CV_LOAD_IMAGE_COLOR); //Ler a imagem passada por parametro via terminal
        if(!imagem.data) //Verificar se a imagem foi carregada
        {
            cout <<  "Falha ao abrir a imagem" << endl;
            return -1;
        }

        cout<<"\nResolucao da imagem: width "<<imagem.cols<<" x heigth "<<imagem.rows<<endl;

        Mat imagem_trocada;
        trocar_regioes(imagem,imagem_trocada);
        waitKey(0);

        string caminho(argv[1]),formato,nome_imagem_saida;
        separar_nome_do_arquivo(caminho,nome_imagem_saida,formato);
        nome_imagem_saida.append(" - Editado");
        nome_imagem_saida.append(formato);
        cout<<"\n Imagem editada salva como: "<<nome_imagem_saida<<endl;
        imwrite(nome_imagem_saida,imagem_trocada);//Salvar imagem
        return 0;
    }
    else
    {
        cout <<  "Falha ao interpretar solicitacao. Utilizar 'regions.exe \"nome_da_imagem.***\"' " << endl;
        return -1;
    }
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

int trocar_regioes(const Mat &imagem,Mat &imagem_trocada)
{
    //Mapa das regioes
    // 1 2
    // 3 4
    int width_cut,height_cut;

    width_cut = imagem.cols/2;
    height_cut = imagem.rows/2;
    int width_2x = width_cut*2,height_2x = height_cut*2;

    imagem_trocada.create(height_2x,width_2x,imagem.type());

    cout<<"\nResolucao normalizada par da imagem: width "<<imagem_trocada.cols<<" x heigth "<<imagem_trocada.rows<<endl;

    for(int i = 0; i < width_cut; i++){//Regiao 1
        for(int j = 0; j < height_cut; j++){
            imagem_trocada.at<Vec3b>(j,i) = imagem.at<Vec3b>(j+height_cut,i+width_cut);
        }
    }

    for(int i = width_cut; i < width_2x; i++){//Regiao 2
        for(int j = 0; j < height_cut; j++){
            imagem_trocada.at<Vec3b>(j,i) = imagem.at<Vec3b>(j+height_cut,i-width_cut);
        }
    }

    for(int i = 0; i < width_cut; i++){//Regiao 3
        for(int j = height_cut; j < height_2x; j++){
            imagem_trocada.at<Vec3b>(j,i) = imagem.at<Vec3b>(j-height_cut,i+width_cut);
        }
    }

    for(int i = width_cut; i < width_2x; i++){//Regiao 4
        for(int j = height_cut; j < height_2x; j++){
            imagem_trocada.at<Vec3b>(j,i) = imagem.at<Vec3b>(j-height_cut,i-width_cut);
        }
    }

    namedWindow( "Display window Regioes trocadas",WINDOW_KEEPRATIO|WINDOW_NORMAL);// Create a window for display.
    imshow( "Display window Regioes trocadas",imagem_trocada);                   // Show our image inside it.                                         // Wait for a keystroke in the window
    cout<<"Passou";
    return 0;
}
