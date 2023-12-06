#include <iostream>
#include <vector>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

using namespace cv;
using namespace std;

Mat img;
vector<vector<int>> newPoints;

// ordem hsvmin, hsvmax;
vector<vector<int>> coresUsadas{{0, 149, 101, 72, 255, 255},   // seleção vermelho
                                {19, 94, 173, 88, 255, 255},   // seleção amarelo
                                {89, 125, 95, 179, 255, 255}}; // seleção azul

vector<Scalar> coresDeseho{{0, 42, 255},   // azul
                                {0, 255, 255}, // amarelo
                                {255, 0, 0}};  // vermelho

Point getContours(Mat imgDil)
{
    vector<vector<Point>> contours;
    vector<Vec4i> hierarchy;

    findContours(imgDil, contours, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

    vector<vector<Point>> conPoly(contours.size());
    vector<Rect> boundRect(contours.size());

    Point myPoint(0, 0);

    for (int i = 0; i < contours.size(); i++)
    {
        int area = contourArea(contours[i]);
        cout << area << endl;

        string objectType;

        if (area > 1000)
        {
            float peri = arcLength(contours[i], true);
            approxPolyDP(contours[i], conPoly[i], 0.02 * peri, true);

            cout << conPoly[i].size() << endl;
            boundRect[i] = boundingRect(conPoly[i]);
            myPoint.x = boundRect[i].x + boundRect[i].width / 2;
            myPoint.y = boundRect[i].y;

            drawContours(img, conPoly, i, Scalar(255, 0, 255), 2);
            rectangle(img, boundRect[i].tl(), boundRect[i].br(), Scalar(0, 255, 0), 5);
        }
    }

    return myPoint;
}

void findColor(Mat img)
{
    Mat imgHSV;
    cvtColor(img, imgHSV, COLOR_BGR2HSV);

    for (int i = 0; i < coresUsadas.size(); i++)
    {

        Scalar lower(coresUsadas[i][0], coresUsadas[i][1], coresUsadas[i][2]);
        Scalar upper(coresUsadas[i][3], coresUsadas[i][4], coresUsadas[i][5]);
        Mat mask;
        inRange(imgHSV, lower, upper, mask);
        Point myPoint = getContours(mask);

        if (myPoint.x != 0 && myPoint.y != 0)
        {
            newPoints.push_back({myPoint.x, myPoint.y, i});
        }
    }
}

void drawOnCanvas()
{
    for (size_t i = 0; i < newPoints.size(); i++)
    {
        circle(img, Point(newPoints[i][0], newPoints[i][1]), 10, coresDeseho[newPoints[i][2]],FILLED);
    }
    
}

int main()
{

    VideoCapture cap(0);

    while (true)
    {
        cap.read(img);

        findColor(img);
        drawOnCanvas();

        imshow("Image", img);
        waitKey(20);
    }
}
