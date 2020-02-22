package com.example.datx02_20_21;

import android.content.Context;
import android.opengl.GLSurfaceView;
import android.view.MotionEvent;
import android.view.ScaleGestureDetector;
import android.view.View;

class FireListener implements GLSurfaceView.OnTouchListener{

    private double touchX, touchY, oldX, oldY;
    private float scaleFactor = 1.0f;
    private ScaleGestureDetector scaleDetector;
    private long lastScale = 0;
    public FireListener(Context context){
        scaleDetector = new ScaleGestureDetector(context, new ScaleListener());
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
}
