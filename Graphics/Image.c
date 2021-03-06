//
//  Image.c
//  Graphics
//
//  Created by jr2339 on 10/18/16.
//  Copyright © 2016 jr2339. All rights reserved.
//

#include "Image.h"
int readMagicNumber(FILE *fp) {
    int magic_number;
    if (!fscanf(fp, "P%d\n", &magic_number)){
        fprintf(stderr,"file is not in PPM format, we can't read it");
        exit(1);
    }
    return magic_number;
}
/**************************************************************************************************************
 **************************************************************************************************************/
void skip_comments(FILE *f_source){
    char ch;
    /*skip the comments*/
    ch = getc(f_source); // ch is int
    /*
     gets the next character (an unsigned char) from the specified
     stream and advances the position indicator for the stream.
     */
    while(ch =='#'){
        do{
            ch=getc(f_source);
        }
        while(ch!='\n'); //read to the end of the line
        ch=getc(f_source);
    }
    if(!isdigit(ch)){
        fprintf(stderr,"can't read header information from PPM format\n");
    }
    else{
        ungetc(ch, f_source); //put that digital back
    }
}
/**************************************************************************************************************
 **************************************************************************************************************/
// readPPMHeader help us to detemine which format is our source image

long int readHeader(FILE *f_source, Buffer *buffer, int *source_width,int *source_height,int Ratio){
    
    //int source_width,source_height,
    int source_maxval;
    skip_comments(f_source);
    //read the width, height,amd maximum value for a pixel
    fscanf(f_source, "%d %d %d ",source_width,source_height,&source_maxval);
    
    buffer->width = *source_width/Ratio;
    buffer->height = *source_height/Ratio;
    buffer->maxval = source_maxval;
    
    if(buffer->maxval >= 65336 ||buffer->maxval <= 0){
        fprintf(stderr,"image is not ture-color(8byte), read failed\n");
        exit(1);
    }
    return ftell(f_source);
}

/**************************************************************************************************************
 **************************************************************************************************************/

Buffer *ImageRead(const char *filename,int Ratio,int max){
    time_t t;
    srand((unsigned)time(&t));
    
    
    int span = rand()%max;
    int index = span;
    
    
    FILE *f_source = fopen(filename,"r");
    if(!f_source){
        fprintf(stderr,"can't open file for reading \n");
        exit(1);
    }
    
    int source_width,source_height;
    
    Buffer *buffer = (Buffer *)malloc(sizeof(Buffer));
    
    if(!buffer){
        fprintf(stderr,"Can't allocate memory for new image");
        exit(1);
    }
    buffer->magic_number = readMagicNumber(f_source);
    long int header_length = readHeader(f_source,buffer,&source_width,&source_height,Ratio);
    int size = source_width*source_height;
    buffer->box = (Box *)malloc(sizeof(Box)*size);
    *buffer->box = (Box){};
    
    if(buffer->magic_number==6){
        unsigned char c ;//init
       
        fseek(f_source,3*index, SEEK_CUR);
        while (index < size) {     //Dr.plamer tola me should be the origianl size of image
            int x = ((index%source_width)/(Ratio));  //col_index in buffer
            int y = ((index/source_width)/(Ratio)); // row_index in buffer
            
            //printf("%d\n",index);
            
            fread(&c, 1, 1, f_source);
            buffer->box[y*buffer->width+x].r += c;
            fread(&c, 1, 1, f_source);
            buffer->box[y*buffer->width+x].g += c;
            fread(&c, 1, 1, f_source);
            buffer->box[y*buffer->width+x].b += c;
            
            buffer->box[y*buffer->width+x].count++;
            
            
            span = rand()%max;
            fseek(f_source,3*span, SEEK_CUR);
            
            index += 1+span;
   
        }
        
    }
    else{
        fprintf(stderr,"image format is not correct");
        exit(1);
    }
    
    int buffer_size = buffer->width*buffer->height;
    int sum =0;
    for (int i=0; i<buffer_size; i++) {
        
        if (buffer->box[i].count != 0) {
            buffer->box[i].r = (buffer->box[i].r)/(buffer->box[i].count);
            buffer->box[i].g = (buffer->box[i].g)/(buffer->box[i].count);
            buffer->box[i].b = (buffer->box[i].b)/(buffer->box[i].count);
        }
        else{
            //if we didn't simple at this box, we choose the first one.
            unsigned char c ;//init
            int x = (i%buffer->width)*Ratio;  //col_index in buffer
            int y = (i/buffer->width)*Ratio; // row_index in buffer
            fseek(f_source, header_length+(y*source_width+x)*3, SEEK_SET);
            fread(&c, 1, 1, f_source);
            buffer->box[i].r = c;
            fread(&c, 1, 1, f_source);
            buffer->box[i].g = c;
            fread(&c, 1, 1, f_source);
            buffer->box[i].b = c;
        }
        sum +=buffer->box[i].count;
    }
    //double ave_count = (double)sum/buffer_size;
    //printf("the average count is %f\n",ave_count);
    return buffer;
}
/**************************************************************************************************************
 **************************************************************************************************************/
