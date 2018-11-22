#ifdef _WIN32
#include <windows.h>

void sleep(unsigned milliseconds)
{
	Sleep(milliseconds);
}
#else
#include <unistd.h>

void sleep(unsigned milliseconds)
{
	usleep(milliseconds * 1000); // takes microseconds
}
#endif

#include "opencv2/opencv.hpp"
#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/core/core.hpp"
#include <iostream>
#include <chrono>
#include <thread>

using namespace std;
using namespace cv;

void conta()
{
	int c = 0;
	while (c < 10)
	{
		c++;
		sleep(1000);
	}
	cout << c << " C\n";
}

bool verificaMovimento(int preto, int mexeu);

Mat calculaDiferenca(Mat &img1, Mat &img2, Mat &img3);

int main(int argc, const char** argv) {
	VideoCapture capture;
	Mat img, frame, ultima, penultima, antepenultima;
	int mexeu=0;
	capture = VideoCapture(0);
//	namedWindow("Tela de captura", CV_WINDOW_AUTOSIZE);

	capture.open(0);
	capture >> ultima;
	cvtColor(ultima, ultima, COLOR_RGB2GRAY);
	capture >> penultima;
	cvtColor(penultima, penultima, COLOR_RGB2GRAY);
	capture >> antepenultima;
	cvtColor(antepenultima, antepenultima, COLOR_RGB2GRAY);

	while (capture.isOpened() == 1)
	{
		while (1)
		{
			std::thread first(conta);
			first.join();
			Mat dif = calculaDiferenca(antepenultima, penultima, ultima);
			int preto = 0, branco = 0;
			namedWindow("Tela de captura", CV_WINDOW_AUTOSIZE);
			imshow("Tela de captura", dif);
			namedWindow("Tela de captura", CV_WINDOW_AUTOSIZE);
			imshow("Tela Original", ultima);
			for (int i = 0; i < dif.rows; i++)
			{
				for (int j = 0; j < dif.cols; j++)
				{
					int ca=dif.at<char>(i, j);
					if (ca == 0)
						preto++;
					else if (ca == -1)
						branco++;

				}

			}
			cout << "Sai da imagem -------\n";
			cout << preto << "\n";
			bool verifica = verificaMovimento(preto, mexeu);
			if (verifica)
				mexeu++;

			capture >> frame;
			if (frame.empty())
				break;

			capture >> ultima;
			cvtColor(ultima, ultima, COLOR_RGB2GRAY);
			capture >> penultima;
			cvtColor(penultima, penultima, COLOR_RGB2GRAY);
			capture >> antepenultima;
			cvtColor(antepenultima, antepenultima, COLOR_RGB2GRAY);

			char c = (char)waitKey(1);

			// Press q to exit from window
			if (c == 27 || c == 'q' || c == 'Q')
				break;
		}
	}
}



	Mat calculaDiferenca(Mat &img1, Mat &img2, Mat &img3)
	{
		Mat d1, d2, imagem;
		absdiff(img3, img2, d1);
		absdiff(img2, img1, d2);
		bitwise_and(d1, d2, imagem);
		threshold(imagem, imagem, 15, 255, THRESH_BINARY_INV);
		//threshold(imagem, imagem, 15, 255, THRESH_BINARY);
		return imagem;
	}

	bool verificaMovimento(int preto, int mexeu) {
		if (preto >= 80)
		{
			cout << "Mexeu " << mexeu << " vezes\n";
			return true;
		}
		else
			return false;
	}

