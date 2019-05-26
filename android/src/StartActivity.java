package com.raulrita.neurojam;

import android.app.Activity;
import android.view.View;
import android.view.*;
import android.widget.*;
import android.view.ViewGroup.*;
import android.content.Intent;
import android.os.Build;
import android.os.Bundle;

import android.app.NativeActivity;
import android.content.pm.ActivityInfo;

import android.util.Log;

public class StartActivity extends android.app.NativeActivity
{
	@Override
	protected void onCreate(Bundle savedInstanceState) 
	{
		getWindow().getDecorView().setSystemUiVisibility(
				View.SYSTEM_UI_FLAG_LAYOUT_STABLE
				| View.SYSTEM_UI_FLAG_LAYOUT_HIDE_NAVIGATION
				| View.SYSTEM_UI_FLAG_LAYOUT_FULLSCREEN
				| View.SYSTEM_UI_FLAG_HIDE_NAVIGATION
				| View.SYSTEM_UI_FLAG_FULLSCREEN
				| View.SYSTEM_UI_FLAG_IMMERSIVE_STICKY);			

		getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);		
		
		super.onCreate(savedInstanceState);

        setRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_PORTRAIT);

        System.loadLibrary("neurojam");
	}
}