////////////////////////////////////////////////////////////////////////////////////////////////////
//
// faceReco.cpp
// this file implements the magic miror which recognizes 
// people and talk to them. Like in White Snow. 
//
// This file is strongly inspired
// from Philipp Wagner works
// http://docs.opencv.org/trunk/modules/contrib/doc/facerec/tutorial/facerec_video_recognition.html
// Thank you Philipp, you're good.
//
// This file is put as exemple - considered it as a draft source code
// debug lines are still here. No memory optimization etc.
// Many constant are hard-coded. Should be in a conf file, etc.
// but it does the job
//
// Pierre Raufast - 2013
// (for a better world, read Giono's books)
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "opencv2/core/core.hpp"
#include "opencv2/contrib/contrib.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/objdetect/objdetect.hpp"
#include "/home/pi/pierre/libfacerec-0.04/include/facerec.hpp"


#include <iostream>
#include <fstream>
#include <sstream>
#include "time.h"

using namespace cv;
using namespace std;

// some constants to manage nb of people to learn+ id of people 
#define MAX_PEOPLE 		4
#define P_PIERRE		0
#define P_NATACHA		1
#define P_LISA			3
#define P_NELLIE		2

// for debug and trace
#define TRACE 1
#define DEBUG_MODE 0
#define DEBUG if (DEBUG_MODE==1)

// update here your opencv haarcascades path
string glasses_cascade_name = "/usr/share/opencv/haarcascades/haarcascade_eye_tree_eyeglasses.xml";
string eyes_cascade_name 	= "/usr/share/opencv/haarcascades/haarcascade_eye.xml";

CvPoint Myeye_left;
CvPoint Myeye_right;
CascadeClassifier eyes_cascade;
CascadeClassifier glasses_cascade;

// name of people
string  people[MAX_PEOPLE];

// nb of times RPI talks to one guy
int nbSpeak[MAX_PEOPLE];

int bHisto;

// nb of picture learnt by people
int nPictureById[MAX_PEOPLE];

/////////////////////////////////////////////////
// trace if TRACE==1
/////////////////////////////////////////////////
void trace(string s)
{
	if (TRACE==1)
	{
		cout<<s<<"\n";
	}
}
/////////////////////////////////////////////////
//
// read csv files. 
// Fully copied from Philipp Wagner works
// http://docs.opencv.org/trunk/modules/contrib/doc/facerec/tutorial/facerec_video_recognition.html
// 
////////////////////////////////////////////////
static void read_csv(const string& filename, vector<Mat>& images, vector<int>& labels, char separator = ';') {
    std::ifstream file(filename.c_str(), ifstream::in);
    if (!file) {
        string error_message = "(E) No valid input file was given, please check the given filename.";
        CV_Error(CV_StsBadArg, error_message);
    }
    string line, path, classlabel;
    int nLine=0;
    while (getline(file, line)) {
        stringstream liness(line);
        getline(liness, path, separator);
        getline(liness, classlabel);
        if(!path.empty() && !classlabel.empty()) 
        {
        	// read the file and build the picture collection
            images.push_back(imread(path, 0));
            labels.push_back(atoi(classlabel.c_str()));
            nPictureById[atoi(classlabel.c_str())]++;
        	nLine++;
        }
    }
    
    // write number of picture by people
    // did you notice? I'm not familiar with string classe :-)
    // I prefer my old good char* ...
    // (what a pity)
	char sTmp[128];
    sprintf(sTmp,"(init) %d pictures read to train",nLine);
    trace((string)(sTmp));
	for (int j=0;j<MAX_PEOPLE;j++)
	{
		sprintf(sTmp,"(init) %d pictures of %s (%d) read to train",nPictureById[j],people[j].c_str(),j);
   	 	trace((string)(sTmp));
	}
}


