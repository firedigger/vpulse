#include "videoreader.h"

VideoReader::VideoReader():
    portion(140)
{
    av_register_all();
    //this->frames=
    //template<typename _Tp> _Tp* allocate(size_t n)
    //template<Mat> Mat* allocate(100);
    //this->frames=cv::allocate<Mat>(300);
    //this->frames=cv::allocate<Mat(100,200,CV_8UC(3))>(300);
}

 VideoReader::~VideoReader()
{
    //template<typename _Tp> void deallocate(_Tp* ptr, size_t n)
     //template<Mat> void deallocate(this->frames,100);
     //Mat::allocator
     //cv::deallocate(this->frames,300);



}
int VideoReader::ReadFrames(const char* videofilename_in, int pyramid_level)
 {
     AVFormatContext *pFormatCtx=NULL;

     // Open video file
     //char* filename_in = "face.mp4";

     //Look header info
     if(avformat_open_input(&pFormatCtx, videofilename_in, NULL, NULL)!=0)
          {printf("Couldn't open video file  \n");return -1;} // Couldn't open file

     // Retrieve stream information
     if(avformat_find_stream_info(pFormatCtx,NULL)<0)
       {printf("Couldn't find stream information\n");return -1;} // Couldn't find stream information

     // Dump information about file onto standard error
     av_dump_format(pFormatCtx, 0, videofilename_in, 0);


     //search video stream
     int i;
     AVCodecContext *pCodecCtx;

     // Find the first video stream
     int videoStream=-1;
     for(i=0; i<pFormatCtx->nb_streams; i++)
       if(pFormatCtx->streams[i]->codec->codec_type==AVMEDIA_TYPE_VIDEO) {
         videoStream=i;
         break;
       }
     if(videoStream==-1)
       return -1; // Didn't find a video stream

     // Get a pointer to the codec context for the video stream
     pCodecCtx=pFormatCtx->streams[videoStream]->codec;


     //Search codec
     AVCodec *pCodec;

     // Find the decoder for the video stream
     pCodec=avcodec_find_decoder(pCodecCtx->codec_id);
     if(pCodec==NULL) {
       fprintf(stderr, "Unsupported codec!\n");
       return -1; // Codec not found
     }
     // Open codec
     if(avcodec_open(pCodecCtx, pCodec)<0)
       return -1; // Could not open codec

     //Store frame
     AVFrame *pFrame;

     // Allocate video frame
     pFrame=avcodec_alloc_frame();


     // Allocate an AVFrame structure
     AVFrame * pFrameRGB=avcodec_alloc_frame();
     if(pFrameRGB==NULL)
       return -1;

     uint8_t *buffer;
     int numBytes;
     // Determine required buffer size and allocate buffer
     numBytes=avpicture_get_size(PIX_FMT_RGB24, pCodecCtx->width,
                                 pCodecCtx->height);
     buffer=(uint8_t *)av_malloc(numBytes*sizeof(uint8_t));


     // Assign appropriate parts of buffer to image planes in pFrameRGB
     // Note that pFrameRGB is an AVFrame, but AVFrame is a superset
     // of AVPicture
     avpicture_fill((AVPicture *)pFrameRGB, buffer, PIX_FMT_RGB24,
                     pCodecCtx->width, pCodecCtx->height);

     //this->frameHeight=pCodecCtx->height;
     //this->frameWidth=pCodecCtx->width;

     SwsContext *pSWSContext = sws_getContext(pCodecCtx->width, pCodecCtx->height, pCodecCtx->pix_fmt, pCodecCtx->width, pCodecCtx->height, PIX_FMT_RGB24, SWS_BILINEAR, 0, 0, 0);

     int frameFinished;
     AVPacket packet;

     i=0;
     while(av_read_frame(pFormatCtx, &packet)>=0)
     {
       // Is this a packet from the video stream?
       if(packet.stream_index==videoStream)
       {
             // Decode video frame
         avcodec_decode_video2(pCodecCtx, pFrame, &frameFinished,
                              &packet);

         // Did we get a video frame?
         if(frameFinished)
         {
              //Scale the raw data/convert it to our video buffer.
             sws_scale(pSWSContext, pFrame->data, pFrame->linesize, 0, pCodecCtx->height, pFrameRGB->data, pFrameRGB->linesize);
             // Save the frame to disk
             if(++i<=FRAMES_MAX)
             {
                 char* imageOutName = "frame1_mat.ppm";
                 char* imageDownName = "frame1_down_new.ppm";
                 //Mat* image_mat=new Mat();//(pCodecCtx->height, pCodecCtx->width,pFrameRGB->linesize[0]*pCodecCtx->height, ch);
                 this->frames[i-1]=new Mat();
                 this->blured_frames[i-1]= new Mat();
                 Mat*image_mat =this->frames[i-1];
                 Mat*image_blured = this->blured_frames[i-1];
                 //image_mat->create(pCodecCtx->height,pCodecCtx->width,CV_8UC(3));
                image_mat->create(pCodecCtx->height,pCodecCtx->width,CV_8UC(3));
                image_blured->create(pCodecCtx->height,pCodecCtx->width,CV_8UC(3));
                 for(int row=0; row<pCodecCtx->height;row++)
                 {
                     for(int col=0; col<pCodecCtx->width; col++)
                     {
                         image_mat->at<Vec3b>(row,col)[0]=pFrameRGB->data[0][row*pFrameRGB->linesize[0]+col*3+2];
                         image_mat->at<Vec3b>(row,col)[1]=pFrameRGB->data[0][row*pFrameRGB->linesize[0]+col*3+1];
                         image_mat->at<Vec3b>(row,col)[2]=pFrameRGB->data[0][row*pFrameRGB->linesize[0]+col*3+0];

                         image_blured->at<Vec3b>(row,col)[0]=pFrameRGB->data[0][row*pFrameRGB->linesize[0]+col*3+2];
                         image_blured->at<Vec3b>(row,col)[1]=pFrameRGB->data[0][row*pFrameRGB->linesize[0]+col*3+1];
                         image_blured->at<Vec3b>(row,col)[2]=pFrameRGB->data[0][row*pFrameRGB->linesize[0]+col*3+0];
                        // if((row<5)&&(col<5)&&(i<5))
                        // {
                        //     printf("image_mat->at<Vec3b>(row,col)[0]=%uc\n",image_mat->at<Vec3b>(row,col)[0]);
                        //     printf("image_mat->at<Vec3b>(row,col)[1]=%uc\n",image_mat->at<Vec3b>(row,col)[1]);
                        //     printf("image_mat->at<Vec3b>(row,col)[2]=%uc\n",image_mat->at<Vec3b>(row,col)[2]);
                        // }
                     }

                 }

               //imwrite( imageOutName, *(this->frames[i-1]) );
               if(pyramid_level>=1)
               {
                    //Mat* image_down = new Mat(*image_mat);
                   for(int lvl=1; lvl<=pyramid_level;lvl++)
                   {
                    pyrDown(*image_blured, *image_blured, Size(image_blured->cols/2, image_blured->rows/2));
                   }
               }
               //imwrite(imageDownName, *image_mat);
               //SaveFrame(pFrameRGB, pCodecCtx->width, pCodecCtx->height, i);

             }
         }
       }

       // Free the packet that was allocated by av_read_frame
       av_free_packet(&packet);
     }

     this->frameHeight=this->blured_frames[0]->rows;
     this->frameWidth=this->blured_frames[0]->cols;
     this->NumberOfFrames=i-6; //throw out 6 last frames
     printf("numberOfFrames(-6)=%d\n",this->NumberOfFrames);
     av_free(buffer);
     av_free(pFrameRGB);
     // Free the YUV frame
     av_free(pFrame);

     // Close the codec
     avcodec_close(pCodecCtx);

     // Close the video file
     av_close_input_file(pFormatCtx);

 }

