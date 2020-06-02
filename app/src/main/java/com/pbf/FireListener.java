package com.pbf;

import android.content.Context;
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

    FireListener(Queue<Runnable> taskQueue, Context context) {
        this.taskQueue = taskQueue;
        scaleDetector = new ScaleGestureDetector(context, this);
    }

    @Override
    public boolean onTouch(View v, MotionEvent event) {
        double touchX = event.getX();
        double touchY = event.getY();
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
}