/////////////////////////////////////////////////
//
// speak to the guy nÂ° nWho
// very easy implementation : 
// - we build a text file with text to say
// - we use "espeak" as a command line to speak
//
/////////////////////////////////////////////////
void saySomething(int nWho)
{
	FILE *f = NULL;
	
	// get local time
	struct tm *pdh;
	time_t intps;
	intps=time(NULL);
	pdh = localtime(&intps);
				
	// is that the first time RPI talks to him/her ?
	if (nbSpeak[nWho]==0)
	{
		// first time, say hello
		trace("First time, say hello to "+people[nWho]);
		
		// rewrite the tmp file 
		f= fopen("speak.txt","w");
		if (f==NULL)
		{
			cout<<"(E) unable to write file\n";
			return;
		}
		else
		{
		
			// say "hello" in french
			fprintf(f,"bonjour %s.\n",people[nWho].c_str());
			
			// depending of people, say what you want. 
			if (nWho==P_NATACHA)
			{
				// fake line : temperature is harcoded. 
				// but it's easy to do put here, all information get from internet
				// weather, google calendar API, ... ---> see on step 7 more sophisticated exemple
				// just get the information here or get it by daemon, write in a file and read the file here.
				fprintf(f,"Pas la peine de me demander, tu es toujours la plus belle.\n");
				fprintf(f,"Je te souhaite une excellente fin de journÃ©e\n");							
			}
			if (nWho==P_PIERRE)
			{
				// for exemple, here, say how time is it.
				fprintf(f,"Il est %d heures et %d minutes.\n",pdh->tm_hour,pdh->tm_min);				
			}
			if (nWho==P_NELLIE)
			{
				// ah, ah.
				// if (pdh->tm_hour>8)
				// fprintf(f,"Don't forget to wash your teeth.\n");
				
			}
			if (nWho==P_LISA)
			{
				// ah, ah.
				// if (pdh->tm_hour>8)
				// fprintf(f,"Don't forget to wash your teeth.\n");
				
			}
			
			// close the file
			fclose(f);
			
			// speak using espeak
			// -f = name of the file
			// -vfr = voice french
			// +f5 : the fifth female voice (could be : +f3, +h1)...
			// -s130 : change speed (default 160 ?)
			// 2>/dev/null: espeak generates error, just send them to /dev/null
			
			system("espeak -f speak.txt -vfr+f5 -s130 2>/dev/null");
		}
	}
	
	// if it's 15 times that RPI recognized the guy,
	// then, it's worth to talk to him one more time
	// but once again, you could implement here
	// whatever dialog logic you want
	// talk every hour, every time, every minute, ....
	// this is given as an exemple
	if (nbSpeak[nWho]==15)
	{
		trace("Still here ?  "+people[nWho]);
		// write tmp file
		f= fopen("speak.txt","w");
		if (f==NULL)
		{
			cout<<"(E) impossible d'Ã©crire le fichier\n";
			return;
		}
		else
		{
			// once again....
			if (nWho==P_NATACHA)
			{
				// let's talk to her in english
				fprintf(f,"May the force be with you.\n");						
			}
			if (nWho==P_PIERRE)
			{
				// do you want to take a coffee ?
				fprintf(f,"%s, que dirais tu de prendre un petit cafÃ© ?.\n",people[nWho].c_str()); 		
			}
			if (nWho==P_NELLIE)
			{
				// ...
			}
			if (nWho==P_LISA)
			{
				// ...
			}

			// close file
			fclose(f);
			
			// to change... if Pierre, RPI talks in french, if Natacha, RPI talks in english
			if (nWho==P_PIERRE) system("espeak -f speak.txt -vfr+f5 -s130 2>/dev/null");
			if (nWho==P_NATACHA) system("espeak -f speak.txt  -s130  2>/dev/null");
		}
	}
	
	// increment the visitor...
	nbSpeak[nWho]++;	
	
}

///////////////////////////////////////////////////
//
// traceStep -- for debug
//
///////////////////////////////////////////////////

void traceStep(int nStep)
{
	cout<<"("<<nStep<<")"<<clock()<<"\n";
}


///////////////////////////////////////////////////
//
// main
//
///////////////////////////////////////////////////