void print_frame_txt(Mat* frame,const char* filename)
{
    int fr_rows = frame->rows;
    int fr_cols = frame->cols;
    FILE * stream;
            if ((stream=fopen(filename, "w")) != 0)
    {
        fprintf(stream, "row\t\t R \t\t\t G\t\t\tB\t\t\n");
        for(int curr_row = 0; curr_row < fr_rows; curr_row++)
        {
            fprintf(stream, "\n \t row = %d\n",curr_row);
            for(int curr_col = 0; curr_col <fr_cols; curr_col++)
            {
                fprintf(stream, "%d %u \t|\t %u \t|\t %u",curr_row, frame->at<Vec3b>(curr_row,curr_col)[0],frame->at<Vec3b>(curr_row,curr_col)[1],frame->at<Vec3b>(curr_row,curr_col)[2]);
                fputc('\n',stream);
            }

        }
    }
    fclose(stream);
}

int VideoReader::PrintFrames(void)
{
    if(this->frames[0]==NULL)
    {
        printf("Error! No frames detected.\n");return(1);
    }
    else
    {
        char frame_filename[30];
        char frame_filename_txt[30];
        char frame_filenameB_txt[32];

        for(int i=0; i<NumberOfFrames; i++)
        {
            //if((i<10)||(i>NumberOfFrames-10)){              //FIXME
            sprintf(frame_filename, "frame%d.ppm", i);
            //sprintf(frame_filename_txt, "FramesInputTxt/frame%d.txt",i);
            //sprintf(frame_filenameB_txt, "FramesBlurredTxt/frame%d.txt",i);
            imwrite(frame_filename, *(this->/*blured_*/frames[i]));

            //print_frame_txt(this->frames[i],frame_filename_txt);
            //if((i<5)||(i>282)){
            //    print_frame_txt(this->blured_frames[i],frame_filenameB_txt);}
            //}
        }
    }

}

