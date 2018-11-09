#include <string.h>
#include <math.h>
#include <complex.h>
#include <liquid/liquid.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <unistd.h>
#include "fann.h"
#include "float.h"
//gcc -lfann -o hook hook.c
int gyro_wrist_output;
int gyro_arm_output;
int accel_arm_output;
int gyro_wrist_input;
int gyro_arm_input;
int accel_arm_input;

enum {
        START,
        TRAIN,
        READY,
        INPUT,
        COMPUTATION,
        TEST
} state;

void data()
{
	//need the directory: Synchronized_Dual_Sensor_Data_Acquisition
	//use the Acquire_LowPass_Continuous_1, Acquire_LowPass_Continuous_2
	//generate motion_data_output_1.csv,motion_data_output_2.csv
	system("./Dual_SensorTile_Acquire.sh -t 1");
}

////////////////////////////////////////////////////////////////////////



void train_wrist_gyro()
{
    const unsigned int num_input = 1;
    const unsigned int num_output = 2;
    const unsigned int num_layers = 3;
    const unsigned int num_neurons_hidden = 7;
    const float desired_error = (const float) 0.0001;
    const unsigned int max_epochs = 5000;
    const unsigned int epochs_between_reports = 100;

    struct fann *ann = fann_create_standard(num_layers, num_input,
        num_neurons_hidden, num_output);

    fann_set_activation_function_hidden(ann, FANN_SIGMOID_SYMMETRIC);
    fann_set_activation_function_output(ann, FANN_SIGMOID_SYMMETRIC);

    fann_train_on_file(ann, "train_data_wrist_gyro.txt", max_epochs,
        epochs_between_reports, desired_error);

    fann_save(ann, "TEST_wrist_gyro.net");
    fann_destroy(ann);
}

void test_wrist_gyro()
{
    int i;
    int direction;
    float max;
    fann_type *calc_out;
    fann_type input[1];
    struct fann *ann2;
    ann2 = fann_create_from_file("TEST_wrist_gyro.net");
    
    max = -100;
    input[0] = gyro_wrist_input;
    calc_out = fann_run(ann2, input);
    for (i = 0; i < 2; i++) {
        if (calc_out[i] > max) {
            max = calc_out[i];
            direction = i;
        }
    }
    direction=direction+1;
    if(direction==1)
    {
        gyro_wrist_output=1;
        //printf("You correctly flipped the wrist!\n");
    }
    else if(direction==2)
    {
        gyro_wrist_output=0;
        //printf("You did not flip your wrist!\n");
    }
    fann_destroy(ann2);
}
void train_arm_gyro()
{
    const unsigned int num_input = 1;
    const unsigned int num_output = 2;
    const unsigned int num_layers = 3;
    const unsigned int num_neurons_hidden = 7;
    const float desired_error = (const float) 0.0001;
    const unsigned int max_epochs = 5000;
    const unsigned int epochs_between_reports = 100;

    struct fann *ann = fann_create_standard(num_layers, num_input,
        num_neurons_hidden, num_output);

    fann_set_activation_function_hidden(ann, FANN_SIGMOID_SYMMETRIC);
    fann_set_activation_function_output(ann, FANN_SIGMOID_SYMMETRIC);

    fann_train_on_file(ann, "train_data_arm_gyro.txt", max_epochs,
        epochs_between_reports, desired_error);

    fann_save(ann, "TEST_arm_gyro.net");
    fann_destroy(ann);

}
void test_arm_gyro()
{
    
    int i;
    int direction;
    float max;
    fann_type *calc_out;
    fann_type input[1];
    struct fann *ann2;
    ann2 = fann_create_from_file("TEST_arm_gyro.net");
    max = -100;

    input[0] = gyro_arm_input;

    calc_out = fann_run(ann2, input);
    for (i = 0; i < 2; i++) {
        if (calc_out[i] > max) {
            max = calc_out[i];
            direction = i;
        }
    }
    direction=direction+1;
    if(direction==1)
    {
        gyro_arm_output=1;
        //printf("You are correct!\n");
    }
    else if(direction==2)
    {
        gyro_arm_output=0;
        //printf("You rotate your arm!\n");
    }
    
    fann_destroy(ann2);
}
void test_arm_accel()
{
    if(accel_arm_input >1800)
    {
        accel_arm_output=2;
        //printf("You move too fast!\n");
    }
    else if(accel_arm_input <1300)
    {
        accel_arm_output=0;
        //printf("You move too slow!\n");
    }
    else
    {
        accel_arm_output=1;
        //printf("You are correct!\n");
    }
}
void conclusion()
{
        if(gyro_wrist_output==1)
        {
            printf("You correctly flipped the wrist!\n");
        }
        else if(gyro_wrist_output==0)
        {
            printf("You did not flip your wrist!\n");
        }
        if(gyro_arm_output==1)
        {
            printf("You lift your arm correctly!\n");
        }
        else if(gyro_arm_output==0)
        {
            printf("You should lift your arm straightly!\n");
        }
        if(accel_arm_output==1)
        {
            printf("You are correct in moving your arm!\n");
        }
        else if(accel_arm_output==2)
        {
            printf("You move too fast in moving your arm!\n");
        }
        else if(accel_arm_output==0)
        {
            printf("You move too slow in moving your arm!\n");
        }
    printf("%d\n",accel_arm_input);
    printf("%d\n",gyro_arm_input);
    printf("%d\n",gyro_wrist_input);
}
void test()
{
    /*
        printf("Enter wrist_gyro, arm_gyro,arm_accel input\n");
        printf("Use space to seperate each input\n");
        scanf("%d %d %d", &accel_arm_input, &gyro_wrist_input,&gyro_arm_input);
    */
        gyro_wrist_input=gyro_wrist_input+7500;
        gyro_arm_input=gyro_arm_input-3000;
        test_wrist_gyro();
        test_arm_gyro();
        test_arm_accel();
        conclusion();
        sleep(1);
}
////////////////////////////////////////////////////////////////////////
float readcsvMax(int file,int position)
{
    //file refers to the file to be read
    //position refers to column number start from 0
    //the first row will be omitted.
    float number;
    float max=-FLT_MAX;
    FILE *fp = NULL; 

    if(file==1)
    {fp = fopen("motion_data_output_1.csv", "at+");}
    else if(file==2)
    {fp = fopen("motion_data_output_2.csv", "at+");}

    char *record;
    char *line;  
    char buffer[1024];  
    if (fp != NULL)  
    {  
        //fseek(fp, 170L, SEEK_SET); 
        char delims[] = ",";  
        char *result = NULL;  
        int j = 0; 
        line = fgets(buffer, sizeof(buffer), fp);
        while ((line = fgets(buffer, sizeof(buffer), fp))!=NULL)  
        {  

            record = strtok(line, ",");  
            number = atof(record);
            while (record != NULL)
            {    
                if (j == position)
                {
                    if(number>max)
                    {
                    max=number;  
                    } 
                    break;
                }
                record = strtok(NULL, ",");  
                number = atof(record);
                j++; 
            }  
            j = 0;  
        }  
        fclose(fp);  
        fp = NULL;  
    }
    return max;  
}

