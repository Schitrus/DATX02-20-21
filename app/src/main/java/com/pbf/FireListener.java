package com.pbf;

import android.content.Context;
import android.opengl.GLSurfaceView;
import android.view.MotionEvent;
import android.view.OrientationEventListener;
import android.view.OrientationListener;
import android.view.ScaleGestureDetector;
import android.view.View;

import java.util.Queue;

class FireListener implements View.OnTouchListener, View.OnClickListener {

    private final Queue<Runnable> taskQueue;
    private double touchX, touchY, oldX, oldY;
    private float scaleFactor = 1.0f;
    private ScaleGestureDetector scaleDetector;
    private long lastScale = 0;
    FireListener(Queue<Runnable> taskQueue, Context context) {
        this.taskQueue = taskQueue;
        scaleDetector = new ScaleGestureDetector(context, new ScaleListener());
    }

    @Override
    public boolean onTouch(View v, MotionEvent event) {
        touchX = event.getX();
        touchY = event.getY();
        final double deltaX = touchX - oldX;
        final double deltaY = touchY - oldY;
        oldX = touchX;
        oldY = touchY;
        float oldscale = scaleFactor;
        scaleDetector.onTouchEvent(event);
        if (oldscale != scaleFactor)
            lastScale = scaleDetector.getEventTime();
        if(((lastScale + 100) < event.getEventTime()) && event.getAction() == MotionEvent.ACTION_MOVE) {
            taskQueue.add(new Runnable() {  //functional interfaces apparently require a minimum of sdk 24, and thus aren't available with a minimum of sdk 21
                @Override
                public void run() {
                    touch(deltaX, deltaY);
                }
            });
        }

        if(deltaX == 0.0 && deltaY == 0.0)
            v.performClick();

        return true;
    }

    @Override
    public void onClick(View v) {

        taskQueue.add(new Runnable() {  //functional interfaces apparently require a minimum of sdk 24, and thus aren't available with a minimum of sdk 21
            @Override
            public void run() {
                onClick();
            }
        });
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

    public native void onClick();
}