int VideoReader::PyrUpBlured(int pyramid_level)
{
    if(pyramid_level>=1)
    {
        for(int NofFr = 0; NofFr < this->NumberOfFrames; NofFr++)
        {
            Mat* image_mat=this->blured_frames[NofFr];
            for(int lvl=1; lvl<=pyramid_level;lvl++)
            {
                //pyrUp(*image_mat, *image_mat, Size(image_mat->cols*2, image_mat->rows*2));
                cv::resize(*image_mat,*image_mat,Size(image_mat->cols*2, image_mat->rows*2),0,0,INTER_CUBIC);
            }
        }
    }
}

int VideoReader::AddPulseToFrames(void)
{
    for(int NofFr = 0; NofFr < this->NumberOfFrames; NofFr++)
    {
        Mat* image_mat=this->frames[NofFr];
        Mat* image_blured=this->blured_frames[NofFr];
        for(int row=0; row<this->frames[0]->rows;row++)
        {
            for(int col=0; col<this->frames[0]->cols; col++)
            {
                image_mat->at<Vec3b>(row,col)[0]=image_mat->at<Vec3b>(row,col)[0]+image_blured->at<Vec3b>(row,col)[0];
                image_mat->at<Vec3b>(row,col)[1]=image_mat->at<Vec3b>(row,col)[1]+image_blured->at<Vec3b>(row,col)[1];
                image_mat->at<Vec3b>(row,col)[2]=image_mat->at<Vec3b>(row,col)[2]+image_blured->at<Vec3b>(row,col)[2];
            }
        }
     }
}

int VideoReader::getFrameHeight(void)
{
    return(this->frameHeight);
}

int VideoReader::getFrameWidth(void)
{
    return(this->frameWidth);
}

int VideoReader::getNumberOfFrames(void)
{
    return(this->NumberOfFrames);
}

Mat** VideoReader::getFrames(void)
{
    return(this->frames);
}

Mat** VideoReader::getBluredFrames(void)
{
    return(this->blured_frames);
}
int VideoReader::get_portion(void)
{
    return(portion);
}

