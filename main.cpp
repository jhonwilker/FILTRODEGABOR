#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <math.h>
using namespace std;
cv::Mat GaborCurvo(int ks, double v, double mi, double c, double sig){

    int hks = (ks-1)/2;
    double curva = c;
    double varV = v;
    double varMi = mi;
    double varSig = sig*CV_PI;
    double varPhiInMi = varMi*(CV_PI/8);
    double ex = (v+2)/2;
    double varKInV = 1/pow(2,ex);
    double x_theta;
    double y_theta;
    double var1;
    double del =2.0/(ks-1);
    cv::Mat kernel(ks,ks, CV_32F);
    for (int y=-hks; y<=hks; y++)
    {
        for (int x=-hks; x<=hks; x++)
        {
            x_theta = x*del*cos(varPhiInMi)+y*del*sin(varPhiInMi);
            y_theta = -x*del*sin(varPhiInMi)+y*del*cos(varPhiInMi);
            var1=x_theta+curva*(pow(y_theta,2));
            kernel.at<float>(hks+y,hks+x) = (float)(pow(varKInV,2)/pow(varSig,2))*exp(-(pow(varKInV,2)/2*pow(varSig,2))*(pow(var1,2) + pow(y_theta,2)))*exp(varKInV*var1-exp(-0.5*pow(varSig,2)));

        }
    }
    return kernel;
}

cv::Mat GaborSimples(int ks, double sig, double th, double lm, double ps){
    int hks = (ks-1)/2;
    double theta = th*CV_PI/180;
    double psi = ps*CV_PI/180;
    double del = 2.0/(ks-1);
    double lmbd = lm;
    double sigma = sig/ks;
    double x_theta;
    double y_theta;
    cv::Mat kernel(ks,ks, CV_32F);
    for (int y=-hks; y<=hks; y++)
    {
        for (int x=-hks; x<=hks; x++)
        {
            x_theta = x*del*cos(theta)+y*del*sin(theta);
            y_theta = -x*del*sin(theta)+y*del*cos(theta);
            kernel.at<float>(hks+y,hks+x) = (float)exp(-0.5*(pow(x_theta,2)+pow(y_theta,2))/pow(sigma,2))* cos(2*CV_PI*x_theta/lmbd + psi);
        }
    }
    return kernel;
}

int escolha=1;
int pos_v = 3;
int pos_mi = 5;
int pos_curva = 3;
int kernel_size=21;

int pos_sigma= 1;
int pos_lm = 50;
int pos_th = 0;
int pos_psi = 90;

cv::Mat src_f;
cv::Mat dest;
void Process1(int , void *)
{
    double sig = pos_sigma;
    double lm = 0.5+pos_lm/100.0;
    double th = pos_th;
    double ps = pos_psi;
    cv::Mat kernel = GaborSimples(kernel_size, sig, th, lm, ps);
    cv::filter2D(src_f, dest, CV_32F, kernel);
    cv::imshow("Process window", dest);
    cv::filter2D(src_f, dest, CV_32F, kernel);
    cv::Mat Lkernel(kernel_size*20, kernel_size*20, CV_32F);
    cv::resize(kernel, Lkernel, Lkernel.size());
    Lkernel /= 2.;
    Lkernel += 0.5;
    cv::imshow("Kernel", Lkernel);
    cv::Mat mag;
    cv::pow(dest, 2.0, mag);
    cv::imshow("Acentuação 2x", mag);
}
void Process2(int , void *)
{
    cv::Mat kernel = GaborCurvo(kernel_size,pos_v,pos_mi,pos_curva,pos_sigma);
    cv::filter2D(src_f, dest, CV_32F, kernel);
    cv::imshow("Process window", dest);
    cv::Mat Lkernel(kernel_size*20, kernel_size*20, CV_32F);
    cv::resize(kernel, Lkernel, Lkernel.size());
    Lkernel /= 2.;
    Lkernel += 0.5;
    cv::imshow("Kernel", Lkernel);
    cv::Mat mag;
    cv::pow(dest, 2.0, mag);
    cv::imshow("Acentuação 2x", mag);
}

int main(int argc, char** argv)
{
    if (!kernel_size%2)
    {
        kernel_size+=1;
    }
   while(escolha!=0){
    cout<< "1 - GABOR SIMPLES" <<endl;
    cout<< "2 - GABOR CURVO" <<endl;
    cout<< "Para outra opcao feche todos os outras janelas da aplicacao." <<endl;
    cout<< "Digite qual filtro deseja testar:" <<endl;
    cin>>escolha;
    if (escolha==1){
        cv::Mat image = cv::imread("img.png",1);
        cv::imshow("Src", image);
        cv::Mat src;
        cv::cvtColor(image, src, CV_BGR2GRAY);
        src.convertTo(src_f, CV_32F, 1.0/255, 0);
        cv::namedWindow("Process window", 1);
        cv::createTrackbar("Sigma", "Process window", &pos_sigma, kernel_size, Process1);
        cv::createTrackbar("Lambda", "Process window", &pos_lm, 100, Process1);
        cv::createTrackbar("Theta", "Process window", &pos_th, 180, Process1);
        cv::createTrackbar("Psi", "Process window", &pos_psi, 360, Process1);
        Process1(0,0);
        cv::waitKey(0);
        return main(0,0);
    }else if(escolha==2){
        cv::Mat image = cv::imread("img.png",1);
        cv::imshow("Src", image);
        cv::Mat src;
        cv::cvtColor(image, src, CV_BGR2GRAY);
        src.convertTo(src_f, CV_32F, 1.0/255, 0);
        cv::namedWindow("Process window", 1);
        cv::createTrackbar("Mi", "Process window", &pos_mi, kernel_size, Process2);
        cv::createTrackbar("V", "Process window", &pos_v, kernel_size, Process2);
        cv::createTrackbar("Sigma", "Process window", &pos_sigma, kernel_size, Process2);
        cv::createTrackbar("Curva", "Process window", &pos_curva, kernel_size, Process2);
        Process2(0,0);
        cv::waitKey(0);
    }else{
        return 0;
    }
   }
    return 0;
}
