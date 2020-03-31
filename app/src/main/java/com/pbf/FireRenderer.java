package com.pbf;

import android.content.Context;
import android.content.res.AssetManager;
import android.opengl.GLSurfaceView;
import android.util.Log;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

public class FireRenderer implements GLSurfaceView.Renderer {

    private Context context;

    public FireRenderer(Context context){
        this.context = context;
    }

    @Override
    public void onSurfaceCreated(GL10 gl, EGLConfig config) {
        String extensions = gl.glGetString(gl.GL_EXTENSIONS);
        boolean has_float_buffer = (extensions.contains("EXT_color_buffer_float"));
        if (!has_float_buffer) {
            Log.e("FIRE", "EXT_color_buffer_float not supported, terminating program. . .");
            System.exit(-1);
        }
        if(init() == 0){
            Log.e("FIRE", "Failed to initialize the fire. . .");
            System.exit(-1);
        }
    }

    @Override
    public void onSurfaceChanged(GL10 gl, int width, int height) {
        resize(width, height);
    }

    @Override
    public void onDrawFrame(GL10 gl) {
        update();
        FPSCounter.logFrame();
    }

    public native void update();
    public native void resize(int width, int height);
    private native int init();

}

class FPSCounter {
    private static long startTime = System.nanoTime();
    private static int frames = 0;

    static public void logFrame() {
        frames++;
        if(System.nanoTime() - startTime >= 1000000000) {
            Log.d("FPSCounter", "fps: " + frames);
            frames = 0;
            startTime = System.nanoTime();
        }
    }
}