void VideoReader::CVReadVideo(const char* videofilename_in)
{
    //QTime tt;
    //cvNamedWindow("original",CV_WINDOW_FULLSCREEN);
    cvframe_=NULL;
    //workstady=0;
    CvCapture* capture =cvCreateFileCapture(videofilename_in);
    //processor* Pr1=new processor();
    //int framePortion=30*5;
    int framesRed=0;
    int currentPortion=0;
    while(1)
    {
                cvframe_=/*(Mat*)*/cvQueryFrame(capture);
                if(!cvframe_){break;}
                frames[framesRed]= new Mat;
                //blured_frames[framesRed]= new Mat;
                Mat* mat_frame = new Mat(cvframe_);
                blured_frames[framesRed]= mat_frame;
                frames[framesRed]->create(mat_frame->rows, mat_frame->cols,CV_8UC(3));
                mat_frame->copyTo(*frames[framesRed]);
                /*if(framesRed==0){
                const char* frn1="frame1frIpl.txt";
                print_frame_txt(&mat_frame,frn1);}*/
                /*if(framesRed==0)
                {
                    allocateMatBuffer(mat_frame->cols,mat_frame->rows);
                }*/
                /*if(framesRed>2){IplImage cvBlframe(mat_frame);
                cvShowImage("original",&cvBlframe);}*/
                //if(framesRed>0){
                    //mat_frame->copyTo(*cvMatframe);
                //}
                //============================================================
                //QTime tt;
                //tt.start();
                for(int lvl=0; lvl<3; lvl++)
                {
                    pyrDown(*mat_frame, *mat_frame, Size(mat_frame->cols/2, mat_frame->rows/2));
                }
                //qDebug("(CVREAD)time elapsed: %d ms",tt.elapsed());
                /*if(framesRed==0){Pr1->init(framePortion,mat_frame.cols,mat_frame.rows,30);
                }*/
                framesRed++;
                currentPortion++;
                /*if((currentPortion<=framePortion)&&(currentPortion==framesRed))
                {
                    Pr1->addFrame(&mat_frame);
                    Pr1->IncFrCounter();
                }
                else
                {
                   mode=1;
                   //Pr1->changeBufferFlag();
                   Pr1->initFFT_IFFT();
                   tt.start();
                   Pr1->work(50.0/60.0,60.0/60.0,185);
                   qDebug("time elapsed: %d ms",tt.elapsed());
                   Pr1->clearFFT_IFFT();
                   currentPortion=0;
                }*/
                //pyrUp(*cvframe_, *cvframe_, Size(cvframe_->cols*2, cvframe_->rows*2));

                //============================================================
                //char c=cvWaitKey(33);
                //if(c==27){break;}

    }
    //printf("counter=%d\n",Pr1->getFrCounter());
    //cvDestroyWindow("original");
    fps= cvGetCaptureProperty(capture,CV_CAP_PROP_FPS);
    this->frameHeight=this->blured_frames[0]->rows;
    this->frameWidth=this->blured_frames[0]->cols;
    this->NumberOfFrames=framesRed-10; //throw out 10 last frames
    cvReleaseCapture(&capture);
    qDebug("Noffr = %d, frH=%d, frW= %d",NumberOfFrames,frameHeight,frameWidth);
    //delete(Pr1);
}

void VideoReader::CVReadVideoCAM(void)
{
    //QTime tt;
    //cvNamedWindow("original1",CV_WINDOW_FULLSCREEN);
    cvframe_=NULL;
    //workstady=0;
    CvCapture* capture = cvCreateCameraCapture(CV_CAP_ANY);
    assert(capture);
    //int framePortion=30*5;
    int framesRed=0;
    int currentPortion=0;
    while(1)
    {
                cvframe_=/*(Mat*)*/cvQueryFrame(capture);
                if(!cvframe_){break;}
                cvShowImage("original1",cvframe_);
                frames[framesRed]= new Mat;
                //blured_frames[framesRed]= new Mat;
                Mat* mat_frame = new Mat(cvframe_);
                blured_frames[framesRed]= mat_frame;
                frames[framesRed]->create(mat_frame->rows, mat_frame->cols,CV_8UC(3));
                mat_frame->copyTo(*frames[framesRed]);
                for(int lvl=0; lvl<4; lvl++)
                {
                    pyrDown(*mat_frame, *mat_frame, Size(mat_frame->cols/2, mat_frame->rows/2));
                }

                framesRed++;
                currentPortion++;
                char c=cvWaitKey(33);
                if(c==27){break;}
    }
    cvDestroyWindow("original1");
    fps= cvGetCaptureProperty(capture,CV_CAP_PROP_FPS);
    this->frameHeight=this->blured_frames[0]->rows;
    this->frameWidth=this->blured_frames[0]->cols;
    this->NumberOfFrames=framesRed-20; //throw out 10 last frames
    cvReleaseCapture(&capture);
    qDebug("Noffr = %d, frH=%d, frW= %d",NumberOfFrames,frameHeight,frameWidth);
}

