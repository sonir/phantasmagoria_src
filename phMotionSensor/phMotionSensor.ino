
//Set number of connected sensor
const int sensor_num = 1;
//Reflash rate (in MS)
const int reflesh_rate = 50;
//lowpass_length
const int lowpass_length = 10;
const float threshold = 0.97;

//buff for lowpass to avoid chuttering
float lowpass_buf[sensor_num][lowpass_length];
//Init the pin number of digital in
int sensors [] = {2,5,8,12,13}; //def 2,3,4,5,6
//Array for storage the get value
int sensors_val[sensor_num];
int tmp_sensors_val[sensor_num];



int lowpass(int sensor_id, float fVal){

	float subtotal=0.;
 	float avg = 0.;
	char s[16];
	char t[16];


	//Stack
	for(int i=(lowpass_length-1);i>=0;i--){

		if(i==0){ //If the index is 0, store the newest value

			lowpass_buf[sensor_id][i]=(int)fVal%(int)10.0f; //decode the value
			continue;

		}

		//Slide All values
		lowpass_buf[sensor_id][i] = lowpass_buf[sensor_id][i-1];
	}


	//Sum total
	for(int i=0;i<lowpass_length;i++){
	    subtotal+=lowpass_buf[sensor_id][i];

	 //  	int val = (int)100*lowpass_buf[sensor_id][i];
		// sprintf(t, "sensor[%d][%d] is  %d" , sensor_id, i, val);
		// Serial.println(t);


	}


	//Calc AVG
	avg = subtotal/(float)lowpass_length;
	// int avg_i = (int)100*avg;
	// sprintf(t, "Sensor[%d] : avg is  %d" , sensor_id, avg_i);
	// Serial.println(t);
	

	if(avg>=threshold || (1.0-avg)>=threshold){
	    // Serial.println("true");
	    return 1;
	}else{
	    // Serial.println("false");
	    return 0;
	}

}

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
      //if( tmp_sensors_val[i]^sensors_val[i] ){ //Check Difference

        //If the value was changed
        //sensors_val[i]=tmp_sensors_val[i]; //Store the lowpass_length value
        //Check is it chuttering or not with Lowpass
      	if( !lowpass(i,(float)tmp_sensors_val[i]) ){

      		// Serial.println("detect chuttering ignore.");

      	} else{

      		if( tmp_sensors_val[i]^sensors_val[i] ){
      	        sensors_val[i]=tmp_sensors_val[i]; //Store the lowpass_length value
		        Serial.println(sensors_val[i]); //Send the lowpass_length value to serial port 
		        delay(20);       
		
      		}

      	}

      //}
      
  }    
    

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

   		for(int j=0; j<lowpass_length; j++){

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
