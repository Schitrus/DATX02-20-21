package com.pbf;

import android.content.Context;
import android.hardware.Sensor;
import android.hardware.SensorManager;
import android.opengl.GLSurfaceView;

public class FireView extends GLSurfaceView {

    public FireRenderer renderer;
    public FireListener listener;

    public FireView(Context context, SensorManager sensorManager){
        super(context);
        init(context, sensorManager);
    }

    public void init(Context context, SensorManager sensorManager){

        setEGLConfigChooser(8, 8, 8, 0, 16, 0);
        setEGLContextClientVersion(3);

        renderer = new FireRenderer(context);
        setRenderer(renderer);

        listener = new FireListener(context, sensorManager);
        setOnTouchListener(listener);
    }

}