void VideoReader::MatTest(char* f_name)
{
        CvCapture* capture =cvCreateFileCapture(f_name);
        cvframe_=/*(Mat*)*/cvQueryFrame(capture);
        Mat* mat_frame =new Mat(cvframe_);
        Mat* image_mat = new Mat(mat_frame->rows,mat_frame->cols,CV_8UC(3));
        Mat* image_mat_db = new Mat(mat_frame->rows,mat_frame->cols,CV_32FC(3));
        for(int row=0; row<mat_frame->rows;row++)
        {
            for(int col=0; col<mat_frame->cols; col++)
            {
         image_mat->at<Vec3b>(row,col)[0]=mat_frame->at<Vec3b>(row,col)[0];
         image_mat->at<Vec3b>(row,col)[1]=mat_frame->at<Vec3b>(row,col)[1];
         image_mat->at<Vec3b>(row,col)[2]=mat_frame->at<Vec3b>(row,col)[2];

         image_mat_db->at<Vec3f>(row,col)[0]=(float)mat_frame->at<Vec3b>(row,col)[0];
         image_mat_db->at<Vec3f>(row,col)[1]=(float)mat_frame->at<Vec3b>(row,col)[1];
         image_mat_db->at<Vec3f>(row,col)[2]=(float)mat_frame->at<Vec3b>(row,col)[2];
         if(row<5&&col<5)
         {
             qDebug("orig = %d %d %d",mat_frame->at<Vec3b>(row,col)[0],mat_frame->at<Vec3b>(row,col)[1],mat_frame->at<Vec3b>(row,col)[2]);
             qDebug("double = %lf %lf %lf",image_mat_db->at<Vec3f>(row,col)[0],image_mat_db->at<Vec3f>(row,col)[1],image_mat_db->at<Vec3f>(row,col)[2]);
         }
            }
        }
        cvReleaseCapture(&capture);
        char* image1 = "TestMat/testMatUchar.ppm";
        char* image2 = "TestMat/testMatDb.ppm";
        imwrite(image1, *image_mat);
        imwrite(image2, *image_mat_db);
}

