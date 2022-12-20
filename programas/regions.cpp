#include <iostream>
#include <opencv2/opencv.hpp>

#define CV_LOAD_IMAGE_COLOR IMREAD_COLOR

using namespace std;
using namespace cv;

void pegar_coordenadas(Size &P1,Size &P2);//Rotina para coletar dois pontos para formar uma região quadrada
Vec3b cor_inversa(Vec3b pixel);//Função para retornar a cor inversa de um pixel
int manipular_pixels(int argc,char** argv,Size P1,Size P2, Mat &imagem);//Função que utiliza os pontos fornecidos e o metodo de cor inversa para inverter a cor de uma região na imagem
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
        Size P1,P2;//Variaveis dos pontos fornecidos pelo usuario
        cout<<"\nResolucao da imagem: width "<<imagem.cols<<" x heigth "<<imagem.rows<<endl;
        pegar_coordenadas(P1,P2);//Solicitar os pontos
        manipular_pixels(argc,argv,P1,P2,imagem);//Modificar região
        waitKey(0);
        string caminho(argv[1]),formato,nome_imagem_saida;
        separar_nome_do_arquivo(caminho,nome_imagem_saida,formato);
        nome_imagem_saida.append("-Editado");
        nome_imagem_saida.append(formato);
        cout<<"\n Imagem editada salva como: "<<nome_imagem_saida<<endl;
        imwrite(nome_imagem_saida,imagem);//Salvar imagem
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

void pegar_coordenadas(Size &P1,Size &P2)
{
    int w,h;
    cout<<"Inserir Ponto P1:"<<endl;
    cout<<"P1.w =";
    //cin.ignore();
    cin>>w;
    cout<<"\nw ="<<w;
    cout<<"P1.h =";
    cin.ignore();
    cin>>h;
    if(w<0)w=0;
    if(h<0)h=0;
    P1 = Size(w,h);
    cout<<"Inserir Ponto P2:"<<endl;
    cout<<"P2.w=";
    cin.ignore();
    cin>>w;
    cout<<"P2.h =";
    cin.ignore();
    cin>>h;
    if(w<0)w=0;
    if(h<0)h=0;
    P2 = Size(w,h);
    cout<<"\ncout<<P1;"<<P1;
    cout<<"\ncout<<P2;"<<P2;
}

int manipular_pixels(int argc,char** argv,Size P1,Size P2, Mat &imagem)
{
    if( argc != 2)
    {
     cout <<" Usage: display_image ImageToLoadAndDisplay" << endl;
     return -1;
    }

    int
    ho,//height inicio
    hf,//height final
    wo,//width inicio
    wf;//width final
    if(P1.width<P2.width){
        wo=P1.width;wf=P2.width;}
    else{
        wo=P2.width;wf=P1.width;}//Nesse ponto os limites laterais do quadrado serao definidos
    if(P1.height<P2.height){
        ho=P1.height;hf=P2.height;}
    else{
        ho=P2.height;hf=P1.height;}//Nesse ponto os superior e inferior do quadrado serao definidos

    if(wf>imagem.cols)wf=imagem.cols;

    if(hf>imagem.cols)hf=imagem.rows;

    for(int i = wo; i < wf; i++){
        for(int j = ho; j < hf; j++){
            imagem.at<Vec3b>(j,i) = cor_inversa(imagem.at<Vec3b>(j,i));
        }
    }
    namedWindow( "Display window Editada",WINDOW_KEEPRATIO|WINDOW_NORMAL);// Create a window for display.
    imshow( "Display window Editada",imagem);                   // Show our image inside it.                                         // Wait for a keystroke in the window
    return 0;
}

Vec3b cor_inversa(Vec3b pixel)
{
    pixel[0] = 255 - pixel[0];//Negativo canal B
    pixel[1] = 255 - pixel[1];//Negativo canal G
    pixel[2] = 255 - pixel[2];//Negativo canal R
    return pixel;
}

int display_image(int argc, char** argv)
{
    if( argc != 2)
    {
     cout <<" Usage: display_image ImageToLoadAndDisplay" << endl;
     return -1;
    }

    Mat image;
    image = imread(argv[1], CV_LOAD_IMAGE_COLOR);   // Read the file

    if(! image.data )                              // Check for invalid input
    {
        cout <<  "Could not open or find the image" << std::endl ;
        return -1;
    }

    namedWindow( "Display window 1",WINDOW_KEEPRATIO|WINDOW_NORMAL);// Create a window for display.
    imshow( "Display window 1", image );                   // Show our image inside it.
    //waitKey(0);                                          // Wait for a keystroke in the window
    return 0;
}