double ave(const char *filename,int Ratio,int max){
    double ave_count =0;
   
    time_t t;
    srand((unsigned)time(&t));
    
    
    int span = rand()%max;
    int index = span;
    
    
    FILE *f_source = fopen(filename,"r");
    if(!f_source){
        fprintf(stderr,"can't open file for reading \n");
        exit(1);
    }
    
    int source_width,source_height;
    
    Buffer *buffer = (Buffer *)malloc(sizeof(Buffer));
    
    if(!buffer){
        fprintf(stderr,"Can't allocate memory for new image");
        exit(1);
    }
    buffer->magic_number = readMagicNumber(f_source);
    long int header_length = readHeader(f_source,buffer,&source_width,&source_height,Ratio);
    int size = source_width*source_height;
    buffer->box = (Box *)malloc(sizeof(Box)*size);
    *buffer->box = (Box){};
    
    if(buffer->magic_number==6){
        unsigned char c ;//init
        
        fseek(f_source,3*index, SEEK_CUR);
        while (index < size) {     //Dr.plamer tola me should be the origianl size of image
            int x = ((index%source_width)/(Ratio));  //col_index in buffer
            int y = ((index/source_width)/(Ratio)); // row_index in buffer
            
            //printf("%d\n",index);
            
            fread(&c, 1, 1, f_source);
            buffer->box[y*buffer->width+x].r += c;
            fread(&c, 1, 1, f_source);
            buffer->box[y*buffer->width+x].g += c;
            fread(&c, 1, 1, f_source);
            buffer->box[y*buffer->width+x].b += c;
            
            buffer->box[y*buffer->width+x].count++;
            
            
            span = rand()%max;
            fseek(f_source,3*span, SEEK_CUR);
            
            index += 1+span;
            
        }
        
    }
    else{
        fprintf(stderr,"image format is not correct");
        exit(1);
    }
    
    int buffer_size = buffer->width*buffer->height;
    int sum =0;
    for (int i=0; i<buffer_size; i++) {
        
        if (buffer->box[i].count != 0) {
            buffer->box[i].r = (buffer->box[i].r)/(buffer->box[i].count);
            buffer->box[i].g = (buffer->box[i].g)/(buffer->box[i].count);
            buffer->box[i].b = (buffer->box[i].b)/(buffer->box[i].count);
        }
        else{
            //if we didn't simple at this box, we choose the first one.
            unsigned char c ;//init
            int x = (i%buffer->width)*Ratio;  //col_index in buffer
            int y = (i/buffer->width)*Ratio; // row_index in buffer
            fseek(f_source, header_length+(y*source_width+x)*3, SEEK_SET);
            fread(&c, 1, 1, f_source);
            buffer->box[i].r = c;
            fread(&c, 1, 1, f_source);
            buffer->box[i].g = c;
            fread(&c, 1, 1, f_source);
            buffer->box[i].b = c;
        }
        sum +=buffer->box[i].count;
    }
    ave_count = (double)sum/buffer_size;
    //printf("==========the average count is %f==========\n",ave_count);
    return ave_count;
    
}
/**************************************************************************************************************
 **************************************************************************************************************/
void ImageWrite(Buffer *buffer, const char *filename){
    int size = buffer->width * buffer->height;
    //printf("buffer width is %d, buffer height is %d\n",buffer->width,buffer->height);
    FILE *f_des = fopen(filename, "w");
    if (!f_des){
        fprintf(stderr,"cannot open file for writing");
    }
    unsigned char ch;
    
    fprintf(f_des, "P%d\n%d %d\n%d\n",6,buffer->width, buffer->height, buffer->maxval);
    for(int i=0; i<size;i++){
        ch=buffer->box[i].r;
        fwrite(&ch, 1, 1, f_des);
        ch=buffer->box[i].g;
        fwrite(&ch, 1, 1, f_des);
        ch=buffer->box[i].b;
        fwrite(&ch, 1, 1, f_des);
    }
    
    fclose(f_des);
    
    
}