void VideoReader::CVReadVideoRT(const char* videofilename_in, processor* Pr1, double fLow, double fHight,double ampFactor)
{
    //QTime tt;

    cvframe_=NULL;
    //workstady=0;
    CvCapture* capture =cvCreateFileCapture(videofilename_in);
    //CvCapture* capture = cvCreateCameraCapture(CV_CAP_ANY);
    //assert(capture);
    //this->fps= cvGetCaptureProperty(capture,CV_CAP_PROP_FPS);       //FIXME!
    fps=30;
    //int framePortion=30*5;
    int framesRed=0;
    int currentPortion=0;
    int mode =0;
    int stady=0;
    int wait_=0;
    int portion_ind=0;
    int* teor_freq=(int*)malloc(sizeof(int)*(portion/2+1));
    for(int i = 0; i<portion/2+1; i++){ teor_freq[i]=0;}
    int tmp_ind=0;
    cvNamedWindow("original1",CV_WINDOW_AUTOSIZE);
QTime tt;
    while(1)
    {

        if(mode==0)
        {
                cvframe_=/*(Mat*)*/cvQueryFrame(capture);
                if(!cvframe_){break;}
                tt.start();
                //qDebug("framesRed=%d",framesRed);
                //frames[framesRed]= new Mat;
                //blured_frames[framesRed]= new Mat;
                Mat* mat_frame = new Mat(cvframe_);
                blured_frames[framesRed]= mat_frame;
                //frames[framesRed]->create(mat_frame->rows, mat_frame->cols,CV_8UC(3));
                //mat_frame->copyTo(*frames[framesRed]);
                if(framesRed==0){
                    this->frameHeightOr=this->blured_frames[0]->rows;
                    this->frameWidthOr=this->blured_frames[0]->cols;
                    LengthRend=frameHeightOr*frameWidthOr*3;}
                //IplImage cvBlframe(*mat_frame);
                cvShowImage("original1",cvframe_);
                //if(framesRed>0){
                    //mat_frame->copyTo(*cvMatframe);
                //}
                //============================================================
                for(int lvl=0; lvl<4; lvl++)
                {
                    pyrDown(*mat_frame, *mat_frame, Size(mat_frame->cols/2, mat_frame->rows/2));
                }
                if(framesRed==0)
                {
                    Pr1->init(this->get_portion(),blured_frames[0]->rows,blured_frames[0]->cols,fps);
                   /* qDebug("rows=%d",blured_frames[0]->rows);
                    qDebug("cols=%d",blured_frames[0]->cols);
                    qDebug("portion=%d",portion);
                    qDebug("fps1=%lf",Pr1->getFPS());*/
                }
                framesRed++;
                currentPortion++;
                if(currentPortion==this->portion+1)
                {
                    mode=2;
                }
                wait_ = (int)(100.0/3.0-tt.elapsed());
                //qDebug("wait=%d",wait_);
                if(wait_<=0){wait_=1;}
                char c=cvWaitKey(wait_);
                if(c==27){break;}
        }else{
            if(mode==1)
            {
                cvframe_=/*(Mat*)*/cvQueryFrame(capture);
                if(!cvframe_){break;}
                tt.start();
                //qDebug("framesRed=%d",framesRed);
                //frames[framesRed]= new Mat;
                //blured_frames[framesRed]= new Mat;
                Mat* mat_frame = new Mat(cvframe_);
                //blured_frames[framesRed]= mat_frame;
                //frames[framesRed]->create(mat_frame->rows, mat_frame->cols,CV_8UC(3));
                //mat_frame->copyTo(*frames[framesRed]);
                Pr1->render(mat_frame,LengthRend,frameHeightOr,frameWidthOr,portion,portion_ind);
                //IplImage cvBlframe(*mat_frame);
                cvShowImage("original1",cvframe_);
                portion_ind++;
                if(portion_ind>=portion){
                    portion_ind=0;
                }
                //if(framesRed>0){
                    //mat_frame->copyTo(*cvMatframe);
                //}
                //============================================================
                framesRed++;
                //currentPortion++;
                wait_ = 33-(int)tt.elapsed();
                //qDebug("wait=%d",wait_);
                if(wait_<=0){wait_=1;}
                char c=cvWaitKey(wait_);
                if(c==27){break;}
            }
            if(mode==2)
            {

                cvframe_=/*(Mat*)*/cvQueryFrame(capture);

                if(!cvframe_){cvDestroyWindow("original1");break;}
                //IplImage cvBlframe(*mat_frame);
                cvShowImage("original1",cvframe_);
                tt.start();
                //QTime tt;
                switch(stady)
                {
                case 0:
                    Pr1->FramesToVector(this->getBluredFrames(),Pr1->getAllFrames(),Pr1->getFrW(),Pr1->getFrH(),Pr1->getNFr());
                    //qDebug("FramesToVec, FRRed=%d",framesRed);
                    break;
                case 1:
                    //QTime tt1;
                    //tt1.start();
                    Pr1->normalize(Pr1->getAllFrames(),(long) Pr1->getNFr()*Pr1->getFrH()*Pr1->getFrW()*3,255.0);
                    Pr1->rgb2yiq(Pr1->getAllFrames(),Pr1->getFrW(),Pr1->getFrH(), Pr1->getNFr());
                    //qDebug("time elapsed: %d ms",tt1.elapsed());
                    break;
                case 2:
                    Pr1->InitFFT_IFFT_createFrMask(fLow,fHight);
                    break;
                case 3:
                    //tt.start();
                    for(int i=0; i<Pr1->getFrH()*Pr1->getFrW(); i++)
                    {
                        //fft:compute
                        Pr1->copyVector((double*)&Pr1->getAllFrames()[i*Pr1->getNFr()],Pr1->get_in_fft(),Pr1->getNFr());
                        fftw_execute(*Pr1->get_fft_plan());
                        //ifft:compute
                        Pr1->applyMask(Pr1->get_out_fft(),Pr1->get_in_ifft(),Pr1->get_mask(),Pr1->getNFr()/2+1,teor_freq);
                        fftw_execute(*Pr1->get_ifft_plan());
                        //Pr1->normalize(Pr1->get_out_ifft(), (long)Pr1->getNFr(),(double)Pr1->getNFr()/ampFactor);
                        Pr1->copyVector(Pr1->get_out_ifft(), &Pr1->getAllFrames()[i*Pr1->getNFr()],Pr1->getNFr());
                    }
                    //qDebug("(FFT)time elapsed: %d ms",tt.elapsed());
                    break;
                case 4:
                    //tt.start();
                    for(int i=Pr1->getFrH()*Pr1->getFrW(); i<Pr1->getFrH()*Pr1->getFrW()*2; i++)
                    {
                        //fft:compute
                        Pr1->copyVector((double*)&Pr1->getAllFrames()[i*Pr1->getNFr()],Pr1->get_in_fft(),Pr1->getNFr());
                        fftw_execute(*Pr1->get_fft_plan());
                        //ifft:compute
                        Pr1->applyMask(Pr1->get_out_fft(),Pr1->get_in_ifft(),Pr1->get_mask(),Pr1->getNFr()/2+1,teor_freq);
                        fftw_execute(*Pr1->get_ifft_plan());
                        //Pr1->normalize(Pr1->get_out_ifft(), (long)Pr1->getNFr(),(double)Pr1->getNFr()/ampFactor);
                        Pr1->copyVector(Pr1->get_out_ifft(), &Pr1->getAllFrames()[i*Pr1->getNFr()],Pr1->getNFr());
                    }
                    //qDebug("(FFT)time elapsed: %d ms",tt.elapsed());
                    break;
                case 5:
                    //tt.start();
                    for(int i=Pr1->getFrH()*Pr1->getFrW()*2; i<Pr1->getFrH()*Pr1->getFrW()*3; i++)
                    {
                        //fft:compute
                        Pr1->copyVector((double*)&Pr1->getAllFrames()[i*Pr1->getNFr()],Pr1->get_in_fft(),Pr1->getNFr());
                        fftw_execute(*Pr1->get_fft_plan());
                        //ifft:compute
                        Pr1->applyMask(Pr1->get_out_fft(),Pr1->get_in_ifft(),Pr1->get_mask(),Pr1->getNFr()/2+1,teor_freq);
                        fftw_execute(*Pr1->get_ifft_plan());
                        //Pr1->normalize(Pr1->get_out_ifft(), (long)Pr1->getNFr(),(double)Pr1->getNFr()/ampFactor);
                        Pr1->copyVector(Pr1->get_out_ifft(), &Pr1->getAllFrames()[i*Pr1->getNFr()],Pr1->getNFr());
                    }
                    //qDebug("(FFT)time elapsed: %d ms",tt.elapsed());
                    break;
                case 6:
                    Pr1->normalize(Pr1->getAllFrames(), (long)Pr1->getNFr()*Pr1->getFrH()*Pr1->getFrW()*3,(double)Pr1->getNFr()/ampFactor);
                    Pr1->ClearFFT_IFFT_mask();
                    break;
                case 7:
                    double rate_t; //int tmp_ind_val=0;
                    for(int ind=0; ind <portion/2+1; ind ++)
                    {
                        if(teor_freq[ind]>teor_freq[tmp_ind])
                        {
                            tmp_ind=ind;
                            qDebug("ind = %d",ind);
                        }
                    }
                    rate_t= Pr1->getRealMaskFr()[tmp_ind];
                    //free(Pr1->getRealMaskFr());             //FIXME TOPOR!
                    qDebug("Rate_t=%lf, tmp = %lf",rate_t*60.0,1.0/rate_t*60.0);
                    double rate;
                    Pr1->countPulse(&rate,ampFactor);
                    qDebug("Rate=%lf",1.0/rate*60.0);
                case 8:
                    Pr1->AllocRendBuff(LengthRend,Pr1->getNFr());
                    break;
                /*case 9:
                    for(int frame_number =0; frame_number <Pr1->getNFr();frame_number++)
                    {
                        Pr1->NearInterpolation2(Pr1->getAllFrames(),Pr1->get_pulse_frames(),Pr1->getFrW(),Pr1->getFrH(),frameWidthOr,frameHeightOr,Pr1->getNFr(),frame_number);
                    }
                    break;*/
                case 9:
                    //mode =1;
                    break;
                case 11:
                    break;
                }
                stady++;
                framesRed++;
                currentPortion++;
                wait_ = 33-tt.elapsed();
                if(wait_<=0){wait_=1;}
                char c=cvWaitKey(wait_);
                if(c==27){break;}
            }
        }
//qDebug("(mode%d)time elapsed: %d ms",mode,tt.elapsed());
    }
    //printf("counter=%d\n",Pr1->getFrCounter());
    cvDestroyWindow("original1");
    //fps= cvGetCaptureProperty(capture,CV_CAP_PROP_FPS);
    this->frameHeight=this->blured_frames[0]->rows;
    this->frameWidth=this->blured_frames[0]->cols;
    this->NumberOfFrames=framesRed-10; //throw out 10 last frames
    cvReleaseCapture(&capture);
    //qDebug("Noffr = %d, frH=%d, frW= %d",NumberOfFrames,frameHeight,frameWidth);
    //delete(Pr1);
}

