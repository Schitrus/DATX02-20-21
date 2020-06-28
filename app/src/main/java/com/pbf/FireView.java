package com.pbf;

import android.content.Context;
import android.hardware.Sensor;
import android.hardware.SensorManager;
import android.opengl.GLSurfaceView;
import android.widget.ImageView;

import java.util.Queue;
import java.util.concurrent.ConcurrentLinkedQueue;

public class FireView extends GLSurfaceView {

    public FireRenderer renderer;
    public FireListener listener;

    public FireView(Context context, ImageView loading, SensorManager sensorManager){
        super(context);
        setEGLConfigChooser(8, 8, 8, 0, 16, 0);
        setEGLContextClientVersion(3);

        // A queue used to send tasks from the listener to the renderer, to make sure that all calls to the native library happens on the same thread and with a gl-context
        Queue<Runnable> inputTaskQueue = new ConcurrentLinkedQueue<>();

        renderer = new FireRenderer(inputTaskQueue, context, loading);
        setRenderer(renderer);

        listener = new FireListener(inputTaskQueue, context, sensorManager);
        setOnTouchListener(listener);
        setOnClickListener(listener);
    }

}