int main(int argc, const char *argv[]) {
	

	   // Check for valid command line arguments, print usage
	   // if no arguments were given.
	cout<<"start\n";
	   if ((argc != 4)&&(argc!=3)) {
	       cout << "usage: " << argv[0] << " ext_files  seuil(opt) \n files.ext histo(0/1) 5000 \n" << endl;
	       exit(1);
	   }
	   int PREDICTION_SEUIL ;
	   // set value by default for prediction treshold = minimum value to recognize
	if (argc==3) { trace("(init) prediction treeshold = 4500.0 by default");PREDICTION_SEUIL = 4500.0;}
	if (argc==4) PREDICTION_SEUIL = atoi(argv[3]);
	
	// do we do a color histogram equalization ? 
	bHisto=atoi(argv[2]);
	
	// load for eyes reco
	if( !eyes_cascade.load( eyes_cascade_name ) ){ printf("--(!)Error loading\n"); return -1; };
	if( !glasses_cascade.load( glasses_cascade_name ) ){ printf("--(!)Error loading\n"); return -1; };
	
	//
	// init people
	//
	Mat gray, frame,original,face,face_resized;


	// init people, should be do in a config file,
	// but I don't have time, I need to go to swimming pool
	// with my daughters
	// and they prefer to swimm than to see their father do a config file
	// life is hard.
	people[P_PIERRE] 	= "Pierre";
	people[P_NATACHA] 	= "Natacha";
	people[P_NELLIE] 	= "Nellie";
	people[P_LISA]		= "Lisa";
	
	// init...
	// reset counter
	for (int i=0;i>MAX_PEOPLE;i++) 
	{
		nbSpeak[i] =0;
		nPictureById[i]=0;
	}
	int bFirstDisplay	=1;
	trace("(init) People initialized");
	
	// Get the path to your CSV
	string fn_csv = string(argv[1]);
	
	// change with opencv path	
	string fn_haar = "/usr/share/opencv/haarcascades/haarcascade_frontalface_alt.xml";
	DEBUG cout<<"(OK) csv="<<fn_csv<<"\n";

    
    vector<Mat> images;
    vector<int> labels;
    
    // Read in the data (fails if no valid input filename is given, but you'll get an error message):
    try {
        read_csv(fn_csv, images, labels);
		DEBUG cout<<"(OK) read CSV ok\n";
    	} 
    catch (cv::Exception& e) 
    {
        cerr << "Error opening file \"" << fn_csv << "\". Reason: " << e.msg << endl;
        exit(1);
    }

	// get heigh, witdh of 1st images--> must be the same
    int im_width = images[0].cols;
    int im_height = images[0].rows;
	trace("(init) taille images ok");
 
 	//
    // Create a FaceRecognizer and train it on the given images:
	//
	
	// this a Eigen model, but you could replace with Fisher model (in this case
	// threshold value should be lower) (try)	
	Eigenfaces model;
	
    //	Fisherfaces model; 
    
    // train the model with your nice collection of pictures	
    trace("(init) start train images");
    model.train(images, labels);
 	trace("(init) train images : ok");
 
	// load face model
    CascadeClassifier face_cascade;
    if (!face_cascade.load(fn_haar))
   	{
    			cout <<"(E) face cascade model not loaded :"+fn_haar+"\n"; 
    			return -1;
    }
    trace("(init) Load modele : ok");
    
    // capture video
    CvCapture* capture;
 	capture = cvCaptureFromCAM(-1);
 	
 	// set size of webcam 320x240, enough for RPI power
	cvSetCaptureProperty(capture, CV_CAP_PROP_FRAME_WIDTH,320);
	cvSetCaptureProperty(capture, CV_CAP_PROP_FRAME_HEIGHT, 240);
	trace("(init) webcam initialized : ok");
	
	// can't capture, doc'
	if (!capture)
	{   
		cout << "(E) Capture Device cannot be opened." << endl;
        return -1;
    }
    int nFirst=0;
    
	// Holds the current frame from the Video device
	for(;;)
	{
		// get the picture from webcam
		original= cvQueryFrame( capture);
		char key;        		
      
   		// Convert the current frame to grayscale:
        cvtColor(original, gray, CV_BGR2GRAY);
        
        // and equalize Histo (as model pictures)
        if (bHisto)equalizeHist( gray, gray);        
		
        vector< Rect_<int> > faces;

        // detect faces
		face_cascade.detectMultiScale(gray, faces, 1.1, 3, CV_HAAR_SCALE_IMAGE, Size(60,60));	

		// for each faces founded
  	  	for(int i = 0; i < faces.size(); i++) 
  	  	{       
  	  		// crop face (pretty easy with opencv, don't you think ? 
            Rect face_i = faces[i];
            face = gray(face_i);
           
            
			//  resized face and display it
			cv::resize(face, face_resized, Size(im_width, im_height), 1.0, 1.0, INTER_CUBIC);
			imshow("face", face_resized);
			
			if (bFirstDisplay) // first display, allow more time to display
			{
				key = (char) waitKey(100);
				bFirstDisplay = 0;
			}
			else
			{
				key = (char) waitKey(10);
			}
			
			
			nFirst++;
			
			// at this stage, face is detect,
			// now, we try to predict who is it ? 
			char sTmp[256];		
			double predicted_confidence	= 0.0;
			int prediction				= -1;
			model.predict(face_resized,prediction,predicted_confidence);
			
			// create a rectangle around the face      
			rectangle(original, face_i, CV_RGB(0, 255 ,0), 1);
				
			// if good prediction : > threshold 
			if (predicted_confidence>PREDICTION_SEUIL)
			{
				// trace
				sprintf(sTmp,"+ prediction ok = %s (%d) confiance = (%d)",people[prediction].c_str(),prediction,(int)predicted_confidence);
				trace((string)(sTmp));
			
			 	// display name of the guy on the picture
				string box_text;
				if (prediction<MAX_PEOPLE)
				{
					box_text = "Id="+people[prediction];
				}
				else
				{
					trace("(E) prediction id incohÃ©rent");
				}
				int pos_x = std::max(face_i.tl().x - 10, 0);
				int pos_y = std::max(face_i.tl().y - 10, 0);			   
				putText(original, box_text, Point(pos_x, pos_y), FONT_HERSHEY_PLAIN, 1.0, CV_RGB(0,255,0), 1.0);
				
				/// Show the result:
        		imshow("Reco", original);
        		key = (char) waitKey(10);
        		
				// say hello to ...
				saySomething(prediction);
				}
				else
				{
					// oh my god ! prediction result is too low
					// RPI prefers not to name my wife with the name of my mother-in-law.
					// just do nothing, except trace
					sprintf(sTmp,"- prediction too low = %s (%d) confiance = (%d)",people[prediction].c_str(),prediction,(int)predicted_confidence);
					trace((string)(sTmp));
				} 
			}
	
		    // Show the result:
		    // notice we display twice the picture, first time before .predict, one after. 
		    // to double display freq.
		    imshow("Reco", original);
		   
		    // And display it:
		    key = (char) waitKey(100);

		}
	return 0;
}

// That's all folks