double VideoReader::getfps(void)
{
    return(fps);
}

void VideoReader::tmpframereader(const char* videofilename_in)
{
    cvframe_=NULL;
    CvCapture* capture =cvCreateFileCapture(videofilename_in);
    int framesRed=0;
    int currentPortion=0;
    while(1)
    {
                cvframe_=cvQueryFrame(capture);
                if(!cvframe_){break;}
                frames[framesRed]= new Mat;
                Mat* mat_frame = new Mat(cvframe_);
                frames[framesRed]->create(mat_frame->rows, mat_frame->cols,CV_8UC(3));
                mat_frame->copyTo(*frames[framesRed]);
                framesRed++;
                currentPortion++;
    }
    cvReleaseCapture(&capture);
}


void VideoReader::CVWriteVideo(const char* videofilename_out)
{
    CvSize frSize;
    frSize.width=frames[0]->cols;
    frSize.height=frames[0]->rows;

    CvVideoWriter* writer = cvCreateVideoWriter("videofilename_out",
                                                CV_FOURCC('M','J','P','G'),
                                                fps,
                                                frSize);
    for(int i=0; i<NumberOfFrames; i++)
    {
        IplImage tmp(*frames[i]);
        cvWriteFrame(writer,&tmp);
    }
    cvReleaseVideoWriter(&writer);
}

