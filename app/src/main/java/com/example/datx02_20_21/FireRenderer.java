package com.example.datx02_20_21;

import android.opengl.GLSurfaceView;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

public class FireRenderer implements GLSurfaceView.Renderer {

    @Override
    public void onDrawFrame(GL10 gl) {
        update();
    }

    @Override
    public void onSurfaceCreated(GL10 gl, EGLConfig config) {
        //nothing
    }

    @Override
    public void onSurfaceChanged(GL10 gl, int width, int height) {
        init(width, height);
    }

    public native void update();

    public native void init(int width, int height);

}
