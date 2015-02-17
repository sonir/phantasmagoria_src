
//Set number of connected sensor
const int sensor_num = 5;
//Reflash rate (in MS)
const int reflesh_rate = 50;
//lowpass_freq
const int lowpass_freq = 1;

//buff for lowpass to avoid chuttering
float lowpass_buf[sensor_num][lowpass_freq];
//Init the pin number of digital in
int sensors [] = {2,5,8,12,13}; //def 2,3,4,5,6
//Array for storage the get value
int sensors_val[sensor_num];
int tmp_sensors_val[sensor_num];


void getParams()
{
  for(int i=0; i<sensor_num; i++)
  {
     //Storage the get value
     int got_val= digitalRead(sensors[i]);     
    //Invert the val
    got_val = 1-got_val;
    //Encode the val for serial ( index+ x 10 = place ID , x 1 = sensor value)
    tmp_sensors_val[i]  = ( (i+1)*10 )+got_val;    
  }
}

void sendParams()
{
  
  for(int i=0;i<sensor_num; i++)
  {
      if( tmp_sensors_val[i]^sensors_val[i] ){ //Check Difference

        //If the value was changed
        sensors_val[i]=tmp_sensors_val[i]; //Store the new value
        //Check is it chuttering or not with Lowpass
      	float chuttering_chk = lowpass(i,tmp_sensors_val[i]);

      	if( chuttering_chk!=1.0 || chuttering_chk!=0.0){
      		Serial.println("detect chuttering ignore.");
      		continue;
      	}

        Serial.println(sensors_val[i]); //Send the new value to serial port 
        delay(20);       
      }
      
  }    
    
   

}


float lowpass (int sensor_id , int iVal){

	// float *pBuf = lowpass_buf[sensor_id];
	float sub_total = 0.;

	// //Reflesh buff
	// for(int i=lowpass_freq; i>0; i--){

	// 	pBuf[i]=pBuf[i-1];
	// 	sub_total += pBuf[i]; //Add all value for calc avg

	// }

	// pBuf[0] = (float)iVal;
	// sub_total += pBuf[0];


	//Reflesh buff
	for(int i=lowpass_freq; i>0; i--){

		lowpass_buf[sensor_id][i] = lowpass_buf[sensor_id][i-1];
		sub_total += lowpass_buf[sensor_id][i]; //Add all value for calc avg

	}

	lowpass_buf[sensor_id][0] = (float)iVal;
	sub_total += lowpass_buf[sensor_id][0];


	//Calc AVG
	return (sub_total/(float)sensor_id);




}


void setup()
{
   //Start Serial Connection
   Serial.begin(9600);
   
   //Setup Pin modes
   for(int i=0; i<sensor_num; i++){

     pinMode(sensors[i],INPUT);
        
   }
   
   //Init Sensors Val
   for(int i=0; i<sensor_num; i++){
   
     sensors_val[i]=0;
     tmp_sensors_val[i]=0;
     
   }

   //Init Lowpass buffer
   for(int i=0; i<sensor_num;i++){

   		for(int j=0; j<lowpass_freq; j++){

   			lowpass_buf[i][j]=0.;

   		}

   }
     
  
}

void loop()
{  
  getParams();
  sendParams();
  delay(reflesh_rate); 
}