void VideoReader::CVOutputVideo(void)
{
    cvNamedWindow("original",CV_WINDOW_AUTOSIZE);
    //cvframe_=NULL;
    for(int i=0; i<NumberOfFrames/*portion*/; i++)
    {
        IplImage cvframe(*frames[i]);
        cvShowImage("original",&cvframe);
        char c=cvWaitKey(33);
        if(c==27){break;}
    }

    cvDestroyWindow("original");
}

void VideoReader::createProcessor(processor* Pr1)
{
    //Pr1=new processor();
    Pr1->init(this->get_portion(),this->getFrameHeight(),this->getFrameWidth(), this->getfps());
    Pr1->FramesToVector(this->getBluredFrames(),Pr1->getAllFrames(),Pr1->getFrW(),Pr1->getFrH(),Pr1->getNFr());
    //processor* Pr1= new processor();
    //Pr1->init(Curr_video->get_portion(),Curr_video->getFrameHeight(),Curr_video->getFrameWidth(), Curr_video->getfps()/*,Curr_video->getBluredFrames()*/);
    /*QTime tt;
    tt.start();
    Pr1->FramesToVector(Curr_video->getBluredFrames(),Pr1->getAllFrames(),Pr1->getFrW(),Pr1->getFrH(),Pr1->getNFr());
    qDebug("time elapsed: %d ms",tt.elapsed());*/


}
