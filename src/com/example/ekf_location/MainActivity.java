package com.example.ekf_location;

import android.support.v7.app.ActionBarActivity;
import android.util.Log;
import android.support.v7.app.ActionBar;
import android.support.v4.app.Fragment;
import android.os.Bundle;
import android.provider.Settings;
import android.view.LayoutInflater;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.view.ViewGroup;
import android.view.View.OnClickListener;
import android.widget.Toast;

//import com.javacodegeeks.android.locationservicetest.MainActivity.MyLocationListener;

import android.content.Context;
import android.content.Intent;
import android.hardware.Sensor;
import android.hardware.SensorEvent;
import android.hardware.SensorEventListener;
import android.hardware.SensorManager;
import android.hardware.Sensor;
import android.hardware.SensorManager;

import android.location.Criteria;
import android.location.Location;
import android.location.LocationListener;
import android.location.LocationManager;


import java.io.BufferedReader;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.OutputStreamWriter;
import java.sql.Date;
import java.text.SimpleDateFormat;

import android.content.Context;
import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.File;
import java.io.FileReader;
import java.io.FileWriter;
import java.io.IOException;

import java.*;


public class MainActivity extends ActionBarActivity 
implements LocationListener,SensorEventListener{

	static{
		System.loadLibrary("EKF_Location");
	}
	// GPS Manager
	private LocationManager locationManager;

	// Sensor manager
	private SensorManager mSensorManager;
	private String provider;
	private Sensor mAccelerometer,mGyrometer,mMagnetometer;
	private Criteria criteria;

	// Files manager
	private File fileTelemetry;
	FileWriter fw;
	BufferedWriter bw;
	Boolean bisOpen = false;

	native private void sensorEvent(float x,float y,float z,int iTypeSensor,float f_dt);

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_main);
		/*if (savedInstanceState == null) {
			getSupportFragmentManager().beginTransaction().add(R.id.container, new PlaceholderFragment()).commit();
		}*/

		mSensorManager 	= (SensorManager) getSystemService(SENSOR_SERVICE);
		mAccelerometer 	= mSensorManager.getDefaultSensor(Sensor.TYPE_ACCELEROMETER);
		mGyrometer		= mSensorManager.getDefaultSensor(Sensor.TYPE_GYROSCOPE);
		mMagnetometer	= mSensorManager.getDefaultSensor(Sensor.TYPE_MAGNETIC_FIELD);

		mSensorManager.registerListener(this, mAccelerometer, SensorManager.SENSOR_DELAY_FASTEST);
		mSensorManager.registerListener(this, mGyrometer	, SensorManager.SENSOR_DELAY_FASTEST);
		mSensorManager.registerListener(this, mMagnetometer	, SensorManager.SENSOR_DELAY_FASTEST);

		// Get the location manager
		locationManager = (LocationManager) getSystemService(Context.LOCATION_SERVICE);
		// Define the criteria how to select the location provider
		criteria = new Criteria();
		criteria.setAccuracy(Criteria.ACCURACY_COARSE);	//default

		// user defines the criteria
		criteria.setCostAllowed(false); 
		// get the best provider depending on the criteria
		provider = locationManager.getBestProvider(criteria, false);

		// the last known location of this provider
		Location location = locationManager.getLastKnownLocation(provider);

		if (location != null) {
			onLocationChanged(location);
		} else {
			// leads to the settings because there is no last known location
			Intent intent = new Intent(Settings.ACTION_LOCATION_SOURCE_SETTINGS);
			startActivity(intent);
		}
		// location updates: at least 1 meter and 200millsecs change
		locationManager.requestLocationUpdates(provider, 0, 0, this);


		// Create the file where the telemetry will be stored
		try {
			SimpleDateFormat sdf = new SimpleDateFormat("yyyy-MM-dd-HH:mm:ss");
			String fpath = "/sdcard/"+sdf.format(new Date(System.currentTimeMillis()))+".txt";
			
			Log.i("DATA_LOCATION ",fpath);
	
			File fileTelemetry = new File(fpath);

			// If file does not exists, then create it
			if (!fileTelemetry.exists()) {
				fileTelemetry.createNewFile();
			}

			fw = new FileWriter(fileTelemetry.getAbsoluteFile());
			bw = new BufferedWriter(fw);

			bisOpen = true;
		} catch (IOException e) {
			e.printStackTrace();
			return;
		}
	}

	@Override
	public boolean onCreateOptionsMenu(Menu menu) {
		// Inflate the menu; this adds items to the action bar if it is present.
		getMenuInflater().inflate(R.menu.main, menu);
		return true;
	}

	@Override
	public boolean onOptionsItemSelected(MenuItem item) {
		// Handle action bar item clicks here. The action bar will
		// automatically handle clicks on the Home/Up button, so long
		// as you specify a parent activity in AndroidManifest.xml.
		int id = item.getItemId();
		if (id == R.id.action_settings) {
			return true;
		}
		return super.onOptionsItemSelected(item);
	}

	long starttime2;
	public void onLocationChanged(Location location) {

		long CurrentMilisecond =  System.currentTimeMillis();
		String ValToWrite = "Timer:" + String.valueOf(CurrentMilisecond);
		ValToWrite += ";Latitude:"+String.valueOf(location.getLatitude());
		ValToWrite += ";Longitude:"+String.valueOf(location.getLongitude());
		ValToWrite += ";Altitude:"+String.valueOf(location.getAltitude());
		ValToWrite += ";Speed:"+ String.valueOf(location.getSpeed());
		ValToWrite += ";Bearing:"+ String.valueOf(location.getBearing());
		ValToWrite += ";Accuraccy:"+ String.valueOf(location.getAccuracy());
		ValToWrite += "\n";
		
	
		//long millis = tmp - starttime2;
		//Log.i("DATA_MEM ",String.format("millis %f ",millis/1000.0));
		//starttime2 = tmp;
		/*Toast.makeText(MainActivity.this,  "Location changed!",
				Toast.LENGTH_SHORT).show();*/
		//Log.i("DATA_LOCATION ","Latitude: "+String.valueOf(location.getLatitude())+" Longitude: "+String.valueOf(location.getLongitude()));

		try {
			if(bisOpen)
			{
			bw.write(ValToWrite);
			bw.flush();
			}
		} catch (IOException e) {
			e.printStackTrace();
			return;
		}
	}

	@Override
	public void onStatusChanged(String provider, int status, Bundle extras) {
		/*Toast.makeText(MainActivity.this, provider + "'s status changed to "+status +"!",
				Toast.LENGTH_SHORT).show();*/
	}

	@Override
	public void onProviderEnabled(String provider) {
		/*Toast.makeText(MainActivity.this, "Provider " + provider + " enabled!",
				Toast.LENGTH_SHORT).show();*/
	}

	@Override
	public void onProviderDisabled(String provider) {
		/*Toast.makeText(MainActivity.this, "Provider " + provider + " disabled!",
				Toast.LENGTH_SHORT).show();*/
	}

	long starttime;
	public void onSensorChanged(SensorEvent event) {

		if (event.sensor.getType() == Sensor.TYPE_ACCELEROMETER)
		{ 
			long CurrentMilisecond =  System.currentTimeMillis();
			String ValToWrite = "Timer:" + String.valueOf(CurrentMilisecond);
			ValToWrite += ";AccX:"+String.valueOf(event.values[0]);
			ValToWrite += ";AccY:"+String.valueOf(event.values[1]);
			ValToWrite += ";AccZ:"+String.valueOf(event.values[2]);
			ValToWrite += "\n";
			try {
				if(bisOpen)
				{
				bw.write(ValToWrite);
				bw.flush();
				}
			} catch (IOException e) {
				e.printStackTrace();
				return;
			}
			//			Log.i("DATA_MEM ",String.format("Acc x: %.2f",event.values[0])+String.format(";y: %.2f",event.values[1])+String.format(";z: %.2f",event.values[2]) );
		}

		if (event.sensor.getType() == Sensor.TYPE_GYROSCOPE)
		{ 
			long CurrentMilisecond =  System.currentTimeMillis();
			String ValToWrite = "Timer:" + String.valueOf(CurrentMilisecond);
			ValToWrite += ";GyroX:"+String.valueOf(event.values[0]);
			ValToWrite += ";GyroY:"+String.valueOf(event.values[1]);
			ValToWrite += ";GyroZ:"+String.valueOf(event.values[2]);
			ValToWrite += "\n";
			try {
				if(bisOpen)
				{
				bw.write(ValToWrite);
				bw.flush();
				}
			} catch (IOException e) {
				e.printStackTrace();
				return;
			}
			
			/*long tmp =  System.currentTimeMillis();
			long millis = tmp - starttime;
			Log.i("DATA_MEM ",String.format("millis %f ",millis/1000.0));

			starttime = tmp;
			sensorEvent(event.values[0],event.values[1],event.values[2],1,(float)(millis/1000.0));
			 */	//			Log.i("DATA_MEM ",String.format("Gyro x: %.2f",event.values[0])+String.format(";y: %.2f",event.values[1])+String.format(";z: %.2f",event.values[2]) );
		}


		if (event.sensor.getType() == Sensor.TYPE_MAGNETIC_FIELD)
		{ 
			long CurrentMilisecond =  System.currentTimeMillis();
			String ValToWrite = "Timer:" + String.valueOf(CurrentMilisecond);
			ValToWrite += ";MagX:"+String.valueOf(event.values[0]);
			ValToWrite += ";MagY:"+String.valueOf(event.values[1]);
			ValToWrite += ";MagZ:"+String.valueOf(event.values[2]);
			ValToWrite += "\n";
			try {
				if(bisOpen)
				{
				bw.write(ValToWrite);
				bw.flush();
				}
			} catch (IOException e) {
				e.printStackTrace();
				return;
			}
			
			//sensorEvent(event.values[0],event.values[1],event.values[2],0);
			//Log.i("DATA_MEM ",String.format(" Mag %.2f %.2f %.2f",event.values[0],event.values[1],event.values[2]));

			//Log.i("DATA_MEM ",String.format("Mag x: %.2f",event.values[0])+String.format(";y: %.2f",event.values[1])+String.format(";z: %.2f",event.values[2]) );
			/*float Tan_Psi = (float)((event.values[2]*Math.sin(0) - event.values[1]*Math.cos(0))/(  event.values[0]*Math.cos(0) +  event.values[1]*Math.sin(0)*Math.sin(0) + event.values[2]*Math.sin(0)*Math.cos(0)));
			float Psi = (float)Math.atan(Tan_Psi);
			float Y = (float)(event.values[2]*Math.sin(0) - event.values[1]*Math.cos(0));
			float X =  (float)( event.values[0]*Math.cos(0) +  event.values[1]*Math.sin(0)*Math.sin(0) + event.values[2]*Math.sin(0)*Math.cos(0));
			float Psi2 = (float)Math.atan2(Y ,X );

			float incliaison1 =(float)(Math.acos(event.values[0]/Math.cos(Psi)*Math.PI/180)*180/Math.PI);
			float incliaison2 =(float)(Math.acos(Math.abs(event.values[0])/(Math.cos(Psi))*Math.PI/180)*180/Math.PI);
			Log.i("DATA_MEM ",String.format(" Psi2 : %.2f, Inclianaison %.2f %.2f  Mag %.2f %.2f %.2f",Psi2*180/Math.PI,incliaison1,incliaison2,event.values[0],event.values[1],event.values[2]));
			 */
		}

		return;
	}


	@Override
	public void onAccuracyChanged(Sensor sensor, int accuracy) {

	}


}
