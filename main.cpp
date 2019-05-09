#include <iostream>
#include <fstream>
#include <string>
#include <omp.h>
#include <stdlib.h>
#include <chrono> 
#include <iomanip>

#define strSize 202
#define numOfSeq 400
#define maxHigh 20

//#define gap -1.832482334
//#define mismatch -2.451795405
//#define match 3.621354295

using namespace std;
using namespace std::chrono;

void clearArray(double myBioArray[strSize][strSize]) {

	for (int i = 0; i < strSize; i++)
	{
		for (int j = 0; j < strSize; j++)
		{
			myBioArray[i][j] = 0;
		}
	}
}

void setArray(double myBioArray[strSize][strSize]) {

	for (int i = 0; i < strSize - 2; i++) {
		myBioArray[i + 2][1] = myBioArray[i + 1][1] - 1; //sutun
	}
	for (int j = 0; j < strSize - 2; j++) {
		myBioArray[1][j + 2] = myBioArray[1][j + 1] - 1; //satir
	}
	myBioArray[1][1] = 0;
}

void start(double myBioArray[strSize][strSize], double textArray1[strSize], double textArray2[strSize]) {

	for (int i = 2; i < strSize; i++) {
		for (int j = 2; j < strSize; j++)
		{
			myBioArray[i][0] = textArray1[i - 2];
			myBioArray[0][j] = textArray2[j - 2];
		}
	}
}

void nwAlgorithm(double myBioArray[strSize][strSize], double myMax[maxHigh],int first,int second,int myid1[numOfSeq],int myid2[numOfSeq],double gap,double mismatch, double match) {

	double gap1, gap2, match1;
	//double gap = -1.832482334, mismatch = -2.451795405, match = 3.621354295;
	for (int i = 0; i < strSize - 2; i++)
	{
		for (int j = 0; j < strSize - 2; j++)
		{
			gap1 = myBioArray[j + 1][i + 2] + gap;
			gap2 = myBioArray[j + 2][i + 1] + gap;

			if (myBioArray[0][i + 2] == myBioArray[j + 2][0])
			{
				match1 = myBioArray[j + 1][i + 1] + match;
			}
			else
			{
				match1 = myBioArray[j + 1][i + 1] + mismatch;
			}

			if (match1 > gap1)
			{
				if (match1 > gap2)
				{
					myBioArray[j + 2][i + 2] = match1;
				}
				else
				{
					myBioArray[j + 2][i + 2] = gap2;
				}
			}
			else
			{
				if (gap1 > gap2)
				{
					myBioArray[j + 2][i + 2] = gap1;
				}
				else
				{
					myBioArray[j + 2][i + 2] = gap2;
				}
			}

		}
	}

	for (int i = 0; i < maxHigh; i++)
	{
		if (myBioArray[strSize - 1][strSize - 1] > myMax[i])
		{
			for (int j = i; j < maxHigh - 1; j++)
			{
				myMax[i + 1] = myMax[i];
				myid1[i + 1] = myid1[i];
				myid2[i + 1] = myid2[i];
			}
			myMax[i] = myBioArray[strSize - 1][strSize - 1];
			myid1[i] = first;
			myid2[i] = second;

			i = maxHigh;
		}
	}
}

void showMyMaxArray(double myMax[maxHigh],int myid1[maxHigh],int myid2[maxHigh])
{
	for (int i = 0; i < maxHigh; i++)
	{
		cout<<"Seq-> " << myid1[i]<< " \tSeq-> " << myid2[i] <<" \tMatch Score  " << fixed << setprecision(9) << myMax[i] << endl;
	}
}

int main() {

	auto startT = high_resolution_clock::now();
	double myBioArray[strSize][strSize];
	double myMax[maxHigh] = { 0 };
	double gap = -1.832482334, mismatch = -2.451795405, match = 3.621354295;
	long hour=0, min=0, second=0 ,miliSec=0 ,myTime;
	char buyuktur = '>';
	int id = 0;
	string text;
	double textArray[numOfSeq][strSize];
	int myid1[maxHigh] = { 0 };
	int myid2[maxHigh] = { 0 };

	/////////// READ AND WRÝTE TO ARRAY /////////
	ifstream theFile("sequence.txt");
	
	while (theFile >> buyuktur >> id >> text) {
		for (int i = id; i < numOfSeq; i++) {
			for (int j = 0; j < strSize - 2; j++) {
				textArray[i][j] = text[j];
			}
		}
	}

	theFile.close();
	/////////////////////////////////////////////
	cout << "Gap= " << fixed << setprecision(9) << gap << " Match= " << match << " Mismatch= " << mismatch << endl <<endl;
	clearArray(myBioArray);
	setArray(myBioArray);

	#pragma omp parallel num_threads(100)
	{
		int i=0, j=0;
		#pragma omp for private(i,j,myBioArray) 
		for (i = 0; i < numOfSeq; i++)
		{
			for (j = i + 1; j < numOfSeq; j++)
			{
				start(myBioArray, textArray[i], textArray[j]);
				nwAlgorithm(myBioArray, myMax,i,j,myid1,myid2,gap,mismatch,match);
			}
		}
	}
	//paralelsiz
	/*for (int i = 0; i < numOfSeq; i++)
	{
		for (int j = i + 1; j < numOfSeq; j++)
		{
			start(myBioArray, textArray[i], textArray[j]);
			nwAlgorithm(myBioArray, myMax,i,j,myid1,myid2,gap,mismatch,match);
		}
	}*/

	showMyMaxArray(myMax,myid1,myid2);

	//////////////Process Timer////////////////////////
	auto stop = high_resolution_clock::now();
	auto duration = duration_cast<milliseconds>(stop - startT);
	myTime = duration.count();
	hour = (myTime / 3600000);
	min = (myTime/60000) % 60;
	second = (myTime/1000) % 60;
	miliSec = myTime % 1000;

	cout << endl << "For " << numOfSeq << " Sequence And "<< (numOfSeq * (numOfSeq - 1)) / 2 << " Comparison" << endl;
	cout << "Time Taken by Program: " << hour << " Hour " <<min << " Min " << second << " Seconds " << miliSec <<" Millisecond" << endl;

	system("PAUSE");
}