float readcsvMin(int file,int position)
{
    //file refers to the file to be read
    //position refers to column number start from 0
    //the first row will be omitted.
    float number;
    float min=FLT_MAX;
    FILE *fp = NULL; 

    if(file==1)
    {fp = fopen("motion_data_output_1.csv", "at+");}
    else if(file==2)
    {fp = fopen("motion_data_output_2.csv", "at+");}

    char *record;
    char *line;  
    char buffer[1024];  
    if (fp != NULL)  
    {  
        //fseek(fp, 170L, SEEK_SET); 
        char delims[] = ",";  
        char *result = NULL;  
        int j = 0; 
        line = fgets(buffer, sizeof(buffer), fp);
        while ((line = fgets(buffer, sizeof(buffer), fp))!=NULL)  
        {  

            record = strtok(line, ",");  
            number = atof(record);
            while (record != NULL)
            {    
                if (j == position)
                {
                    if(number<min)
                    {
                    min=number;  
                    } 
                    break;
                }
                record = strtok(NULL, ",");  
                number = atof(record);
                j++; 
            }  
            j = 0;  
        }  
        fclose(fp);  
        fp = NULL;  
    }
    return min;  
}

////////////////////////////////////////////////////////////////////////

int main(void)
{

    state = READY;

    int run=1;

    while(run==1){
        switch (state) {
            case START:
                fprintf(stdout,"Start of program, ready to train.\n");
                state = TRAIN;
                break;
            case TRAIN:
                train_arm_gyro();
                train_wrist_gyro();
                state = READY;
                break;
            case READY:
                fprintf(stdout,"Get ready to input a new motion for classidication in 5 seconds\n");
                sleep(5);
                state=INPUT;
                break;
            case INPUT:
                data();
                state=COMPUTATION;
                break;
            case COMPUTATION:
                accel_arm_input=readcsvMax(2,2);
                gyro_arm_input=readcsvMax(2,6);
                gyro_wrist_input=readcsvMin(1,4);
                state=TEST;
                break;
            case TEST:
                test();
                run--;
                break;

        }     
                 
    }

    
    return 0;
}
