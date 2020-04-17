package com.pbf;

import android.content.Context;
import android.hardware.Sensor;
import android.hardware.SensorEvent;
import android.hardware.SensorEventListener;
import android.hardware.SensorManager;
import android.opengl.GLSurfaceView;
import android.util.Log;
import android.view.MotionEvent;
import android.view.OrientationEventListener;
import android.view.OrientationListener;
import android.view.ScaleGestureDetector;
import android.view.View;

class FireListener implements GLSurfaceView.OnTouchListener {

    private double touchX, touchY, oldX, oldY;
    private float scaleFactor = 1.0f;
    private ScaleGestureDetector scaleDetector;
    private long lastScale = 0;
    private RotationSensorListener rotationSensorListener;
    private SensorManager sensorManager;
    private Sensor rotationVectorSensor;
    private final float[] orientationRotationMatrix = new float[9];

    public FireListener(Context context, SensorManager sensorManager){
        scaleDetector = new ScaleGestureDetector(context, new ScaleListener());
        rotationSensorListener = new RotationSensorListener();
        this.sensorManager = sensorManager;
        this.rotationVectorSensor = sensorManager.getDefaultSensor(Sensor.TYPE_GAME_ROTATION_VECTOR);
        if(rotationVectorSensor == null){
            // Rotation vector sensor not found
            Log.e("FireListener", "Rotation vector sensor not found");
        }

        // Register sensor listener for updates every 10ms = 10000us
        sensorManager.registerListener(rotationSensorListener, rotationVectorSensor, SensorManager.SENSOR_DELAY_GAME);
    }

    public boolean onTouch(View v, MotionEvent event){
        touchX = event.getX();
        touchY = event.getY();
        double deltaX = touchX - oldX;
        double deltaY = touchY - oldY;
        oldX = touchX;
        oldY = touchY;
        float oldscale = scaleFactor;
        scaleDetector.onTouchEvent(event);
        if (oldscale != scaleFactor)
            lastScale = scaleDetector.getEventTime();
        if(((lastScale + 100) < event.getEventTime()) && event.getAction() == MotionEvent.ACTION_MOVE)
            touch(deltaX, deltaY);

        return true;
    }

    public void onOrientationChanged(int orientation){

    }

    private class RotationSensorListener implements SensorEventListener {

        @Override
        public void onSensorChanged(SensorEvent sensorEvent) {
            // Double check if sensor is of correct type
            if(sensorEvent.sensor.getType() == Sensor.TYPE_GAME_ROTATION_VECTOR){
                SensorManager.getRotationMatrixFromVector(orientationRotationMatrix, sensorEvent.values);
                rotationSensor(orientationRotationMatrix);
            }
        }

        @Override
        public void onAccuracyChanged(Sensor sensor, int i) {
            // Unimplemented for now
        }
    }

    private class ScaleListener
            extends ScaleGestureDetector.SimpleOnScaleGestureListener {
        @Override
        public boolean onScale(ScaleGestureDetector detector) {
            scaleFactor *= detector.getScaleFactor();

            // Don't let the object get too small or too large.
            scaleFactor = Math.max(0.1f, scaleFactor);

            scale(scaleFactor, detector.getFocusX(), detector.getFocusY());
            return true;
        }
    }

    public native void scale(float scaleFactor, double scaleX, double scaleY);
    public native void touch(double dx, double dy);
    public native void rotationSensor(float[] rotationMatrix);

}
