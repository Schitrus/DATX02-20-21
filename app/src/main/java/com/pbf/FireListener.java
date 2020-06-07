package com.pbf;

import android.content.Context;
import android.hardware.Sensor;
import android.hardware.SensorEvent;
import android.hardware.SensorEventListener;
import android.hardware.SensorManager;
import android.opengl.GLSurfaceView;
import android.util.Log;
import android.view.MotionEvent;
import android.view.ScaleGestureDetector;
import android.view.View;

import java.util.Queue;

class FireListener extends ScaleGestureDetector.SimpleOnScaleGestureListener implements View.OnTouchListener, View.OnClickListener {

    private final Queue<Runnable> taskQueue;
    private double oldXTouch, oldYTouch;
    private float scaleFactor = 1.0f;
    private ScaleGestureDetector scaleDetector;
    private long lastScale = 0;
    private long lastClick = 0;

    private RotationSensorListener rotationSensorListener;
    private SensorManager sensorManager;
    private Sensor rotationVectorSensor;
    private final float[] orientationRotationMatrix = new float[9];


    FireListener(Queue<Runnable> taskQueue, Context context, SensorManager sensorManager) {
        this.taskQueue = taskQueue;
        scaleDetector = new ScaleGestureDetector(context, this);
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

    @Override
    public boolean onTouch(View v, MotionEvent event) {
        final double touchX = event.getX();
        final double touchY = event.getY();
        final double deltaX = touchX - oldXTouch;
        final double deltaY = touchY - oldYTouch;
        oldXTouch = touchX;
        oldYTouch = touchY;
        float oldScale = scaleFactor;
        scaleDetector.onTouchEvent(event);
        if (oldScale != scaleFactor)
            lastScale = scaleDetector.getEventTime();
        if(((lastScale + 100) < event.getEventTime()) && event.getAction() == MotionEvent.ACTION_MOVE) {
            taskQueue.add(new Runnable() {  //functional interfaces apparently require a minimum of sdk 24, and thus aren't available with a minimum of sdk 21
                @Override
                public void run() {
                    touch(touchX, touchY, deltaX, deltaY);
                }
            });
        }

        //if(deltaX == 0.0 && deltaY == 0.0)
        //    v.performClick();

        return true;
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

    @Override
    public void onClick(View v) {

        if(System.currentTimeMillis() > lastClick + 100) {
            lastClick = System.currentTimeMillis();
            taskQueue.add(new Runnable() {  //functional interfaces apparently require a minimum of sdk 24, and thus aren't available with a minimum of sdk 21
                @Override
                public void run() {
                    onClick();
                }
            });
        }
    }

    @Override
    public boolean onScale(final ScaleGestureDetector detector) {
        scaleFactor = detector.getScaleFactor();

        taskQueue.add(new Runnable() {
            @Override
            public void run() {
                scale(scaleFactor, detector.getFocusX(), detector.getFocusY());
            }
        });
        return true;
    }

    public native void scale(float scaleFactor, double scaleX, double scaleY);

    public native void touch(double x, double y, double dx, double dy);

    public native void onClick();

    public native void rotationSensor(float[] rotationMatrix);

}