#include "arucoProcessing.h"
#include "calibration.h"


#include "iostream"

using namespace std;
using namespace cv;

void generate4markers()
{
	vector<Mat> arucoMarkers;

	Ptr <aruco::Dictionary> arucoDictionary = aruco::getPredefinedDictionary(aruco::PREDEFINED_DICTIONARY_NAME::DICT_6X6_50);

	Mat arucoMarkerTemp;
	for (int i = 0; i < 4; i++)
	{
		aruco::drawMarker(arucoDictionary, i, 500, arucoMarkerTemp, 1);
		arucoMarkers.push_back(arucoMarkerTemp);
		imwrite("aruco marker id=" + to_string(i) + " 6X6_50.jpg", arucoMarkers[i]);
	}

}

int main() 
{
	////////////////////// ОТРИСОВКА картинки НАД 4 МАРКЕРОМ //////////////////////

	Mat cameraMatrix;
	Mat distanceCoef;
	vector<vector<Point2f>> markerCorners;
	vector<int> markerIds;
	vector<Vec3d> rotationVector, translationVector;

	//загружаем картинку для отрисовки
	Mat image = imread("image.jpg");

	//загружаем параметры калибровки
	loadCalibParameters("calibParam.txt", cameraMatrix, distanceCoef);

	Mat frame;
	VideoCapture vid(0);
	if (!vid.isOpened())
	{
		return 0;
	}

	const int FPS = 30;
	namedWindow("Webcam", WINDOW_AUTOSIZE);

	while (true)
	{
		if (!vid.read(frame))
			break;

		//находим маркеры и матрицы преобразования до них
		markersDetection(frame, markerCorners, markerIds, rotationVector,
			translationVector, cameraMatrix, distanceCoef);

		if (markerIds.size() == 4)
		{
			vector<Point2f> corners(4);
			
			//ищем точки
			for (int i = 0; i < markerIds.size(); i++)
			{
				if (markerIds[i] == 0)
					corners.at(0) = (markerCorners[i][0]);
				else if (markerIds[i] == 1)
					corners.at(1) = (markerCorners[i][1]);
				else if (markerIds[i] == 2)
					corners.at(3) = (markerCorners[i][3]);
				else if (markerIds[i] == 3)
					corners.at(2) = (markerCorners[i][2]);
			}

			//рисуем кружочки в углах
			for (int i = 0; i < markerIds.size(); i++)
			{
				circle(frame, corners[i], 2, Scalar(255, 0, 0), 2);
				cout << corners[i] << endl;
			}

			addImageOnImageHomography(image, frame, corners);
		}
		

		imshow("Webcam", frame);

		int key = waitKey(1000 / FPS);
		if (key == 27)
			break;
	}
}

